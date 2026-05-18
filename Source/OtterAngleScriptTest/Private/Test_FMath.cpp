// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/UnrealMathUtility.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFMathTests,
	"OtterAngleScript.FMath",
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
		Engine = GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));

		ScriptModule = Engine->GetModule("OtterAngleScriptMathTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(BasicScalarFunctions)
	{
		static const char Script[] = R"(
int RunBasicScalarFunctions()
{
    if (FMath::Abs(-7) != 7 || FMath::Abs(int64(-9)) != int64(9))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(FMath::Abs(-2.5), 2.5, 0.000001))
    {
        return -2;
    }

    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Sin(Pi * 0.5), 1.0, 0.000001))
    {
        return -3;
    }
    if (!FMath::IsNearlyEqual(FMath::Cos(0.0), 1.0, 0.000001))
    {
        return -4;
    }
    if (!FMath::IsNearlyEqual(FMath::Tan(Pi * 0.25), 1.0, 0.0001))
    {
        return -5;
    }
    if (!FMath::IsNearlyEqual(FMath::Asin(0.5), Pi / 6.0, 0.0001))
    {
        return -6;
    }
    if (!FMath::IsNearlyEqual(FMath::Acos(0.5), Pi / 3.0, 0.0001))
    {
        return -7;
    }
    if (!FMath::IsNearlyEqual(FMath::Atan(1.0), Pi / 4.0, 0.0001))
    {
        return -8;
    }
    if (!FMath::IsNearlyEqual(FMath::Atan2(1.0, 1.0), Pi / 4.0, 0.0001))
    {
        return -9;
    }
    if (!FMath::IsNearlyEqual(FMath::Sqrt(81.0), 9.0, 0.000001))
    {
        return -10;
    }
    if (!FMath::IsNearlyEqual(FMath::Pow(3.0, 4.0), 81.0, 0.000001))
    {
        return -11;
    }
    if (!FMath::IsNearlyEqual(FMath::Fmod(7.5, 2.0), 1.5, 0.000001))
    {
        return -12;
    }
    if (!FMath::IsNearlyEqual(FMath::Exp(1.0), 2.718281828459045, 0.00001))
    {
        return -13;
    }
    if (!FMath::IsNearlyEqual(FMath::Loge(2.718281828459045), 1.0, 0.00001))
    {
        return -14;
    }
    if (!FMath::IsNearlyEqual(FMath::LogX(10.0, 1000.0), 3.0, 0.00001))
    {
        return -15;
    }
    if (!FMath::IsNearlyEqual(FMath::Log2(8.0), 3.0, 0.00001))
    {
        return -16;
    }

    float SinValue = 0.0f;
    float CosValue = 0.0f;
    FMath::SinCos(SinValue, CosValue, 0.5f);
    if (!FMath::IsNearlyEqual(SinValue, FMath::Sin(0.5f), 0.001))
    {
        return -17;
    }
    if (!FMath::IsNearlyEqual(CosValue, FMath::Cos(0.5f), 0.001))
    {
        return -18;
    }

    if (!FMath::IsNearlyEqual(FMath::FastAsin(0.5), FMath::Asin(0.5), 0.001))
    {
        return -19;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("BasicScalarFunctions", Script, "int RunBasicScalarFunctions()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(PredicatesRangesAndAngles)
	{
		static const char Script[] = R"(
int RunPredicatesRangesAndAngles()
{
    if (!FMath::IsWithin(5, 0, 10) || FMath::IsWithin(10, 0, 10))
    {
        return -1;
    }
    if (!FMath::IsWithinInclusive(10, 0, 10))
    {
        return -2;
    }
    if (!FMath::IsNearlyEqual(1.0, 1.00005, 0.001))
    {
        return -3;
    }
    if (!FMath::IsNearlyZero(0.00005, 0.001))
    {
        return -4;
    }
    if (!FMath::IsNearlyEqualByULP(1.0, 1.0, 4))
    {
        return -5;
    }
    if (!FMath::IsPowerOfTwo(64) || FMath::IsPowerOfTwo(63))
    {
        return -6;
    }
    if (FMath::Max3(4, 8, 2) != 8 || !FMath::IsNearlyEqual(FMath::Min3(4.0, 8.0, 2.0), 2.0, 0.000001))
    {
        return -7;
    }
    if (FMath::Max3Index(1, 5, 3) != 1 || FMath::Min3Index(1.0, -4.0, 3.0) != 1)
    {
        return -8;
    }
    if (FMath::Square(5) != 25 || !FMath::IsNearlyEqual(FMath::Cube(2.0), 8.0, 0.000001))
    {
        return -9;
    }
    if (FMath::Clamp(15, 0, 10) != 10 || !FMath::IsNearlyEqual(FMath::Clamp(-1.5, -1.0, 5.0), -1.0, 0.000001))
    {
        return -10;
    }
    if (!FMath::IsNearlyEqual(FMath::Wrap(370.0, 0.0, 360.0), 10.0, 0.000001))
    {
        return -11;
    }
    if (FMath::WrapExclusive(13, 0, 10) != 3)
    {
        return -12;
    }
    if (FMath::GridSnap(13, 5) != 15 || !FMath::IsNearlyEqual(FMath::GridSnap(12.4, 5.0), 10.0, 0.000001))
    {
        return -13;
    }
    if (FMath::DivideAndRoundUp(10, 3) != 4 || FMath::DivideAndRoundDown(10, 3) != 3 || FMath::DivideAndRoundNearest(10, 3) != 3)
    {
        return -14;
    }

    if (!FMath::IsNearlyEqual(FMath::RadiansToDegrees(3.14159265358979323846), 180.0, 0.00001))
    {
        return -15;
    }
    if (!FMath::IsNearlyEqual(FMath::DegreesToRadians(180.0), 3.14159265358979323846, 0.00001))
    {
        return -16;
    }
    if (!FMath::IsNearlyEqual(FMath::ClampAngle(270.0, -45.0, 45.0), -45.0, 0.00001))
    {
        return -17;
    }
    if (!FMath::IsNearlyEqual(FMath::FindDeltaAngleDegrees(10.0, 350.0), -20.0, 0.00001))
    {
        return -18;
    }
    if (!FMath::IsNearlyEqual(FMath::FindDeltaAngleRadians(0.0, 4.71238898038469), -1.5707963267948966, 0.00001))
    {
        return -19;
    }
    if (!FMath::IsNearlyEqual(FMath::UnwindRadians(12.566370614359172), 0.0, 0.00001))
    {
        return -20;
    }
    if (!FMath::IsNearlyEqual(FMath::UnwindDegrees(540.0), 180.0, 0.00001))
    {
        return -21;
    }

    double RelativeAngle = 450.0;
    FMath::WindRelativeAnglesDegrees(10.0, RelativeAngle);
    if (!FMath::IsNearlyEqual(RelativeAngle, 90.0, 0.00001))
    {
        return -22;
    }

    if (!FMath::IsNearlyEqual(FMath::FixedTurn(10.0f, 80.0f, 15.0f), 25.0, 0.00001))
    {
        return -23;
    }

    double Radius = 0.0;
    double Angle = 0.0;
    FMath::CartesianToPolar(0.0, 1.0, Radius, Angle);
    if (!FMath::IsNearlyEqual(Radius, 1.0, 0.00001))
    {
        return -24;
    }
    if (!FMath::IsNearlyEqual(Angle, 1.5707963267948966, 0.0001))
    {
        return -25;
    }

    double X = 0.0;
    double Y = 0.0;
    FMath::PolarToCartesian(2.0, 1.5707963267948966, X, Y);
    if (!FMath::IsNearlyEqual(X, 0.0, 0.0001))
    {
        return -26;
    }
    if (!FMath::IsNearlyEqual(Y, 2.0, 0.0001))
    {
        return -27;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("PredicatesRangesAndAngles", Script, "int RunPredicatesRangesAndAngles()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InterpolationHelpers)
	{
		static const char Script[] = R"(
int RunInterpolationHelpers()
{
    if (!FMath::IsNearlyEqual(FMath::Lerp(10.0, 20.0, 0.25), 12.5, 0.000001))
    {
        return -1;
    }
    if (!FMath::IsNearlyEqual(FMath::LerpStable(10.0, 20.0, 0.25), 12.5, 0.000001))
    {
        return -2;
    }
    if (!FMath::IsNearlyEqual(FMath::BiLerp(0.0, 10.0, 20.0, 30.0, 0.5, 0.5), 15.0, 0.000001))
    {
        return -3;
    }
    if (!FMath::IsNearlyEqual(FMath::CubicInterp(0.0, 1.0, 10.0, 1.0, 0.5), 5.0, 0.000001))
    {
        return -4;
    }
    if (!FMath::IsNearlyEqual(FMath::CubicInterpDerivative(0.0, 1.0, 10.0, 1.0, 0.5), 14.5, 0.000001))
    {
        return -5;
    }
    if (!FMath::IsNearlyEqual(FMath::CubicInterpSecondDerivative(0.0, 1.0, 10.0, 1.0, 0.5), 0.0, 0.000001))
    {
        return -6;
    }
    if (!FMath::IsNearlyEqual(FMath::InterpEaseIn(0.0, 10.0, 0.5, 2.0), 2.5, 0.000001))
    {
        return -7;
    }
    if (!FMath::IsNearlyEqual(FMath::InterpEaseOut(0.0, 10.0, 0.5, 2.0), 7.5, 0.000001))
    {
        return -8;
    }
    if (!FMath::IsNearlyEqual(FMath::InterpEaseInOut(0.0, 10.0, 0.25, 2.0), 1.25, 0.000001))
    {
        return -9;
    }
    if (!FMath::IsNearlyEqual(FMath::InterpStep(0.0, 10.0, 0.5, 5), 5.0, 0.000001))
    {
        return -10;
    }
    if (!FMath::IsNearlyEqual(FMath::SmoothStep(0.0, 10.0, 5.0), 0.5, 0.000001))
    {
        return -11;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("InterpolationHelpers", Script, "int RunInterpolationHelpers()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RandomHelpers)
	{
		static const char Script[] = R"(
int RunRandomHelpers()
{
    if (FMath::RandHelper(0) != 0 || FMath::RandHelper64(0) != int64(0))
    {
        return -1;
    }
    if (FMath::RandRange(4, 4) != 4 || FMath::RandRange(int64(8), int64(8)) != int64(8))
    {
        return -2;
    }
    if (!FMath::IsNearlyEqual(FMath::RandRange(2.5, 2.5), 2.5, 0.000001))
    {
        return -3;
    }
    if (!FMath::IsNearlyEqual(FMath::FRandRange(6.25, 6.25), 6.25, 0.000001))
    {
        return -4;
    }

    for (int Index = 0; Index < 64; ++Index)
    {
        const int Value = FMath::RandHelper(7);
        if (Value < 0 || Value >= 7)
        {
            return -5;
        }

        const int64 Value64 = FMath::RandHelper64(int64(9));
        if (Value64 < int64(0) || Value64 >= int64(9))
        {
            return -6;
        }

        const int Ranged = FMath::RandRange(-3, 5);
        if (Ranged < -3 || Ranged > 5)
        {
            return -7;
        }

        const double FloatRanged = FMath::FRandRange(-1.0, 1.0);
        if (FloatRanged < -1.0 || FloatRanged > 1.0)
        {
            return -8;
        }

        const bool RandomBool = FMath::RandBool();
        if (RandomBool != true && RandomBool != false)
        {
            return -9;
        }
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("RandomHelpers", Script, "int RunRandomHelpers()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif
#endif
