// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Vector2D.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FVector2D UnrealBuildVector2D(double X, double Y)
	{
		return FVector2D(X, Y);
	}

	bool UnrealAcceptsVector2D(const FVector2D& Value)
	{
		return Value == FVector2D(3.0, 4.0);
	}

	bool RegisterFVector2DInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FVector2D UnrealBuildVector2D(double X, double Y)",
			asFUNCTION(UnrealBuildVector2D),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsVector2D(const FVector2D &in Value)",
			asFUNCTION(UnrealAcceptsVector2D),
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
	FOtterAngleScriptFVector2DTests,
	"OtterAngleScript.FVector2D",
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
		ASSERT_THAT(IsTrue(RegisterFVector2DInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptVector2DTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector2D")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V;
    if (V.X != 0.0 || V.Y != 0.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ScalarConstructor)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(5.0);
    if (V.X != 5.0 || V.Y != 5.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ScalarCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(XYConstructor)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(3.0, 4.0);
    if (V.X != 3.0 || V.Y != 4.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("XYCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ConstructFromFVector)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector Src(7.0, 8.0, 9.0);
    FVector2D V(Src);
    if (V.X != 7.0 || V.Y != 8.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("CtorFromFVector", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(1.0, 2.0);
    V.X = 10.0;
    V.Y = 20.0;
    if (V.X != 10.0 || V.Y != 20.0) { return -1; }
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
    FVector2D A(1.0, 2.0);
    FVector2D B;
    B = A;
    if (B.X != 1.0 || B.Y != 2.0) { return -1; }
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
    FVector2D A(3.0, 4.0);
    if (!UnrealAcceptsVector2D(A)) { return -1; }
    FVector2D B(3.0, 5.0);
    if (A == B) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("EqualsOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(AddOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D A(1.0, 2.0);
    FVector2D B(3.0, 4.0);
    FVector2D C = A + B;
    if (C.X != 4.0 || C.Y != 6.0) { return -1; }
    FVector2D D = A + 10.0;
    if (D.X != 11.0 || D.Y != 12.0) { return -2; }
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
    FVector2D A(5.0, 6.0);
    FVector2D B(2.0, 1.0);
    FVector2D C = A - B;
    if (C.X != 3.0 || C.Y != 5.0) { return -1; }
    FVector2D D = A - 1.0;
    if (D.X != 4.0 || D.Y != 5.0) { return -2; }
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
    FVector2D A(2.0, 3.0);
    FVector2D B = A * 2.0;
    if (B.X != 4.0 || B.Y != 6.0) { return -1; }
    FVector2D C = A * FVector2D(3.0, 4.0);
    if (C.X != 6.0 || C.Y != 12.0) { return -2; }
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
    FVector2D A(6.0, 8.0);
    FVector2D B = A / 2.0;
    if (B.X != 3.0 || B.Y != 4.0) { return -1; }
    FVector2D C = A / FVector2D(2.0, 4.0);
    if (C.X != 3.0 || C.Y != 2.0) { return -2; }
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
    FVector2D A(3.0, -4.0);
    FVector2D B = -A;
    if (B.X != -3.0 || B.Y != 4.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("NegOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(DotOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D A(1.0, 0.0);
    FVector2D B(0.0, 1.0);
    double Dot = A | B;
    if (Dot != 0.0) { return -1; }
    FVector2D C(2.0, 3.0);
    double Dot2 = C | C;
    if (Dot2 != 13.0) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DotOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(CrossOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D A(1.0, 0.0);
    FVector2D B(0.0, 1.0);
    double Cross = A ^ B;
    if (Cross != 1.0) { return -1; }
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
    FVector2D A(2.0, 4.0);
    A += FVector2D(1.0, 1.0);
    if (A.X != 3.0 || A.Y != 5.0) { return -1; }
    A -= FVector2D(1.0, 2.0);
    if (A.X != 2.0 || A.Y != 3.0) { return -2; }
    A *= 2.0;
    if (A.X != 4.0 || A.Y != 6.0) { return -3; }
    A /= 2.0;
    if (A.X != 2.0 || A.Y != 3.0) { return -4; }
    A *= FVector2D(3.0, 2.0);
    if (A.X != 6.0 || A.Y != 6.0) { return -5; }
    A /= FVector2D(2.0, 3.0);
    if (A.X != 3.0 || A.Y != 2.0) { return -6; }
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
    FVector2D V(7.0, 8.0);
    if (V[0] != 7.0) { return -1; }
    if (V[1] != 8.0) { return -2; }
    V[0] = 10.0;
    if (V.X != 10.0) { return -3; }
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
    FVector2D V;
    V.Set(5.0, 6.0);
    if (V.X != 5.0 || V.Y != 6.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SetMethod", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(SizeLength)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(3.0, 4.0);
    if (!FMath::IsNearlyEqual(V.Size(), 5.0, 0.0001)) { return -1; }
    if (!FMath::IsNearlyEqual(V.Length(), 5.0, 0.0001)) { return -2; }
    if (!FMath::IsNearlyEqual(V.SizeSquared(), 25.0, 0.0001)) { return -3; }
    if (!FMath::IsNearlyEqual(V.SquaredLength(), 25.0, 0.0001)) { return -4; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SizeLength", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Normalization)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(3.0, 0.0);
    bool bNormalized = V.Normalize();
    if (!bNormalized) { return -1; }
    if (!FMath::IsNearlyEqual(V.X, 1.0, 0.0001)) { return -2; }
    FVector2D W(4.0, 0.0);
    FVector2D Safe = W.GetSafeNormal();
    if (!FMath::IsNearlyEqual(Safe.X, 1.0, 0.0001)) { return -3; }
    FVector2D Zero(0.0, 0.0);
    FVector2D ZeroNorm = Zero.GetSafeNormal();
    if (ZeroNorm.X != 0.0 || ZeroNorm.Y != 0.0) { return -4; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Normalize", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Predicates)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D Z(0.0, 0.0);
    if (!Z.IsZero()) { return -1; }
    if (!Z.IsNearlyZero()) { return -2; }
    FVector2D N(0.0001, 0.0);
    if (!N.IsNearlyZero(0.001)) { return -3; }
    FVector2D V(1.0, 2.0);
    if (!V.Equals(FVector2D(1.0, 2.0))) { return -4; }
    if (!V.ContainsNaN() == false) { return -5; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Predicates", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ComponentwiseComparisons)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D A(1.0, 2.0);
    FVector2D B(3.0, 4.0);
    if (!A.ComponentwiseAllLessThan(B)) { return -1; }
    if (!B.ComponentwiseAllGreaterThan(A)) { return -2; }
    if (!A.ComponentwiseAllLessOrEqual(A)) { return -3; }
    if (!A.ComponentwiseAllGreaterOrEqual(A)) { return -4; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ComponentwiseCmp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToDirectionAndLength)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(3.0, 0.0);
    FVector2D Dir;
    double Len = 0.0;
    V.ToDirectionAndLength(Dir, Len);
    if (!FMath::IsNearlyEqual(Len, 3.0, 0.0001)) { return -1; }
    if (!FMath::IsNearlyEqual(Dir.X, 1.0, 0.0001)) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToDirectionAndLength", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Clamping)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(10.0, -10.0);
    FVector2D Clamped = V.ClampAxes(-5.0, 5.0);
    if (Clamped.X != 5.0 || Clamped.Y != -5.0) { return -1; }
    FVector2D Big(6.0, 8.0);
    FVector2D ClampedSize = Big.GetClampedToSize(0.0, 5.0);
    if (!FMath::IsNearlyEqual(ClampedSize.Size(), 5.0, 0.001)) { return -2; }
    FVector2D MaxClamped = Big.GetClampedToMaxSize(5.0);
    if (!FMath::IsNearlyEqual(MaxClamped.Size(), 5.0, 0.001)) { return -3; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Clamping", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(RoundToVector)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(1.7, 2.3);
    FVector2D R = V.RoundToVector();
    if (R.X != 2.0 || R.Y != 2.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("RoundToVector", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector2D V(1.0, 2.0);
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
    FVector2D A(1.0, 0.0);
    FVector2D B(0.0, 1.0);
    double Dot = FVector2D::DotProduct(A, B);
    if (Dot != 0.0) { return -1; }
    double Cross = FVector2D::CrossProduct(A, B);
    if (Cross != 1.0) { return -2; }
    double Dist = FVector2D::Distance(A, B);
    if (!FMath::IsNearlyEqual(Dist, 1.4142135, 0.0001)) { return -3; }
    FVector2D MaxVec = FVector2D::Max(A, B);
    if (MaxVec.X != 1.0 || MaxVec.Y != 1.0) { return -4; }
    FVector2D Zero = FVector2D::Zero();
    if (Zero.X != 0.0 || Zero.Y != 0.0) { return -5; }
    FVector2D One = FVector2D::One();
    if (One.X != 1.0 || One.Y != 1.0) { return -6; }
    FVector2D UX = FVector2D::UnitX();
    if (UX.X != 1.0 || UX.Y != 0.0) { return -7; }
    FVector2D UY = FVector2D::UnitY();
    if (UY.X != 0.0 || UY.Y != 1.0) { return -8; }
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
