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
		return FRotator(0.0 / 0.0000010, 1.0, 2.0);
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

	TEST_METHOD(ConstructorsOperatorsAndComparisons)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));

		static const char Script[] = R"(
int RunConstructorsOperatorsAndComparisons()
{
    FRotator DefaultValue;
    if (!DefaultValue.opEquals(FRotator(0.0, 0.0, 0.0)))
    {
        return -1;
    }

    FRotator Scalar(5.0);
    if (Scalar.Pitch != 5.0 || Scalar.Yaw != 5.0 || Scalar.Roll != 5.0)
    {
        return -2;
    }

    FRotator Zeroed(ForceInit);
    if (!Zeroed.opEquals(FRotator(0.0, 0.0, 0.0)))
    {
        return -17;
    }

    FRotator Value(10.0, 20.0, 30.0);
    FRotator Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -3;
    }

    FRotator Assigned;
    Assigned = Value;
    if (!Assigned.opEquals(Value))
    {
        return -4;
    }

    FRotator FromQuat(FQuat(0.0, 0.0, 0.0, 1.0));
    if (!FromQuat.Equals(FRotator(0.0, 0.0, 0.0), 0.0001))
    {
        return -5;
    }

    if (!(Value + FRotator(1.0, 2.0, 3.0)).opEquals(FRotator(11.0, 22.0, 33.0)))
    {
        return -6;
    }

    if (!(Value - FRotator(1.0, 2.0, 3.0)).opEquals(FRotator(9.0, 18.0, 27.0)))
    {
        return -7;
    }

    if (!(Value * 2.0).opEquals(FRotator(20.0, 40.0, 60.0)))
    {
        return -8;
    }

    if (!(2.0 * Value).opEquals(FRotator(20.0, 40.0, 60.0)))
    {
        return -9;
    }

    FRotator Mutating(1.0, 2.0, 3.0);
    Mutating += FRotator(1.0, 1.0, 1.0);
    Mutating -= FRotator(0.0, 1.0, 1.0);
    Mutating *= 2.0;
    if (!Mutating.opEquals(FRotator(4.0, 4.0, 6.0)))
    {
        return -10;
    }

    if (!FRotator(0.0, 360.0, 0.0).IsNearlyZero())
    {
        return -11;
    }

    if (!FRotator(0.0, 360.0, 0.0).IsZero())
    {
        return -12;
    }

    if (!FRotator(0.0, 360.0, 0.0).Equals(FRotator(0.0, 0.0, 0.0)))
    {
        return -13;
    }

    FRotator Equivalent = Value.GetEquivalentRotator();
    if (Value.Equals(Equivalent))
    {
        return -14;
    }

    if (!Value.EqualsOrientation(Equivalent))
    {
        return -15;
    }

    Value.Add(1.0, 2.0, 3.0);
    if (!Value.opEquals(FRotator(11.0, 22.0, 33.0)))
    {
        return -16;
    }

    return 17;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorConstructorsOperators", Script, "int RunConstructorsOperatorsAndComparisons()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 17));
	}

	TEST_METHOD(InstanceMethods)
	{
		static const char Script[] = R"(
int RunInstanceMethods()
{
    FRotator Inverse = FRotator(0.0, 90.0, 0.0).GetInverse();
    if (!Inverse.RotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }

    if (!FRotator(13.0, 27.0, 44.0).GridSnap(FRotator(10.0, 15.0, 30.0)).opEquals(FRotator(10.0, 30.0, 30.0)))
    {
        return -2;
    }

    if (!FRotator(0.0, 90.0, 0.0).Vector().Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -3;
    }

    if (!FRotator(0.0, 0.0, 0.0).Quaternion().Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -4;
    }

    if (!FRotator(10.0, 20.0, 30.0).Euler().Equals(FVector(10.0, 20.0, 30.0), 0.0001))
    {
        return -5;
    }

    if (!FRotator(0.0, 90.0, 0.0).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -6;
    }

    if (!FRotator(0.0, 90.0, 0.0).UnrotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -7;
    }

    if (!FRotator(370.0, -10.0, 540.0).Clamp().opEquals(FRotator(10.0, 350.0, 180.0)))
    {
        return -8;
    }

    if (!FRotator(0.0, 540.0, 0.0).GetNormalized().opEquals(FRotator(0.0, 180.0, 0.0)))
    {
        return -9;
    }

    if (!FRotator(-10.0, 370.0, -540.0).GetDenormalized().opEquals(FRotator(350.0, 10.0, 180.0)))
    {
        return -10;
    }

    FRotator AxisValue(10.0, 20.0, 30.0);
    if (!UnrealNearlyEqual(AxisValue.GetComponentForAxis(X), 30.0, 0.0001))
    {
        return -11;
    }

    if (!UnrealNearlyEqual(AxisValue.GetComponentForAxis(Y), 10.0, 0.0001))
    {
        return -12;
    }

    if (!UnrealNearlyEqual(AxisValue.GetComponentForAxis(Z), 20.0, 0.0001))
    {
        return -13;
    }

    AxisValue.SetComponentForAxis(X, 40.0);
    AxisValue.SetComponentForAxis(Y, 50.0);
    AxisValue.SetComponentForAxis(Z, 60.0);
    if (!AxisValue.opEquals(FRotator(50.0, 60.0, 40.0)))
    {
        return -14;
    }

    FRotator ToNormalize(0.0, 540.0, 0.0);
    ToNormalize.Normalize();
    if (!ToNormalize.opEquals(FRotator(0.0, 180.0, 0.0)))
    {
        return -15;
    }

    FRotator Winding;
    FRotator Remainder;
    FRotator(450.0, -540.0, 30.0).GetWindingAndRemainder(Winding, Remainder);
    if (!(Winding + Remainder).opEquals(FRotator(450.0, -540.0, 30.0)))
    {
        return -16;
    }

    if (!Remainder.opEquals(Remainder.GetNormalized()))
    {
        return -17;
    }

    if (!UnrealNearlyEqual(FRotator(10.0, 20.0, 30.0).GetManhattanDistance(FRotator(1.0, 2.0, 3.0)), 54.0, 0.0001))
    {
        return -18;
    }

    FRotator Equivalent = FRotator(10.0, 20.0, 30.0).GetEquivalentRotator();
    if (!Equivalent.opEquals(FRotator(170.0, 200.0, 210.0)))
    {
        return -19;
    }

    FRotator Closest = Equivalent;
    FRotator(10.0, 20.0, 30.0).SetClosestToMe(Closest);
    if (!Closest.opEquals(FRotator(10.0, 20.0, 30.0)))
    {
        return -20;
    }

    FString FullText = FRotator(10.0, 20.0, 30.0).ToString();
    if (!FullText.StartsWith("P="))
    {
        return -21;
    }

    FString CompactText = FRotator(10.0, 20.0, 30.0).ToCompactString();
    if (CompactText.IsEmpty())
    {
        return -22;
    }

    FRotator Parsed;
    if (!Parsed.InitFromString("P=10.0 Y=20.0 R=30.0"))
    {
        return -23;
    }

    if (!Parsed.opEquals(FRotator(10.0, 20.0, 30.0)))
    {
        return -24;
    }

    if (!UnrealRotatorWithNaN().ContainsNaN())
    {
        return -25;
    }

    return 25;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorInstanceMethods", Script, "int RunInstanceMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 25));
	}

	TEST_METHOD(StaticNamespaceFunctions)
	{
		static const char Script[] = R"(
int RunStaticNamespaceFunctions()
{
    if (!UnrealNearlyEqual(FRotator::ClampAxis(-10.0), 350.0, 0.0001))
    {
        return -1;
    }

    if (!UnrealNearlyEqual(FRotator::NormalizeAxis(350.0), -10.0, 0.0001))
    {
        return -2;
    }

    uint8 ByteValue = FRotator::CompressAxisToByte(90.0);
    if (!UnrealNearlyEqual(FRotator::DecompressAxisFromByte(ByteValue), 90.0, 2.0))
    {
        return -3;
    }

    uint16 ShortValue = FRotator::CompressAxisToShort(180.0);
    if (!UnrealNearlyEqual(FRotator::DecompressAxisFromShort(ShortValue), 180.0, 0.1))
    {
        return -4;
    }

    if (!FRotator::MakeFromEuler(FVector(10.0, 20.0, 30.0)).Euler().Equals(FVector(10.0, 20.0, 30.0), 0.0001))
    {
        return -5;
    }

    return 5;
}
)";

		asIScriptFunction* Function = BuildFunction("FRotatorStaticFunctions", Script, "int RunStaticNamespaceFunctions()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 5));
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
