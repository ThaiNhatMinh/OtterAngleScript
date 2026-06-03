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

	TEST_METHOD(Abs)
	{
		static const char Script[] = R"(
int RunAbs()
{
    if (FMath::Abs(-7) != 7 || FMath::Abs(int64(-9)) != int64(9))
    {
        return -1;
    }
    if (!FMath::IsNearlyEqual(FMath::Abs(-2.5), 2.5, 0.000001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Abs", Script, "int RunAbs()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Sin)
	{
		static const char Script[] = R"(
int RunSin()
{
    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Sin(Pi * 0.5), 1.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Sin", Script, "int RunSin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Cos)
	{
		static const char Script[] = R"(
int RunCos()
{
    if (!FMath::IsNearlyEqual(FMath::Cos(0.0), 1.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Cos", Script, "int RunCos()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Tan)
	{
		static const char Script[] = R"(
int RunTan()
{
    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Tan(Pi * 0.25), 1.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Tan", Script, "int RunTan()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Asin)
	{
		static const char Script[] = R"(
int RunAsin()
{
    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Asin(0.5), Pi / 6.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Asin", Script, "int RunAsin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Acos)
	{
		static const char Script[] = R"(
int RunAcos()
{
    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Acos(0.5), Pi / 3.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Acos", Script, "int RunAcos()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Atan)
	{
		static const char Script[] = R"(
int RunAtan()
{
    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Atan(1.0), Pi / 4.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Atan", Script, "int RunAtan()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Atan2)
	{
		static const char Script[] = R"(
int RunAtan2()
{
    const double Pi = 3.14159265358979323846;
    if (!FMath::IsNearlyEqual(FMath::Atan2(1.0, 1.0), Pi / 4.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Atan2", Script, "int RunAtan2()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SinCos)
	{
		static const char Script[] = R"(
int RunSinCos()
{
    float SinValue = 0.0f;
    float CosValue = 0.0f;
    FMath::SinCos(SinValue, CosValue, 0.5f);
    if (!FMath::IsNearlyEqual(SinValue, FMath::Sin(0.5f), 0.001))
    {
        return -1;
    }
    if (!FMath::IsNearlyEqual(CosValue, FMath::Cos(0.5f), 0.001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("SinCos", Script, "int RunSinCos()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FastAsin)
	{
		static const char Script[] = R"(
int RunFastAsin()
{
    if (!FMath::IsNearlyEqual(FMath::FastAsin(0.5), FMath::Asin(0.5), 0.001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FastAsin", Script, "int RunFastAsin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Sqrt)
	{
		static const char Script[] = R"(
int RunSqrt()
{
    if (!FMath::IsNearlyEqual(FMath::Sqrt(81.0), 9.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Sqrt", Script, "int RunSqrt()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Pow)
	{
		static const char Script[] = R"(
int RunPow()
{
    if (!FMath::IsNearlyEqual(FMath::Pow(3.0, 4.0), 81.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Pow", Script, "int RunPow()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Fmod)
	{
		static const char Script[] = R"(
int RunFmod()
{
    if (!FMath::IsNearlyEqual(FMath::Fmod(7.5, 2.0), 1.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Fmod", Script, "int RunFmod()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Exp)
	{
		static const char Script[] = R"(
int RunExp()
{
    if (!FMath::IsNearlyEqual(FMath::Exp(1.0), 2.718281828459045, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Exp", Script, "int RunExp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Loge)
	{
		static const char Script[] = R"(
int RunLoge()
{
    if (!FMath::IsNearlyEqual(FMath::Loge(2.718281828459045), 1.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Loge", Script, "int RunLoge()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(LogX)
	{
		static const char Script[] = R"(
int RunLogX()
{
    if (!FMath::IsNearlyEqual(FMath::LogX(10.0, 1000.0), 3.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LogX", Script, "int RunLogX()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Log2)
	{
		static const char Script[] = R"(
int RunLog2()
{
    if (!FMath::IsNearlyEqual(FMath::Log2(8.0), 3.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Log2", Script, "int RunLog2()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsWithin)
	{
		static const char Script[] = R"(
int RunIsWithin()
{
    if (!FMath::IsWithin(5, 0, 10) || FMath::IsWithin(10, 0, 10))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsWithin", Script, "int RunIsWithin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsWithinInclusive)
	{
		static const char Script[] = R"(
int RunIsWithinInclusive()
{
    if (!FMath::IsWithinInclusive(10, 0, 10))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsWithinInclusive", Script, "int RunIsWithinInclusive()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsNearlyEqual)
	{
		static const char Script[] = R"(
int RunIsNearlyEqual()
{
    if (!FMath::IsNearlyEqual(1.0, 1.00005, 0.001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsNearlyEqual", Script, "int RunIsNearlyEqual()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsNearlyZero)
	{
		static const char Script[] = R"(
int RunIsNearlyZero()
{
    if (!FMath::IsNearlyZero(0.00005, 0.001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsNearlyZero", Script, "int RunIsNearlyZero()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsNearlyEqualByULP)
	{
		static const char Script[] = R"(
int RunIsNearlyEqualByULP()
{
    if (!FMath::IsNearlyEqualByULP(1.0, 1.0, 4))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsNearlyEqualByULP", Script, "int RunIsNearlyEqualByULP()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsPowerOfTwo)
	{
		static const char Script[] = R"(
int RunIsPowerOfTwo()
{
    if (!FMath::IsPowerOfTwo(64) || FMath::IsPowerOfTwo(63))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsPowerOfTwo", Script, "int RunIsPowerOfTwo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Max3)
	{
		static const char Script[] = R"(
int RunMax3()
{
    if (FMath::Max3(4, 8, 2) != 8)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Max3", Script, "int RunMax3()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Min3)
	{
		static const char Script[] = R"(
int RunMin3()
{
    if (!FMath::IsNearlyEqual(FMath::Min3(4.0, 8.0, 2.0), 2.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Min3", Script, "int RunMin3()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Max3Index)
	{
		static const char Script[] = R"(
int RunMax3Index()
{
    if (FMath::Max3Index(1, 5, 3) != 1)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Max3Index", Script, "int RunMax3Index()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Min3Index)
	{
		static const char Script[] = R"(
int RunMin3Index()
{
    if (FMath::Min3Index(1.0, -4.0, 3.0) != 1)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Min3Index", Script, "int RunMin3Index()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Square)
	{
		static const char Script[] = R"(
int RunSquare()
{
    if (FMath::Square(5) != 25)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Square", Script, "int RunSquare()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Cube)
	{
		static const char Script[] = R"(
int RunCube()
{
    if (!FMath::IsNearlyEqual(FMath::Cube(2.0), 8.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Cube", Script, "int RunCube()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Clamp)
	{
		static const char Script[] = R"(
int RunClamp()
{
    if (FMath::Clamp(15, 0, 10) != 10 || !FMath::IsNearlyEqual(FMath::Clamp(-1.5, -1.0, 5.0), -1.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Clamp", Script, "int RunClamp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Wrap)
	{
		static const char Script[] = R"(
int RunWrap()
{
    if (!FMath::IsNearlyEqual(FMath::Wrap(370.0, 0.0, 360.0), 10.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Wrap", Script, "int RunWrap()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(WrapExclusive)
	{
		static const char Script[] = R"(
int RunWrapExclusive()
{
    if (FMath::WrapExclusive(13, 0, 10) != 3)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("WrapExclusive", Script, "int RunWrapExclusive()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GridSnap)
	{
		static const char Script[] = R"(
int RunGridSnap()
{
    if (FMath::GridSnap(13, 5) != 15 || !FMath::IsNearlyEqual(FMath::GridSnap(12.4, 5.0), 10.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GridSnap", Script, "int RunGridSnap()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(DivideAndRoundUp)
	{
		static const char Script[] = R"(
int RunDivideAndRoundUp()
{
    if (FMath::DivideAndRoundUp(10, 3) != 4)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("DivideAndRoundUp", Script, "int RunDivideAndRoundUp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(DivideAndRoundDown)
	{
		static const char Script[] = R"(
int RunDivideAndRoundDown()
{
    if (FMath::DivideAndRoundDown(10, 3) != 3)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("DivideAndRoundDown", Script, "int RunDivideAndRoundDown()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(DivideAndRoundNearest)
	{
		static const char Script[] = R"(
int RunDivideAndRoundNearest()
{
    if (FMath::DivideAndRoundNearest(10, 3) != 3)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("DivideAndRoundNearest", Script, "int RunDivideAndRoundNearest()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RadiansToDegrees)
	{
		static const char Script[] = R"(
int RunRadiansToDegrees()
{
    if (!FMath::IsNearlyEqual(FMath::RadiansToDegrees(3.14159265358979323846), 180.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("RadiansToDegrees", Script, "int RunRadiansToDegrees()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(DegreesToRadians)
	{
		static const char Script[] = R"(
int RunDegreesToRadians()
{
    if (!FMath::IsNearlyEqual(FMath::DegreesToRadians(180.0), 3.14159265358979323846, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("DegreesToRadians", Script, "int RunDegreesToRadians()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ClampAngle)
	{
		static const char Script[] = R"(
int RunClampAngle()
{
    if (!FMath::IsNearlyEqual(FMath::ClampAngle(270.0, -45.0, 45.0), -45.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("ClampAngle", Script, "int RunClampAngle()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FindDeltaAngleDegrees)
	{
		static const char Script[] = R"(
int RunFindDeltaAngleDegrees()
{
    if (!FMath::IsNearlyEqual(FMath::FindDeltaAngleDegrees(10.0, 350.0), -20.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FindDeltaAngleDegrees", Script, "int RunFindDeltaAngleDegrees()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FindDeltaAngleRadians)
	{
		static const char Script[] = R"(
int RunFindDeltaAngleRadians()
{
    if (!FMath::IsNearlyEqual(FMath::FindDeltaAngleRadians(0.0, 4.71238898038469), -1.5707963267948966, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FindDeltaAngleRadians", Script, "int RunFindDeltaAngleRadians()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(UnwindRadians)
	{
		static const char Script[] = R"(
int RunUnwindRadians()
{
    if (!FMath::IsNearlyEqual(FMath::UnwindRadians(12.566370614359172), 0.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UnwindRadians", Script, "int RunUnwindRadians()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(UnwindDegrees)
	{
		static const char Script[] = R"(
int RunUnwindDegrees()
{
    if (!FMath::IsNearlyEqual(FMath::UnwindDegrees(540.0), 180.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UnwindDegrees", Script, "int RunUnwindDegrees()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(WindRelativeAnglesDegrees)
	{
		static const char Script[] = R"(
int RunWindRelativeAnglesDegrees()
{
    double RelativeAngle = 450.0;
    FMath::WindRelativeAnglesDegrees(10.0, RelativeAngle);
    if (!FMath::IsNearlyEqual(RelativeAngle, 90.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("WindRelativeAnglesDegrees", Script, "int RunWindRelativeAnglesDegrees()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FixedTurn)
	{
		static const char Script[] = R"(
int RunFixedTurn()
{
    if (!FMath::IsNearlyEqual(FMath::FixedTurn(10.0f, 80.0f, 15.0f), 25.0, 0.00001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FixedTurn", Script, "int RunFixedTurn()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CartesianToPolar)
	{
		static const char Script[] = R"(
int RunCartesianToPolar()
{
    double Radius = 0.0;
    double Angle = 0.0;
    FMath::CartesianToPolar(0.0, 1.0, Radius, Angle);
    if (!FMath::IsNearlyEqual(Radius, 1.0, 0.00001))
    {
        return -1;
    }
    if (!FMath::IsNearlyEqual(Angle, 1.5707963267948966, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("CartesianToPolar", Script, "int RunCartesianToPolar()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(PolarToCartesian)
	{
		static const char Script[] = R"(
int RunPolarToCartesian()
{
    double X = 0.0;
    double Y = 0.0;
    FMath::PolarToCartesian(2.0, 1.5707963267948966, X, Y);
    if (!FMath::IsNearlyEqual(X, 0.0, 0.0001))
    {
        return -1;
    }
    if (!FMath::IsNearlyEqual(Y, 2.0, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("PolarToCartesian", Script, "int RunPolarToCartesian()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Lerp)
	{
		static const char Script[] = R"(
int RunLerp()
{
    if (!FMath::IsNearlyEqual(FMath::Lerp(10.0, 20.0, 0.25), 12.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("Lerp", Script, "int RunLerp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(LerpStable)
	{
		static const char Script[] = R"(
int RunLerpStable()
{
    if (!FMath::IsNearlyEqual(FMath::LerpStable(10.0, 20.0, 0.25), 12.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LerpStable", Script, "int RunLerpStable()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(BiLerp)
	{
		static const char Script[] = R"(
int RunBiLerp()
{
    if (!FMath::IsNearlyEqual(FMath::BiLerp(0.0, 10.0, 20.0, 30.0, 0.5, 0.5), 15.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("BiLerp", Script, "int RunBiLerp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CubicInterp)
	{
		static const char Script[] = R"(
int RunCubicInterp()
{
    if (!FMath::IsNearlyEqual(FMath::CubicInterp(0.0, 1.0, 10.0, 1.0, 0.5), 5.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("CubicInterp", Script, "int RunCubicInterp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CubicInterpDerivative)
	{
		static const char Script[] = R"(
int RunCubicInterpDerivative()
{
    if (!FMath::IsNearlyEqual(FMath::CubicInterpDerivative(0.0, 1.0, 10.0, 1.0, 0.5), 14.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("CubicInterpDerivative", Script, "int RunCubicInterpDerivative()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CubicInterpSecondDerivative)
	{
		static const char Script[] = R"(
int RunCubicInterpSecondDerivative()
{
    if (!FMath::IsNearlyEqual(FMath::CubicInterpSecondDerivative(0.0, 1.0, 10.0, 1.0, 0.5), 0.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("CubicInterpSecondDerivative", Script, "int RunCubicInterpSecondDerivative()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InterpEaseIn)
	{
		static const char Script[] = R"(
int RunInterpEaseIn()
{
    if (!FMath::IsNearlyEqual(FMath::InterpEaseIn(0.0, 10.0, 0.5, 2.0), 2.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("InterpEaseIn", Script, "int RunInterpEaseIn()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InterpEaseOut)
	{
		static const char Script[] = R"(
int RunInterpEaseOut()
{
    if (!FMath::IsNearlyEqual(FMath::InterpEaseOut(0.0, 10.0, 0.5, 2.0), 7.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("InterpEaseOut", Script, "int RunInterpEaseOut()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InterpEaseInOut)
	{
		static const char Script[] = R"(
int RunInterpEaseInOut()
{
    if (!FMath::IsNearlyEqual(FMath::InterpEaseInOut(0.0, 10.0, 0.25, 2.0), 1.25, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("InterpEaseInOut", Script, "int RunInterpEaseInOut()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InterpStep)
	{
		static const char Script[] = R"(
int RunInterpStep()
{
    if (!FMath::IsNearlyEqual(FMath::InterpStep(0.0, 10.0, 0.5, 5), 5.0, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("InterpStep", Script, "int RunInterpStep()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SmoothStep)
	{
		static const char Script[] = R"(
int RunSmoothStep()
{
    if (!FMath::IsNearlyEqual(FMath::SmoothStep(0.0, 10.0, 5.0), 0.5, 0.000001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("SmoothStep", Script, "int RunSmoothStep()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RandHelper)
	{
		static const char Script[] = R"(
int RunRandHelper()
{
    if (FMath::RandHelper(0) != 0 || FMath::RandHelper64(0) != int64(0))
    {
        return -1;
    }

    for (int Index = 0; Index < 64; ++Index)
    {
        const int Value = FMath::RandHelper(7);
        if (Value < 0 || Value >= 7)
        {
            return -2;
        }

        const int64 Value64 = FMath::RandHelper64(int64(9));
        if (Value64 < int64(0) || Value64 >= int64(9))
        {
            return -3;
        }
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("RandHelper", Script, "int RunRandHelper()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RandRange)
	{
		static const char Script[] = R"(
int RunRandRange()
{
    if (FMath::RandRange(4, 4) != 4 || FMath::RandRange(int64(8), int64(8)) != int64(8))
    {
        return -1;
    }
    if (!FMath::IsNearlyEqual(FMath::RandRange(2.5, 2.5), 2.5, 0.000001))
    {
        return -2;
    }

    for (int Index = 0; Index < 64; ++Index)
    {
        const int Ranged = FMath::RandRange(-3, 5);
        if (Ranged < -3 || Ranged > 5)
        {
            return -3;
        }
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("RandRange", Script, "int RunRandRange()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FRandRange)
	{
		static const char Script[] = R"(
int RunFRandRange()
{
    if (!FMath::IsNearlyEqual(FMath::FRandRange(6.25, 6.25), 6.25, 0.000001))
    {
        return -1;
    }

    for (int Index = 0; Index < 64; ++Index)
    {
        const double FloatRanged = FMath::FRandRange(-1.0, 1.0);
        if (FloatRanged < -1.0 || FloatRanged > 1.0)
        {
            return -2;
        }
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FRandRange", Script, "int RunFRandRange()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RandBool)
	{
		static const char Script[] = R"(
int RunRandBool()
{
    for (int Index = 0; Index < 64; ++Index)
    {
        const bool RandomBool = FMath::RandBool();
        if (RandomBool != true && RandomBool != false)
        {
            return -1;
        }
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("RandBool", Script, "int RunRandBool()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif
#endif
