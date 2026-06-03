// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Rotator.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FRotator UnrealBuildRotator(double Pitch, double Yaw, double Roll)
	{
		return FRotator(Pitch, Yaw, Roll);
	}

	bool UnrealAcceptsRotator(const FRotator& Value)
	{
		return Value.Equals(FRotator(11.0, 12.0, 13.0), 0.0001);
	}

	double UnrealSumRotator(const FRotator& Value)
	{
		return Value.Pitch + Value.Yaw + Value.Roll;
	}

	bool UnrealNearlyEqual(double A, double B, double Tolerance)
	{
		return FMath::Abs(A - B) <= Tolerance;
	}

	FRotator UnrealRotatorWithNaN()
	{
		return FRotator(std::numeric_limits<float>::quiet_NaN(), 1.0, 2.0);
	}

	bool RegisterFRotatorInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FRotator UnrealBuildRotator(double Pitch, double Yaw, double Roll)",
			asFUNCTION(UnrealBuildRotator),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsRotator(const FRotator &in Value)",
			asFUNCTION(UnrealAcceptsRotator),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealSumRotator(const FRotator &in Value)",
			asFUNCTION(UnrealSumRotator),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealNearlyEqual(double A, double B, double Tolerance)",
			asFUNCTION(UnrealNearlyEqual),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FRotator UnrealRotatorWithNaN()",
			asFUNCTION(UnrealRotatorWithNaN),
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
	FOtterAngleScriptFRotatorTests,
	"OtterAngleScript.FRotator",
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
		ASSERT_THAT(IsTrue(RegisterFRotatorInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptRotatorTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(DefaultConstructor)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));

		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FRotator DefaultValue;
    if (!DefaultValue.opEquals(FRotator(0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorDefaultCtor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScalarConstructor)
	{
		static const char Script[] = R"(
int RunScalarConstructor()
{
    FRotator Scalar(5.0);
    if (Scalar.Pitch != 5.0 || Scalar.Yaw != 5.0 || Scalar.Roll != 5.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorScalarCtor", Script, "int RunScalarConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(PitchYawRollConstructor)
	{
		static const char Script[] = R"(
int RunPitchYawRollConstructor()
{
    FRotator Value(10.0, 20.0, 30.0);
    if (Value.Pitch != 10.0 || Value.Yaw != 20.0 || Value.Roll != 30.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorPYRCtor", Script, "int RunPitchYawRollConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ForceInitConstructor)
	{
		static const char Script[] = R"(
int RunForceInitConstructor()
{
    FRotator Zeroed(ForceInit);
    if (!Zeroed.opEquals(FRotator(0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorForceInitCtor", Script, "int RunForceInitConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FRotator Value(10.0, 20.0, 30.0);
    FRotator Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorCopyCtor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AssignmentOperator)
	{
		static const char Script[] = R"(
int RunAssignmentOperator()
{
    FRotator Value(10.0, 20.0, 30.0);
    FRotator Assigned;
    Assigned = Value;
    if (!Assigned.opEquals(Value))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorAssign", Script, "int RunAssignmentOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FromQuatConstructor)
	{
		static const char Script[] = R"(
int RunFromQuatConstructor()
{
    FRotator FromQuat(FQuat(0.0, 0.0, 0.0, 1.0));
    if (!FromQuat.Equals(FRotator(0.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorQuatCtor", Script, "int RunFromQuatConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AdditionOperator)
	{
		static const char Script[] = R"(
int RunAdditionOperator()
{
    FRotator Value(10.0, 20.0, 30.0);
    if (!(Value + FRotator(1.0, 2.0, 3.0)).opEquals(FRotator(11.0, 22.0, 33.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorAdd", Script, "int RunAdditionOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SubtractionOperator)
	{
		static const char Script[] = R"(
int RunSubtractionOperator()
{
    FRotator Value(10.0, 20.0, 30.0);
    if (!(Value - FRotator(1.0, 2.0, 3.0)).opEquals(FRotator(9.0, 18.0, 27.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorSub", Script, "int RunSubtractionOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MultiplyByScalar)
	{
		static const char Script[] = R"(
int RunMultiplyByScalar()
{
    FRotator Value(10.0, 20.0, 30.0);
    if (!(Value * 2.0).opEquals(FRotator(20.0, 40.0, 60.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorMul", Script, "int RunMultiplyByScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScalarMultiplyRotator)
	{
		static const char Script[] = R"(
int RunScalarMultiplyRotator()
{
    FRotator Value(10.0, 20.0, 30.0);
    if (!(2.0 * Value).opEquals(FRotator(20.0, 40.0, 60.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorMulR", Script, "int RunScalarMultiplyRotator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompoundAssignment)
	{
		static const char Script[] = R"(
int RunCompoundAssignment()
{
    FRotator Mutating(1.0, 2.0, 3.0);
    Mutating += FRotator(1.0, 1.0, 1.0);
    Mutating -= FRotator(0.0, 1.0, 1.0);
    Mutating *= 2.0;
    if (!Mutating.opEquals(FRotator(4.0, 4.0, 6.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorCompAssign", Script, "int RunCompoundAssignment()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsNearlyZero)
	{
		static const char Script[] = R"(
int RunIsNearlyZero()
{
    if (!FRotator(0.0, 360.0, 0.0).IsNearlyZero())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorIsNearlyZero", Script, "int RunIsNearlyZero()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsZero)
	{
		static const char Script[] = R"(
int RunIsZero()
{
    if (!FRotator(0.0, 360.0, 0.0).IsZero())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorIsZero", Script, "int RunIsZero()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Equals)
	{
		static const char Script[] = R"(
int RunEquals()
{
    if (!FRotator(0.0, 360.0, 0.0).Equals(FRotator(0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorEquals", Script, "int RunEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(EqualsOrientation)
	{
		static const char Script[] = R"(
int RunEqualsOrientation()
{
    FRotator Value(10.0, 20.0, 30.0);
    FRotator Equivalent = Value.GetEquivalentRotator();
    if (Value.Equals(Equivalent))
    {
        return -1;
    }
    if (!Value.EqualsOrientation(Equivalent))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorEqualsOrient", Script, "int RunEqualsOrientation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Add)
	{
		static const char Script[] = R"(
int RunAdd()
{
    FRotator Value(10.0, 20.0, 30.0);
    Value.Add(1.0, 2.0, 3.0);
    if (!Value.opEquals(FRotator(11.0, 22.0, 33.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorAddMethod", Script, "int RunAdd()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetInverse)
	{
		static const char Script[] = R"(
int RunGetInverse()
{
    FRotator Inverse = FRotator(0.0, 90.0, 0.0).GetInverse();
    if (!Inverse.RotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorGetInverse", Script, "int RunGetInverse()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GridSnap)
	{
		static const char Script[] = R"(
int RunGridSnap()
{
    if (!FRotator(13.0, 27.0, 44.0).GridSnap(FRotator(10.0, 15.0, 30.0)).opEquals(FRotator(10.0, 30.0, 30.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorGridSnap", Script, "int RunGridSnap()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Vector)
	{
		static const char Script[] = R"(
int RunVector()
{
    if (!FRotator(0.0, 90.0, 0.0).Vector().Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorVector", Script, "int RunVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Quaternion)
	{
		static const char Script[] = R"(
int RunQuaternion()
{
    if (!FRotator(0.0, 0.0, 0.0).Quaternion().Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorQuatMethod", Script, "int RunQuaternion()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Euler)
	{
		static const char Script[] = R"(
int RunEuler()
{
    if (!FRotator(10.0, 20.0, 30.0).Euler().Equals(FVector(30.0, 10.0f, 20.0f), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		auto Rotator = FRotator(10.0, 20.0, 30.0);
		auto Euler = Rotator.Euler();

		asIScriptFunction* Function = BuildFunction("FRotatorEuler", Script, "int RunEuler()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RotateVector)
	{
		static const char Script[] = R"(
int RunRotateVector()
{
    if (!FRotator(0.0, 90.0, 0.0).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorRotateVec", Script, "int RunRotateVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(UnrotateVector)
	{
		static const char Script[] = R"(
int RunUnrotateVector()
{
    if (!FRotator(0.0, 90.0, 0.0).UnrotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorUnrotateVec", Script, "int RunUnrotateVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Clamp)
	{
		static const char Script[] = R"(
int RunClamp()
{
    if (!FRotator(370.0, -10.0, 540.0).Clamp().opEquals(FRotator(10.0, 350.0, 180.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorClamp", Script, "int RunClamp()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetNormalized)
	{
		static const char Script[] = R"(
int RunGetNormalized()
{
    if (!FRotator(0.0, 540.0, 0.0).GetNormalized().opEquals(FRotator(0.0, 180.0, 0.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorGetNormalized", Script, "int RunGetNormalized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetDenormalized)
	{
		static const char Script[] = R"(
int RunGetDenormalized()
{
    if (!FRotator(-10.0, 370.0, -540.0).GetDenormalized().opEquals(FRotator(350.0, 10.0, 180.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorGetDenormalized", Script, "int RunGetDenormalized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetComponentForAxis)
	{
		static const char Script[] = R"(
int RunGetComponentForAxis()
{
    FRotator AxisValue(10.0, 20.0, 30.0);
    if (!UnrealNearlyEqual(AxisValue.GetComponentForAxis(X), 30.0, 0.0001))
    {
        return -1;
    }
    if (!UnrealNearlyEqual(AxisValue.GetComponentForAxis(Y), 10.0, 0.0001))
    {
        return -2;
    }
    if (!UnrealNearlyEqual(AxisValue.GetComponentForAxis(Z), 20.0, 0.0001))
    {
        return -3;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorGetComponent", Script, "int RunGetComponentForAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetComponentForAxis)
	{
		static const char Script[] = R"(
int RunSetComponentForAxis()
{
    FRotator AxisValue(10.0, 20.0, 30.0);
    AxisValue.SetComponentForAxis(X, 40.0);
    AxisValue.SetComponentForAxis(Y, 50.0);
    AxisValue.SetComponentForAxis(Z, 60.0);
    if (!AxisValue.opEquals(FRotator(50.0, 60.0, 40.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorSetComponent", Script, "int RunSetComponentForAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Normalize)
	{
		static const char Script[] = R"(
int RunNormalize()
{
    FRotator ToNormalize(0.0, 540.0, 0.0);
    ToNormalize.Normalize();
    if (!ToNormalize.opEquals(FRotator(0.0, 180.0, 0.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorNormalize", Script, "int RunNormalize()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetWindingAndRemainder)
	{
		static const char Script[] = R"(
int RunGetWindingAndRemainder()
{
    FRotator Winding;
    FRotator Remainder;
    FRotator(450.0, -540.0, 30.0).GetWindingAndRemainder(Winding, Remainder);
    if (!(Winding + Remainder).opEquals(FRotator(450.0, -540.0, 30.0)))
    {
        return -1;
    }
    if (!Remainder.opEquals(Remainder.GetNormalized()))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorWindingRem", Script, "int RunGetWindingAndRemainder()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetManhattanDistance)
	{
		static const char Script[] = R"(
int RunGetManhattanDistance()
{
    if (!UnrealNearlyEqual(FRotator(10.0, 20.0, 30.0).GetManhattanDistance(FRotator(1.0, 2.0, 3.0)), 54.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorManhattan", Script, "int RunGetManhattanDistance()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetEquivalentRotator)
	{
		static const char Script[] = R"(
int RunGetEquivalentRotator()
{
    FRotator Equivalent = FRotator(10.0, 20.0, 30.0).GetEquivalentRotator();
    if (!Equivalent.opEquals(FRotator(170.0, 200.0, 210.0)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorEquivRot", Script, "int RunGetEquivalentRotator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetClosestToMe)
	{
		static const char Script[] = R"(
int RunSetClosestToMe()
{
    FRotator Equivalent = FRotator(10.0, 20.0, 30.0).GetEquivalentRotator();
    FRotator Closest = Equivalent;
    FRotator(10.0, 20.0, 30.0).SetClosestToMe(Closest);
    if (!Closest.opEquals(FRotator(170.0, 200.0, 210.0)))
    {
        return -1;
    }
    return 1;
}
)";
		FRotator Equivalent = FRotator(10.0, 20.0, 30.0).GetEquivalentRotator();
		FRotator Closest = Equivalent;
		FRotator(10.0, 20.0, 30.0).SetClosestToMe(Closest);

		asIScriptFunction* Function = BuildFunction("FRotatorSetClosest", Script, "int RunSetClosestToMe()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FString FullText = FRotator(10.0, 20.0, 30.0).ToString();
    if (!FullText.StartsWith("P="))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorToString", Script, "int RunToString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToCompactString)
	{
		static const char Script[] = R"(
int RunToCompactString()
{
    FString CompactText = FRotator(10.0, 20.0, 30.0).ToCompactString();
    if (CompactText.IsEmpty())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorToCompact", Script, "int RunToCompactString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InitFromString)
	{
		static const char Script[] = R"(
int RunInitFromString()
{
    FRotator Parsed;
    if (!Parsed.InitFromString("P=10.0 Y=20.0 R=30.0"))
    {
        return -1;
    }
    if (!Parsed.opEquals(FRotator(10.0, 20.0, 30.0)))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorInitStr", Script, "int RunInitFromString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ContainsNaN)
	{
		static const char Script[] = R"(
int RunContainsNaN()
{
    if (!UnrealRotatorWithNaN().ContainsNaN())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorNaN", Script, "int RunContainsNaN()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ClampAxis)
	{
		static const char Script[] = R"(
int RunClampAxis()
{
    if (!UnrealNearlyEqual(FRotator::ClampAxis(-10.0), 350.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorClampAxis", Script, "int RunClampAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NormalizeAxis)
	{
		static const char Script[] = R"(
int RunNormalizeAxis()
{
    if (!UnrealNearlyEqual(FRotator::NormalizeAxis(350.0), -10.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorNormAxis", Script, "int RunNormalizeAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompressDecompressByte)
	{
		static const char Script[] = R"(
int RunCompressDecompressByte()
{
    uint8 ByteValue = FRotator::CompressAxisToByte(90.0);
    if (!UnrealNearlyEqual(FRotator::DecompressAxisFromByte(ByteValue), 90.0, 2.0))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorCompByte", Script, "int RunCompressDecompressByte()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompressDecompressShort)
	{
		static const char Script[] = R"(
int RunCompressDecompressShort()
{
    uint16 ShortValue = FRotator::CompressAxisToShort(180.0);
    if (!UnrealNearlyEqual(FRotator::DecompressAxisFromShort(ShortValue), 180.0, 0.1))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorCompShort", Script, "int RunCompressDecompressShort()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeFromEuler)
	{
		static const char Script[] = R"(
int RunMakeFromEuler()
{
    if (!FRotator::MakeFromEuler(FVector(10.0, 20.0, 30.0)).Euler().Equals(FVector(10.0, 20.0, 30.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorMakeEuler", Script, "int RunMakeFromEuler()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScriptCallsUnrealWithRotator)
	{
		static const char Script[] = R"(
int RunRotatorInteropTest()
{
    FRotator Value = UnrealBuildRotator(11.0, 12.0, 13.0);
    if (!UnrealAcceptsRotator(Value))
    {
        return -1;
    }

    if (!UnrealNearlyEqual(UnrealSumRotator(Value), 36.0, 0.0001))
    {
        return -2;
    }

    return int(Value.Pitch + Value.Yaw + Value.Roll);
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorInterop", Script, "int RunRotatorInteropTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 36));
	}

	TEST_METHOD(UnrealCallsScriptWithRotator)
	{
		static const char Script[] = R"(
FRotator BuildRotatorForUnreal(const FRotator &in Value)
{
    FRotator Result(Value);
    Result.Pitch += 1.0;
    Result.Yaw += 2.0;
    Result.Roll += 3.0;
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorUnrealCall", Script, "FRotator BuildRotatorForUnreal(const FRotator &in Value)");
		ASSERT_THAT(IsNotNull(Function));

		FRotator Input(2.0, 4.0, 6.0);

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

		const FRotator* ReturnValue = static_cast<const FRotator*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(ReturnValue->Equals(FRotator(3.0, 6.0, 9.0), 0.0001)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
