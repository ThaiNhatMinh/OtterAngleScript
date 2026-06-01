// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "AudioParameter.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFAudioParameterTests,
	"OtterAngleScript.FAudioParameter",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFAudioParameterTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FAudioParameter")));
	}

	TEST_METHOD(EAudioParameterTypeEnum)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EAudioParameterType")));

		static const char Script[] = R"(
int RunEAudioParameterTypeEnum()
{
    EAudioParameterType T = EAudioParameterType::Float;
    if (T != EAudioParameterType::Float)
    {
        return -1;
    }
    if (T == EAudioParameterType::None)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterEAudioParameterTypeEnum", Script, "int RunEAudioParameterTypeEnum()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FAudioParameter Param;
    if (Param.ParamType != EAudioParameterType::None)
    {
        return -1;
    }
    if (!Param.ParamName.IsNone())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromName)
	{
		static const char Script[] = R"(
int RunConstructFromName()
{
    FName Name("MyParam");
    FAudioParameter Param(Name);
    if (Param.ParamName.IsNone())
    {
        return -1;
    }
    if (!Param.ParamName.opEquals(Name))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromName", Script, "int RunConstructFromName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromNameFloat)
	{
		static const char Script[] = R"(
int RunConstructFromNameFloat()
{
    FAudioParameter Param(FName("FloatParam"), 3.14f);
    if (Param.ParamType != EAudioParameterType::Float)
    {
        return -1;
    }
    if (Param.FloatParam < 3.13f || Param.FloatParam > 3.15f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromNameFloat", Script, "int RunConstructFromNameFloat()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromNameBool)
	{
		static const char Script[] = R"(
int RunConstructFromNameBool()
{
    FAudioParameter Param(FName("BoolParam"), true);
    if (Param.ParamType != EAudioParameterType::Boolean)
    {
        return -1;
    }
    if (!Param.BoolParam)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromNameBool", Script, "int RunConstructFromNameBool()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromNameInt)
	{
		static const char Script[] = R"(
int RunConstructFromNameInt()
{
    FAudioParameter Param(FName("IntParam"), 42);
    if (Param.ParamType != EAudioParameterType::Integer)
    {
        return -1;
    }
    if (Param.IntParam != 42)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromNameInt", Script, "int RunConstructFromNameInt()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromNameObject)
	{
		static const char Script[] = R"(
int RunConstructFromNameObject()
{
    FAudioParameter Param(FName("ObjectParam"), null);
    if (Param.ParamType != EAudioParameterType::Object)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromNameObject", Script, "int RunConstructFromNameObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromNameString)
	{
		static const char Script[] = R"(
int RunConstructFromNameString()
{
    FAudioParameter Param(FName("StringParam"), "hello");
    if (Param.ParamType != EAudioParameterType::String)
    {
        return -1;
    }
    if (Param.StringParam != "hello")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromNameString", Script, "int RunConstructFromNameString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromNameType)
	{
		static const char Script[] = R"(
int RunConstructFromNameType()
{
    FAudioParameter Param(FName("TypeParam"), EAudioParameterType::Trigger);
    if (Param.ParamType != EAudioParameterType::Trigger)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterConstructFromNameType", Script, "int RunConstructFromNameType()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ScalarProperties)
	{
		static const char Script[] = R"(
int RunScalarProperties()
{
    FAudioParameter Param;
    Param.ParamName   = FName("TestName");
    Param.FloatParam  = 1.5f;
    Param.BoolParam   = true;
    Param.IntParam    = 7;
    Param.StringParam = "test";
    Param.TypeName    = FName("TestType");

    if (Param.ParamName.IsNone())     { return -1; }
    if (Param.FloatParam < 1.4f)      { return -2; }
    if (!Param.BoolParam)             { return -3; }
    if (Param.IntParam != 7)          { return -4; }
    if (Param.StringParam != "test")  { return -5; }
    if (Param.TypeName.IsNone())      { return -6; }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterScalarProperties", Script, "int RunScalarProperties()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ParamTypeVirtualProperty)
	{
		static const char Script[] = R"(
int RunParamTypeVirtualProperty()
{
    FAudioParameter Param;
    Param.ParamType = EAudioParameterType::Integer;
    if (Param.ParamType != EAudioParameterType::Integer)
    {
        return -1;
    }
    Param.ParamType = EAudioParameterType::Boolean;
    if (Param.ParamType != EAudioParameterType::Boolean)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterParamTypeVirtualProperty", Script, "int RunParamTypeVirtualProperty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ObjectParamVirtualProperty)
	{
		static const char Script[] = R"(
int RunObjectParamVirtualProperty()
{
    FAudioParameter Param;
    if (Param.ObjectParam !is null)
    {
        return -1;
    }
    Param.ObjectParam = null;
    if (Param.ObjectParam !is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterObjectParamVirtualProperty", Script, "int RunObjectParamVirtualProperty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MergeMethod)
	{
		static const char Script[] = R"(
int RunMergeMethod()
{
    FAudioParameter Source(FName("Source"), 9.0f);
    FAudioParameter Target;
    Target.Merge(Source, true, true, false);
    // bInTakeName=true: Target should take Source's name
    if (!Target.ParamName.opEquals(FName("Source")))
    {
        return -1;
    }
    // bInTakeType=true: Target should take Float type
    if (Target.ParamType != EAudioParameterType::Float)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterMergeMethod", Script, "int RunMergeMethod()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CreateDefaultArrayStatic)
	{
		static const char Script[] = R"(
int RunCreateDefaultArrayStatic()
{
    FAudioParameter Arr = FAudioParameter::CreateDefaultArray(FName("MyArray"), 3);
    if (Arr.ParamType != EAudioParameterType::NoneArray)
    {
        return -1;
    }
    if (!Arr.ParamName.opEquals(FName("MyArray")))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterCreateDefaultArrayStatic", Script, "int RunCreateDefaultArrayStatic()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FAudioParameter Original(FName("Orig"), 5.0f);
    FAudioParameter Copy(Original);
    if (Copy.FloatParam < 4.9f || Copy.FloatParam > 5.1f)
    {
        return -1;
    }
    if (!Copy.ParamName.opEquals(Original.ParamName))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunAssignOperator()
{
    FAudioParameter Source(FName("Src"), true);
    FAudioParameter Target;
    Target = Source;
    if (!Target.BoolParam)
    {
        return -1;
    }
    if (!Target.ParamName.opEquals(Source.ParamName))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAudioParameterAssignOperator", Script, "int RunAssignOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
