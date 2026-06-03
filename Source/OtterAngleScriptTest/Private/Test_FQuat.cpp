// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Quat.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"
const double HalfPi = 1.5707963267948966;

namespace
{
	FQuat UnrealBuildQuat(double X, double Y, double Z, double W)
	{
		return FQuat(X, Y, Z, W);
	}

	bool UnrealAcceptsQuat(const FQuat& Value)
	{
		return Value.Equals(FQuat(11.0, 12.0, 13.0, 14.0), 0.0001);
	}

	double UnrealSumQuat(const FQuat& Value)
	{
		return Value.X + Value.Y + Value.Z + Value.W;
	}

	FQuat UnrealQuatWithNaN()
	{
		return FQuat(std::numeric_limits<double>::quiet_NaN(), 1.0, 2.0, 3.0);
	}

	bool RegisterFQuatInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FQuat UnrealBuildQuat(double X, double Y, double Z, double W)",
			asFUNCTION(UnrealBuildQuat),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsQuat(const FQuat &in Value)",
			asFUNCTION(UnrealAcceptsQuat),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealSumQuat(const FQuat &in Value)",
			asFUNCTION(UnrealSumQuat),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FQuat UnrealQuatWithNaN()",
			asFUNCTION(UnrealQuatWithNaN),
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
	FOtterAngleScriptFQuatTests,
	"OtterAngleScript.FQuat",
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
		ASSERT_THAT(IsTrue(RegisterFQuatInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptQuatTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeInfo)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FQuat")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));
	}

