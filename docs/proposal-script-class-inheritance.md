# Proposal: Script Class Inheritance from Bound C++ Classes

**Status:** Draft  
**Date:** 2026-06-08  

---

## 1. Executive Summary

Allow AngelScript script classes to `inherit` from C++ classes that are bound to the engine via `RegisterObjectType`. This enables patterns like:

```angelscript
class MyActor : AActor
{
    void BeginPlay() override
    {
        Super::BeginPlay();
        Print("Hello from script!");
    }
}
```

The AngelScript engine (with existing UE5 patches) **already supports** structural inheritance — script classes can declare a C++ base type and inherit its properties and methods. However, **virtual method dispatch** (the `override` keyword) does not currently work for C++ base methods because they are registered as native functions rather than `asFUNC_VIRTUAL`.

This proposal lays out a phased approach to close that gap.

---

## 2. Current State Analysis

### 2.1 What the Engine Already Supports

The engine's `as_builder.cpp` (lines 3360–3603) handles inheritance during compilation:

| Capability | Status | Details |
|---|---|---|
| Script class inherits from C++ value type | ✅ Works | `objType->size != 0` path at line 3372 |
| Script class inherits from C++ ref type | ✅ Works | UE5 patch at line 3374: `|| !(objType->flags & asOBJ_SCRIPT_OBJECT)` |
| `final` classes blocked | ✅ Works | `asOBJ_NOINHERIT` check at line 3364 |
| Base properties inherited at correct offsets | ✅ Works | Lines 3520–3527 |
| Base methods copied to derived `methods[]` | ✅ Works | Line 3601 |
| Script override detection (same name+signature) | ✅ Works | Lines 3529–3586 — overridden methods moved to VFT |
| Base flags propagated to derived type | ✅ Works | Line 3434 |
| Circular inheritance detection | ✅ Works | Lines 3389–3402 |

### 2.2 The Virtual Dispatch Gap

The critical issue is at `as_builder.cpp` lines 3588–3592:

```cpp
if( !found
    && baseType->flags & asOBJ_SCRIPT_OBJECT  // UE5: only copy if base is script
    )
{
    ot->virtualFunctionTable.PushLast(baseType->virtualFunctionTable[m]);
}
```

When the base type is a C++ bound class (not `asOBJ_SCRIPT_OBJECT`):
- Base methods are **not** added to the derived class's `virtualFunctionTable`.
- Base methods are still added to `methods[]` (line 3601), so they are callable.
- Script overrides go into the VFT (lines 3555, 3581) — the override functions exist.

**The runtime dispatch gap** (`as_context.cpp:1590–1639`):

```cpp
if( currentFunction->funcType == asFUNC_VIRTUAL )  // ← KEY CHECK
{
    asCObjectType *objType = obj->objType;
    realFunc = objType->virtualFunctionTable[currentFunction->vfTableIdx];
}
```

C++ methods registered via `RegisterObjectMethod` are native or `asFUNC_SCRIPT`, **never** `asFUNC_VIRTUAL`. Therefore:
- A call to `baseObj.SomeMethod()` dispatches directly to the C++ function pointer.
- Even if a script subclass has an override in its VFT, the override is **never consulted**.

### 2.3 In Practice

```angelscript
class MyActor : AActor
{
    void BeginPlay() override  // This override IS created in the VFT ✓
    {
        Print("Hello from script!");
    }
}

void Test(AActor@ actor)
{
    actor.BeginPlay();  // Calls C++ AActor::BeginPlay directly — override IGNORED ✗
}
```

The `override` syntax is accepted during compilation but at runtime the call bypasses the VFT entirely.

---

## 3. Proposed Solution

### 3.1 Design Philosophy

Three-layer solution:

