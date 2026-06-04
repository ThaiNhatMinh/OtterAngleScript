// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Binding.h"
#include "Containers/UnrealString.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

#include <new>

namespace
{
	// -----------------------------------------------------------------------
	// Custom structs with custom alignment containing floats
	// -----------------------------------------------------------------------

	// 8-byte aligned struct with 2 floats (size = 8, alignment = 8)
	struct alignas(8) FAlign8Float2
	{
		float X;
		float Y;
	};

	// 16-byte aligned struct with 4 floats (size = 16, alignment = 16)
	struct alignas(16) FAlign16Float4
	{
		float A;
		float B;
		float C;
		float D;
	};

	// 16-byte aligned struct with 4 floats (size = 16, alignment = 16)
	struct FAlign16Float4_noalign
	{
		float A;
		float B;
		float C;
		float D;
	};

	// -----------------------------------------------------------------------
	// Constructors / destructors for FAlign8Float2
	// -----------------------------------------------------------------------

	static void Align8Float2_DefaultConstruct(FAlign8Float2* Memory)
	{
		new (Memory) FAlign8Float2{0.0f, 0.0f};
	}

	static void Align8Float2_CopyConstruct(const FAlign8Float2& Other, FAlign8Float2* Memory)
	{
		new (Memory) FAlign8Float2{Other.X, Other.Y};
	}

	static void Align8Float2_ConstructXY(float InX, float InY, FAlign8Float2* Memory)
	{
		new (Memory) FAlign8Float2{InX, InY};
	}

	static void Align8Float2_Destruct(FAlign8Float2* Memory)
	{
		Memory->~FAlign8Float2();
	}

	// -----------------------------------------------------------------------
	// Operators for FAlign8Float2
	// -----------------------------------------------------------------------

	static FAlign8Float2& Align8Float2_Assign(FAlign8Float2& Value, const FAlign8Float2& Other)
	{
		Value = Other;
		return Value;
	}

	static bool Align8Float2_OpEquals(const FAlign8Float2& A, const FAlign8Float2& B)
	{
		return A.X == B.X && A.Y == B.Y;
	}

	// -----------------------------------------------------------------------
	// Constructors / destructors for FAlign16Float4
	// -----------------------------------------------------------------------

	static void Align16Float4_DefaultConstruct(FAlign16Float4* Memory)
	{
		new (Memory) FAlign16Float4{0.0f, 0.0f, 0.0f, 0.0f};
	}

	static void Align16Float4_CopyConstruct(const FAlign16Float4& Other, FAlign16Float4* Memory)
	{
		new (Memory) FAlign16Float4{Other.A, Other.B, Other.C, Other.D};
	}

	static void Align16Float4_ConstructABCD(float InA, float InB, float InC, float InD, FAlign16Float4* Memory)
	{
		new (Memory) FAlign16Float4{InA, InB, InC, InD};
	}

	static void Align16Float4_Destruct(FAlign16Float4* Memory)
	{
		Memory->~FAlign16Float4();
	}

	// -----------------------------------------------------------------------
	// Operators for FAlign16Float4
	// -----------------------------------------------------------------------

	static FAlign16Float4& Align16Float4_Assign(FAlign16Float4_noalign& Value, const FAlign16Float4_noalign& Other)
	{
		Value = Other;
		return (FAlign16Float4 & )Value;
	}

	static bool Align16Float4_OpEquals(const FAlign16Float4& A, const FAlign16Float4& B)
	{
		return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
	}

	// -----------------------------------------------------------------------
	// Interop helpers
	// -----------------------------------------------------------------------

	/** Returns sizeof(FAlign8Float2) from script to verify expected layout. */
	static int32 Script_Align8Float2_SizeOf()
	{
		return static_cast<int32>(sizeof(FAlign8Float2));
	}

	/** Returns the alignment of FAlign8Float2 from script. */
	static int32 Script_Align8Float2_AlignOf()
	{
		return static_cast<int32>(alignof(FAlign8Float2));
	}

	/** Returns sizeof(FAlign16Float4) from script to verify expected layout. */
	static int32 Script_Align16Float4_SizeOf()
	{
		return static_cast<int32>(sizeof(FAlign16Float4));
	}

