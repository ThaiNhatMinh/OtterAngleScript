// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Vector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FVector UnrealBuildVector(double X, double Y, double Z)
	{
		return FVector(X, Y, Z);
	}

	bool UnrealAcceptsVector(const FVector& Value)
	{
		return Value == FVector(11.0, 12.0, 13.0);
	}

	double UnrealSumVector(const FVector& Value)
	{
		return Value.X + Value.Y + Value.Z;
	}

	FVector UnrealVectorWithNaN()
	{
		return FVector(std::numeric_limits<double>::quiet_NaN(), 1.0, 2.0);
	}

	bool RegisterFVectorInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FVector UnrealBuildVector(double X, double Y, double Z)",
			asFUNCTION(UnrealBuildVector),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsVector(const FVector &in Value)",
			asFUNCTION(UnrealAcceptsVector),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealSumVector(const FVector &in Value)",
			asFUNCTION(UnrealSumVector),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FVector UnrealVectorWithNaN()",
			asFUNCTION(UnrealVectorWithNaN),
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
	FOtterAngleScriptFVectorTests,
	"OtterAngleScript.FVector",
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
		ASSERT_THAT(IsTrue(RegisterFVectorInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptVectorTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypesRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector2D")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FQuat")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FPlane")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FText")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FVector Value;
    if (!FMath::IsNearlyEqual(Value.X, 0.0, 0.0001) || !FMath::IsNearlyEqual(Value.Y, 0.0, 0.0001) || !FMath::IsNearlyEqual(Value.Z, 0.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ScalarConstructor)
	{
		static const char Script[] = R"(
int RunScalarConstructor()
{
    FVector Scalar(2.0);
    if (Scalar.X != 2.0 || Scalar.Y != 2.0 || Scalar.Z != 2.0)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorScalarConstructor", Script, "int RunScalarConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(XYZConstructor)
	{
		static const char Script[] = R"(
int RunXYZConstructor()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!Value.opEquals(UnrealBuildVector(1.0, 2.0, 3.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorXYZConstructor", Script, "int RunXYZConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Vector2DConstructor)
	{
		static const char Script[] = R"(
int RunVector2DConstructor()
{
    FVector2D Base2D(4.0, 5.0);
    if (!Base2D.Equals(FVector2D(4.0, 5.0), 0.0))
    {
        return -1;
    }

    FVector From2D(Base2D, 6.0);
    if (!From2D.opEquals(FVector(4.0, 5.0, 6.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorVector2DConstructor", Script, "int RunVector2DConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FVector Value(1.0, 2.0, 3.0);
    FVector Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AssignmentOperator)
	{
		static const char Script[] = R"(
int RunAssignmentOperator()
{
    FVector Source(1.0, 2.0, 3.0);
    FVector Dest;
    Dest = Source;
    if (!Dest.opEquals(Source))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorAssignmentOperator", Script, "int RunAssignmentOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(EqualityOperator)
	{
		static const char Script[] = R"(
int RunEqualityOperator()
{
    FVector A(1.0, 2.0, 3.0);
    FVector B(1.0, 2.0, 3.0);
    FVector C(3.0, 2.0, 1.0);
    if (!A.opEquals(B) || A.opEquals(C))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorEqualityOperator", Script, "int RunEqualityOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CrossOperator)
	{
		static const char Script[] = R"(
int RunCrossOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!(Value ^ FVector(0.0, 0.0, 1.0)).opEquals(FVector(2.0, -1.0, 0.0)))
    {
        return -1;
    }

    if (!Value.Cross(FVector(0.0, 0.0, 1.0)).opEquals(FVector(2.0, -1.0, 0.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorCrossOperator", Script, "int RunCrossOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(DotOperator)
	{
		static const char Script[] = R"(
int RunDotOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!FMath::IsNearlyEqual(Value | FVector(2.0, 0.0, 1.0), 5.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(Value.Dot(FVector(2.0, 0.0, 1.0)), 5.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorDotOperator", Script, "int RunDotOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddOperator)
	{
		static const char Script[] = R"(
int RunAddOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!(Value + FVector(4.0, 5.0, 6.0)).opEquals(FVector(5.0, 7.0, 9.0)))
    {
        return -1;
    }

    if (!(Value + 1.0).opEquals(FVector(2.0, 3.0, 4.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorAddOperator", Script, "int RunAddOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SubtractOperator)
	{
		static const char Script[] = R"(
int RunSubtractOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!(Value - FVector(1.0, 1.0, 1.0)).opEquals(FVector(0.0, 1.0, 2.0)))
    {
        return -1;
    }

    if (!(Value - 1.0).opEquals(FVector(0.0, 1.0, 2.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSubtractOperator", Script, "int RunSubtractOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MultiplyOperator)
	{
		static const char Script[] = R"(
int RunMultiplyOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!(Value * 2.0).opEquals(FVector(2.0, 4.0, 6.0)))
    {
        return -1;
    }

    if (!(Value * FVector(2.0, 3.0, 4.0)).opEquals(FVector(2.0, 6.0, 12.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorMultiplyOperator", Script, "int RunMultiplyOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(DivideOperator)
	{
		static const char Script[] = R"(
int RunDivideOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!((Value * 2.0) / 2.0).opEquals(Value))
    {
        return -1;
    }

    if (!((Value * FVector(2.0, 3.0, 4.0)) / FVector(2.0, 3.0, 4.0)).opEquals(Value))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorDivideOperator", Script, "int RunDivideOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(NegateOperator)
	{
		static const char Script[] = R"(
int RunNegateOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!(-Value).opEquals(FVector(-1.0, -2.0, -3.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorNegateOperator", Script, "int RunNegateOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CompoundAssignmentOperators)
	{
		static const char Script[] = R"(
int RunCompoundAssignmentOperators()
{
    FVector Mutating(1.0, 2.0, 3.0);
    Mutating += FVector(1.0, 1.0, 1.0);
    Mutating -= FVector(1.0, 0.0, 1.0);
    Mutating *= 2.0;
    Mutating /= 2.0;
    Mutating *= FVector(2.0, 1.0, 0.5);
    Mutating /= FVector(2.0, 1.0, 0.5);
    if (!Mutating.opEquals(FVector(1.0, 3.0, 3.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorCompoundAssignmentOperators", Script, "int RunCompoundAssignmentOperators()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IndexOperator)
	{
		static const char Script[] = R"(
int RunIndexOperator()
{
    FVector Value(1.0, 2.0, 3.0);
    Value[0] = 4.0;
    if (!FMath::IsNearlyEqual(Value[0], 4.0, 0.0001))
    {
        return -1;
    }

    const FVector ConstValue(Value);
    if (!FMath::IsNearlyEqual(ConstValue[0], 4.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIndexOperator", Script, "int RunIndexOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ComponentMethod)
	{
		static const char Script[] = R"(
int RunComponentMethod()
{
    FVector Value(1.0, 2.0, 3.0);
    Value.Component(1) = 7.0;
    if (!FMath::IsNearlyEqual(Value.Component(1), 7.0, 0.0001))
    {
        return -1;
    }

    const FVector ConstValue(Value);
    if (!FMath::IsNearlyEqual(ConstValue.Component(1), 7.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorComponentMethod", Script, "int RunComponentMethod()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsValidIndex)
	{
		static const char Script[] = R"(
int RunIsValidIndex()
{
    FVector Value(1.0, 2.0, 3.0);
    if (!Value.IsValidIndex(2) || Value.IsValidIndex(3))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIsValidIndex", Script, "int RunIsValidIndex()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetComponentForAxis)
	{
		static const char Script[] = R"(
int RunGetComponentForAxis()
{
    FVector Value(4.0, 8.0, 6.0);
    if (!FMath::IsNearlyEqual(Value.GetComponentForAxis(Y), 8.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetComponentForAxis", Script, "int RunGetComponentForAxis()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SetComponentForAxis)
	{
		static const char Script[] = R"(
int RunSetComponentForAxis()
{
    FVector Value(1.0, 2.0, 3.0);
    Value.SetComponentForAxis(Y, 8.0);
    if (!FMath::IsNearlyEqual(Value.GetComponentForAxis(Y), 8.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSetComponentForAxis", Script, "int RunSetComponentForAxis()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Set)
	{
		static const char Script[] = R"(
int RunSet()
{
    FVector Value(1.0, 2.0, 3.0);
    Value.Set(9.0, 10.0, 11.0);
    if (!Value.Equals(FVector(9.0, 10.0, 11.0), 0.0))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSet", Script, "int RunSet()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AllComponentsEqual)
	{
		static const char Script[] = R"(
int RunAllComponentsEqual()
{
    if (!FVector(3.0, 3.0002, 3.0001).AllComponentsEqual(0.001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorAllComponentsEqual", Script, "int RunAllComponentsEqual()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetMax)
	{
		static const char Script[] = R"(
int RunGetMax()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!FMath::IsNearlyEqual(Mixed.GetMax(), 5.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetMax", Script, "int RunGetMax()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetAbsMax)
	{
		static const char Script[] = R"(
int RunGetAbsMax()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!FMath::IsNearlyEqual(Mixed.GetAbsMax(), 5.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetAbsMax", Script, "int RunGetAbsMax()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetMin)
	{
		static const char Script[] = R"(
int RunGetMin()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!FMath::IsNearlyEqual(Mixed.GetMin(), -4.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetMin", Script, "int RunGetMin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetAbsMin)
	{
		static const char Script[] = R"(
int RunGetAbsMin()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!FMath::IsNearlyEqual(Mixed.GetAbsMin(), 2.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetAbsMin", Script, "int RunGetAbsMin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ComponentMin)
	{
		static const char Script[] = R"(
int RunComponentMin()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!Mixed.ComponentMin(FVector(-3.0, 1.0, -5.0)).opEquals(FVector(-3.0, 1.0, -5.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorComponentMin", Script, "int RunComponentMin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ComponentMax)
	{
		static const char Script[] = R"(
int RunComponentMax()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!Mixed.ComponentMax(FVector(-3.0, 1.0, -5.0)).opEquals(FVector(-2.0, 5.0, -4.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorComponentMax", Script, "int RunComponentMax()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetAbs)
	{
		static const char Script[] = R"(
int RunGetAbs()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!Mixed.GetAbs().opEquals(FVector(2.0, 5.0, 4.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetAbs", Script, "int RunGetAbs()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SizeAndLength)
	{
		static const char Script[] = R"(
int RunSizeAndLength()
{
    FVector LengthVector(3.0, 4.0, 12.0);
    if (!FMath::IsNearlyEqual(LengthVector.Size(), 13.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(LengthVector.Length(), 13.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSizeAndLength", Script, "int RunSizeAndLength()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SizeSquaredAndSquaredLength)
	{
		static const char Script[] = R"(
int RunSizeSquaredAndSquaredLength()
{
    FVector LengthVector(3.0, 4.0, 12.0);
    if (!FMath::IsNearlyEqual(LengthVector.SizeSquared(), 169.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(LengthVector.SquaredLength(), 169.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSizeSquaredAndSquaredLength", Script, "int RunSizeSquaredAndSquaredLength()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Size2D)
	{
		static const char Script[] = R"(
int RunSize2D()
{
    FVector LengthVector(3.0, 4.0, 12.0);
    if (!FMath::IsNearlyEqual(LengthVector.Size2D(), 5.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSize2D", Script, "int RunSize2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SizeSquared2D)
	{
		static const char Script[] = R"(
int RunSizeSquared2D()
{
    FVector LengthVector(3.0, 4.0, 12.0);
    if (!FMath::IsNearlyEqual(LengthVector.SizeSquared2D(), 25.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorSizeSquared2D", Script, "int RunSizeSquared2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsNearlyZero)
	{
		static const char Script[] = R"(
int RunIsNearlyZero()
{
    if (!FVector().IsNearlyZero())
    {
        return -1;
    }

    if (!FVector(0.0, 0.0, 0.00001).IsNearlyZero(0.001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIsNearlyZero", Script, "int RunIsNearlyZero()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsZero)
	{
		static const char Script[] = R"(
int RunIsZero()
{
    if (!FVector().IsZero())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIsZero", Script, "int RunIsZero()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsUnit)
	{
		static const char Script[] = R"(
int RunIsUnit()
{
    if (!FVector(1.0, 0.0, 0.0).IsUnit())
    {
        return -1;
    }

    if (!FVector(1.0, 0.0, 0.0).IsUnit(0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIsUnit", Script, "int RunIsUnit()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsNormalized)
	{
		static const char Script[] = R"(
int RunIsNormalized()
{
    if (!FVector(1.0, 0.0, 0.0).IsNormalized())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIsNormalized", Script, "int RunIsNormalized()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Normalize)
	{
		static const char Script[] = R"(
int RunNormalize()
{
    FVector Normalized(3.0, 0.0, 4.0);
    if (!Normalized.Normalize())
    {
        return -1;
    }

    if (!Normalized.Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -2;
    }

    FVector Tiny(0.0, 0.0, 0.0);
    if (Tiny.Normalize(0.1))
    {
        return -3;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorNormalize", Script, "int RunNormalize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetUnsafeNormal)
	{
		static const char Script[] = R"(
int RunGetUnsafeNormal()
{
    if (!FVector(3.0, 0.0, 4.0).GetUnsafeNormal().Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetUnsafeNormal", Script, "int RunGetUnsafeNormal()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetSafeNormal)
	{
		static const char Script[] = R"(
int RunGetSafeNormal()
{
    if (!FVector(3.0, 0.0, 4.0).GetSafeNormal().Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -1;
    }

    if (!FVector(3.0, 0.0, 4.0).GetSafeNormal(0.1).Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -2;
    }

    if (!FVector().GetSafeNormal(0.1, FVector(9.0, 8.0, 7.0)).opEquals(FVector(9.0, 8.0, 7.0)))
    {
        return -3;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetSafeNormal", Script, "int RunGetSafeNormal()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetSafeNormal2D)
	{
		static const char Script[] = R"(
int RunGetSafeNormal2D()
{
    if (!FVector(3.0, 4.0, 9.0).GetSafeNormal2D().Equals(FVector(0.6, 0.8, 0.0), 0.0001))
    {
        return -1;
    }

    if (!FVector(3.0, 4.0, 9.0).GetSafeNormal2D(0.1).Equals(FVector(0.6, 0.8, 0.0), 0.0001))
    {
        return -2;
    }

    if (!FVector().GetSafeNormal2D(0.1, FVector(1.0, 2.0, 3.0)).opEquals(FVector(1.0, 2.0, 3.0)))
    {
        return -3;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetSafeNormal2D", Script, "int RunGetSafeNormal2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToDirectionAndLength)
	{
		static const char Script[] = R"(
int RunToDirectionAndLength()
{
    FVector OutDir;
    double OutLength = 0.0;
    FVector(3.0, 4.0, 0.0).ToDirectionAndLength(OutDir, OutLength);
    if (!OutDir.Equals(FVector(0.6, 0.8, 0.0), 0.0001) || !FMath::IsNearlyEqual(OutLength, 5.0, 0.0001))
    {
        return -1;
    }

    float OutLengthFloat = 0.0f;
    FVector(0.0, 3.0, 4.0).ToDirectionAndLength(OutDir, OutLengthFloat);
    if (!OutDir.Equals(FVector(0.0, 0.6, 0.8), 0.0001) || !FMath::IsNearlyEqual(OutLengthFloat, 5.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToDirectionAndLength", Script, "int RunToDirectionAndLength()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetSignVector)
	{
		static const char Script[] = R"(
int RunGetSignVector()
{
    if (!FVector(-1.0, 0.0, 2.0).GetSignVector().opEquals(FVector(-1.0, 1.0, 1.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetSignVector", Script, "int RunGetSignVector()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Projection)
	{
		static const char Script[] = R"(
int RunProjection()
{
    if (!FVector(4.0, 6.0, 2.0).Projection().Equals(FVector(2.0, 3.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorProjection", Script, "int RunProjection()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetUnsafeNormal2D)
	{
		static const char Script[] = R"(
int RunGetUnsafeNormal2D()
{
    if (!FVector(3.0, 4.0, 9.0).GetUnsafeNormal2D().Equals(FVector(0.6, 0.8, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetUnsafeNormal2D", Script, "int RunGetUnsafeNormal2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GridSnap)
	{
		static const char Script[] = R"(
int RunGridSnap()
{
    if (!FVector(5.2, 7.8, 9.1).GridSnap(2.0).opEquals(FVector(6.0, 8.0, 10.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGridSnap", Script, "int RunGridSnap()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(BoundToCube)
	{
		static const char Script[] = R"(
int RunBoundToCube()
{
    if (!FVector(10.0, -10.0, 1.0).BoundToCube(3.0).opEquals(FVector(3.0, -3.0, 1.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorBoundToCube", Script, "int RunBoundToCube()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(BoundToBox)
	{
		static const char Script[] = R"(
int RunBoundToBox()
{
    if (!FVector(10.0, -10.0, 1.0).BoundToBox(FVector(-1.0, -2.0, -3.0), FVector(1.0, 2.0, 3.0)).opEquals(FVector(1.0, -2.0, 1.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorBoundToBox", Script, "int RunBoundToBox()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetClampedToSize)
	{
		static const char Script[] = R"(
int RunGetClampedToSize()
{
    if (!FVector(10.0, 0.0, 0.0).GetClampedToSize(2.0, 4.0).Equals(FVector(4.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetClampedToSize", Script, "int RunGetClampedToSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetClampedToSize2D)
	{
		static const char Script[] = R"(
int RunGetClampedToSize2D()
{
    if (!FVector(3.0, 4.0, 7.0).GetClampedToSize2D(2.0, 4.0).Equals(FVector(2.4, 3.2, 7.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetClampedToSize2D", Script, "int RunGetClampedToSize2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetClampedToMaxSize)
	{
		static const char Script[] = R"(
int RunGetClampedToMaxSize()
{
    if (!FVector(10.0, 0.0, 0.0).GetClampedToMaxSize(4.0).Equals(FVector(4.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetClampedToMaxSize", Script, "int RunGetClampedToMaxSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetClampedToMaxSize2D)
	{
		static const char Script[] = R"(
int RunGetClampedToMaxSize2D()
{
    if (!FVector(6.0, 8.0, 9.0).GetClampedToMaxSize2D(5.0).Equals(FVector(3.0, 4.0, 9.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorGetClampedToMaxSize2D", Script, "int RunGetClampedToMaxSize2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddBounded)
	{
		static const char Script[] = R"(
int RunAddBounded()
{
    FVector AddBoundedDefaultValue(1.0, 2.0, 3.0);
    AddBoundedDefaultValue.AddBounded(FVector(1.0, 1.0, 1.0));
    if (!AddBoundedDefaultValue.opEquals(FVector(2.0, 3.0, 4.0)))
    {
        return -1;
    }

    FVector AddBoundedRadiusValue(1.0, 1.0, 1.0);
    AddBoundedRadiusValue.AddBounded(FVector(10.0, 0.0, 0.0), 3.0);
    if (AddBoundedRadiusValue.Size() > 3.0001)
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorAddBounded", Script, "int RunAddBounded()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Reciprocal)
	{
		static const char Script[] = R"(
int RunReciprocal()
{
    if (!FVector(2.0, 4.0, -4.0).Reciprocal().opEquals(FVector(0.5, 0.25, -0.25)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorReciprocal", Script, "int RunReciprocal()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsUniform)
	{
		static const char Script[] = R"(
int RunIsUniform()
{
    if (!FVector(5.0, 5.0, 5.0).IsUniform())
    {
        return -1;
    }

    if (!FVector(5.0, 5.0, 5.0002).IsUniform(0.001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorIsUniform", Script, "int RunIsUniform()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MirrorByVector)
	{
		static const char Script[] = R"(
int RunMirrorByVector()
{
    if (!FVector(1.0, -2.0, 0.0).MirrorByVector(FVector(0.0, 1.0, 0.0)).opEquals(FVector(1.0, 2.0, 0.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorMirrorByVector", Script, "int RunMirrorByVector()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MirrorByPlane)
	{
		static const char Script[] = R"(
int RunMirrorByPlane()
{
    if (!FVector(1.0, 2.0, 3.0).MirrorByPlane(FPlane(0.0, 0.0, 1.0, 0.0)).Equals(FVector(1.0, 2.0, -3.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorMirrorByPlane", Script, "int RunMirrorByPlane()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RotateAngleAxis)
	{
		static const char Script[] = R"(
int RunRotateAngleAxis()
{
    if (!FVector(1.0, 0.0, 0.0).RotateAngleAxis(90.0, FVector(0.0, 0.0, 1.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorRotateAngleAxis", Script, "int RunRotateAngleAxis()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RotateAngleAxisRad)
	{
		static const char Script[] = R"(
int RunRotateAngleAxisRad()
{
    if (!FVector(1.0, 0.0, 0.0).RotateAngleAxisRad(1.5707963267948966, FVector(0.0, 0.0, 1.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorRotateAngleAxisRad", Script, "int RunRotateAngleAxisRad()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CosineAngle2D)
	{
		static const char Script[] = R"(
int RunCosineAngle2D()
{
    if (!FMath::IsNearlyEqual(FVector(1.0, 0.0, 5.0).CosineAngle2D(FVector(0.0, 2.0, 7.0)), 0.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorCosineAngle2D", Script, "int RunCosineAngle2D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ProjectOnTo)
	{
		static const char Script[] = R"(
int RunProjectOnTo()
{
    if (!FVector(3.0, 4.0, 0.0).ProjectOnTo(FVector(1.0, 0.0, 0.0)).Equals(FVector(3.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorProjectOnTo", Script, "int RunProjectOnTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ProjectOnToNormal)
	{
		static const char Script[] = R"(
int RunProjectOnToNormal()
{
    if (!FVector(3.0, 4.0, 0.0).ProjectOnToNormal(FVector(0.0, 1.0, 0.0)).Equals(FVector(0.0, 4.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorProjectOnToNormal", Script, "int RunProjectOnToNormal()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToOrientationRotator)
	{
		static const char Script[] = R"(
int RunToOrientationRotator()
{
    FRotator Orientation = FVector(1.0, 0.0, 0.0).ToOrientationRotator();
    if (!Orientation.Equals(FRotator(0.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToOrientationRotator", Script, "int RunToOrientationRotator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Rotation)
	{
		static const char Script[] = R"(
int RunRotation()
{
    if (!FVector(1.0, 0.0, 0.0).Rotation().Equals(FRotator(0.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorRotation", Script, "int RunRotation()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToOrientationQuat)
	{
		static const char Script[] = R"(
int RunToOrientationQuat()
{
    if (!FVector(1.0, 0.0, 0.0).ToOrientationQuat().Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToOrientationQuat", Script, "int RunToOrientationQuat()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FindBestAxisVectors)
	{
		static const char Script[] = R"(
int RunFindBestAxisVectors()
{
    FVector Axis1;
    FVector Axis2;
    FVector(0.0, 0.0, 1.0).FindBestAxisVectors(Axis1, Axis2);
    if (!FMath::IsNearlyEqual(Axis1 | FVector(0.0, 0.0, 1.0), 0.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(Axis2 | FVector(0.0, 0.0, 1.0), 0.0, 0.0001))
    {
        return -2;
    }

    if (!FMath::IsNearlyEqual(Axis1 | Axis2, 0.0, 0.0001))
    {
        return -3;
    }

    if (!FMath::IsNearlyEqual(Axis1.Size(), 1.0, 0.0001) || !FMath::IsNearlyEqual(Axis2.Size(), 1.0, 0.0001))
    {
        return -4;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorFindBestAxisVectors", Script, "int RunFindBestAxisVectors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(UnwindEuler)
	{
		static const char Script[] = R"(
int RunUnwindEuler()
{
    FVector Euler(270.0, -190.0, 540.0);
    Euler.UnwindEuler();
    if (Euler.X < -180.0 || Euler.X > 180.0 || Euler.Y < -180.0 || Euler.Y > 180.0 || Euler.Z < -180.0 || Euler.Z > 180.0)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorUnwindEuler", Script, "int RunUnwindEuler()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ContainsNaN)
	{
		static const char Script[] = R"(
int RunContainsNaN()
{
    if (!UnrealVectorWithNaN().ContainsNaN())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorContainsNaN", Script, "int RunContainsNaN()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FString FullText = FVector(1.0, 2.0, 3.0).ToString();
    if (!FullText.StartsWith("X="))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToText)
	{
		static const char Script[] = R"(
int RunToText()
{
    FText Localized = FVector(1.0, 2.0, 3.0).ToText();
    if (Localized.ToString().IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToText", Script, "int RunToText()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToCompactString)
	{
		static const char Script[] = R"(
int RunToCompactString()
{
    FString Compact = FVector(1.0, 2.0, 3.0).ToCompactString();
    if (Compact.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToCompactString", Script, "int RunToCompactString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToCompactText)
	{
		static const char Script[] = R"(
int RunToCompactText()
{
    FText CompactText = FVector(1.0, 2.0, 3.0).ToCompactText();
    if (CompactText.ToString().IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorToCompactText", Script, "int RunToCompactText()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InitFromString)
	{
		static const char Script[] = R"(
int RunInitFromString()
{
    FVector Parsed;
    if (!Parsed.InitFromString("X=1.5 Y=2.5 Z=3.5"))
    {
        return -1;
    }

    if (!Parsed.Equals(FVector(1.5, 2.5, 3.5), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorInitFromString", Script, "int RunInitFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InitFromCompactString)
	{
		static const char Script[] = R"(
int RunInitFromCompactString()
{
    FString Compact = FVector(1.0, 2.0, 3.0).ToCompactString();
    FVector ParsedCompact;
    if (!ParsedCompact.InitFromCompactString(Compact))
    {
        return -1;
    }

    if (!ParsedCompact.Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorInitFromCompactString", Script, "int RunInitFromCompactString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(UnitCartesianToSpherical)
	{
		static const char Script[] = R"(
int RunUnitCartesianToSpherical()
{
    FVector2D Spherical = FVector(1.0, 0.0, 0.0).UnitCartesianToSpherical();
    if (!FMath::IsNearlyEqual(Spherical.X, 1.5707963267948966, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(Spherical.Y, 0.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorUnitCartesianToSpherical", Script, "int RunUnitCartesianToSpherical()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(HeadingAngle)
	{
		static const char Script[] = R"(
int RunHeadingAngle()
{
    if (!FMath::IsNearlyEqual(FVector(0.0, 1.0, 0.0).HeadingAngle(), 1.5707963267948966, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorHeadingAngle", Script, "int RunHeadingAngle()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticCrossProduct)
	{
		static const char Script[] = R"(
int RunStaticCrossProduct()
{
    if (!FVector::CrossProduct(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 1.0)).opEquals(FVector(2.0, -1.0, 0.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticCrossProduct", Script, "int RunStaticCrossProduct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticDotProduct)
	{
		static const char Script[] = R"(
int RunStaticDotProduct()
{
    if (!FMath::IsNearlyEqual(FVector::DotProduct(FVector(1.0, 2.0, 3.0), FVector(2.0, 0.0, 1.0)), 5.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticDotProduct", Script, "int RunStaticDotProduct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticSlerpVectorToDirection)
	{
		static const char Script[] = R"(
int RunStaticSlerpVectorToDirection()
{
    if (!FVector::SlerpVectorToDirection(FVector(2.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0), 0.5).Equals(FVector(1.41421356237, 1.41421356237, 0.0), 0.001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticSlerpVectorToDirection", Script, "int RunStaticSlerpVectorToDirection()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticSlerpNormals)
	{
		static const char Script[] = R"(
int RunStaticSlerpNormals()
{
    if (!FVector::SlerpNormals(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0), 0.5).Equals(FVector(0.70710678118, 0.70710678118, 0.0), 0.001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticSlerpNormals", Script, "int RunStaticSlerpNormals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticCreateOrthonormalBasis)
	{
		static const char Script[] = R"(
int RunStaticCreateOrthonormalBasis()
{
    FVector XAxis(1.0, 0.0, 0.0);
    FVector YAxis(0.0, 2.0, 0.0);
    FVector ZAxis(0.0, 0.0, 3.0);
    FVector::CreateOrthonormalBasis(XAxis, YAxis, ZAxis);
    if (!FMath::IsNearlyEqual(XAxis.Size(), 1.0, 0.0001) || !FMath::IsNearlyEqual(YAxis.Size(), 1.0, 0.0001) || !FMath::IsNearlyEqual(ZAxis.Size(), 1.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticCreateOrthonormalBasis", Script, "int RunStaticCreateOrthonormalBasis()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticPointsAreSame)
	{
		static const char Script[] = R"(
int RunStaticPointsAreSame()
{
    if (!FVector::PointsAreSame(FVector(1.0, 1.0, 1.0), FVector(1.0, 1.0, 1.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticPointsAreSame", Script, "int RunStaticPointsAreSame()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticPointsAreNear)
	{
		static const char Script[] = R"(
int RunStaticPointsAreNear()
{
    if (!FVector::PointsAreNear(FVector(1.0, 1.0, 1.0), FVector(1.05, 1.0, 1.0), 0.1))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticPointsAreNear", Script, "int RunStaticPointsAreNear()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticPointPlaneDist)
	{
		static const char Script[] = R"(
int RunStaticPointPlaneDist()
{
    if (!FMath::IsNearlyEqual(FVector::PointPlaneDist(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0)), 3.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticPointPlaneDist", Script, "int RunStaticPointPlaneDist()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticPointPlaneProject)
	{
		static const char Script[] = R"(
int RunStaticPointPlaneProject()
{
    if (!FVector::PointPlaneProject(FVector(1.0, 2.0, 3.0), FPlane(0.0, 0.0, 1.0, 0.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -1;
    }

    if (!FVector::PointPlaneProject(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 0.0), FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -2;
    }

    if (!FVector::PointPlaneProject(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -3;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticPointPlaneProject", Script, "int RunStaticPointPlaneProject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticVectorPlaneProject)
	{
		static const char Script[] = R"(
int RunStaticVectorPlaneProject()
{
    if (!FVector::VectorPlaneProject(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 1.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticVectorPlaneProject", Script, "int RunStaticVectorPlaneProject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticDist)
	{
		static const char Script[] = R"(
int RunStaticDist()
{
    if (!FMath::IsNearlyEqual(FVector::Dist(FVector(0.0, 0.0, 0.0), FVector(3.0, 4.0, 0.0)), 5.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(FVector::Distance(FVector(0.0, 0.0, 0.0), FVector(3.0, 4.0, 0.0)), 5.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticDist", Script, "int RunStaticDist()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticDistXY)
	{
		static const char Script[] = R"(
int RunStaticDistXY()
{
    if (!FMath::IsNearlyEqual(FVector::DistXY(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 5.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(FVector::Dist2D(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 5.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticDistXY", Script, "int RunStaticDistXY()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticDistSquared)
	{
		static const char Script[] = R"(
int RunStaticDistSquared()
{
    if (!FMath::IsNearlyEqual(FVector::DistSquared(FVector(0.0, 0.0, 0.0), FVector(3.0, 4.0, 12.0)), 169.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticDistSquared", Script, "int RunStaticDistSquared()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticDistSquaredXY)
	{
		static const char Script[] = R"(
int RunStaticDistSquaredXY()
{
    if (!FMath::IsNearlyEqual(FVector::DistSquaredXY(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 25.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(FVector::DistSquared2D(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 25.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticDistSquaredXY", Script, "int RunStaticDistSquaredXY()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticBoxPushOut)
	{
		static const char Script[] = R"(
int RunStaticBoxPushOut()
{
    if (!FMath::IsNearlyEqual(FVector::BoxPushOut(FVector(0.0, 0.0, 1.0), FVector(1.0, 2.0, 3.0)), 3.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticBoxPushOut", Script, "int RunStaticBoxPushOut()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticMin)
	{
		static const char Script[] = R"(
int RunStaticMin()
{
    if (!FVector::Min(FVector(1.0, 5.0, 3.0), FVector(2.0, 4.0, 6.0)).opEquals(FVector(1.0, 4.0, 3.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticMin", Script, "int RunStaticMin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticMax)
	{
		static const char Script[] = R"(
int RunStaticMax()
{
    if (!FVector::Max(FVector(1.0, 5.0, 3.0), FVector(2.0, 4.0, 6.0)).opEquals(FVector(2.0, 5.0, 6.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticMax", Script, "int RunStaticMax()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticMin3)
	{
		static const char Script[] = R"(
int RunStaticMin3()
{
    if (!FVector::Min3(FVector(3.0, 7.0, 5.0), FVector(2.0, 6.0, 4.0), FVector(1.0, 8.0, 3.0)).opEquals(FVector(1.0, 6.0, 3.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticMin3", Script, "int RunStaticMin3()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticMax3)
	{
		static const char Script[] = R"(
int RunStaticMax3()
{
    if (!FVector::Max3(FVector(3.0, 7.0, 5.0), FVector(2.0, 6.0, 4.0), FVector(1.0, 8.0, 3.0)).opEquals(FVector(3.0, 8.0, 5.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticMax3", Script, "int RunStaticMax3()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticParallel)
	{
		static const char Script[] = R"(
int RunStaticParallel()
{
    if (!FVector::Parallel(FVector(1.0, 0.0, 0.0), FVector(1.0, 0.0, 0.0)))
    {
        return -1;
    }

    if (!FVector::Parallel(FVector(1.0, 0.0, 0.0), FVector(0.999, 0.01, 0.0), 0.99))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticParallel", Script, "int RunStaticParallel()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticCoincident)
	{
		static const char Script[] = R"(
int RunStaticCoincident()
{
    if (!FVector::Coincident(FVector(1.0, 0.0, 0.0), FVector(1.0, 0.0, 0.0)))
    {
        return -1;
    }

    if (!FVector::Coincident(FVector(1.0, 0.0, 0.0), FVector(0.999, 0.01, 0.0), 0.99))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticCoincident", Script, "int RunStaticCoincident()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticOrthogonal)
	{
		static const char Script[] = R"(
int RunStaticOrthogonal()
{
    if (!FVector::Orthogonal(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)))
    {
        return -1;
    }

    if (!FVector::Orthogonal(FVector(1.0, 0.0, 0.0), FVector(0.05, 1.0, 0.0), 0.1))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticOrthogonal", Script, "int RunStaticOrthogonal()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticCoplanar)
	{
		static const char Script[] = R"(
int RunStaticCoplanar()
{
    if (!FVector::Coplanar(FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0), FVector(1.0, 1.0, 0.0), FVector(0.0, 0.0, 1.0)))
    {
        return -1;
    }

    if (!FVector::Coplanar(FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0), FVector(1.0, 1.0, 0.0), FVector(0.0, 0.0, 1.0), 0.999))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticCoplanar", Script, "int RunStaticCoplanar()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticTriple)
	{
		static const char Script[] = R"(
int RunStaticTriple()
{
    if (!FMath::IsNearlyEqual(FVector::Triple(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0), FVector(0.0, 0.0, 1.0)), 1.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticTriple", Script, "int RunStaticTriple()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticRadiansToDegrees)
	{
		static const char Script[] = R"(
int RunStaticRadiansToDegrees()
{
    if (!FVector::RadiansToDegrees(FVector(3.141592653589793, 1.5707963267948966, 0.0)).Equals(FVector(180.0, 90.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticRadiansToDegrees", Script, "int RunStaticRadiansToDegrees()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StaticDegreesToRadians)
	{
		static const char Script[] = R"(
int RunStaticDegreesToRadians()
{
    if (!FVector::DegreesToRadians(FVector(180.0, 90.0, 0.0)).Equals(FVector(3.141592653589793, 1.5707963267948966, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticDegreesToRadians", Script, "int RunStaticDegreesToRadians()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IndexOutOfRangeThrows)
	{
		static const char Script[] = R"(
int RunOutOfRange()
{
    FVector Value(1.0, 2.0, 3.0);
    return int(Value[3]);
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorOutOfRange", Script, "int RunOutOfRange()");
		const int Result = ExecuteFunction(Function);
		ASSERT_THAT(IsTrue(Result == asEXECUTION_EXCEPTION));
		ASSERT_THAT(IsTrue(FString(Context->GetExceptionString()) == FString("FVector index out of range")));
	}

	TEST_METHOD(ScriptCallsUnrealWithVector)
	{
		static const char Script[] = R"(
int RunVectorInteropTest()
{
    FVector Value = UnrealBuildVector(11.0, 12.0, 13.0);
    if (!UnrealAcceptsVector(Value))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(UnrealSumVector(Value), 36.0, 0.0001))
    {
        return -2;
    }

    return int(Value.X + Value.Y + Value.Z);
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorInterop", Script, "int RunVectorInteropTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 36));
	}

	TEST_METHOD(UnrealCallsScriptWithVector)
	{
		static const char Script[] = R"(
FVector BuildVectorForUnreal(const FVector &in Value)
{
    FVector Result(Value);
    Result.X = Result.X + 1.0;
    Result.Y = Result.Y + 2.0;
    Result.Z = Result.Z + 3.0;
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorUnrealCall", Script, "FVector BuildVectorForUnreal(const FVector &in Value)");
		ASSERT_THAT(IsNotNull(Function));

		FVector Input(2.0, 4.0, 6.0);

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));
		ASSERT_THAT(IsTrue(Context->SetArgObject(0, &Input) >= 0));

		const int ExecuteResult = Context->Execute();
		if (ExecuteResult == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		ASSERT_THAT(IsTrue(ExecuteResult == asEXECUTION_FINISHED));

		const FVector* ReturnValue = static_cast<const FVector*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(*ReturnValue == FVector(3.0, 6.0, 9.0)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