1. **Binding API** — Add macros/helpers to mark bound types as "open for inheritance" and register methods in a virtual-dispatch-compatible way.
2. **Trampoline Dispatch** — For each virtual-callable C++ method, register a generic trampoline that inspects the runtime object type, checks the VFT for an override, and dispatches accordingly.
3. **Engine Enhancement** (optional, Phase 2) — Add `asFUNC_VIRTUAL` registration support directly to the engine to eliminate trampoline overhead.

### 3.2 Phase 1 — Inheritance-Ready Binding Infrastructure

**Goal:** Script classes can extend C++ types, access base properties/methods, and **override virtual methods** via a trampoline dispatch layer. Requires **zero engine changes**.

#### A) New Binding.h Macros

```cpp
// Mark a type as inheritable (ensures asOBJ_NOINHERIT is not set)
#define DECLARE_INHERITABLE_CLASS(ClassName, Size, Flags) \
    Result = Engine->RegisterObjectType(#ClassName, Size, \
        (Flags) & ~asOBJ_NOINHERIT); \
    check(Result >= 0)

// Register a virtual-dispatch-capable method via a generic trampoline
#define REGISTER_VIRTUAL_METHOD(ClassType, Declaration, NativeFunc, TrampolineFunc) \
    Result = Engine->RegisterObjectMethod(#ClassType, Declaration, \
        asFUNCTION(TrampolineFunc), asCALL_GENERIC); \
    check(Result >= 0)
```

#### B) Trampoline Dispatch Pattern

The trampoline is a generic-calling-convention function registered in place of the native C++ method, and it handles the VFT lookup.

**Core VFT accessor** — since `asCObjectType::virtualFunctionTable` is internal to the AS SDK, we expose a minimal accessor via a free function declared in the plugin:

```cpp
// In VirtualDispatch.h — accesses asCObjectType internals via forward declaration
namespace OtterAS
{
    // Returns the function at vftIdx for the given script object's runtime type,
    // or nullptr if no override exists.
    asIScriptFunction* GetVirtualFunction(asIScriptObject* obj, int vftIdx);
    int GetVirtualFunctionIndex(asIScriptObject* obj, const char* funcDecl);
}
```

**Implementation** (in `VirtualDispatch.cpp`) uses `friend` access or the private header path (`source/as_objecttype.h`) already included in the build:

```cpp
#include "as_objecttype.h"  // Already available via ThirdParty SDK source

asIScriptFunction* OtterAS::GetVirtualFunction(asIScriptObject* obj, int vftIdx)
{
    auto* ot = static_cast<asCObjectType*>(obj->GetObjectType());
    if (vftIdx >= 0 && vftIdx < (int)ot->virtualFunctionTable.GetLength())
        return ot->virtualFunctionTable[vftIdx];
    return nullptr;
}
```

**Generic trampoline template**:

```cpp
template<typename Class, typename... Args>
struct VirtualMethodTrampoline;

// For a void-returning method with one int parameter, e.g.: void SetHealth(int)
template<typename Class, typename Arg>
struct VirtualMethodTrampoline<Class, void, Arg>
{
    using NativeFuncPtr = void (*)(Class*, Arg);

    static void Trampoline(asIScriptGeneric* gen)
    {
        void* objPtr = gen->GetObject();
        auto* scriptObj = static_cast<asIScriptObject*>(objPtr);

        // Check for script override
        asIScriptFunction* override_ = OtterAS::GetVirtualFunction(scriptObj, VFTIndex);
        if (override_)
        {
            // Call script override via a temporary context
            auto* ctx = gen->GetEngine()->RequestContext();
            ctx->Prepare(override_);
            ctx->SetObject(objPtr);
            ctx->SetArgDWord(0, gen->GetArgDWord(0));
            ctx->Execute();
            gen->GetEngine()->ReturnContext(ctx);
        }
        else
        {
            // Fall back to native C++ call
            NativeFunc(static_cast<Class*>(objPtr), static_cast<Arg>(gen->GetArgDWord(0)));
        }
    }

    static inline NativeFuncPtr NativeFunc = nullptr;
    static inline int VFTIndex = -1;
};
```