	TEST_METHOD(ForceInitConstructor)
	{
		static const char Script[] = R"(
int RunForceInitConstructor()
{
    FQuat Identity(ForceInit);
    if (!Identity.opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatForceInitCtor", Script, "int RunForceInitConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ForceInitToZeroConstructor)
	{
		static const char Script[] = R"(
int RunForceInitToZeroConstructor()
{
    FQuat Zeroed(ForceInitToZero);
    if (!Zeroed.opEquals(FQuat(0.0, 0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatZeroCtor", Script, "int RunForceInitToZeroConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(XYZWConstructor)
	{
		static const char Script[] = R"(
int RunXYZWConstructor()
{
    FQuat Value(0.0, 0.0, 0.7071067811865476, 0.7071067811865476);
    if (!Value.opEquals(FQuat(0.0, 0.0, 0.7071067811865476, 0.7071067811865476)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatXYZWCtor", Script, "int RunXYZWConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FQuat Value(0.0, 0.0, 0.7071067811865476, 0.7071067811865476);
    FQuat Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatCopyCtor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AssignmentOperator)
	{
		static const char Script[] = R"(
int RunAssignmentOperator()
{
    FQuat Value(0.0, 0.0, 0.7071067811865476, 0.7071067811865476);
    FQuat Assigned(ForceInit);
    Assigned = Value;
    if (!Assigned.opEquals(Value))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAssign", Script, "int RunAssignmentOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FromRotatorConstructor)
	{
		static const char Script[] = R"(
int RunFromRotatorConstructor()
{
    FQuat FromRotator(FRotator(0.0, 90.0, 0.0));
    if (!FromRotator.RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatRotCtor", Script, "int RunFromRotatorConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FromAxisAngleConstructor)
	{
		static const char Script[] = R"(
int RunFromAxisAngleConstructor()
{
    const double HalfPi = 1.5707963267948966;
    FQuat FromAxisAngle(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!FromAxisAngle.RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAxisAngleCtor", Script, "int RunFromAxisAngleConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AdditionOperator)
	{
		static const char Script[] = R"(
int RunAdditionOperator()
{
    if (!(FQuat(1.0, 2.0, 3.0, 4.0) + FQuat(4.0, 3.0, 2.0, 1.0)).opEquals(FQuat(5.0, 5.0, 5.0, 5.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAdd", Script, "int RunAdditionOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SubtractionOperator)
	{
		static const char Script[] = R"(
int RunSubtractionOperator()
{
    if (!(FQuat(5.0, 5.0, 5.0, 5.0) - FQuat(1.0, 2.0, 3.0, 4.0)).opEquals(FQuat(4.0, 3.0, 2.0, 1.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSub", Script, "int RunSubtractionOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DotProductOperator)
	{
		static const char Script[] = R"(
int RunDotProductOperator()
{
    if (!FMath::IsNearlyEqual(FQuat(1.0, 2.0, 3.0, 4.0) | FQuat(4.0, 3.0, 2.0, 1.0), 20.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatDot", Script, "int RunDotProductOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MultiplyQuatByVector)
	{
		static const char Script[] = R"(
int RunMultiplyQuatByVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat FromAxisAngle(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!(FromAxisAngle * FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatMulVec", Script, "int RunMultiplyQuatByVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MultiplyQuatByQuat)
	{
		static const char Script[] = R"(
int RunMultiplyQuatByQuat()
{
    const double HalfPi = 1.5707963267948966;
    FQuat FromAxisAngle(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!((FromAxisAngle * FromAxisAngle) * FVector(1.0, 0.0, 0.0)).Equals(FVector(-1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatMulQuat", Script, "int RunMultiplyQuatByQuat()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MultiplyByScalar)
	{
		static const char Script[] = R"(
int RunMultiplyByScalar()
{
    if (!(FQuat(1.0, 2.0, 3.0, 4.0) * 2.0).opEquals(FQuat(2.0, 4.0, 6.0, 8.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatMulScale", Script, "int RunMultiplyByScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScalarMultiplyQuat)
	{
		static const char Script[] = R"(
int RunScalarMultiplyQuat()
{
    if (!(2.0 * FQuat(1.0, 2.0, 3.0, 4.0)).opEquals(FQuat(2.0, 4.0, 6.0, 8.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatMulScaleR", Script, "int RunScalarMultiplyQuat()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DivisionOperator)
	{
		static const char Script[] = R"(
int RunDivisionOperator()
{
    if (!(FQuat(2.0, 4.0, 6.0, 8.0) / 2.0).opEquals(FQuat(1.0, 2.0, 3.0, 4.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatDiv", Script, "int RunDivisionOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompoundAddSubAssign)
	{
		static const char Script[] = R"(
int RunCompoundAddSubAssign()
{
    FQuat Mutating(1.0, 2.0, 3.0, 4.0);
    Mutating += FQuat(4.0, 3.0, 2.0, 1.0);
    Mutating -= FQuat(1.0, 1.0, 1.0, 1.0);
    if (!Mutating.opEquals(FQuat(4.0, 4.0, 4.0, 4.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatCompAddSub", Script, "int RunCompoundAddSubAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompoundScaleAssign)
	{
		static const char Script[] = R"(
int RunCompoundScaleAssign()
{
    FQuat ScaleMutating(1.0, 2.0, 3.0, 4.0);
    ScaleMutating *= 2.0;
    ScaleMutating /= 4.0;
    if (!ScaleMutating.opEquals(FQuat(0.5, 1.0, 1.5, 2.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatCompScale", Script, "int RunCompoundScaleAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompoundQuatAssign)
	{
		static const char Script[] = R"(
int RunCompoundQuatAssign()
{
    const double HalfPi = 1.5707963267948966;
    FQuat FromAxisAngle(FVector(0.0, 0.0, 1.0), HalfPi);
    FQuat Compose = FromAxisAngle;
    Compose *= FromAxisAngle;
    if (!(Compose * FVector(1.0, 0.0, 0.0)).Equals(FVector(-1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatCompQuat", Script, "int RunCompoundQuatAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(EqualsMethod)
	{
		static const char Script[] = R"(
int RunEqualsMethod()
{
    if (!FQuat(0.0, 0.0, 0.0, -1.0).Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatEquals", Script, "int RunEqualsMethod()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(EqualityOperator)
	{
		static const char Script[] = R"(
int RunEqualityOperator()
{
    if (FQuat(0.0, 0.0, 0.0, -1.0).opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatOpEquals", Script, "int RunEqualityOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsIdentity)
	{
		static const char Script[] = R"(
int RunIsIdentity()
{
    FQuat Identity(ForceInit);
    if (!Identity.IsIdentity())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatIsIdentity", Script, "int RunIsIdentity()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Euler)
	{
		static const char Script[] = R"(
int RunEuler()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.Euler().Equals(FVector(0.0, 0.0, 90.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
		auto Euler = QuarterTurn.Euler();

		asIScriptFunction* Function = BuildFunction("FQuatEuler", Script, "int RunEuler()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Normalize)
	{
		static const char Script[] = R"(
int RunNormalize()
{
    FQuat ToNormalize(0.0, 0.0, 0.0, 2.0);
    ToNormalize.Normalize();
    if (!ToNormalize.opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatNormalize", Script, "int RunNormalize()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetNormalized)
	{
		static const char Script[] = R"(
int RunGetNormalized()
{
    if (!FQuat(0.0, 0.0, 0.0, 2.0).GetNormalized().opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatGetNormalized", Script, "int RunGetNormalized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsNormalized)
	{
		static const char Script[] = R"(
int RunIsNormalized()
{
    FQuat Identity(ForceInit);
    if (!Identity.IsNormalized())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatIsNormalized", Script, "int RunIsNormalized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Size)
	{
		static const char Script[] = R"(
int RunSize()
{
    if (!FMath::IsNearlyEqual(FQuat(1.0, 2.0, 2.0, 0.0).Size(), 3.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSize", Script, "int RunSize()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SizeSquared)
	{
		static const char Script[] = R"(
int RunSizeSquared()
{
    if (!FMath::IsNearlyEqual(FQuat(1.0, 2.0, 2.0, 0.0).SizeSquared(), 9.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSizeSq", Script, "int RunSizeSquared()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetAngle)
	{
		static const char Script[] = R"(
int RunGetAngle()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!FMath::IsNearlyEqual(QuarterTurn.GetAngle(), HalfPi, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatGetAngle", Script, "int RunGetAngle()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToAxisAndAngle)
	{
		static const char Script[] = R"(
int RunToAxisAndAngle()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FVector Axis;
    double Angle = 0.0;
    QuarterTurn.ToAxisAndAngle(Axis, Angle);
    if (!Axis.Equals(FVector(0.0, 0.0, 1.0), 0.0001) || !FMath::IsNearlyEqual(Angle, HalfPi, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAxisAngle", Script, "int RunToAxisAndAngle()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToSwingTwist)
	{
		static const char Script[] = R"(
int RunToSwingTwist()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FQuat Swing;
    FQuat Twist;
    QuarterTurn.ToSwingTwist(FVector(0.0, 0.0, 1.0), Swing, Twist);
    if (!Swing.IsIdentity(0.0001) || !Twist.Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSwingTwist", Script, "int RunToSwingTwist()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RotateVector)
	{
		static const char Script[] = R"(
int RunRotateVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatRotateVec", Script, "int RunRotateVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(UnrotateVector)
	{
		static const char Script[] = R"(
int RunUnrotateVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.UnrotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatUnrotateVec", Script, "int RunUnrotateVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(LogAndExp)
	{
		static const char Script[] = R"(
int RunLogAndExp()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.Log().Exp().Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatLogExp", Script, "int RunLogAndExp()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Inverse)
	{
		static const char Script[] = R"(
int RunInverse()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.Inverse().RotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatInverse", Script, "int RunInverse()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(EnforceShortestArcWith)
	{
		static const char Script[] = R"(
int RunEnforceShortestArcWith()
{
    FQuat Identity(ForceInit);
    FQuat Shortest(0.0, 0.0, 0.0, -1.0);
    Shortest.EnforceShortestArcWith(Identity);
    if (!Shortest.opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatShortArc", Script, "int RunEnforceShortestArcWith()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetAxisX)
	{
		static const char Script[] = R"(
int RunGetAxisX()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetAxisX().Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAxisX", Script, "int RunGetAxisX()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetAxisY)
	{
		static const char Script[] = R"(
int RunGetAxisY()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetAxisY().Equals(FVector(-1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAxisY", Script, "int RunGetAxisY()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetAxisZ)
	{
		static const char Script[] = R"(
int RunGetAxisZ()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetAxisZ().Equals(FVector(0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAxisZ", Script, "int RunGetAxisZ()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetForwardVector)
	{
		static const char Script[] = R"(
int RunGetForwardVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetForwardVector().Equals(QuarterTurn.GetAxisX(), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatForward", Script, "int RunGetForwardVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetRightVector)
	{
		static const char Script[] = R"(
int RunGetRightVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetRightVector().Equals(QuarterTurn.GetAxisY(), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatRight", Script, "int RunGetRightVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetUpVector)
	{
		static const char Script[] = R"(
int RunGetUpVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetUpVector().Equals(QuarterTurn.GetAxisZ(), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatUp", Script, "int RunGetUpVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Vector)
	{
		static const char Script[] = R"(
int RunVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.Vector().Equals(QuarterTurn.GetAxisX(), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatVector", Script, "int RunVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Rotator)
	{
		static const char Script[] = R"(
int RunRotator()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.Rotator().Equals(FRotator(0.0, 90.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatRotator", Script, "int RunRotator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetRotationAxis)
	{
		static const char Script[] = R"(
int RunGetRotationAxis()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.GetRotationAxis().Equals(FVector(0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatRotAxis", Script, "int RunGetRotationAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AngularDistance)
	{
		static const char Script[] = R"(
int RunAngularDistance()
{
    const double HalfPi = 1.5707963267948966;
    FQuat Identity(ForceInit);
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!FMath::IsNearlyEqual(Identity.AngularDistance(QuarterTurn), HalfPi, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatAngDist", Script, "int RunAngularDistance()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FString Text = QuarterTurn.ToString();
    if (!Text.StartsWith("X="))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatToString", Script, "int RunToString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InitFromString)
	{
		static const char Script[] = R"(
int RunInitFromString()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FQuat Parsed;
    if (!Parsed.InitFromString("X=0.0 Y=0.0 Z=0.7071067811865476 W=0.7071067811865476"))
    {
        return -1;
    }
    if (!Parsed.Equals(QuarterTurn, 0.0001))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatInitStr", Script, "int RunInitFromString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ContainsNaN)
	{
		static const char Script[] = R"(
int RunContainsNaN()
{
    if (!UnrealQuatWithNaN().ContainsNaN())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatNaN", Script, "int RunContainsNaN()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeFromEuler)
	{
		static const char Script[] = R"(
int RunMakeFromEuler()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!FQuat::MakeFromEuler(FVector(0.0, 0.0, 90.0)).Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		auto Euler = FQuat::MakeFromEuler(FVector(0.0, 90.0, 0.0));

		FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);

		asIScriptFunction* Function = BuildFunction("FQuatMakeEuler", Script, "int RunMakeFromEuler()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FindBetween)
	{
		static const char Script[] = R"(
int RunFindBetween()
{
    if (!FQuat::FindBetween(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatFindBetween", Script, "int RunFindBetween()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FindBetweenNormals)
	{
		static const char Script[] = R"(
int RunFindBetweenNormals()
{
    if (!FQuat::FindBetweenNormals(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatFindNormals", Script, "int RunFindBetweenNormals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FindBetweenVectors)
	{
		static const char Script[] = R"(
int RunFindBetweenVectors()
{
    if (!FQuat::FindBetweenVectors(FVector(2.0, 0.0, 0.0), FVector(0.0, 3.0, 0.0)).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatFindVectors", Script, "int RunFindBetweenVectors()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Error)
	{
		static const char Script[] = R"(
int RunError()
{
    FQuat Identity(ForceInit);
    if (!FMath::IsNearlyEqual(FQuat::Error(Identity, Identity), 0.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatError", Script, "int RunError()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ErrorAutoNormalize)
	{
		static const char Script[] = R"(
int RunErrorAutoNormalize()
{
    if (!FMath::IsNearlyEqual(FQuat::ErrorAutoNormalize(FQuat(0.0, 0.0, 0.0, 2.0), FQuat(0.0, 0.0, 0.0, 5.0)), 0.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatErrNorm", Script, "int RunErrorAutoNormalize()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FastLerp)
	{
		static const char Script[] = R"(
int RunFastLerp()
{
    FQuat Identity(ForceInit);
    if (!FQuat::FastLerp(Identity, Identity, 0.5).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatFastLerp", Script, "int RunFastLerp()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FastBilerp)
	{
		static const char Script[] = R"(
int RunFastBilerp()
{
    FQuat Identity(ForceInit);
    if (!FQuat::FastBilerp(Identity, Identity, Identity, Identity, 0.5, 0.5).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatFastBilerp", Script, "int RunFastBilerp()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SlerpNotNormalized)
	{
		static const char Script[] = R"(
int RunSlerpNotNormalized()
{
    FQuat Identity(ForceInit);
    if (!FQuat::Slerp_NotNormalized(Identity, Identity, 0.25).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSlerpNN", Script, "int RunSlerpNotNormalized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Slerp)
	{
		const double InvSqrtTwo = 0.7071067811865476;
		static const char Script[] = R"(
int RunSlerp()
{
    const double HalfPi = 1.5707963267948966;
    const double InvSqrtTwo = 0.7071067811865476;
    FQuat Identity(ForceInit);
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!FQuat::Slerp(Identity, QuarterTurn, 0.5).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(InvSqrtTwo, InvSqrtTwo, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSlerp", Script, "int RunSlerp()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SlerpFullPathNotNormalized)
	{
		static const char Script[] = R"(
int RunSlerpFullPathNotNormalized()
{
    FQuat Identity(ForceInit);
    if (!FQuat::SlerpFullPath_NotNormalized(Identity, Identity, 0.5).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSlerpFPNN", Script, "int RunSlerpFullPathNotNormalized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SlerpFullPath)
	{
		static const char Script[] = R"(
int RunSlerpFullPath()
{
    FQuat Identity(ForceInit);
    if (!FQuat::SlerpFullPath(Identity, Identity, 0.5).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSlerpFP", Script, "int RunSlerpFullPath()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Squad)
	{
		static const char Script[] = R"(
int RunSquad()
{
    FQuat Identity(ForceInit);
    if (!FQuat::Squad(Identity, Identity, Identity, Identity, 0.5).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSquad", Script, "int RunSquad()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SquadFullPath)
	{
		static const char Script[] = R"(
int RunSquadFullPath()
{
    FQuat Identity(ForceInit);
    if (!FQuat::SquadFullPath(Identity, Identity, Identity, Identity, 0.5).opEquals(Identity))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatSquadFP", Script, "int RunSquadFullPath()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CalcTangents)
	{
		static const char Script[] = R"(
int RunCalcTangents()
{
    FQuat Identity(ForceInit);
    FQuat Tangent;
    FQuat::CalcTangents(Identity, Identity, Identity, 0.0, Tangent);
    if (!Tangent.IsIdentity(0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatTangents", Script, "int RunCalcTangents()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScriptCallsUnrealWithQuat)
	{
		static const char Script[] = R"(
int RunQuatInteropTest()
{
    FQuat Value = UnrealBuildQuat(11.0, 12.0, 13.0, 14.0);
    if (!UnrealAcceptsQuat(Value))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(UnrealSumQuat(Value), 50.0, 0.0001))
    {
        return -2;
    }

    return int(Value.X + Value.Y + Value.Z + Value.W);
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatInterop", Script, "int RunQuatInteropTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 50));
	}

	TEST_METHOD(UnrealCallsScriptWithQuat)
	{
		static const char Script[] = R"(
FQuat BuildQuatForUnreal(const FQuat &in Value)
{
    FQuat Result(Value);
    Result.X += 1.0;
    Result.Y += 2.0;
    Result.Z += 3.0;
    Result.W += 4.0;
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatUnrealCall", Script, "FQuat BuildQuatForUnreal(const FQuat &in Value)");
		ASSERT_THAT(IsNotNull(Function));

		FQuat Input(2.0, 4.0, 6.0, 8.0);

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

		const FQuat* ReturnValue = static_cast<const FQuat*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(ReturnValue->Equals(FQuat(3.0, 6.0, 9.0, 12.0), 0.0001)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