	/** Returns the alignment of FAlign16Float4 from script. */
	static int32 Script_Align16Float4_AlignOf()
	{
		return static_cast<int32>(alignof(FAlign16Float4));
	}

	/** Fills an FAlign8Float2 passed by ref from script. */
	static void Script_FillAlign8Float2(FAlign8Float2& OutValue, float InX, float InY)
	{
		OutValue.X = InX;
		OutValue.Y = InY;
	}

	/** Fills an FAlign16Float4 passed by ref from script. */
	static void Script_FillAlign16Float4(FAlign16Float4& OutValue, float InA, float InB, float InC, float InD)
	{
		OutValue.A = InA;
		OutValue.B = InB;
		OutValue.C = InC;
		OutValue.D = InD;
	}

	/** Reads FAlign8Float2 fields from script and returns a checksum int. */
	static int32 Script_ChecksumAlign8Float2(const FAlign8Float2& Value)
	{
		const int32 Xi = static_cast<int32>(Value.X * 10.0f);
		const int32 Yi = static_cast<int32>(Value.Y * 10.0f);
		return Xi + Yi * 100;
	}

	/** Reads FAlign16Float4 fields from script and returns a checksum int. */
	static int32 Script_ChecksumAlign16Float4(const FAlign16Float4& Value)
	{
		const int32 Ai = static_cast<int32>(Value.A * 10.0f);
		const int32 Bi = static_cast<int32>(Value.B * 10.0f);
		const int32 Ci = static_cast<int32>(Value.C * 10.0f);
		const int32 Di = static_cast<int32>(Value.D * 10.0f);
		return Ai + Bi * 100 + Ci * 10000 + Di * 1000000;
	}

	// -----------------------------------------------------------------------
	// Registration helper
	// -----------------------------------------------------------------------