#### C) Per-Type Virtual Method Table Index Tracking

Since `vfTableIdx` is assigned during compilation (line 3175 of `as_builder.cpp`), we can't know it at registration time. Instead, we maintain a **lookup table** per base class:

```cpp
// In VirtualDispatch.h
class FVirtualMethodRegistry
{
public:
    // Called after Build() to cache VFT indices by method declaration
    static void CacheVFTIndices(asIScriptEngine* Engine, const char* ClassName);
    static int GetVFTIndex(const char* ClassName, const char* MethodDecl);
private:
    static TMap<FString, TMap<FString, int>> ClassMethodToVFTIndex;
};
```

After `Module->Build()`, iterate the base type's methods and map each to its VFT slot:

```cpp
void FVirtualMethodRegistry::CacheVFTIndices(asIScriptEngine* Engine, const char* ClassName)
{
    auto* typeInfo = Engine->GetTypeInfoByName(ClassName);
    if (!typeInfo) return;

    auto& methodMap = ClassMethodToVFTIndex.FindOrAdd(ClassName);
    for (asUINT i = 0; i < typeInfo->GetMethodCount(); i++)
    {
        asIScriptFunction* func = typeInfo->GetMethodByIndex(i);
        methodMap.Add(func->GetDeclaration(), i);  // i == vfTableIdx for base methods
    }
}
```

### 3.3 Phase 2 — Engine-Level `asFUNC_VIRTUAL` Support (Longer Term)

**Goal:** Eliminate trampoline overhead by allowing native methods to be registered with `asFUNC_VIRTUAL`.

#### A) `as_builder.cpp` — Populate C++ Base Type VFT

Change lines 3588–3592 from:

```cpp
if( !found
    && baseType->flags & asOBJ_SCRIPT_OBJECT  // ← Remove this guard
    )
```

To:

```cpp
if( !found )
{
    if( baseType->flags & asOBJ_SCRIPT_OBJECT )
    {
        ot->virtualFunctionTable.PushLast(baseType->virtualFunctionTable[m]);
    }
    else
    {
        // C++ base: create asFUNC_VIRTUAL proxy wrapping the native method
        asCScriptFunction* vf = asNEW(asCScriptFunction)(engine, module, asFUNC_VIRTUAL);
        vf->vfTableIdx = int(ot->virtualFunctionTable.GetLength());
        // ... copy signature, funcPtr, etc. from baseType->methods[m] ...
        ot->virtualFunctionTable.PushLast(vf);
    }
    // AddRef handled below
}
```

#### B) `angelscript.h` — Public API Addition

```cpp
// New method on asIScriptEngine:
virtual int RegisterObjectMethod(const char* obj, const char* declaration,
    const asSFuncPtr& funcPtr, asDWORD callConv,
    void* auxiliary = 0, bool isVirtual = false) = 0;

// New method on asITypeInfo:
virtual asIScriptFunction* GetVirtualFunctionByIndex(asUINT index) const = 0;
```

### 3.4 Registration Flow Comparison

**Current (no inheritance):**
```cpp
Engine->RegisterObjectType("AMyBase", sizeof(AMyBase), asOBJ_VALUE | asGetTypeTraits<AMyBase>());
Engine->RegisterObjectMethod("AMyBase", "void BeginPlay()", asFUNCTION(AMyBase_BeginPlay), asCALL_CDECL);
```

**Phase 1 (trampoline):**
```cpp
DECLARE_INHERITABLE_CLASS(AMyBase, sizeof(AMyBase), asOBJ_VALUE | asGetTypeTraits<AMyBase>());
// RegisterMethod + cache VFT index
REGISTER_VIRTUAL_METHOD(AMyBase, "void BeginPlay()",
    &AMyBase::BeginPlay, VirtualMethodTrampoline<AMyBase, void>::Trampoline);
// After Build():
FVirtualMethodRegistry::CacheVFTIndices(Engine, "AMyBase");
```

