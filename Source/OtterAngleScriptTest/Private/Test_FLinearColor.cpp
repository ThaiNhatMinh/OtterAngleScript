// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Color.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FLinearColor UnrealBuildLinearColor(float R, float G, float B, float A)
	{
		return FLinearColor(R, G, B, A);
	}

	bool UnrealAcceptsLinearColor(const FLinearColor& Color)
	{
		return Color.Equals(FLinearColor(0.1f, 0.2f, 0.3f, 0.4f), 0.0001f);
	}

	float UnrealSumLinearColor(const FLinearColor& Color)
	{
		return Color.R + Color.G + Color.B + Color.A;
	}

	bool RegisterFLinearColorInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FLinearColor UnrealBuildLinearColor(float R, float G, float B, float A)",
			asFUNCTION(UnrealBuildLinearColor),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsLinearColor(const FLinearColor &in Color)",
			asFUNCTION(UnrealAcceptsLinearColor),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"float UnrealSumLinearColor(const FLinearColor &in Color)",
			asFUNCTION(UnrealSumLinearColor),
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
	FOtterAngleScriptFLinearColorTests,
	"OtterAngleScript.FLinearColor",
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
		ASSERT_THAT(IsTrue(RegisterFLinearColorInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptLinearColorTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FLinearColor")));

		static const char Script[] = R"(
int TestDefaultConstructor()
{
    FLinearColor Value;
    FLinearColor Value2;
    Value2 = Value;
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_DefaultCtor", Script, "int TestDefaultConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int TestCopyConstructor()
{
    FLinearColor Source(0.1f, 0.2f, 0.3f, 0.4f);
    FLinearColor Copy(Source);
    if (Copy.R != Source.R || Copy.G != Source.G || Copy.B != Source.B || Copy.A != Source.A)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_CopyCtor", Script, "int TestCopyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ForceInitConstructor)
	{
		static const char Script[] = R"(
int TestForceInitConstructor()
{
    FLinearColor Value(ForceInit);
    if (Value.R != 0.0f || Value.G != 0.0f || Value.B != 0.0f || Value.A != 0.0f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ForceInitCtor", Script, "int TestForceInitConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RGBConstructor)
	{
		static const char Script[] = R"(
int TestRGBConstructor()
{
    FLinearColor Value(0.5f, 0.25f, 0.75f);
    if (Value.R != 0.5f || Value.G != 0.25f || Value.B != 0.75f || Value.A != 1.0f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_RGBCtor", Script, "int TestRGBConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RGBAConstructor)
	{
		static const char Script[] = R"(
int TestRGBAConstructor()
{
    FLinearColor Value(0.1f, 0.2f, 0.3f, 0.4f);
    if (Value.R != 0.1f || Value.G != 0.2f || Value.B != 0.3f || Value.A != 0.4f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_RGBACtor", Script, "int TestRGBAConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opAssign)
	{
		static const char Script[] = R"(
int TestOpAssign()
{
    FLinearColor Source(0.1f, 0.2f, 0.3f, 0.4f);
    FLinearColor Target;
    Target = Source;
    if (Target.R != Source.R || Target.G != Source.G || Target.B != Source.B || Target.A != Source.A)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opAssign", Script, "int TestOpAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int TestOpEquals()
{
    FLinearColor A(0.1f, 0.2f, 0.3f, 0.4f);
    FLinearColor B(0.1f, 0.2f, 0.3f, 0.4f);
    FLinearColor C(0.9f, 0.8f, 0.7f, 0.6f);
    if (!A.opEquals(B))
    {
        return -1;
    }
    if (A.opEquals(C))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opEquals", Script, "int TestOpEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opAdd)
	{
		static const char Script[] = R"(
int TestOpAdd()
{
    FLinearColor Result = FLinearColor(0.1f, 0.2f, 0.3f, 0.4f) + FLinearColor(0.1f, 0.1f, 0.1f, 0.1f);
    if (!Result.Equals(FLinearColor(0.2f, 0.3f, 0.4f, 0.5f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opAdd", Script, "int TestOpAdd()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opSub)
	{
		static const char Script[] = R"(
int TestOpSub()
{
    FLinearColor Result = FLinearColor(0.5f, 0.6f, 0.7f, 0.8f) - FLinearColor(0.1f, 0.1f, 0.1f, 0.1f);
    if (!Result.Equals(FLinearColor(0.4f, 0.5f, 0.6f, 0.7f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opSub", Script, "int TestOpSub()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opMulColor)
	{
		static const char Script[] = R"(
int TestOpMulColor()
{
    FLinearColor Result = FLinearColor(0.5f, 0.4f, 0.3f, 1.0f) * FLinearColor(2.0f, 2.0f, 2.0f, 1.0f);
    if (!Result.Equals(FLinearColor(1.0f, 0.8f, 0.6f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opMulColor", Script, "int TestOpMulColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opMulScalar)
	{
		static const char Script[] = R"(
int TestOpMulScalar()
{
    FLinearColor Result = FLinearColor(0.5f, 0.25f, 0.125f, 1.0f) * 2.0f;
    if (!Result.Equals(FLinearColor(1.0f, 0.5f, 0.25f, 2.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opMulScalar", Script, "int TestOpMulScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opMulScalarReverse)
	{
		static const char Script[] = R"(
int TestOpMulScalarReverse()
{
    FLinearColor Result = 2.0f * FLinearColor(0.5f, 0.25f, 0.125f, 1.0f);
    if (!Result.Equals(FLinearColor(1.0f, 0.5f, 0.25f, 2.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opMulScalarReverse", Script, "int TestOpMulScalarReverse()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opDivColor)
	{
		static const char Script[] = R"(
int TestOpDivColor()
{
    FLinearColor Result = FLinearColor(1.0f, 0.8f, 0.6f, 1.0f) / FLinearColor(2.0f, 2.0f, 2.0f, 1.0f);
    if (!Result.Equals(FLinearColor(0.5f, 0.4f, 0.3f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opDivColor", Script, "int TestOpDivColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opDivScalar)
	{
		static const char Script[] = R"(
int TestOpDivScalar()
{
    FLinearColor Result = FLinearColor(1.0f, 0.5f, 0.25f, 2.0f) / 2.0f;
    if (!Result.Equals(FLinearColor(0.5f, 0.25f, 0.125f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opDivScalar", Script, "int TestOpDivScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opAddAssign)
	{
		static const char Script[] = R"(
int TestOpAddAssign()
{
    FLinearColor Value(0.1f, 0.2f, 0.3f, 0.4f);
    Value += FLinearColor(0.1f, 0.1f, 0.1f, 0.1f);
    if (!Value.Equals(FLinearColor(0.2f, 0.3f, 0.4f, 0.5f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opAddAssign", Script, "int TestOpAddAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opSubAssign)
	{
		static const char Script[] = R"(
int TestOpSubAssign()
{
    FLinearColor Value(0.5f, 0.6f, 0.7f, 0.8f);
    Value -= FLinearColor(0.1f, 0.1f, 0.1f, 0.1f);
    if (!Value.Equals(FLinearColor(0.4f, 0.5f, 0.6f, 0.7f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opSubAssign", Script, "int TestOpSubAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opMulAssignColor)
	{
		static const char Script[] = R"(
int TestOpMulAssignColor()
{
    FLinearColor Value(0.5f, 0.5f, 0.5f, 1.0f);
    Value *= FLinearColor(2.0f, 2.0f, 2.0f, 1.0f);
    if (!Value.Equals(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opMulAssignColor", Script, "int TestOpMulAssignColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opMulAssignScalar)
	{
		static const char Script[] = R"(
int TestOpMulAssignScalar()
{
    FLinearColor Value(0.5f, 0.25f, 0.125f, 0.5f);
    Value *= 2.0f;
    if (!Value.Equals(FLinearColor(1.0f, 0.5f, 0.25f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opMulAssignScalar", Script, "int TestOpMulAssignScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opDivAssignColor)
	{
		static const char Script[] = R"(
int TestOpDivAssignColor()
{
    FLinearColor Value(1.0f, 1.0f, 1.0f, 1.0f);
    Value /= FLinearColor(2.0f, 2.0f, 2.0f, 1.0f);
    if (!Value.Equals(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opDivAssignColor", Script, "int TestOpDivAssignColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opDivAssignScalar)
	{
		static const char Script[] = R"(
int TestOpDivAssignScalar()
{
    FLinearColor Value(1.0f, 0.5f, 0.25f, 2.0f);
    Value /= 2.0f;
    if (!Value.Equals(FLinearColor(0.5f, 0.25f, 0.125f, 1.0f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_opDivAssignScalar", Script, "int TestOpDivAssignScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Equals)
	{
		static const char Script[] = R"(
int TestEquals()
{
    FLinearColor A(0.1f, 0.2f, 0.3f, 0.4f);
    FLinearColor B(0.10001f, 0.20001f, 0.30001f, 0.40001f);
    if (!A.Equals(B, 0.001f))
    {
        return -1;
    }
    if (A.Equals(FLinearColor(0.9f, 0.8f, 0.7f, 0.6f), 0.001f))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_Equals", Script, "int TestEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyWithNewOpacity)
	{
		static const char Script[] = R"(
int TestCopyWithNewOpacity()
{
    FLinearColor Result = FLinearColor(0.5f, 0.25f, 0.75f, 0.9f).CopyWithNewOpacity(0.5f);
    if (!Result.Equals(FLinearColor(0.5f, 0.25f, 0.75f, 0.5f), 0.0001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_CopyWithNewOpacity", Script, "int TestCopyWithNewOpacity()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetClamped)
	{
		static const char Script[] = R"(
int TestGetClamped()
{
    FLinearColor DefaultClamped = FLinearColor(-0.5f, 0.5f, 1.5f, 1.0f).GetClamped();
    if (!DefaultClamped.Equals(FLinearColor(0.0f, 0.5f, 1.0f, 1.0f), 0.0001f))
    {
        return -1;
    }
    FLinearColor RangeClamped = FLinearColor(-1.0f, 0.5f, 2.0f, 0.3f).GetClamped(0.2f, 0.8f);
    if (!RangeClamped.Equals(FLinearColor(0.2f, 0.5f, 0.8f, 0.3f), 0.0001f))
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_GetClamped", Script, "int TestGetClamped()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(LinearRGBToHSV)
	{
		static const char Script[] = R"(
int TestLinearRGBToHSV()
{
    FLinearColor HSV = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f).LinearRGBToHSV();
    if (HSV.B < 0.9f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_LinearRGBToHSV", Script, "int TestLinearRGBToHSV()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(HSVToLinearRGB)
	{
		static const char Script[] = R"(
int TestHSVToLinearRGB()
{
    FLinearColor BackToRGB = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f).LinearRGBToHSV().HSVToLinearRGB();
    if (!BackToRGB.Equals(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f), 0.001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_HSVToLinearRGB", Script, "int TestHSVToLinearRGB()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Desaturate)
	{
		static const char Script[] = R"(
int TestDesaturate()
{
    FLinearColor Result = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f).Desaturate(1.0f);
    if (Result.R - Result.G > 0.001f || Result.R - Result.B > 0.001f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_Desaturate", Script, "int TestDesaturate()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetLuminance)
	{
		static const char Script[] = R"(
int TestGetLuminance()
{
    float White = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).GetLuminance();
    if (White < 0.9f || White > 1.2f)
    {
        return -1;
    }
    float Black = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f).GetLuminance();
    if (Black > 0.0001f)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_GetLuminance", Script, "int TestGetLuminance()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetMax)
	{
		static const char Script[] = R"(
int TestGetMax()
{
    float MaxVal = FLinearColor(0.1f, 0.8f, 0.3f, 0.5f).GetMax();
    if (MaxVal != 0.8f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_GetMax", Script, "int TestGetMax()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetMin)
	{
		static const char Script[] = R"(
int TestGetMin()
{
    float MinVal = FLinearColor(0.1f, 0.8f, 0.3f, 0.5f).GetMin();
    if (MinVal != 0.1f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_GetMin", Script, "int TestGetMin()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsAlmostBlack)
	{
		static const char Script[] = R"(
int TestIsAlmostBlack()
{
    if (!FLinearColor(0.0f, 0.0f, 0.0f, 1.0f).IsAlmostBlack())
    {
        return -1;
    }
    if (FLinearColor(1.0f, 0.0f, 0.0f, 1.0f).IsAlmostBlack())
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_IsAlmostBlack", Script, "int TestIsAlmostBlack()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int TestToString()
{
    FString Str = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f).ToString();
    if (!Str.Contains("R="))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ToString", Script, "int TestToString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InitFromString)
	{
		static const char Script[] = R"AA(
int TestInitFromString()
{
    FLinearColor Value;
    if (!Value.InitFromString("(R=0.500000,G=0.250000,B=0.750000,A=1.000000)"))
    {
        return -1;
    }
    if (!Value.Equals(FLinearColor(0.5f, 0.25f, 0.75f, 1.0f), 0.001f))
    {
        return -2;
    }
    return 1;
}
)AA";

		asIScriptFunction* Function = BuildFunction("FLC_InitFromString", Script, "int TestInitFromString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int TestProperties()
{
    FLinearColor Value(ForceInit);
    Value.R = 0.1f;
    Value.G = 0.2f;
    Value.B = 0.3f;
    Value.A = 0.4f;
    if (Value.R != 0.1f || Value.G != 0.2f || Value.B != 0.3f || Value.A != 0.4f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_Properties", Script, "int TestProperties()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeFromHSV8)
	{
		static const char Script[] = R"(
int TestMakeFromHSV8()
{
    FLinearColor Result = FLinearColor::MakeFromHSV8(0, 255, 255);
    if (Result.R < 0.9f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_MakeFromHSV8", Script, "int TestMakeFromHSV8()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeRandomColor)
	{
		static const char Script[] = R"(
int TestMakeRandomColor()
{
    FLinearColor Result = FLinearColor::MakeRandomColor();
    if (Result.R < 0.0f || Result.R > 1.0f)
    {
        return -1;
    }
    if (Result.G < 0.0f || Result.G > 1.0f)
    {
        return -2;
    }
    if (Result.B < 0.0f || Result.B > 1.0f)
    {
        return -3;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_MakeRandomColor", Script, "int TestMakeRandomColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeFromColorTemperature)
	{
		static const char Script[] = R"(
int TestMakeFromColorTemperature()
{
    FLinearColor Result = FLinearColor::MakeFromColorTemperature(6500.0f);
    if (Result.R <= 0.0f || Result.G <= 0.0f || Result.B <= 0.0f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_MakeFromColorTemperature", Script, "int TestMakeFromColorTemperature()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeRandomSeededColor)
	{
		static const char Script[] = R"(
int TestMakeRandomSeededColor()
{
    FLinearColor Result1 = FLinearColor::MakeRandomSeededColor(42);
    FLinearColor Result2 = FLinearColor::MakeRandomSeededColor(42);
    if (!Result1.opEquals(Result2))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_MakeRandomSeededColor", Script, "int TestMakeRandomSeededColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(LerpUsingHSV)
	{
		static const char Script[] = R"(
int TestLerpUsingHSV()
{
    FLinearColor ColorA(0.0f, 0.0f, 0.0f, 1.0f);
    FLinearColor ColorB(1.0f, 1.0f, 1.0f, 1.0f);
    FLinearColor Result = FLinearColor::LerpUsingHSV(ColorA, ColorB, 0.5f);
    if (!Result.Equals(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), 0.001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_LerpUsingHSV", Script, "int TestLerpUsingHSV()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Dist)
	{
		static const char Script[] = R"(
int TestDist()
{
    float Distance = FLinearColor::Dist(
        FLinearColor(0.0f, 0.0f, 0.0f, 0.0f),
        FLinearColor(1.0f, 0.0f, 0.0f, 0.0f));
    if (Distance < 0.99f || Distance > 1.01f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_Dist", Script, "int TestDist()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantWhite)
	{
		static const char Script[] = R"(
int TestConstantWhite()
{
    if (!FLinearColor::White.opEquals(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantWhite", Script, "int TestConstantWhite()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantGray)
	{
		static const char Script[] = R"(
int TestConstantGray()
{
    if (!FLinearColor::Gray.Equals(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), 0.001f))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantGray", Script, "int TestConstantGray()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantBlack)
	{
		static const char Script[] = R"(
int TestConstantBlack()
{
    if (!FLinearColor::Black.opEquals(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantBlack", Script, "int TestConstantBlack()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantTransparent)
	{
		static const char Script[] = R"(
int TestConstantTransparent()
{
    if (FLinearColor::Transparent.A != 0.0f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantTransparent", Script, "int TestConstantTransparent()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantRed)
	{
		static const char Script[] = R"(
int TestConstantRed()
{
    if (!FLinearColor::Red.opEquals(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantRed", Script, "int TestConstantRed()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantGreen)
	{
		static const char Script[] = R"(
int TestConstantGreen()
{
    if (!FLinearColor::Green.opEquals(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantGreen", Script, "int TestConstantGreen()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantBlue)
	{
		static const char Script[] = R"(
int TestConstantBlue()
{
    if (!FLinearColor::Blue.opEquals(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantBlue", Script, "int TestConstantBlue()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConstantYellow)
	{
		static const char Script[] = R"(
int TestConstantYellow()
{
    if (!FLinearColor::Yellow.opEquals(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FLC_ConstantYellow", Script, "int TestConstantYellow()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScriptCallsUnrealWithLinearColor)
	{
		static const char Script[] = R"(
int RunLinearColorInteropTest()
{
    FLinearColor Color = UnrealBuildLinearColor(0.1f, 0.2f, 0.3f, 0.4f);
    if (!UnrealAcceptsLinearColor(Color))
    {
        return -1;
    }

    float Sum = UnrealSumLinearColor(Color);
    if (Sum < 0.99f || Sum > 1.01f)
    {
        return -2;
    }

    return int(Sum * 10.0f);
}
)";

		asIScriptFunction* Function = BuildFunction("FLinearColorInterop", Script, "int RunLinearColorInteropTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(UnrealCallsScriptWithLinearColor)
	{
		static const char Script[] = R"(
FLinearColor BuildLinearColorForUnreal(const FLinearColor &in Color)
{
    FLinearColor Result(Color);
    Result.R += 0.1f;
    Result.G += 0.1f;
    Result.B += 0.1f;
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FLinearColorUnrealCall", Script, "FLinearColor BuildLinearColorForUnreal(const FLinearColor &in Color)");
		ASSERT_THAT(IsNotNull(Function));

		FLinearColor Input(0.1f, 0.2f, 0.3f, 0.4f);

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

		const FLinearColor* ReturnValue = static_cast<const FLinearColor*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(ReturnValue->Equals(FLinearColor(0.2f, 0.3f, 0.4f, 0.4f), 0.0001f)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
