// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "Curves/RichCurve.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFRichCurveTests,
	"OtterAngleScript.FRichCurve",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFRichCurveTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FKeyHandle")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRichCurveKey")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRichCurve")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("ERichCurveInterpMode")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("ERichCurveTangentMode")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("ERichCurveTangentWeightMode")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("ERichCurveExtrapolation")));
	}

	TEST_METHOD(FKeyHandleDefaultConstruct)
	{
		static const char Script[] = R"(
int RunFKeyHandleDefaultConstruct()
{
    FKeyHandle Handle;
    if (!Handle.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FKeyHandleDefaultConstruct", Script, "int RunFKeyHandleDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FKeyHandleInvalid)
	{
		static const char Script[] = R"(
int RunFKeyHandleInvalid()
{
    FKeyHandle Handle = FKeyHandle::Invalid();
    if (Handle.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FKeyHandleInvalid", Script, "int RunFKeyHandleInvalid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FKeyHandleEquals)
	{
		static const char Script[] = R"(
int RunFKeyHandleEquals()
{
    FKeyHandle A;
    FKeyHandle B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    FKeyHandle C;
    if (A.opEquals(C))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FKeyHandleEquals", Script, "int RunFKeyHandleEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveKeyDefaultConstruct)
	{
		static const char Script[] = R"(
int RunFRichCurveKeyDefaultConstruct()
{
    FRichCurveKey Key;
    if (Key.Time < -0.0001f || Key.Time > 0.0001f)
    {
        return -1;
    }
    if (Key.Value < -0.0001f || Key.Value > 0.0001f)
    {
        return -2;
    }
    if (Key.InterpMode != ERichCurveInterpMode::RCIM_Linear)
    {
        return -3;
    }
    if (Key.TangentMode != ERichCurveTangentMode::RCTM_Auto)
    {
        return -4;
    }
    if (Key.TangentWeightMode != ERichCurveTangentWeightMode::RCTWM_WeightedNone)
    {
        return -5;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveKeyDefaultConstruct", Script, "int RunFRichCurveKeyDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveKeyTimeValueConstruct)
	{
		static const char Script[] = R"(
int RunFRichCurveKeyTimeValueConstruct()
{
    FRichCurveKey Key(2.5f, 7.0f);
    if (Key.Time < 2.4999f || Key.Time > 2.5001f)
    {
        return -1;
    }
    if (Key.Value < 6.9999f || Key.Value > 7.0001f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveKeyTimeValueConstruct", Script, "int RunFRichCurveKeyTimeValueConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveKeyFullConstruct)
	{
		static const char Script[] = R"(
int RunFRichCurveKeyFullConstruct()
{
    FRichCurveKey Key(1.0f, 5.0f, 2.0f, 3.0f, ERichCurveInterpMode::RCIM_Cubic);
    if (Key.ArriveTangent < 1.9999f || Key.ArriveTangent > 2.0001f)
    {
        return -1;
    }
    if (Key.LeaveTangent < 2.9999f || Key.LeaveTangent > 3.0001f)
    {
        return -2;
    }
    if (Key.InterpMode != ERichCurveInterpMode::RCIM_Cubic)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveKeyFullConstruct", Script, "int RunFRichCurveKeyFullConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveKeyEnumAccessors)
	{
		static const char Script[] = R"(
int RunFRichCurveKeyEnumAccessors()
{
    FRichCurveKey Key;
    Key.InterpMode = ERichCurveInterpMode::RCIM_Constant;
    if (Key.InterpMode != ERichCurveInterpMode::RCIM_Constant)
    {
        return -1;
    }
    Key.TangentMode = ERichCurveTangentMode::RCTM_User;
    if (Key.TangentMode != ERichCurveTangentMode::RCTM_User)
    {
        return -2;
    }
    Key.TangentWeightMode = ERichCurveTangentWeightMode::RCTWM_WeightedBoth;
    if (Key.TangentWeightMode != ERichCurveTangentWeightMode::RCTWM_WeightedBoth)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveKeyEnumAccessors", Script, "int RunFRichCurveKeyEnumAccessors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveKeyEquals)
	{
		static const char Script[] = R"(
int RunFRichCurveKeyEquals()
{
    FRichCurveKey A(1.0f, 2.0f);
    FRichCurveKey B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    FRichCurveKey C(1.0f, 3.0f);
    if (A.opEquals(C))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveKeyEquals", Script, "int RunFRichCurveKeyEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveDefaultConstruct)
	{
		static const char Script[] = R"(
int RunFRichCurveDefaultConstruct()
{
    FRichCurve Curve;
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    if (Curve.GetNumKeys() != 0)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveDefaultConstruct", Script, "int RunFRichCurveDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveAddKey)
	{
		static const char Script[] = R"(
int RunFRichCurveAddKey()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(1.0f, 5.0f);
    if (!Handle.IsValid())
    {
        return -1;
    }
    if (Curve.GetNumKeys() != 1)
    {
        return -2;
    }
    if (Curve.IsEmpty())
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveAddKey", Script, "int RunFRichCurveAddKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveGetKey)
	{
		static const char Script[] = R"(
int RunFRichCurveGetKey()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(2.0f, 10.0f);
    FRichCurveKey Key = Curve.GetKey(Handle);
    if (Key.Time < 1.9999f || Key.Time > 2.0001f)
    {
        return -1;
    }
    if (Key.Value < 9.9999f || Key.Value > 10.0001f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveGetKey", Script, "int RunFRichCurveGetKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveDeleteKey)
	{
		static const char Script[] = R"(
int RunFRichCurveDeleteKey()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(1.0f, 5.0f);
    Curve.DeleteKey(Handle);
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    if (Curve.GetNumKeys() != 0)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveDeleteKey", Script, "int RunFRichCurveDeleteKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveGetFirstLastKey)
	{
		static const char Script[] = R"(
int RunFRichCurveGetFirstLastKey()
{
    FRichCurve Curve;
    Curve.AddKey(1.0f, 10.0f);
    Curve.AddKey(5.0f, 50.0f);
    FRichCurveKey First = Curve.GetFirstKey();
    FRichCurveKey Last  = Curve.GetLastKey();
    if (First.Time < 0.9999f || First.Time > 1.0001f)
    {
        return -1;
    }
    if (Last.Time < 4.9999f || Last.Time > 5.0001f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveGetFirstLastKey", Script, "int RunFRichCurveGetFirstLastKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveSetKeyValue)
	{
		static const char Script[] = R"(
int RunFRichCurveSetKeyValue()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(1.0f, 5.0f);
    Curve.SetKeyValue(Handle, 99.0f, false);
    float Val = Curve.GetKeyValue(Handle);
    if (Val < 98.9999f || Val > 99.0001f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveSetKeyValue", Script, "int RunFRichCurveSetKeyValue()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveSetKeyTime)
	{
		static const char Script[] = R"(
int RunFRichCurveSetKeyTime()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(1.0f, 5.0f);
    Curve.SetKeyTime(Handle, 3.5f);
    float T = Curve.GetKeyTime(Handle);
    if (T < 3.4999f || T > 3.5001f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveSetKeyTime", Script, "int RunFRichCurveSetKeyTime()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveGetTimeRange)
	{
		static const char Script[] = R"(
int RunFRichCurveGetTimeRange()
{
    FRichCurve Curve;
    Curve.AddKey(2.0f, 1.0f);
    Curve.AddKey(8.0f, 2.0f);
    float MinT = 0.0f;
    float MaxT = 0.0f;
    Curve.GetTimeRange(MinT, MaxT);
    if (MinT < 1.9999f || MinT > 2.0001f)
    {
        return -1;
    }
    if (MaxT < 7.9999f || MaxT > 8.0001f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveGetTimeRange", Script, "int RunFRichCurveGetTimeRange()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveGetValueRange)
	{
		static const char Script[] = R"(
int RunFRichCurveGetValueRange()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 3.0f);
    Curve.AddKey(1.0f, 7.0f);
    float MinV = 0.0f;
    float MaxV = 0.0f;
    Curve.GetValueRange(MinV, MaxV);
    if (MinV < 2.9999f || MinV > 3.0001f)
    {
        return -1;
    }
    if (MaxV < 6.9999f || MaxV > 7.0001f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveGetValueRange", Script, "int RunFRichCurveGetValueRange()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveEval)
	{
		static const char Script[] = R"(
int RunFRichCurveEval()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 0.0f);
    Curve.AddKey(1.0f, 10.0f);
    float Val = Curve.Eval(0.0f);
    if (Val < -0.0001f || Val > 0.0001f)
    {
        return -1;
    }
    Val = Curve.Eval(1.0f);
    if (Val < 9.9999f || Val > 10.0001f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveEval", Script, "int RunFRichCurveEval()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveEvalWithDefault)
	{
		static const char Script[] = R"(
int RunFRichCurveEvalWithDefault()
{
    FRichCurve Curve;
    float Val = Curve.Eval(0.5f, 42.0f);
    if (Val < 41.9999f || Val > 42.0001f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveEvalWithDefault", Script, "int RunFRichCurveEvalWithDefault()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveReset)
	{
		static const char Script[] = R"(
int RunFRichCurveReset()
{
    FRichCurve Curve;
    Curve.AddKey(1.0f, 5.0f);
    Curve.AddKey(2.0f, 10.0f);
    Curve.Reset();
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveReset", Script, "int RunFRichCurveReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveDefaultValueAccessors)
	{
		static const char Script[] = R"(
int RunFRichCurveDefaultValueAccessors()
{
    FRichCurve Curve;
    Curve.SetDefaultValue(7.5f);
    float Val = Curve.GetDefaultValue();
    if (Val < 7.4999f || Val > 7.5001f)
    {
        return -1;
    }
    Curve.ClearDefaultValue();
    if (Curve.HasAnyData())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveDefaultValueAccessors", Script, "int RunFRichCurveDefaultValueAccessors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveExtrapolationAccessors)
	{
		static const char Script[] = R"(
int RunFRichCurveExtrapolationAccessors()
{
    FRichCurve Curve;
    Curve.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Linear;
    if (Curve.PreInfinityExtrap != ERichCurveExtrapolation::RCCE_Linear)
    {
        return -1;
    }
    Curve.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Cycle;
    if (Curve.PostInfinityExtrap != ERichCurveExtrapolation::RCCE_Cycle)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveExtrapolationAccessors", Script, "int RunFRichCurveExtrapolationAccessors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveInterpModeAccessors)
	{
		static const char Script[] = R"(
int RunFRichCurveInterpModeAccessors()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(0.0f, 0.0f);
    Curve.SetKeyInterpMode(Handle, ERichCurveInterpMode::RCIM_Constant);
    ERichCurveInterpMode Mode = Curve.GetKeyInterpMode(Handle);
    if (Mode != ERichCurveInterpMode::RCIM_Constant)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveInterpModeAccessors", Script, "int RunFRichCurveInterpModeAccessors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveTangentModeAccessors)
	{
		static const char Script[] = R"(
int RunFRichCurveTangentModeAccessors()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(0.0f, 0.0f);
    Curve.SetKeyTangentMode(Handle, ERichCurveTangentMode::RCTM_Break, false);
    ERichCurveTangentMode Mode = Curve.GetKeyTangentMode(Handle);
    if (Mode != ERichCurveTangentMode::RCTM_Break)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveTangentModeAccessors", Script, "int RunFRichCurveTangentModeAccessors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveTangentWeightModeAccessors)
	{
		static const char Script[] = R"(
int RunFRichCurveTangentWeightModeAccessors()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(0.0f, 0.0f);
    Curve.SetKeyTangentWeightMode(Handle, ERichCurveTangentWeightMode::RCTWM_WeightedArrive, false);
    FRichCurveKey Key = Curve.GetKey(Handle);
    if (Key.TangentWeightMode != ERichCurveTangentWeightMode::RCTWM_WeightedArrive)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveTangentWeightModeAccessors", Script, "int RunFRichCurveTangentWeightModeAccessors()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveShiftCurve)
	{
		static const char Script[] = R"(
int RunFRichCurveShiftCurve()
{
    FRichCurve Curve;
    Curve.AddKey(1.0f, 5.0f);
    Curve.ShiftCurve(2.0f);
    FRichCurveKey Key = Curve.GetFirstKey();
    if (Key.Time < 2.9999f || Key.Time > 3.0001f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveShiftCurve", Script, "int RunFRichCurveShiftCurve()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveScaleCurve)
	{
		static const char Script[] = R"(
int RunFRichCurveScaleCurve()
{
    FRichCurve Curve;
    Curve.AddKey(2.0f, 5.0f);
    Curve.ScaleCurve(0.0f, 2.0f);
    FRichCurveKey Key = Curve.GetFirstKey();
    if (Key.Time < 3.9999f || Key.Time > 4.0001f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveScaleCurve", Script, "int RunFRichCurveScaleCurve()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveAutoSetTangents)
	{
		static const char Script[] = R"(
int RunFRichCurveAutoSetTangents()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 0.0f);
    Curve.AddKey(1.0f, 1.0f);
    Curve.AddKey(2.0f, 0.0f);
    Curve.AutoSetTangents();
    Curve.AutoSetTangents(0.5f);
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveAutoSetTangents", Script, "int RunFRichCurveAutoSetTangents()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveFindKey)
	{
		static const char Script[] = R"(
int RunFRichCurveFindKey()
{
    FRichCurve Curve;
    Curve.AddKey(1.0f, 5.0f);
    FKeyHandle Found = Curve.FindKey(1.0f, 0.001f);
    if (!Found.IsValid())
    {
        return -1;
    }
    FKeyHandle NotFound = Curve.FindKey(99.0f, 0.001f);
    if (NotFound.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveFindKey", Script, "int RunFRichCurveFindKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveKeyExistsAtTime)
	{
		static const char Script[] = R"(
int RunFRichCurveKeyExistsAtTime()
{
    FRichCurve Curve;
    Curve.AddKey(2.0f, 5.0f);
    if (!Curve.KeyExistsAtTime(2.0f, 0.001f))
    {
        return -1;
    }
    if (Curve.KeyExistsAtTime(5.0f, 0.001f))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveKeyExistsAtTime", Script, "int RunFRichCurveKeyExistsAtTime()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveHandleNavigation)
	{
		static const char Script[] = R"(
int RunFRichCurveHandleNavigation()
{
    FRichCurve Curve;
    FKeyHandle H1 = Curve.AddKey(1.0f, 1.0f);
    FKeyHandle H2 = Curve.AddKey(2.0f, 2.0f);
    FKeyHandle H3 = Curve.AddKey(3.0f, 3.0f);

    FKeyHandle First = Curve.GetFirstKeyHandle();
    FKeyHandle Last  = Curve.GetLastKeyHandle();

    FRichCurveKey FirstKey = Curve.GetKey(First);
    if (FirstKey.Time < 0.9999f || FirstKey.Time > 1.0001f)
    {
        return -1;
    }
    FRichCurveKey LastKey = Curve.GetKey(Last);
    if (LastKey.Time < 2.9999f || LastKey.Time > 3.0001f)
    {
        return -2;
    }
    FKeyHandle Next = Curve.GetNextKey(First);
    FRichCurveKey NextKey = Curve.GetKey(Next);
    if (NextKey.Time < 1.9999f || NextKey.Time > 2.0001f)
    {
        return -3;
    }
    FKeyHandle Prev = Curve.GetPreviousKey(Last);
    FRichCurveKey PrevKey = Curve.GetKey(Prev);
    if (PrevKey.Time < 1.9999f || PrevKey.Time > 2.0001f)
    {
        return -4;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveHandleNavigation", Script, "int RunFRichCurveHandleNavigation()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveIsKeyHandleValid)
	{
		static const char Script[] = R"(
int RunFRichCurveIsKeyHandleValid()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(1.0f, 5.0f);
    if (!Curve.IsKeyHandleValid(Handle))
    {
        return -1;
    }
    Curve.DeleteKey(Handle);
    if (Curve.IsKeyHandleValid(Handle))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveIsKeyHandleValid", Script, "int RunFRichCurveIsKeyHandleValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveUpdateOrAddKey)
	{
		static const char Script[] = R"(
int RunFRichCurveUpdateOrAddKey()
{
    FRichCurve Curve;
    Curve.UpdateOrAddKey(1.0f, 5.0f);
    if (Curve.GetNumKeys() != 1)
    {
        return -1;
    }
    // Updating an existing key should not increase key count.
    Curve.UpdateOrAddKey(1.0f, 99.0f);
    if (Curve.GetNumKeys() != 1)
    {
        return -2;
    }
    FRichCurveKey Key = Curve.GetFirstKey();
    if (Key.Value < 98.9999f || Key.Value > 99.0001f)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveUpdateOrAddKey", Script, "int RunFRichCurveUpdateOrAddKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveIsConstant)
	{
		static const char Script[] = R"(
int RunFRichCurveIsConstant()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 5.0f);
    Curve.AddKey(1.0f, 5.0f);
    if (!Curve.IsConstant(0.01f))
    {
        return -1;
    }
    FKeyHandle H = Curve.GetLastKeyHandle();
    Curve.SetKeyValue(H, 10.0f, false);
    if (Curve.IsConstant(0.01f))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveIsConstant", Script, "int RunFRichCurveIsConstant()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveGetIndexSafe)
	{
		static const char Script[] = R"(
int RunFRichCurveGetIndexSafe()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(1.0f, 5.0f);
    int Idx = Curve.GetIndexSafe(Handle);
    if (Idx < 0)
    {
        return -1;
    }
    FKeyHandle Invalid = FKeyHandle::Invalid();
    int InvalidIdx = Curve.GetIndexSafe(Invalid);
    if (InvalidIdx >= 0)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveGetIndexSafe", Script, "int RunFRichCurveGetIndexSafe()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveCopyConstruct)
	{
		static const char Script[] = R"(
int RunFRichCurveCopyConstruct()
{
    FRichCurve Original;
    Original.AddKey(1.0f, 5.0f);
    Original.AddKey(2.0f, 10.0f);
    FRichCurve Copy = Original;
    if (Copy.GetNumKeys() != 2)
    {
        return -1;
    }
    if (!Copy.opEquals(Original))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveCopyConstruct", Script, "int RunFRichCurveCopyConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveOpEquals)
	{
		static const char Script[] = R"(
int RunFRichCurveOpEquals()
{
    FRichCurve A;
    A.AddKey(1.0f, 5.0f);
    FRichCurve B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    B.AddKey(2.0f, 10.0f);
    if (A.opEquals(B))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveOpEquals", Script, "int RunFRichCurveOpEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveHasAnyData)
	{
		static const char Script[] = R"(
int RunFRichCurveHasAnyData()
{
    FRichCurve Curve;
    if (Curve.HasAnyData())
    {
        return -1;
    }
    Curve.AddKey(0.0f, 1.0f);
    if (!Curve.HasAnyData())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveHasAnyData", Script, "int RunFRichCurveHasAnyData()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveBakeCurve)
	{
		static const char Script[] = R"(
int RunFRichCurveBakeCurve()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 0.0f);
    Curve.AddKey(1.0f, 1.0f);
    int KeysBefore = Curve.GetNumKeys();
    Curve.BakeCurve(10.0f);
    int KeysAfter = Curve.GetNumKeys();
    if (KeysAfter < KeysBefore)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveBakeCurve", Script, "int RunFRichCurveBakeCurve()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveRemoveRedundantAutoTangentKeys)
	{
		static const char Script[] = R"(
int RunFRichCurveRemoveRedundantAutoTangentKeys()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 1.0f);
    Curve.AddKey(0.5f, 1.0f);
    Curve.AddKey(1.0f, 1.0f);
    Curve.RemoveRedundantAutoTangentKeys(0.01f);
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveRemoveRedundantAutoTangentKeys", Script, "int RunFRichCurveRemoveRedundantAutoTangentKeys()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRichCurveReserveKeys)
	{
		static const char Script[] = R"(
int RunFRichCurveReserveKeys()
{
    FRichCurve Curve;
    Curve.ReserveKeys(10);
    Curve.AddKey(1.0f, 1.0f);
    if (Curve.GetNumKeys() != 1)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FRichCurveReserveKeys", Script, "int RunFRichCurveReserveKeys()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
