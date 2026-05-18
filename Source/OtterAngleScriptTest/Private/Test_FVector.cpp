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

	TEST_METHOD(ConstructorsOperatorsAndComponents)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector2D")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FQuat")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FPlane")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FText")));

		static const char Script[] = R"(
int RunConstructorsOperatorsAndComponents()
{
    FVector Scalar(2.0);
    if (Scalar.X != 2.0 || Scalar.Y != 2.0 || Scalar.Z != 2.0)
    {
        return -1;
    }

    FVector2D Base2D(4.0, 5.0);
    if (!Base2D.Equals(FVector2D(4.0, 5.0), 0.0))
    {
        return -2;
    }

    FVector From2D(Base2D, 6.0);
    if (!From2D.opEquals(FVector(4.0, 5.0, 6.0)))
    {
        return -3;
    }

    FVector Value(1.0, 2.0, 3.0);
    FVector Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -4;
    }

    FVector Assigned;
    Assigned = Value;
    if (!Assigned.opEquals(Value))
    {
        return -5;
    }

    if (!(Value ^ FVector(0.0, 0.0, 1.0)).opEquals(FVector(2.0, -1.0, 0.0)))
    {
        return -6;
    }

    if (!Value.Cross(FVector(0.0, 0.0, 1.0)).opEquals(FVector(2.0, -1.0, 0.0)))
    {
        return -7;
    }

    if (!FMath::IsNearlyEqual(Value | FVector(2.0, 0.0, 1.0), 5.0, 0.0001))
    {
        return -8;
    }

    if (!FMath::IsNearlyEqual(Value.Dot(FVector(2.0, 0.0, 1.0)), 5.0, 0.0001))
    {
        return -9;
    }

    if (!(Value + FVector(4.0, 5.0, 6.0)).opEquals(FVector(5.0, 7.0, 9.0)))
    {
        return -10;
    }

    if (!(Value - FVector(1.0, 1.0, 1.0)).opEquals(FVector(0.0, 1.0, 2.0)))
    {
        return -11;
    }

    if (!(Value + 1.0).opEquals(FVector(2.0, 3.0, 4.0)))
    {
        return -12;
    }

    if (!(Value - 1.0).opEquals(FVector(0.0, 1.0, 2.0)))
    {
        return -13;
    }

    if (!(Value * 2.0).opEquals(FVector(2.0, 4.0, 6.0)))
    {
        return -14;
    }

    if (!((Value * 2.0) / 2.0).opEquals(Value))
    {
        return -15;
    }

    if (!(Value * FVector(2.0, 3.0, 4.0)).opEquals(FVector(2.0, 6.0, 12.0)))
    {
        return -16;
    }

    if (!((Value * FVector(2.0, 3.0, 4.0)) / FVector(2.0, 3.0, 4.0)).opEquals(Value))
    {
        return -17;
    }

    if (!(-Value).opEquals(FVector(-1.0, -2.0, -3.0)))
    {
        return -18;
    }

    FVector Mutating(1.0, 2.0, 3.0);
    Mutating += FVector(1.0, 1.0, 1.0);
    Mutating -= FVector(1.0, 0.0, 1.0);
    Mutating *= 2.0;
    Mutating /= 2.0;
    Mutating *= FVector(2.0, 1.0, 0.5);
    Mutating /= FVector(2.0, 1.0, 0.5);
    if (!Mutating.opEquals(FVector(1.0, 3.0, 3.0)))
    {
        return -19;
    }

    Value[0] = 4.0;
    if (!FMath::IsNearlyEqual(Value[0], 4.0, 0.0001))
    {
        return -20;
    }

    Value.Component(1) = 7.0;
    if (!FMath::IsNearlyEqual(Value.Component(1), 7.0, 0.0001))
    {
        return -21;
    }

    if (!Value.IsValidIndex(2) || Value.IsValidIndex(3))
    {
        return -22;
    }

    Value.SetComponentForAxis(Y, 8.0);
    if (!FMath::IsNearlyEqual(Value.GetComponentForAxis(Y), 8.0, 0.0001))
    {
        return -23;
    }

    Value.Set(9.0, 10.0, 11.0);
    if (!Value.Equals(FVector(9.0, 10.0, 11.0), 0.0))
    {
        return -24;
    }

    if (!FVector(3.0, 3.0002, 3.0001).AllComponentsEqual(0.001))
    {
        return -25;
    }

    return 25;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorConstructorsOperators", Script, "int RunConstructorsOperatorsAndComponents()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 25));
	}

	TEST_METHOD(MagnitudeNormalizationAndBounds)
	{
		static const char Script[] = R"(
int RunMagnitudeNormalizationAndBounds()
{
    FVector Mixed(-2.0, 5.0, -4.0);
    if (!FMath::IsNearlyEqual(Mixed.GetMax(), 5.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(Mixed.GetAbsMax(), 5.0, 0.0001))
    {
        return -2;
    }

    if (!FMath::IsNearlyEqual(Mixed.GetMin(), -4.0, 0.0001))
    {
        return -3;
    }

    if (!FMath::IsNearlyEqual(Mixed.GetAbsMin(), 2.0, 0.0001))
    {
        return -4;
    }

    if (!Mixed.ComponentMin(FVector(-3.0, 1.0, -5.0)).opEquals(FVector(-3.0, 1.0, -5.0)))
    {
        return -5;
    }

    if (!Mixed.ComponentMax(FVector(-3.0, 1.0, -5.0)).opEquals(FVector(-2.0, 5.0, -4.0)))
    {
        return -6;
    }

    if (!Mixed.GetAbs().opEquals(FVector(2.0, 5.0, 4.0)))
    {
        return -7;
    }

    FVector LengthVector(3.0, 4.0, 12.0);
    if (!FMath::IsNearlyEqual(LengthVector.Size(), 13.0, 0.0001))
    {
        return -8;
    }

    if (!FMath::IsNearlyEqual(LengthVector.Length(), 13.0, 0.0001))
    {
        return -9;
    }

    if (!FMath::IsNearlyEqual(LengthVector.SizeSquared(), 169.0, 0.0001))
    {
        return -10;
    }

    if (!FMath::IsNearlyEqual(LengthVector.SquaredLength(), 169.0, 0.0001))
    {
        return -11;
    }

    if (!FMath::IsNearlyEqual(LengthVector.Size2D(), 5.0, 0.0001))
    {
        return -12;
    }

    if (!FMath::IsNearlyEqual(LengthVector.SizeSquared2D(), 25.0, 0.0001))
    {
        return -13;
    }

    if (!FVector(0.0, 0.0, 0.00001).IsNearlyZero(0.001))
    {
        return -14;
    }

    if (!FVector().IsZero())
    {
        return -15;
    }

    if (!FVector(1.0, 0.0, 0.0).IsUnit())
    {
        return -16;
    }

    if (!FVector(1.0, 0.0, 0.0).IsUnit(0.0001))
    {
        return -17;
    }

    if (!FVector(1.0, 0.0, 0.0).IsNormalized())
    {
        return -18;
    }

    FVector Normalized(3.0, 0.0, 4.0);
    if (!Normalized.Normalize())
    {
        return -19;
    }

    if (!Normalized.Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -20;
    }

    FVector Tiny(0.0, 0.0, 0.0);
    if (Tiny.Normalize(0.1))
    {
        return -21;
    }

    if (!FVector(3.0, 0.0, 4.0).GetUnsafeNormal().Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -22;
    }

    if (!FVector(3.0, 0.0, 4.0).GetSafeNormal().Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -23;
    }

    if (!FVector(3.0, 0.0, 4.0).GetSafeNormal(0.1).Equals(FVector(0.6, 0.0, 0.8), 0.0001))
    {
        return -24;
    }

    if (!FVector().GetSafeNormal(0.1, FVector(9.0, 8.0, 7.0)).opEquals(FVector(9.0, 8.0, 7.0)))
    {
        return -25;
    }

    if (!FVector(3.0, 4.0, 9.0).GetSafeNormal2D().Equals(FVector(0.6, 0.8, 0.0), 0.0001))
    {
        return -26;
    }

    if (!FVector(3.0, 4.0, 9.0).GetSafeNormal2D(0.1).Equals(FVector(0.6, 0.8, 0.0), 0.0001))
    {
        return -27;
    }

    if (!FVector().GetSafeNormal2D(0.1, FVector(1.0, 2.0, 3.0)).opEquals(FVector(1.0, 2.0, 3.0)))
    {
        return -28;
    }

    FVector OutDir;
    double OutLength = 0.0;
    FVector(3.0, 4.0, 0.0).ToDirectionAndLength(OutDir, OutLength);
    if (!OutDir.Equals(FVector(0.6, 0.8, 0.0), 0.0001) || !FMath::IsNearlyEqual(OutLength, 5.0, 0.0001))
    {
        return -29;
    }

    float OutLengthFloat = 0.0f;
    FVector(0.0, 3.0, 4.0).ToDirectionAndLength(OutDir, OutLengthFloat);
    if (!OutDir.Equals(FVector(0.0, 0.6, 0.8), 0.0001) || !FMath::IsNearlyEqual(OutLengthFloat, 5.0, 0.0001))
    {
        return -30;
    }

    if (!FVector(-1.0, 0.0, 2.0).GetSignVector().opEquals(FVector(-1.0, 1.0, 1.0)))
    {
        return -31;
    }

    if (!FVector(4.0, 6.0, 2.0).Projection().Equals(FVector(2.0, 3.0, 1.0), 0.0001))
    {
        return -32;
    }

    if (!FVector(3.0, 4.0, 9.0).GetUnsafeNormal2D().Equals(FVector(0.6, 0.8, 0.0), 0.0001))
    {
        return -33;
    }

    if (!FVector(5.2, 7.8, 9.1).GridSnap(2.0).opEquals(FVector(6.0, 8.0, 10.0)))
    {
        return -34;
    }

    if (!FVector(10.0, -10.0, 1.0).BoundToCube(3.0).opEquals(FVector(3.0, -3.0, 1.0)))
    {
        return -35;
    }

    if (!FVector(10.0, -10.0, 1.0).BoundToBox(FVector(-1.0, -2.0, -3.0), FVector(1.0, 2.0, 3.0)).opEquals(FVector(1.0, -2.0, 1.0)))
    {
        return -36;
    }

    if (!FVector(10.0, 0.0, 0.0).GetClampedToSize(2.0, 4.0).Equals(FVector(4.0, 0.0, 0.0), 0.0001))
    {
        return -37;
    }

    if (!FVector(3.0, 4.0, 7.0).GetClampedToSize2D(2.0, 4.0).Equals(FVector(2.4, 3.2, 7.0), 0.0001))
    {
        return -38;
    }

    if (!FVector(10.0, 0.0, 0.0).GetClampedToMaxSize(4.0).Equals(FVector(4.0, 0.0, 0.0), 0.0001))
    {
        return -39;
    }

    if (!FVector(6.0, 8.0, 9.0).GetClampedToMaxSize2D(5.0).Equals(FVector(3.0, 4.0, 9.0), 0.0001))
    {
        return -40;
    }

    FVector AddBoundedDefaultValue(1.0, 2.0, 3.0);
    AddBoundedDefaultValue.AddBounded(FVector(1.0, 1.0, 1.0));
    if (!AddBoundedDefaultValue.opEquals(FVector(2.0, 3.0, 4.0)))
    {
        return -41;
    }

    FVector AddBoundedRadiusValue(1.0, 1.0, 1.0);
    AddBoundedRadiusValue.AddBounded(FVector(10.0, 0.0, 0.0), 3.0);
    if (AddBoundedRadiusValue.Size() > 3.0001)
    {
        return -42;
    }

    if (!FVector(2.0, 4.0, -4.0).Reciprocal().opEquals(FVector(0.5, 0.25, -0.25)))
    {
        return -43;
    }

    if (!FVector(5.0, 5.0, 5.0002).IsUniform(0.001))
    {
        return -44;
    }

    return 44;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorMagnitudeNormalization", Script, "int RunMagnitudeNormalizationAndBounds()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 44));
	}

	TEST_METHOD(ProjectionRotationAndTextMethods)
	{
		static const char Script[] = R"(
int RunProjectionRotationAndTextMethods()
{
    if (!FVector(1.0, -2.0, 0.0).MirrorByVector(FVector(0.0, 1.0, 0.0)).opEquals(FVector(1.0, 2.0, 0.0)))
    {
        return -1;
    }

    if (!FVector(1.0, 2.0, 3.0).MirrorByPlane(FPlane(0.0, 0.0, 1.0, 0.0)).Equals(FVector(1.0, 2.0, -3.0), 0.0001))
    {
        return -2;
    }

    if (!FVector(1.0, 0.0, 0.0).RotateAngleAxis(90.0, FVector(0.0, 0.0, 1.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -3;
    }

    if (!FVector(1.0, 0.0, 0.0).RotateAngleAxisRad(1.5707963267948966, FVector(0.0, 0.0, 1.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -4;
    }

    if (!FMath::IsNearlyEqual(FVector(1.0, 0.0, 5.0).CosineAngle2D(FVector(0.0, 2.0, 7.0)), 0.0, 0.0001))
    {
        return -5;
    }

    if (!FVector(3.0, 4.0, 0.0).ProjectOnTo(FVector(1.0, 0.0, 0.0)).Equals(FVector(3.0, 0.0, 0.0), 0.0001))
    {
        return -6;
    }

    if (!FVector(3.0, 4.0, 0.0).ProjectOnToNormal(FVector(0.0, 1.0, 0.0)).Equals(FVector(0.0, 4.0, 0.0), 0.0001))
    {
        return -7;
    }

    FRotator Orientation = FVector(1.0, 0.0, 0.0).ToOrientationRotator();
    if (!Orientation.Equals(FRotator(0.0, 0.0, 0.0), 0.0001))
    {
        return -8;
    }

    if (!FVector(1.0, 0.0, 0.0).Rotation().Equals(FRotator(0.0, 0.0, 0.0), 0.0001))
    {
        return -9;
    }

    if (!FVector(1.0, 0.0, 0.0).ToOrientationQuat().Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -10;
    }

    FVector Axis1;
    FVector Axis2;
    FVector(0.0, 0.0, 1.0).FindBestAxisVectors(Axis1, Axis2);
    if (!FMath::IsNearlyEqual(Axis1 | FVector(0.0, 0.0, 1.0), 0.0, 0.0001))
    {
        return -11;
    }

    if (!FMath::IsNearlyEqual(Axis2 | FVector(0.0, 0.0, 1.0), 0.0, 0.0001))
    {
        return -12;
    }

    if (!FMath::IsNearlyEqual(Axis1 | Axis2, 0.0, 0.0001))
    {
        return -13;
    }

    if (!FMath::IsNearlyEqual(Axis1.Size(), 1.0, 0.0001) || !FMath::IsNearlyEqual(Axis2.Size(), 1.0, 0.0001))
    {
        return -14;
    }

    FVector Euler(270.0, -190.0, 540.0);
    Euler.UnwindEuler();
    if (Euler.X < -180.0 || Euler.X > 180.0 || Euler.Y < -180.0 || Euler.Y > 180.0 || Euler.Z < -180.0 || Euler.Z > 180.0)
    {
        return -15;
    }

    if (!UnrealVectorWithNaN().ContainsNaN())
    {
        return -16;
    }

    FString FullText = FVector(1.0, 2.0, 3.0).ToString();
    if (!FullText.StartsWith("X="))
    {
        return -17;
    }

    FText Localized = FVector(1.0, 2.0, 3.0).ToText();
    if (Localized.ToString().IsEmpty())
    {
        return -18;
    }

    FString Compact = FVector(1.0, 2.0, 3.0).ToCompactString();
    if (Compact.IsEmpty())
    {
        return -19;
    }

    FText CompactText = FVector(1.0, 2.0, 3.0).ToCompactText();
    if (CompactText.ToString().IsEmpty())
    {
        return -20;
    }

    FVector Parsed;
    if (!Parsed.InitFromString("X=1.5 Y=2.5 Z=3.5"))
    {
        return -21;
    }

    if (!Parsed.Equals(FVector(1.5, 2.5, 3.5), 0.0001))
    {
        return -22;
    }

    FVector ParsedCompact;
    if (!ParsedCompact.InitFromCompactString(Compact))
    {
        return -23;
    }

    if (!ParsedCompact.Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -24;
    }

    FVector2D Spherical = FVector(1.0, 0.0, 0.0).UnitCartesianToSpherical();
    if (!FMath::IsNearlyEqual(Spherical.X, 1.5707963267948966, 0.0001))
    {
        return -25;
    }

    if (!FMath::IsNearlyEqual(Spherical.Y, 0.0, 0.0001))
    {
        return -26;
    }

    if (!FMath::IsNearlyEqual(FVector(0.0, 1.0, 0.0).HeadingAngle(), 1.5707963267948966, 0.0001))
    {
        return -27;
    }

    return 27;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorProjectionRotationText", Script, "int RunProjectionRotationAndTextMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 27));
	}

	TEST_METHOD(StaticNamespaceFunctions)
	{
		static const char Script[] = R"(
int RunStaticNamespaceFunctions()
{
    if (!FVector::CrossProduct(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 1.0)).opEquals(FVector(2.0, -1.0, 0.0)))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(FVector::DotProduct(FVector(1.0, 2.0, 3.0), FVector(2.0, 0.0, 1.0)), 5.0, 0.0001))
    {
        return -2;
    }

    if (!FVector::SlerpVectorToDirection(FVector(2.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0), 0.5).Equals(FVector(1.41421356237, 1.41421356237, 0.0), 0.001))
    {
        return -3;
    }

    if (!FVector::SlerpNormals(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0), 0.5).Equals(FVector(0.70710678118, 0.70710678118, 0.0), 0.001))
    {
        return -4;
    }

    FVector XAxis(1.0, 0.0, 0.0);
    FVector YAxis(0.0, 2.0, 0.0);
    FVector ZAxis(0.0, 0.0, 3.0);
    FVector::CreateOrthonormalBasis(XAxis, YAxis, ZAxis);
    if (!FMath::IsNearlyEqual(XAxis.Size(), 1.0, 0.0001) || !FMath::IsNearlyEqual(YAxis.Size(), 1.0, 0.0001) || !FMath::IsNearlyEqual(ZAxis.Size(), 1.0, 0.0001))
    {
        return -5;
    }

    if (!FVector::PointsAreSame(FVector(1.0, 1.0, 1.0), FVector(1.0, 1.0, 1.0)))
    {
        return -6;
    }

    if (!FVector::PointsAreNear(FVector(1.0, 1.0, 1.0), FVector(1.05, 1.0, 1.0), 0.1))
    {
        return -7;
    }

    if (!FMath::IsNearlyEqual(FVector::PointPlaneDist(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0)), 3.0, 0.0001))
    {
        return -8;
    }

    if (!FVector::PointPlaneProject(FVector(1.0, 2.0, 3.0), FPlane(0.0, 0.0, 1.0, 0.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -9;
    }

    if (!FVector::PointPlaneProject(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 0.0), FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -10;
    }

    if (!FVector::PointPlaneProject(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -11;
    }

    if (!FVector::VectorPlaneProject(FVector(1.0, 2.0, 3.0), FVector(0.0, 0.0, 1.0)).Equals(FVector(1.0, 2.0, 0.0), 0.0001))
    {
        return -12;
    }

    if (!FMath::IsNearlyEqual(FVector::Dist(FVector(0.0, 0.0, 0.0), FVector(3.0, 4.0, 0.0)), 5.0, 0.0001))
    {
        return -13;
    }

    if (!FMath::IsNearlyEqual(FVector::Distance(FVector(0.0, 0.0, 0.0), FVector(3.0, 4.0, 0.0)), 5.0, 0.0001))
    {
        return -14;
    }

    if (!FMath::IsNearlyEqual(FVector::DistXY(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 5.0, 0.0001))
    {
        return -15;
    }

    if (!FMath::IsNearlyEqual(FVector::Dist2D(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 5.0, 0.0001))
    {
        return -16;
    }

    if (!FMath::IsNearlyEqual(FVector::DistSquared(FVector(0.0, 0.0, 0.0), FVector(3.0, 4.0, 12.0)), 169.0, 0.0001))
    {
        return -17;
    }

    if (!FMath::IsNearlyEqual(FVector::DistSquaredXY(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 25.0, 0.0001))
    {
        return -18;
    }

    if (!FMath::IsNearlyEqual(FVector::DistSquared2D(FVector(0.0, 0.0, 9.0), FVector(3.0, 4.0, 1.0)), 25.0, 0.0001))
    {
        return -19;
    }

    if (!FMath::IsNearlyEqual(FVector::BoxPushOut(FVector(0.0, 0.0, 1.0), FVector(1.0, 2.0, 3.0)), 3.0, 0.0001))
    {
        return -20;
    }

    if (!FVector::Min(FVector(1.0, 5.0, 3.0), FVector(2.0, 4.0, 6.0)).opEquals(FVector(1.0, 4.0, 3.0)))
    {
        return -21;
    }

    if (!FVector::Max(FVector(1.0, 5.0, 3.0), FVector(2.0, 4.0, 6.0)).opEquals(FVector(2.0, 5.0, 6.0)))
    {
        return -22;
    }

    if (!FVector::Min3(FVector(3.0, 7.0, 5.0), FVector(2.0, 6.0, 4.0), FVector(1.0, 8.0, 3.0)).opEquals(FVector(1.0, 6.0, 3.0)))
    {
        return -23;
    }

    if (!FVector::Max3(FVector(3.0, 7.0, 5.0), FVector(2.0, 6.0, 4.0), FVector(1.0, 8.0, 3.0)).opEquals(FVector(3.0, 8.0, 5.0)))
    {
        return -24;
    }

    if (!FVector::Parallel(FVector(1.0, 0.0, 0.0), FVector(1.0, 0.0, 0.0)))
    {
        return -25;
    }

    if (!FVector::Parallel(FVector(1.0, 0.0, 0.0), FVector(0.999, 0.01, 0.0), 0.99))
    {
        return -26;
    }

    if (!FVector::Coincident(FVector(1.0, 0.0, 0.0), FVector(1.0, 0.0, 0.0)))
    {
        return -27;
    }

    if (!FVector::Coincident(FVector(1.0, 0.0, 0.0), FVector(0.999, 0.01, 0.0), 0.99))
    {
        return -28;
    }

    if (!FVector::Orthogonal(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0)))
    {
        return -29;
    }

    if (!FVector::Orthogonal(FVector(1.0, 0.0, 0.0), FVector(0.05, 1.0, 0.0), 0.1))
    {
        return -30;
    }

    if (!FVector::Coplanar(FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0), FVector(1.0, 1.0, 0.0), FVector(0.0, 0.0, 1.0)))
    {
        return -31;
    }

    if (!FVector::Coplanar(FVector(0.0, 0.0, 0.0), FVector(0.0, 0.0, 1.0), FVector(1.0, 1.0, 0.0), FVector(0.0, 0.0, 1.0), 0.999))
    {
        return -32;
    }

    if (!FMath::IsNearlyEqual(FVector::Triple(FVector(1.0, 0.0, 0.0), FVector(0.0, 1.0, 0.0), FVector(0.0, 0.0, 1.0)), 1.0, 0.0001))
    {
        return -33;
    }

    if (!FVector::RadiansToDegrees(FVector(3.141592653589793, 1.5707963267948966, 0.0)).Equals(FVector(180.0, 90.0, 0.0), 0.0001))
    {
        return -34;
    }

    if (!FVector::DegreesToRadians(FVector(180.0, 90.0, 0.0)).Equals(FVector(3.141592653589793, 1.5707963267948966, 0.0), 0.0001))
    {
        return -35;
    }

    return 35;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorStaticFunctions", Script, "int RunStaticNamespaceFunctions()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 35));
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
