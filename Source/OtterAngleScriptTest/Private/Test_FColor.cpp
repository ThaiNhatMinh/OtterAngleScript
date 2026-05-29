// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Color.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFColorTests,
	"OtterAngleScript.FColor",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFColorTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FColor")));

		static const char Script[] = R"(
int TestDefaultConstructor()
{
    FColor Value;
    FColor Value2;
    Value2 = Value;
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_DefaultCtor", Script, "int TestDefaultConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int TestCopyConstructor()
{
    FColor Source(100, 150, 200, 255);
    FColor Copy(Source);
    if (Copy.R != Source.R || Copy.G != Source.G || Copy.B != Source.B || Copy.A != Source.A)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_CopyCtor", Script, "int TestCopyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ForceInitConstructor)
	{
		static const char Script[] = R"(
int TestForceInitConstructor()
{
    FColor Value(ForceInit);
    if (Value.R != 0 || Value.G != 0 || Value.B != 0 || Value.A != 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ForceInitCtor", Script, "int TestForceInitConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RGBConstructor)
	{
		static const char Script[] = R"(
int TestRGBConstructor()
{
    FColor Value(10, 20, 30);
    if (Value.R != 10 || Value.G != 20 || Value.B != 30 || Value.A != 255)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_RGBCtor", Script, "int TestRGBConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RGBAConstructor)
	{
		static const char Script[] = R"(
int TestRGBAConstructor()
{
    FColor Value(10, 20, 30, 128);
    if (Value.R != 10 || Value.G != 20 || Value.B != 30 || Value.A != 128)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_RGBACtor", Script, "int TestRGBAConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(BitsConstructor)
	{
		static const char Script[] = R"(
int TestBitsConstructor()
{
    FColor A(10, 20, 30, 40);
    FColor B(A.Bits);
    if (B.R != A.R || B.G != A.G || B.B != A.B || B.A != A.A)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_BitsCtor", Script, "int TestBitsConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opAssign)
	{
		static const char Script[] = R"(
int TestOpAssign()
{
    FColor Source(100, 150, 200, 255);
    FColor Target;
    Target = Source;
    if (Target.R != Source.R || Target.G != Source.G || Target.B != Source.B || Target.A != Source.A)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_opAssign", Script, "int TestOpAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int TestOpEquals()
{
    FColor A(100, 150, 200, 255);
    FColor B(100, 150, 200, 255);
    FColor C(10, 20, 30, 40);
    if (!(A == B))
    {
        return -1;
    }
    if (A == C)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_opEquals", Script, "int TestOpEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(opAddAssign)
	{
		static const char Script[] = R"(
int TestOpAddAssign()
{
    FColor A(100, 100, 100, 100);
    FColor B(50, 60, 70, 80);
    A += B;
    if (A.R != 150 || A.G != 160 || A.B != 170 || A.A != 180)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_opAddAssign", Script, "int TestOpAddAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(WithAlpha)
	{
		static const char Script[] = R"(
int TestWithAlpha()
{
    FColor Base(100, 150, 200, 255);
    FColor Result = Base.WithAlpha(128);
    if (Result.R != 100 || Result.G != 150 || Result.B != 200 || Result.A != 128)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_WithAlpha", Script, "int TestWithAlpha()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ReinterpretAsLinear)
	{
		static const char Script[] = R"(
int TestReinterpretAsLinear()
{
    FColor Color(255, 0, 0, 255);
    FLinearColor Linear = Color.ReinterpretAsLinear();
    if (Linear.R < 0.999f || Linear.G != 0.0f || Linear.B != 0.0f || Linear.A < 0.999f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ReinterpretAsLinear", Script, "int TestReinterpretAsLinear()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToHex)
	{
		static const char Script[] = R"(
int TestToHex()
{
    FColor Color(255, 0, 128, 255);
    FString Hex = Color.ToHex();
    if (Hex.Len() == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ToHex", Script, "int TestToHex()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int TestToString()
{
    FColor Color(100, 150, 200, 255);
    FString Str = Color.ToString();
    if (Str.Len() == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ToString", Script, "int TestToString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InitFromString)
	{
		static const char Script[] = R"(
int TestInitFromString()
{
    FColor Color;
    bool bSuccess = Color.InitFromString("R=100 G=150 B=200 A=255");
    if (!bSuccess)
    {
        return -1;
    }
    if (Color.R != 100 || Color.G != 150 || Color.B != 200 || Color.A != 255)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_InitFromString", Script, "int TestInitFromString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToPackedARGB)
	{
		static const char Script[] = R"(
int TestToPackedARGB()
{
    FColor Color(255, 0, 0, 255);
    uint Packed = Color.ToPackedARGB();
    if (Packed == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ToPackedARGB", Script, "int TestToPackedARGB()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToPackedABGR)
	{
		static const char Script[] = R"(
int TestToPackedABGR()
{
    FColor Color(255, 0, 0, 255);
    uint Packed = Color.ToPackedABGR();
    if (Packed == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ToPackedABGR", Script, "int TestToPackedABGR()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToPackedRGBA)
	{
		static const char Script[] = R"(
int TestToPackedRGBA()
{
    FColor Color(255, 0, 0, 255);
    uint Packed = Color.ToPackedRGBA();
    if (Packed == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ToPackedRGBA", Script, "int TestToPackedRGBA()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToPackedBGRA)
	{
		static const char Script[] = R"(
int TestToPackedBGRA()
{
    FColor Color(255, 0, 0, 255);
    uint Packed = Color.ToPackedBGRA();
    if (Packed == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_ToPackedBGRA", Script, "int TestToPackedBGRA()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FromRGBE)
	{
		static const char Script[] = R"(
int TestFromRGBE()
{
    FColor Color(128, 64, 32, 200);
    FLinearColor Linear = Color.FromRGBE();
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_FromRGBE", Script, "int TestFromRGBE()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeRandomColor)
	{
		static const char Script[] = R"(
int TestMakeRandomColor()
{
    FColor C = FColor::MakeRandomColor();
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_MakeRandomColor", Script, "int TestMakeRandomColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeRedToGreenColorFromScalar)
	{
		static const char Script[] = R"(
int TestMakeRedToGreenColorFromScalar()
{
    FColor Red = FColor::MakeRedToGreenColorFromScalar(0.0f);
    FColor Green = FColor::MakeRedToGreenColorFromScalar(1.0f);
    if (Red.R == 0 && Red.G == 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_MakeRedToGreen", Script, "int TestMakeRedToGreenColorFromScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeFromColorTemperature)
	{
		static const char Script[] = R"(
int TestMakeFromColorTemperature()
{
    FColor C = FColor::MakeFromColorTemperature(6500.0f);
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_MakeFromColorTemp", Script, "int TestMakeFromColorTemperature()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MakeRandomSeededColor)
	{
		static const char Script[] = R"(
int TestMakeRandomSeededColor()
{
    FColor A = FColor::MakeRandomSeededColor(42);
    FColor B = FColor::MakeRandomSeededColor(42);
    if (!(A == B))
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_MakeRandomSeeded", Script, "int TestMakeRandomSeededColor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FromHex)
	{
		static const char Script[] = R"(
int TestFromHex()
{
    FColor C = FColor::FromHex("FF0000FF");
    if (C.R != 255 || C.G != 0 || C.B != 0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_FromHex", Script, "int TestFromHex()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(StaticColorConstants)
	{
		static const char Script[] = R"(
int TestStaticColorConstants()
{
    FColor W = FColor::White;
    FColor Bl = FColor::Black;
    FColor R = FColor::Red;
    FColor G = FColor::Green;
    FColor B = FColor::Blue;
    if (W.R != 255 || W.G != 255 || W.B != 255)
    {
        return -1;
    }
    if (Bl.R != 0 || Bl.G != 0 || Bl.B != 0)
    {
        return -2;
    }
    if (R.R != 255 || R.G != 0 || R.B != 0)
    {
        return -3;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FC_StaticConstants", Script, "int TestStaticColorConstants()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
