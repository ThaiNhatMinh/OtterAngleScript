// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Components/PrimitiveComponent.h"
#include "Misc/AssertionMacros.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "angelscript.h"

#include <new>

namespace
{
	using FWeakPrimitiveComponentPtr = TWeakObjectPtr<UPrimitiveComponent>;
	using FWeakPhysicalMaterialPtr = TWeakObjectPtr<UPhysicalMaterial>;

	static void WeakPrimitiveComponent_DefaultConstruct(FWeakPrimitiveComponentPtr* Memory)
	{
		new (Memory) FWeakPrimitiveComponentPtr();
	}

	static void WeakPrimitiveComponent_CopyConstruct(const FWeakPrimitiveComponentPtr& Other, FWeakPrimitiveComponentPtr* Memory)
	{
		new (Memory) FWeakPrimitiveComponentPtr(Other);
	}

	static void WeakPrimitiveComponent_ConstructObject(UPrimitiveComponent* Object, FWeakPrimitiveComponentPtr* Memory)
	{
		new (Memory) FWeakPrimitiveComponentPtr(Object);
	}

	static void WeakPrimitiveComponent_Destruct(FWeakPrimitiveComponentPtr* Memory)
	{
		Memory->~FWeakPrimitiveComponentPtr();
	}

	static FWeakPrimitiveComponentPtr& WeakPrimitiveComponent_Assign(FWeakPrimitiveComponentPtr& Value, const FWeakPrimitiveComponentPtr& Other)
	{
		Value = Other;
		return Value;
	}

	static FWeakPrimitiveComponentPtr& WeakPrimitiveComponent_AssignObject(FWeakPrimitiveComponentPtr& Value, UPrimitiveComponent* Object)
	{
		Value = Object;
		return Value;
	}

	static bool WeakPrimitiveComponent_Equals(const FWeakPrimitiveComponentPtr& Value, const FWeakPrimitiveComponentPtr& Other)
	{
		return Value == Other;
	}

	static bool WeakPrimitiveComponent_IsValid(const FWeakPrimitiveComponentPtr& Value)
	{
		return Value.IsValid();
	}

	static UPrimitiveComponent* WeakPrimitiveComponent_Get(const FWeakPrimitiveComponentPtr& Value)
	{
		return Value.Get();
	}

	static void WeakPhysicalMaterial_DefaultConstruct(FWeakPhysicalMaterialPtr* Memory)
	{
		new (Memory) FWeakPhysicalMaterialPtr();
	}

	static void WeakPhysicalMaterial_CopyConstruct(const FWeakPhysicalMaterialPtr& Other, FWeakPhysicalMaterialPtr* Memory)
	{
		new (Memory) FWeakPhysicalMaterialPtr(Other);
	}

	static void WeakPhysicalMaterial_ConstructObject(UPhysicalMaterial* Object, FWeakPhysicalMaterialPtr* Memory)
	{
		new (Memory) FWeakPhysicalMaterialPtr(Object);
	}

	static void WeakPhysicalMaterial_Destruct(FWeakPhysicalMaterialPtr* Memory)
	{
		Memory->~FWeakPhysicalMaterialPtr();
	}

	static FWeakPhysicalMaterialPtr& WeakPhysicalMaterial_Assign(FWeakPhysicalMaterialPtr& Value, const FWeakPhysicalMaterialPtr& Other)
	{
		Value = Other;
		return Value;
	}

	static FWeakPhysicalMaterialPtr& WeakPhysicalMaterial_AssignObject(FWeakPhysicalMaterialPtr& Value, UPhysicalMaterial* Object)
	{
		Value = Object;
		return Value;
	}

	static bool WeakPhysicalMaterial_Equals(const FWeakPhysicalMaterialPtr& Value, const FWeakPhysicalMaterialPtr& Other)
	{
		return Value == Other;
	}

	static bool WeakPhysicalMaterial_IsValid(const FWeakPhysicalMaterialPtr& Value)
	{
		return Value.IsValid();
	}

	static UPhysicalMaterial* WeakPhysicalMaterial_Get(const FWeakPhysicalMaterialPtr& Value)
	{
		return Value.Get();
	}
}

void Bind_TWeakObjectPtr(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"TWeakObjectPtr_UPrimitiveComponent",
		sizeof(FWeakPrimitiveComponentPtr),
		asOBJ_VALUE | asGetTypeTraits<FWeakPrimitiveComponentPtr>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPrimitiveComponent, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(WeakPrimitiveComponent_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPrimitiveComponent, asBEHAVE_CONSTRUCT, "void f(const TWeakObjectPtr_UPrimitiveComponent &in Other)", asFUNCTION(WeakPrimitiveComponent_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPrimitiveComponent, asBEHAVE_CONSTRUCT, "void f(UPrimitiveComponent@ Object)", asFUNCTION(WeakPrimitiveComponent_ConstructObject), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPrimitiveComponent, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(WeakPrimitiveComponent_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(TWeakObjectPtr_UPrimitiveComponent, "TWeakObjectPtr_UPrimitiveComponent &opAssign(const TWeakObjectPtr_UPrimitiveComponent &in Other)", asFUNCTION(WeakPrimitiveComponent_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPrimitiveComponent, "TWeakObjectPtr_UPrimitiveComponent &opAssign(UPrimitiveComponent@ Object)", asFUNCTION(WeakPrimitiveComponent_AssignObject), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPrimitiveComponent, "bool opEquals(const TWeakObjectPtr_UPrimitiveComponent &in Other) const", asFUNCTION(WeakPrimitiveComponent_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPrimitiveComponent, "bool IsValid() const", asFUNCTION(WeakPrimitiveComponent_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPrimitiveComponent, "UPrimitiveComponent@ Get() const", asFUNCTION(WeakPrimitiveComponent_Get), asCALL_CDECL_OBJFIRST);

	Result = Engine->RegisterObjectType(
		"TWeakObjectPtr_UPhysicalMaterial",
		sizeof(FWeakPhysicalMaterialPtr),
		asOBJ_VALUE | asGetTypeTraits<FWeakPhysicalMaterialPtr>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPhysicalMaterial, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(WeakPhysicalMaterial_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPhysicalMaterial, asBEHAVE_CONSTRUCT, "void f(const TWeakObjectPtr_UPhysicalMaterial &in Other)", asFUNCTION(WeakPhysicalMaterial_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPhysicalMaterial, asBEHAVE_CONSTRUCT, "void f(UPhysicalMaterial@ Object)", asFUNCTION(WeakPhysicalMaterial_ConstructObject), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(TWeakObjectPtr_UPhysicalMaterial, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(WeakPhysicalMaterial_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(TWeakObjectPtr_UPhysicalMaterial, "TWeakObjectPtr_UPhysicalMaterial &opAssign(const TWeakObjectPtr_UPhysicalMaterial &in Other)", asFUNCTION(WeakPhysicalMaterial_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPhysicalMaterial, "TWeakObjectPtr_UPhysicalMaterial &opAssign(UPhysicalMaterial@ Object)", asFUNCTION(WeakPhysicalMaterial_AssignObject), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPhysicalMaterial, "bool opEquals(const TWeakObjectPtr_UPhysicalMaterial &in Other) const", asFUNCTION(WeakPhysicalMaterial_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPhysicalMaterial, "bool IsValid() const", asFUNCTION(WeakPhysicalMaterial_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(TWeakObjectPtr_UPhysicalMaterial, "UPhysicalMaterial@ Get() const", asFUNCTION(WeakPhysicalMaterial_Get), asCALL_CDECL_OBJFIRST);
}
