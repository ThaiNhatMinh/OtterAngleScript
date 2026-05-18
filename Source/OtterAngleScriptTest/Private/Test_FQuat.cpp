// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Quat.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

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

	TEST_METHOD(ConstructorsOperatorsAndComparisons)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FQuat")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));

		static const char Script[] = R"(
int RunConstructorsOperatorsAndComparisons()
{
    const double HalfPi = 1.5707963267948966;
    const double InvSqrtTwo = 0.7071067811865476;

    FQuat Identity(ForceInit);
    if (!Identity.opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -1;
    }

    FQuat Zeroed(ForceInitToZero);
    if (!Zeroed.opEquals(FQuat(0.0, 0.0, 0.0, 0.0)))
    {
        return -2;
    }

    FQuat Value(0.0, 0.0, InvSqrtTwo, InvSqrtTwo);
    FQuat Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -3;
    }

    FQuat Assigned(ForceInit);
    Assigned = Value;
    if (!Assigned.opEquals(Value))
    {
        return -4;
    }

    FQuat FromRotator(FRotator(0.0, 90.0, 0.0));
    if (!FromRotator.RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -5;
    }

    FQuat FromAxisAngle(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!FromAxisAngle.RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -6;
    }

    if (!(FQuat(1.0, 2.0, 3.0, 4.0) + FQuat(4.0, 3.0, 2.0, 1.0)).opEquals(FQuat(5.0, 5.0, 5.0, 5.0)))
    {
        return -7;
    }

    if (!(FQuat(5.0, 5.0, 5.0, 5.0) - FQuat(1.0, 2.0, 3.0, 4.0)).opEquals(FQuat(4.0, 3.0, 2.0, 1.0)))
    {
        return -8;
    }

    if (!FMath::IsNearlyEqual(FQuat(1.0, 2.0, 3.0, 4.0) | FQuat(4.0, 3.0, 2.0, 1.0), 20.0, 0.0001))
    {
        return -9;
    }

    if (!(FromAxisAngle * FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -10;
    }

    if (!((FromAxisAngle * FromAxisAngle) * FVector(1.0, 0.0, 0.0)).Equals(FVector(-1.0, 0.0, 0.0), 0.0001))
    {
        return -11;
    }

    if (!(FQuat(1.0, 2.0, 3.0, 4.0) * 2.0).opEquals(FQuat(2.0, 4.0, 6.0, 8.0)))
    {
        return -12;
    }

    if (!(2.0 * FQuat(1.0, 2.0, 3.0, 4.0)).opEquals(FQuat(2.0, 4.0, 6.0, 8.0)))
    {
        return -13;
    }

    if (!(FQuat(2.0, 4.0, 6.0, 8.0) / 2.0).opEquals(FQuat(1.0, 2.0, 3.0, 4.0)))
    {
        return -14;
    }

    FQuat Mutating(1.0, 2.0, 3.0, 4.0);
    Mutating += FQuat(4.0, 3.0, 2.0, 1.0);
    Mutating -= FQuat(1.0, 1.0, 1.0, 1.0);
    if (!Mutating.opEquals(FQuat(4.0, 4.0, 4.0, 4.0)))
    {
        return -15;
    }

    FQuat ScaleMutating(1.0, 2.0, 3.0, 4.0);
    ScaleMutating *= 2.0;
    ScaleMutating /= 4.0;
    if (!ScaleMutating.opEquals(FQuat(0.5, 1.0, 1.5, 2.0)))
    {
        return -16;
    }

    FQuat Compose = FromAxisAngle;
    Compose *= FromAxisAngle;
    if (!(Compose * FVector(1.0, 0.0, 0.0)).Equals(FVector(-1.0, 0.0, 0.0), 0.0001))
    {
        return -17;
    }

    if (!FQuat(0.0, 0.0, 0.0, -1.0).Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -18;
    }

    if (FQuat(0.0, 0.0, 0.0, -1.0).opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -19;
    }

    return 19;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatConstructorsOperators", Script, "int RunConstructorsOperatorsAndComparisons()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 19));
	}

	TEST_METHOD(InstanceMethods)
	{
		static const char Script[] = R"(
int RunInstanceMethods()
{
    const double HalfPi = 1.5707963267948966;

    FQuat Identity(ForceInit);
    if (!Identity.IsIdentity())
    {
        return -1;
    }

    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    if (!QuarterTurn.Euler().Equals(FVector(0.0, 90.0, 0.0), 0.0001))
    {
        return -2;
    }

    FQuat ToNormalize(0.0, 0.0, 0.0, 2.0);
    ToNormalize.Normalize();
    if (!ToNormalize.opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -3;
    }

    if (!FQuat(0.0, 0.0, 0.0, 2.0).GetNormalized().opEquals(FQuat(0.0, 0.0, 0.0, 1.0)))
    {
        return -4;
    }

    if (!Identity.IsNormalized())
    {
        return -5;
    }

    if (!FMath::IsNearlyEqual(FQuat(1.0, 2.0, 2.0, 0.0).Size(), 3.0, 0.0001))
    {
        return -6;
    }

    if (!FMath::IsNearlyEqual(FQuat(1.0, 2.0, 2.0, 0.0).SizeSquared(), 9.0, 0.0001))
    {
        return -7;
    }

    if (!FMath::IsNearlyEqual(QuarterTurn.GetAngle(), HalfPi, 0.0001))
    {
        return -8;
    }

    FVector Axis;
    double Angle = 0.0;
    QuarterTurn.ToAxisAndAngle(Axis, Angle);
    if (!Axis.Equals(FVector(0.0, 0.0, 1.0), 0.0001) || !FMath::IsNearlyEqual(Angle, HalfPi, 0.0001))
    {
        return -9;
    }

    FQuat Swing;
    FQuat Twist;
    QuarterTurn.ToSwingTwist(FVector(0.0, 0.0, 1.0), Swing, Twist);
    if (!Swing.IsIdentity(0.0001) || !Twist.Equals(QuarterTurn, 0.0001))
    {
        return -10;
    }

    if (!QuarterTurn.RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -11;
    }

    if (!QuarterTurn.UnrotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -12;
    }

    if (!QuarterTurn.Log().Exp().Equals(QuarterTurn, 0.0001))
    {
        return -13;
    }

    if (!QuarterTurn.Inverse().RotateVector(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -14;
    }

    FQuat Shortest(0.0, 0.0, 0.0, -1.0);
    Shortest.EnforceShortestArcWith(Identity);
    if (!Shortest.opEquals(Identity))
    {
        return -15;
    }

    if (!QuarterTurn.GetAxisX().Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -16;
    }

    if (!QuarterTurn.GetAxisY().Equals(FVector(-1.0, 0.0, 0.0), 0.0001))
    {
        return -17;
    }

    if (!QuarterTurn.GetAxisZ().Equals(FVector(0.0, 0.0, 1.0), 0.0001))
    {
        return -18;
    }

    if (!QuarterTurn.GetForwardVector().Equals(QuarterTurn.GetAxisX(), 0.0001))
    {
        return -19;
    }

    if (!QuarterTurn.GetRightVector().Equals(QuarterTurn.GetAxisY(), 0.0001))
    {
        return -20;
    }

    if (!QuarterTurn.GetUpVector().Equals(QuarterTurn.GetAxisZ(), 0.0001))
    {
        return -21;
    }

    if (!QuarterTurn.Vector().Equals(QuarterTurn.GetAxisX(), 0.0001))
    {
        return -22;
    }

    if (!QuarterTurn.Rotator().Equals(FRotator(0.0, 90.0, 0.0), 0.0001))
    {
        return -23;
    }

    if (!QuarterTurn.GetRotationAxis().Equals(FVector(0.0, 0.0, 1.0), 0.0001))
    {
        return -24;
    }

    if (!FMath::IsNearlyEqual(Identity.AngularDistance(QuarterTurn), HalfPi, 0.0001))
    {
        return -25;
    }

    FString Text = QuarterTurn.ToString();
    if (!Text.StartsWith("X="))
    {
        return -26;
    }

    FQuat Parsed;
    if (!Parsed.InitFromString("X=0.0 Y=0.0 Z=0.7071067811865476 W=0.7071067811865476"))
    {
        return -27;
    }

    if (!Parsed.Equals(QuarterTurn, 0.0001))
    {
        return -28;
    }

    if (!UnrealQuatWithNaN().ContainsNaN())
    {
        return -29;
    }

    return 29;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatInstanceMethods", Script, "int RunInstanceMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 29));
	}

	TEST_METHOD(StaticNamespaceFunctions)
	{
		static const char Script[] = R"(
int RunStaticNamespaceFunctions()
{
    const double HalfPi = 1.5707963267948966;
    const double InvSqrtTwo = 0.7071067811865476;

    FQuat Identity(ForceInit);
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);

    if (!FQuat::MakeFromEuler(FVector(0.0, 90.0, 0.0)).Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }

    if (!FQuat::FindBetween(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -2;
    }

    if (!FQuat::FindBetweenNormals(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -3;
    }

    if (!FQuat::FindBetweenVectors(FVector(2.0, 0.0, 0.0), FVector(0.0, 3.0, 0.0)).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -4;
    }

    if (!FMath::IsNearlyEqual(FQuat::Error(Identity, Identity), 0.0, 0.0001))
    {
        return -5;
    }

    if (!FMath::IsNearlyEqual(FQuat::ErrorAutoNormalize(FQuat(0.0, 0.0, 0.0, 2.0), FQuat(0.0, 0.0, 0.0, 5.0)), 0.0, 0.0001))
    {
        return -6;
    }

    if (!FQuat::FastLerp(Identity, Identity, 0.5).opEquals(Identity))
    {
        return -7;
    }

    if (!FQuat::FastBilerp(Identity, Identity, Identity, Identity, 0.5, 0.5).opEquals(Identity))
    {
        return -8;
    }

    if (!FQuat::Slerp_NotNormalized(Identity, Identity, 0.25).opEquals(Identity))
    {
        return -9;
    }

    if (!FQuat::Slerp(Identity, QuarterTurn, 0.5).RotateVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(InvSqrtTwo, InvSqrtTwo, 0.0), 0.0001))
    {
        return -10;
    }

    if (!FQuat::SlerpFullPath_NotNormalized(Identity, Identity, 0.5).opEquals(Identity))
    {
        return -11;
    }

    if (!FQuat::SlerpFullPath(Identity, Identity, 0.5).opEquals(Identity))
    {
        return -12;
    }

    if (!FQuat::Squad(Identity, Identity, Identity, Identity, 0.5).opEquals(Identity))
    {
        return -13;
    }

    if (!FQuat::SquadFullPath(Identity, Identity, Identity, Identity, 0.5).opEquals(Identity))
    {
        return -14;
    }

    FQuat Tangent;
    FQuat::CalcTangents(Identity, Identity, Identity, 0.0, Tangent);
    if (!Tangent.IsIdentity(0.0001))
    {
        return -15;
    }

    return 15;
}
)";

		asIScriptFunction* Function = BuildFunction("FQuatStaticFunctions", Script, "int RunStaticNamespaceFunctions()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 15));
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