	bool RegisterAlignStructTypes(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = 0;

		// --- FAlign8Float2 (alignas(8), POD, all floats) ---
		Result = Engine->RegisterObjectType("FAlign8Float2", sizeof(FAlign8Float2),
			asOBJ_VALUE | asGetTypeTraits<FAlign8Float2>() | asOBJ_APP_CLASS_ALLFLOATS | asOBJ_POD | asOBJ_APP_CLASS_ALIGN8);
		check(Result >= 0);

		REGISTER_BEHAVIOUR(FAlign8Float2, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Align8Float2_DefaultConstruct), asCALL_CDECL_OBJLAST);
		REGISTER_BEHAVIOUR(FAlign8Float2, asBEHAVE_CONSTRUCT, "void f(const FAlign8Float2 &in Other)", asFUNCTION(Align8Float2_CopyConstruct), asCALL_CDECL_OBJLAST);
		REGISTER_BEHAVIOUR(FAlign8Float2, asBEHAVE_CONSTRUCT, "void f(float InX, float InY)", asFUNCTION(Align8Float2_ConstructXY), asCALL_CDECL_OBJLAST);
		REGISTER_BEHAVIOUR(FAlign8Float2, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Align8Float2_Destruct), asCALL_CDECL_OBJLAST);

		REGISTER_PROPERTY(FAlign8Float2, "float X", X);
		REGISTER_PROPERTY(FAlign8Float2, "float Y", Y);

		REGISTER_METHOD(FAlign8Float2, "FAlign8Float2 &opAssign(const FAlign8Float2 &in Other)", asFUNCTION(Align8Float2_Assign), asCALL_CDECL_OBJFIRST);
		REGISTER_METHOD(FAlign8Float2, "bool opEquals(const FAlign8Float2 &in Other) const", asFUNCTION(Align8Float2_OpEquals), asCALL_CDECL_OBJFIRST);

		// --- FAlign16Float4 (alignas(16), POD, all floats) ---
		Result = Engine->RegisterObjectType("FAlign16Float4", sizeof(FAlign16Float4),
			asOBJ_VALUE | asGetTypeTraits<FAlign16Float4>() | asOBJ_APP_CLASS_ALLFLOATS | asOBJ_POD);
		check(Result >= 0);

		REGISTER_BEHAVIOUR(FAlign16Float4, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Align16Float4_DefaultConstruct), asCALL_CDECL_OBJLAST);
		REGISTER_BEHAVIOUR(FAlign16Float4, asBEHAVE_CONSTRUCT, "void f(const FAlign16Float4 &in Other)", asFUNCTION(Align16Float4_CopyConstruct), asCALL_CDECL_OBJLAST);
		REGISTER_BEHAVIOUR(FAlign16Float4, asBEHAVE_CONSTRUCT, "void f(float InA, float InB, float InC, float InD)", asFUNCTION(Align16Float4_ConstructABCD), asCALL_CDECL_OBJLAST);
		REGISTER_BEHAVIOUR(FAlign16Float4, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Align16Float4_Destruct), asCALL_CDECL_OBJLAST);

		REGISTER_PROPERTY(FAlign16Float4, "float A", A);
		REGISTER_PROPERTY(FAlign16Float4, "float B", B);
		REGISTER_PROPERTY(FAlign16Float4, "float C", C);
		REGISTER_PROPERTY(FAlign16Float4, "float D", D);

		REGISTER_METHOD(FAlign16Float4, "FAlign16Float4 &opAssign(const FAlign16Float4 &in Other)", asFUNCTION(Align16Float4_Assign), asCALL_CDECL_OBJFIRST);
		REGISTER_METHOD(FAlign16Float4, "bool opEquals(const FAlign16Float4 &in Other) const", asFUNCTION(Align16Float4_OpEquals), asCALL_CDECL_OBJFIRST);

		// --- Global interop functions ---
		Result = Engine->RegisterGlobalFunction("int FAlign8Float2SizeOf()", asFUNCTION(Script_Align8Float2_SizeOf), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("int FAlign8Float2AlignOf()", asFUNCTION(Script_Align8Float2_AlignOf), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("int FAlign16Float4SizeOf()", asFUNCTION(Script_Align16Float4_SizeOf), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("int FAlign16Float4AlignOf()", asFUNCTION(Script_Align16Float4_AlignOf), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("void FillAlign8Float2(FAlign8Float2 &out Value, float InX, float InY)",
			asFUNCTION(Script_FillAlign8Float2), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("void FillAlign16Float4(FAlign16Float4 &out Value, float InA, float InB, float InC, float InD)",
			asFUNCTION(Script_FillAlign16Float4), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("int ChecksumAlign8Float2(const FAlign8Float2 &in Value)",
			asFUNCTION(Script_ChecksumAlign8Float2), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction("int ChecksumAlign16Float4(const FAlign16Float4 &in Value)",
			asFUNCTION(Script_ChecksumAlign16Float4), asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptCustomAlignStructTests,
	"OtterAngleScript.AlignStruct",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	asIScriptEngine* Engine = nullptr;
	asIScriptModule* ScriptModule = nullptr;
	asIScriptContext* Context = nullptr;

	asIScriptFunction* BuildFunction(const char* SectionName, const char* Script, const char* Declaration)
	{
		if (!Assert.IsTrue(ScriptModule->AddScriptSection(SectionName, Script) >= 0))
		{
			return nullptr;
		}
		if (!Assert.IsTrue(ScriptModule->Build() >= 0))
		{
			return nullptr;
		}

		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl(Declaration);
		if (!Assert.IsNotNull(Function))
		{
			return nullptr;
		}
		return Function;
	}

	int ExecuteFunction(asIScriptFunction* Function)
	{
		if (Context != nullptr)
		{
			Context->Release();
			Context = nullptr;
		}

		Context = Engine->CreateContext();
		if (!Assert.IsNotNull(Context))
		{
			return -1;
		}
		if (!Assert.IsTrue(Context->Prepare(Function) >= 0))
		{
			return -1;
		}
		return Context->Execute();
	}

	int32 ExecuteIntFunction(asIScriptFunction* Function)
	{
		const int Result = ExecuteFunction(Function);
		if (Result == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		if (!Assert.IsTrue(Result == asEXECUTION_FINISHED))
		{
			return -1;
		}
		return static_cast<int32>(Context->GetReturnDWord());
	}

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));
		ASSERT_THAT(IsTrue(RegisterAlignStructTypes(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptTest", asGM_ALWAYS_CREATE);
		ASSERT_THAT(IsNotNull(ScriptModule));
	}

	AFTER_EACH()
	{
		if (Context != nullptr)
		{
			Context->Release();
			Context = nullptr;
		}

		if (ScriptModule != nullptr)
		{
			ScriptModule->Discard();
			ScriptModule = nullptr;
		}

		Engine = nullptr;
	}

	// -----------------------------------------------------------------------
	// FAlign8Float2 tests (alignas(8))
	// -----------------------------------------------------------------------

	TEST_METHOD(Align8Float2_TypeInfo)
	{
		AddInfo("Verify FAlign8Float2 type is registered with correct size and alignment.");
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FAlign8Float2")));

		static const char Script[] = R"(
int RunAlignmentCheck()
{
    int ExpectedSize = FAlign8Float2SizeOf();
    int ExpectedAlign = FAlign8Float2AlignOf();

    if (ExpectedSize != 8)
        return -1;

    if (ExpectedAlign != 8)
        return -2;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align8SizeCheck", Script, "int RunAlignmentCheck()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align8Float2_ConstructAndProperties)
	{
		AddInfo("Construct FAlign8Float2 in script and access its float properties.");

		static const char Script[] = R"(
int RunConstructAndRead()
{
    FAlign8Float2 Default;
    if (Default.X != 0.0f || Default.Y != 0.0f)
        return -1;

    FAlign8Float2 Value(3.5f, 7.2f);
    if (Value.X != 3.5f || Value.Y != 7.2f)
        return -2;

    FAlign8Float2 Copy(Value);
    if (Copy.X != 3.5f || Copy.Y != 7.2f)
        return -3;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align8Construct", Script, "int RunConstructAndRead()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align8Float2_AssignAndEquals)
	{
		AddInfo("Test assignment and equality for FAlign8Float2.");

		static const char Script[] = R"(
int RunAssignAndEquals()
{
    FAlign8Float2 A(1.0f, 2.0f);
    FAlign8Float2 B;
    B = A;
    if (!B.opEquals(A))
        return -1;

    if (B.X != 1.0f || B.Y != 2.0f)
        return -2;

    FAlign8Float2 C(3.0f, 4.0f);
    if (B.opEquals(C))
        return -3;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align8AssignEquals", Script, "int RunAssignAndEquals()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align8Float2_ScriptToUnreal)
	{
		AddInfo("Pass FAlign8Float2 from script to Unreal interop functions.");

		static const char Script[] = R"(
int RunScriptToUnreal()
{
    FAlign8Float2 Value(2.5f, 4.0f);
    int Sum = ChecksumAlign8Float2(Value);
    if (Sum != (25 + 40 * 100))
        return -1;

    FAlign8Float2 Filled;
    FillAlign8Float2(Filled, 9.0f, 3.0f);
    if (Filled.X != 9.0f || Filled.Y != 3.0f)
        return -2;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align8ScriptToUnreal", Script, "int RunScriptToUnreal()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align8Float2_UnrealToScript)
	{
		AddInfo("Fill FAlign8Float2 from Unreal and read it back from script.");

		static const char Script[] = R"(
FAlign8Float2 ReturnFilledValue()
{
    FAlign8Float2 Result(1.5f, 2.5f);
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("Align8UnrealToScript", Script, "FAlign8Float2 ReturnFilledValue()");
		ASSERT_THAT(IsNotNull(Function));

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));

		const int ExecuteResult = Context->Execute();
		if (ExecuteResult == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}
		ASSERT_THAT(IsTrue(ExecuteResult == asEXECUTION_FINISHED));

		const FAlign8Float2* ReturnValue = static_cast<const FAlign8Float2*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsNear(1.5f, ReturnValue->X, UE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(2.5f, ReturnValue->Y, UE_KINDA_SMALL_NUMBER));
	}

	// -----------------------------------------------------------------------
	// FAlign16Float4 tests (alignas(16))
	// -----------------------------------------------------------------------

	TEST_METHOD(Align16Float4_TypeInfo)
	{
		AddInfo("Verify FAlign16Float4 type is registered with correct size and alignment.");
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FAlign16Float4")));

		static const char Script[] = R"(
int RunAlignmentCheck()
{
    int ExpectedSize = FAlign16Float4SizeOf();
    int ExpectedAlign = FAlign16Float4AlignOf();

    if (ExpectedSize != 16)
        return -1;

    if (ExpectedAlign != 16)
        return -2;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align16SizeCheck", Script, "int RunAlignmentCheck()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align16Float4_ConstructAndProperties)
	{
		AddInfo("Construct FAlign16Float4 in script and access its float properties.");

		static const char Script[] = R"(
int RunConstructAndRead()
{
    FAlign16Float4 Default;
    if (Default.A != 0.0f || Default.B != 0.0f || Default.C != 0.0f || Default.D != 0.0f)
        return -1;

    FAlign16Float4 Value(1.0f, 2.0f, 3.0f, 4.0f);
    if (Value.A != 1.0f || Value.B != 2.0f || Value.C != 3.0f || Value.D != 4.0f)
        return -2;

    FAlign16Float4 Copy(Value);
    if (Copy.A != 1.0f || Copy.B != 2.0f || Copy.C != 3.0f || Copy.D != 4.0f)
        return -3;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align16Construct", Script, "int RunConstructAndRead()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align16Float4_AssignAndEquals)
	{
		AddInfo("Test assignment and equality for FAlign16Float4.");

		static const char Script[] = R"(
int RunAssignAndEquals()
{
    FAlign16Float4 A(1.0f, 2.0f, 3.0f, 4.0f);
    FAlign16Float4 B;
    B = A;
    if (!B.opEquals(A))
        return -1;

    if (B.A != 1.0f || B.B != 2.0f || B.C != 3.0f || B.D != 4.0f)
        return -2;

    FAlign16Float4 C(5.0f, 6.0f, 7.0f, 8.0f);
    if (B.opEquals(C))
        return -3;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align16AssignEquals", Script, "int RunAssignAndEquals()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align16Float4_ScriptToUnreal)
	{
		AddInfo("Pass FAlign16Float4 from script to Unreal interop functions.");

		static const char Script[] = R"(
int RunScriptToUnreal()
{
    FAlign16Float4 Value(2.0f, 3.0f, 4.0f, 5.0f);
    int Sum = ChecksumAlign16Float4(Value);
    if (Sum != (20 + 30 * 100 + 40 * 10000 + 50 * 1000000))
        return -1;

    FAlign16Float4 Filled;
    FillAlign16Float4(Filled, 9.0f, 8.0f, 7.0f, 6.0f);
    if (Filled.A != 9.0f || Filled.B != 8.0f || Filled.C != 7.0f || Filled.D != 6.0f)
        return -2;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Align16ScriptToUnreal", Script, "int RunScriptToUnreal()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Align16Float4_UnrealToScript)
	{
		AddInfo("Fill FAlign16Float4 from script function and verify return value in C++.");

		static const char Script[] = R"(
FAlign16Float4 ReturnFilledValue()
{
    FAlign16Float4 Result(3.0f, 4.0f, 5.0f, 6.0f);
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("Align16UnrealToScript", Script, "FAlign16Float4 ReturnFilledValue()");
		ASSERT_THAT(IsNotNull(Function));

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));

		const int ExecuteResult = Context->Execute();
		if (ExecuteResult == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}
		ASSERT_THAT(IsTrue(ExecuteResult == asEXECUTION_FINISHED));

		const FAlign16Float4* ReturnValue = static_cast<const FAlign16Float4*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsNear(3.0f, ReturnValue->A, UE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(4.0f, ReturnValue->B, UE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(5.0f, ReturnValue->C, UE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(6.0f, ReturnValue->D, UE_KINDA_SMALL_NUMBER));
	}

	// -----------------------------------------------------------------------
	// Cross-type sanity: struct mixed use in array
	// -----------------------------------------------------------------------

	TEST_METHOD(AlignStruct_UsedWithTArray)
	{
		AddInfo("Ensure alignas structs work with TArray binding.");

		static const char Script[] = R"(
int RunArrayOfAlignStructs()
{
    TArray<FAlign8Float2> Array;
    FAlign8Float2 Item(1.0f, 2.0f);
    Array.Add(Item);
    Array.Add(FAlign8Float2(3.0f, 4.0f));

    if (Array.Length() != 2)
        return -1;

    if (Array[0].X != 1.0f || Array[0].Y != 2.0f)
        return -2;

    if (Array[1].X != 3.0f || Array[1].Y != 4.0f)
        return -3;

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("AlignStructInArray", Script, "int RunArrayOfAlignStructs()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
