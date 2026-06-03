// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FVector4f UnrealBuildVector4f(float X, float Y, float Z, float W)
	{
		return FVector4f(X, Y, Z, W);
	}

	bool UnrealAcceptsVector4f(const FVector4f& Value)
	{
		return Value == FVector4f(1.f, 2.f, 3.f, 4.f);
	}

	bool RegisterFVector4fInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FVector4f UnrealBuildVector4f(float X, float Y, float Z, float W)",
			asFUNCTION(UnrealBuildVector4f),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsVector4f(const FVector4f &in Value)",
			asFUNCTION(UnrealAcceptsVector4f),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFVector4fTests,
	"OtterAngleScript.FVector4f",
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

	int32 ExecuteIntFunction(asIScriptFunction* Function)
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

		const int Result = Context->Execute();
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
		ASSERT_THAT(IsTrue(RegisterFVector4fInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptVector4fTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector4f")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V;
    if (V.X != 0.0f || V.Y != 0.0f || V.Z != 0.0f || V.W != 1.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(XYZWConstructor)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(1.0f, 2.0f, 3.0f, 4.0f);
    if (!UnrealAcceptsVector4f(V)) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("XYZWCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ConstructFromLinearColor)
	{
		static const char Script[] = R"(
int RunTest()
{
    FLinearColor C(0.5f, 0.25f, 0.1f, 1.0f);
    FVector4f V(C);
    if (V.X != 0.5f || V.Y != 0.25f || V.Z != 0.1f || V.W != 1.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("CtorFromLinearColor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(1.0f, 2.0f, 3.0f, 4.0f);
    V.X = 10.0f;
    V.Y = 20.0f;
    V.Z = 30.0f;
    V.W = 40.0f;
    if (V.X != 10.0f || V.Y != 20.0f || V.Z != 30.0f || V.W != 40.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Properties", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f B;
    B = A;
    if (!(B == A)) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Assign", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f B(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f C(1.0f, 2.0f, 3.0f, 5.0f);
    if (!(A == B)) { return -1; }
    if (A == C) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("EqualsOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(EqualsMethod)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f B(1.001f, 2.0f, 3.0f, 4.0f);
    if (!A.Equals(B, 0.01f)) { return -1; }
    if (A.Equals(B, 0.0001f)) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("EqualsMethod", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(AddOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f B(5.0f, 6.0f, 7.0f, 8.0f);
    FVector4f C = A + B;
    if (C.X != 6.0f || C.Y != 8.0f || C.Z != 10.0f || C.W != 12.0f) { return -1; }
    FVector4f D = A + 1.0f;
    if (D.X != 2.0f || D.Y != 3.0f || D.Z != 4.0f || D.W != 5.0f) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("AddOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(SubtractOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(5.0f, 6.0f, 7.0f, 8.0f);
    FVector4f B(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f C = A - B;
    if (C.X != 4.0f || C.Y != 4.0f || C.Z != 4.0f || C.W != 4.0f) { return -1; }
    FVector4f D = A - 1.0f;
    if (D.X != 4.0f || D.Y != 5.0f || D.Z != 6.0f || D.W != 7.0f) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SubOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(MultiplyOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 2.0f, 3.0f, 4.0f);
    FVector4f B = A * 2.0f;
    if (B.X != 2.0f || B.Y != 4.0f || B.Z != 6.0f || B.W != 8.0f) { return -1; }
    FVector4f C = A * FVector4f(2.0f, 3.0f, 4.0f, 5.0f);
    if (C.X != 2.0f || C.Y != 6.0f || C.Z != 12.0f || C.W != 20.0f) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("MulOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(DivideOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(2.0f, 4.0f, 6.0f, 8.0f);
    FVector4f B = A / 2.0f;
    if (B.X != 1.0f || B.Y != 2.0f || B.Z != 3.0f || B.W != 4.0f) { return -1; }
    FVector4f C = A / FVector4f(2.0f, 4.0f, 2.0f, 4.0f);
    if (C.X != 1.0f || C.Y != 1.0f || C.Z != 3.0f || C.W != 2.0f) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DivOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(NegateOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, -2.0f, 3.0f, -4.0f);
    FVector4f B = -A;
    if (B.X != -1.0f || B.Y != 2.0f || B.Z != -3.0f || B.W != 4.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("NegOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(CrossOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 0.0f, 0.0f, 0.0f);
    FVector4f B(0.0f, 1.0f, 0.0f, 0.0f);
    FVector4f C = A ^ B;
    if (C.X != 0.0f || C.Y != 0.0f || C.Z != 1.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("CrossOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(CompoundAssign)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(2.0f, 4.0f, 6.0f, 8.0f);
    A += FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (A.X != 3.0f || A.Y != 5.0f || A.Z != 7.0f || A.W != 9.0f) { return -1; }
    A -= FVector4f(1.0f, 2.0f, 3.0f, 4.0f);
    if (A.X != 2.0f || A.Y != 3.0f || A.Z != 4.0f || A.W != 5.0f) { return -2; }
    A *= 2.0f;
    if (A.X != 4.0f || A.Y != 6.0f || A.Z != 8.0f || A.W != 10.0f) { return -3; }
    A /= 2.0f;
    if (A.X != 2.0f || A.Y != 3.0f || A.Z != 4.0f || A.W != 5.0f) { return -4; }
    A *= FVector4f(2.0f, 2.0f, 2.0f, 2.0f);
    if (A.X != 4.0f || A.Y != 6.0f || A.Z != 8.0f || A.W != 10.0f) { return -5; }
    A /= FVector4f(2.0f, 3.0f, 4.0f, 5.0f);
    if (A.X != 2.0f || A.Y != 2.0f || A.Z != 2.0f || A.W != 2.0f) { return -6; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("CompoundAssign", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IndexOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(1.0f, 2.0f, 3.0f, 4.0f);
    if (V[0] != 1.0f) { return -1; }
    if (V[1] != 2.0f) { return -2; }
    if (V[2] != 3.0f) { return -3; }
    if (V[3] != 4.0f) { return -4; }
    V[0] = 10.0f;
    if (V.X != 10.0f) { return -5; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IndexOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(SetMethod)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V;
    V.Set(5.0f, 6.0f, 7.0f, 8.0f);
    if (V.X != 5.0f || V.Y != 6.0f || V.Z != 7.0f || V.W != 8.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SetMethod", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ComponentMinMax)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f A(1.0f, 5.0f, 2.0f, 6.0f);
    FVector4f B(3.0f, 2.0f, 4.0f, 1.0f);
    FVector4f MinV = A.ComponentMin(B);
    if (MinV.X != 1.0f || MinV.Y != 2.0f || MinV.Z != 2.0f || MinV.W != 1.0f) { return -1; }
    FVector4f MaxV = A.ComponentMax(B);
    if (MaxV.X != 3.0f || MaxV.Y != 5.0f || MaxV.Z != 4.0f || MaxV.W != 6.0f) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ComponentMinMax", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(SizeLength)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(3.0f, 4.0f, 0.0f, 0.0f);
    if (!FMath::IsNearlyEqual(V.Size3(), 5.0f, 0.001f)) { return -1; }
    if (!FMath::IsNearlyEqual(V.SizeSquared3(), 25.0f, 0.001f)) { return -2; }
    FVector4f V2(0.0f, 0.0f, 3.0f, 4.0f);
    if (!FMath::IsNearlyEqual(V2.Size(), 5.0f, 0.001f)) { return -3; }
    if (!FMath::IsNearlyEqual(V2.SizeSquared(), 25.0f, 0.001f)) { return -4; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SizeLength", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Predicates)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f Z(0.0f, 0.0f, 0.0f, 0.0f);
    if (!Z.IsNearlyZero()) { return -1; }
    if (!Z.IsNearlyZero3()) { return -2; }
    FVector4f Unit(1.0f, 0.0f, 0.0f, 0.0f);
    if (!Unit.IsUnit3()) { return -3; }
    FVector4f N(0.0001f, 0.0f, 0.0f, 0.0f);
    if (!N.IsNearlyZero3(0.001f)) { return -4; }
    FVector4f V(1.0f, 2.0f, 3.0f, 4.0f);
    if (V.ContainsNaN()) { return -5; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Predicates", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Normalization)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(3.0f, 0.0f, 0.0f, 0.0f);
    FVector4f Safe = V.GetSafeNormal();
    if (!FMath::IsNearlyEqual(Safe.X, 1.0f, 0.001f)) { return -1; }
    FVector4f Zero(0.0f, 0.0f, 0.0f, 0.0f);
    FVector4f ZeroNorm = Zero.GetSafeNormal();
    if (ZeroNorm.X != 0.0f) { return -2; }
    FVector4f V2(5.0f, 0.0f, 0.0f, 0.0f);
    FVector4f UN = V2.GetUnsafeNormal3();
    if (!FMath::IsNearlyEqual(UN.X, 1.0f, 0.001f)) { return -3; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Normalize", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Reflect3)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(1.0f, -1.0f, 0.0f, 0.0f);
    FVector4f Normal(0.0f, 1.0f, 0.0f, 0.0f);
    FVector4f R = V.Reflect3(Normal);
    if (!FMath::IsNearlyEqual(R.X, -1.0f, 0.001f)) { return -1; }
    if (!FMath::IsNearlyEqual(R.Y, -1.0f, 0.001f)) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Reflect3", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(FindBestAxisVectors3)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f Normal(0.0f, 0.0f, 1.0f, 0.0f);
    FVector4f Axis1;
    FVector4f Axis2;
    Normal.FindBestAxisVectors3(Axis1, Axis2);
    if (FMath::IsNearlyEqual(Axis1.Size3(), 0.0f, 0.001f)) { return -1; }
    if (FMath::IsNearlyEqual(Axis2.Size3(), 0.0f, 0.001f)) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("FindBestAxisVectors3", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f V(1.0f, 2.0f, 3.0f, 4.0f);
    FString S = V.ToString();
    if (S.IsEmpty()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToString", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(StaticHelpers)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4f OOne = FVector4f::One();

    if (OOne.X != 1.0f || OOne.Y != 1.0f || OOne.Z != 1.0f || OOne.W != 1.0f) { return -2; }
	OOne.X = 3.2f;
	FVector4f ASD = OOne;

    FVector4f Zero = FVector4f::Zero();
    if (Zero.X != 0.0f || Zero.Y != 0.0f || Zero.Z != 0.0f || Zero.W != 0.0f) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("StaticHelpers", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