**Phase 2 (native `asFUNC_VIRTUAL`):**
```cpp
DECLARE_INHERITABLE_CLASS(AMyBase, sizeof(AMyBase), asOBJ_VALUE | asGetTypeTraits<AMyBase>());
Engine->RegisterObjectMethod("AMyBase", "void BeginPlay()",
    asFUNCTION(AMyBase_BeginPlay), asCALL_CDECL, nullptr, /*isVirtual=*/true);
```

---

## 4. Constraints and Limitations

### 4.1 Value Types vs Reference Types

| Base Type Kind | Inheritance | Notes |
|---|---|---|
| **Value type** (e.g., `FVector`) | ✅ Works | Memory layout MUST be stable. |
| **Reference type** (e.g., `UObject`) | ✅ Works (UE5 patch) | Size 0; script wraps the C++ pointer. |
| **POD types** | ⚠️ Limited | `asOBJ_POD` is incompatible with behaviors. |
| **Template types** | ❌ Not supported | Template instantiation complicates VFT layout. |

### 4.2 Memory Layout

- **Value types**: Script subclass embeds C++ base at offset 0. C++ code receiving a base pointer must not assume it's a standalone object (no `delete`/`free` on base pointers that could be script subclass instances).
- **Reference types**: No layout conflict — the script object stores a handle to the C++ object.

### 4.3 Override Limitations

- Only methods registered via the trampoline (Phase 1) or `isVirtual=true` (Phase 2) can be overridden.
- Property accessors can be overridden if registered as virtual methods.
- Operators can theoretically be virtual but add significant complexity.
- Constructors are **not** overridable — AngelScript always calls the base constructor first.

### 4.4 Engine Modification Scope

| File | Phase 1 | Phase 2 |
|---|---|---|
| `angelscript.h` | None | Add `isVirtual` param + `GetVirtualFunctionByIndex` |
| `as_scriptengine.cpp` | None | Handle `isVirtual`, create VFT proxy |
| `as_builder.cpp` | None | Remove `asOBJ_SCRIPT_OBJECT` guard, populate C++ base VFT |
| `Binding.h` | New macros | Minor updates |
| `VirtualDispatch.h/.cpp` | New files | Simplified/removed |

---

## 5. Full Walkthrough

### 5.1 Script-Side

```angelscript
class AMyBaseClass  // Bound from C++ with virtual methods
{
    void BeginPlay()        // virtual — can be overridden
    {
        // Default C++ impl
    }
    int GetHealth() const   // virtual
    {
        return 0;
    }
}

class MyScriptClass : AMyBaseClass
{
    int bonusHealth;
    MyScriptClass() { bonusHealth = 50; }

    void BeginPlay() override
    {
        Super::BeginPlay();      // Calls C++ base
        Print("Script BeginPlay!");
    }

    int GetHealth() const override
    {
        return Super::GetHealth() + bonusHealth;
    }
}
```

### 5.2 C++ Binding (Phase 1)

```cpp
// In Bind_AMyBaseClass.cpp:

// Native C++ impl
static void AMyBase_BeginPlay_C(asIScriptGeneric* gen) {
    ((AMyBaseClass*)gen->GetObject())->BeginPlay();
}

// Generic trampoline
static void AMyBase_BeginPlay_Trampoline(asIScriptGeneric* gen)
{
    void* obj = gen->GetObject();
    auto* scriptObj = (asIScriptObject*)obj;
    int vftIdx = VirtualMethodRegistry.Get("AMyBaseClass", "void BeginPlay()");
    auto* override_ = OtterAS::GetVirtualFunction(scriptObj, vftIdx);
    if (override_) {
        // Dispatch to script override
        auto* ctx = gen->GetEngine()->RequestContext();
        ctx->Prepare(override_);
        ctx->SetObject(obj);
        ctx->Execute();
        gen->GetEngine()->ReturnContext(ctx);
    } else {
        ((AMyBaseClass*)obj)->BeginPlay();
    }
}

void Bind_AMyBaseClass(asIScriptEngine* Engine)
{
    int Result;
    REGISTER_BEHAVIOUR(AMyBaseClass, asBEHAVE_CONSTRUCT, "void f()",
        asFUNCTION(AMyBase_Construct), asCALL_CDECL);
    // Register virtual method via trampoline
    REGISTER_VIRTUAL_METHOD(AMyBaseClass, "void BeginPlay()",
        AMyBase_BeginPlay_C, AMyBase_BeginPlay_Trampoline);
}
```

---

## 6. Implementation Roadmap

### Milestone 1 — Foundation (Week 1–2)
- [ ] Add `DECLARE_INHERITABLE_CLASS` and `REGISTER_VIRTUAL_METHOD` macros to `Binding.h`
- [ ] Create `VirtualDispatch.h/.cpp` with `GetVirtualFunction()` and trampoline template
- [ ] Implement `FVirtualMethodRegistry` for VFT index caching
- [ ] Demonstrate with one reference type and one value type

### Milestone 2 — Validation (Week 2–3)
- [ ] Write CQTest cases in `OtterAngleScriptTest`:
  - Script inherits from C++ value type, accesses base properties
  - Script inherits from C++ ref type, calls base methods
  - Script overrides virtual method, verify dispatch
  - Multi-level inheritance (C++ → Script → Script)
  - Constructor chain (script subclass calls base constructor)
- [ ] Performance benchmark: trampoline vs direct call

### Milestone 3 — Engine Enhancement (Week 3–5, optional)
- [ ] Modify `RegisterObjectMethod` to accept `isVirtual` parameter
- [ ] Modify `as_builder.cpp` to populate C++ base VFT
- [ ] Add `GetVirtualFunctionByIndex()` to `asITypeInfo` public API
- [ ] Port trampoline bindings to native `asFUNC_VIRTUAL`

### Milestone 4 — Rollout (Week 5–6)
- [ ] Mark key types (AActor, UActorComponent, etc.) as inheritable
- [ ] Add `asOBJ_NOINHERIT` to types that should remain `final`
- [ ] Document the inheritance pattern for script authors

---

## 7. Risk Assessment

| Risk | Severity | Mitigation |
|---|---|---|
| Trampoline per-call overhead | Low | Only affects methods marked virtual; Phase 2 eliminates it. |
| Engine patch conflicts with upstream AS | Medium | Use `// OTTER-BEGIN/END` markers; patches are already maintained for UE5. |
| ABI break in `RegisterObjectMethod` | Low | Use trailing default parameter or new method name. |
| Memory corruption from value-type slicing | Medium | Document and enforce that C++ code must not `delete` base pointers. |
| Binding maintenance burden | Medium | Most types won't need virtual dispatch; macros keep it manageable. |

---

## 8. Alternatives Considered

### 8.1 Pure Script-Side Wrapper
Manually wrap C++ objects in script classes with delegation. **Rejected:** No polymorphic dispatch.

### 8.2 Script-Only Inheritance
Only allow script→script inheritance. **Rejected:** This is the current state.

### 8.3 Code-Generated Trampolines (Supplement)
Use UHT/metaprogramming to auto-generate trampolines from `UFUNCTION` metadata. Viable supplement to Phase 1 if manual authoring is burdensome.

---

## 9. Conclusion

The AngelScript engine already has infrastructure for script class inheritance from C++ types. The **single missing piece** is virtual method dispatch: C++ methods aren't `asFUNC_VIRTUAL`, so the VFT is never consulted at call sites.

**Phase 1** bridges the gap with a generic trampoline pattern requiring **zero engine changes**. **Phase 2** (optional) moves the solution into the engine for zero-overhead dispatch. The binding additions are minimal (two macros, one new source file), and the primary work is in the trampoline implementation and test validation.
