// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "Internationalization/Text.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFTextTests,
	"OtterAngleScript.FText",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFTextTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FText")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FText T;
    if (!T.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FText A = FText::FromString("hello");
    FText B = A;
    if (!B.EqualTo(A))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Assign)
	{
		static const char Script[] = R"(
int RunAssign()
{
    FText A = FText::FromString("assigned");
    FText B;
    B = A;
    if (!B.EqualTo(A))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAssign", Script, "int RunAssign()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FText T = FText::FromString("hello");
    FString S = T.ToString();
    if (S != "hello")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(BuildSourceString)
	{
		static const char Script[] = R"(
int RunBuildSourceString()
{
    FText T = FText::FromString("source");
    FString S = T.BuildSourceString();
    if (S.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextBuildSourceString", Script, "int RunBuildSourceString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsNumeric)
	{
		static const char Script[] = R"(
int RunIsNumeric()
{
    FText Num = FText::AsNumber(42);
    if (!Num.IsNumeric())
    {
        return -1;
    }
    FText Word = FText::FromString("hello");
    if (Word.IsNumeric())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsNumeric", Script, "int RunIsNumeric()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CompareTo)
	{
		static const char Script[] = R"(
int RunCompareTo()
{
    FText A = FText::FromString("apple");
    FText B = FText::FromString("apple");
    FText C = FText::FromString("banana");
    if (A.CompareTo(B) != 0)
    {
        return -1;
    }
    if (A.CompareTo(C) == 0)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextCompareTo", Script, "int RunCompareTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CompareToCaseIgnored)
	{
		static const char Script[] = R"(
int RunCompareToCaseIgnored()
{
    FText A = FText::FromString("Hello");
    FText B = FText::FromString("hello");
    if (A.CompareToCaseIgnored(B) != 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextCompareToCaseIgnored", Script, "int RunCompareToCaseIgnored()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(EqualTo)
	{
		static const char Script[] = R"(
int RunEqualTo()
{
    FText A = FText::FromString("test");
    FText B = FText::FromString("test");
    FText C = FText::FromString("other");
    if (!A.EqualTo(B))
    {
        return -1;
    }
    if (A.EqualTo(C))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextEqualTo", Script, "int RunEqualTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(EqualToCaseIgnored)
	{
		static const char Script[] = R"(
int RunEqualToCaseIgnored()
{
    FText A = FText::FromString("Hello");
    FText B = FText::FromString("hello");
    if (!A.EqualToCaseIgnored(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextEqualToCaseIgnored", Script, "int RunEqualToCaseIgnored()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IdenticalTo)
	{
		static const char Script[] = R"(
int RunIdenticalTo()
{
    FText A = FText::FromString("same");
    FText B = A;
    if (!A.IdenticalTo(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIdenticalTo", Script, "int RunIdenticalTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsEmpty)
	{
		static const char Script[] = R"(
int RunIsEmpty()
{
    FText Empty;
    if (!Empty.IsEmpty())
    {
        return -1;
    }
    FText NonEmpty = FText::FromString("hello");
    if (NonEmpty.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsEmpty", Script, "int RunIsEmpty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsEmptyOrWhitespace)
	{
		static const char Script[] = R"(
int RunIsEmptyOrWhitespace()
{
    FText Empty;
    if (!Empty.IsEmptyOrWhitespace())
    {
        return -1;
    }
    FText NonEmpty = FText::FromString("hello");
    if (NonEmpty.IsEmptyOrWhitespace())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsEmptyOrWhitespace", Script, "int RunIsEmptyOrWhitespace()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToLower)
	{
		static const char Script[] = R"(
int RunToLower()
{
    FText Upper = FText::FromString("HELLO");
    FText Lower = Upper.ToLower();
    if (Lower.IsEmpty())
    {
        return -1;
    }
    FText Expected = FText::FromString("hello");
    if (!Lower.EqualToCaseIgnored(Expected))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextToLower", Script, "int RunToLower()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToUpper)
	{
		static const char Script[] = R"(
int RunToUpper()
{
    FText Lower = FText::FromString("hello");
    FText Upper = Lower.ToUpper();
    if (Upper.IsEmpty())
    {
        return -1;
    }
    FText Expected = FText::FromString("HELLO");
    if (!Upper.EqualToCaseIgnored(Expected))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextToUpper", Script, "int RunToUpper()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsTransient)
	{
		static const char Script[] = R"(
int RunIsTransient()
{
    FText T = FText::FromString("hello");
    if (T.IsTransient())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsTransient", Script, "int RunIsTransient()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsCultureInvariant)
	{
		static const char Script[] = R"(
int RunIsCultureInvariant()
{
    FText Invariant = FText::AsCultureInvariant(FText::FromString("test"));
    if (!Invariant.IsCultureInvariant())
    {
        return -1;
    }
    FText Normal = FText::FromString("hello");
    if (Normal.IsCultureInvariant())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsCultureInvariant", Script, "int RunIsCultureInvariant()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInitializedFromString)
	{
		static const char Script[] = R"(
int RunIsInitializedFromString()
{
    FText T = FText::FromString("hello");
    if (!T.IsInitializedFromString())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsInitializedFromString", Script, "int RunIsInitializedFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsFromStringTable)
	{
		static const char Script[] = R"(
int RunIsFromStringTable()
{
    FText T = FText::FromString("hello");
    if (T.IsFromStringTable())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextIsFromStringTable", Script, "int RunIsFromStringTable()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetEmpty)
	{
		static const char Script[] = R"(
int RunGetEmpty()
{
    FText E = FText::GetEmpty();
    if (!E.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextGetEmpty", Script, "int RunGetEmpty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FromString)
	{
		static const char Script[] = R"(
int RunFromString()
{
    FText T = FText::FromString("world");
    if (T.IsEmpty())
    {
        return -1;
    }
    if (T.ToString() != "world")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFromString", Script, "int RunFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FromName)
	{
		static const char Script[] = R"(
int RunFromName()
{
    FName N("MyName");
    FText T = FText::FromName(N);
    if (T.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFromName", Script, "int RunFromName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsCultureInvariantFromText)
	{
		static const char Script[] = R"(
int RunAsCultureInvariantFromText()
{
    FText Source = FText::FromString("invariant");
    FText Invariant = FText::AsCultureInvariant(Source);
    if (!Invariant.IsCultureInvariant())
    {
        return -1;
    }
    if (Invariant.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsCultureInvariantFromText", Script, "int RunAsCultureInvariantFromText()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsCultureInvariantFromString)
	{
		static const char Script[] = R"(
int RunAsCultureInvariantFromString()
{
    FText Invariant = FText::AsCultureInvariant(FString("invariant_str"));
    if (!Invariant.IsCultureInvariant())
    {
        return -1;
    }
    if (Invariant.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsCultureInvariantFromString", Script, "int RunAsCultureInvariantFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(TrimPreceding)
	{
		static const char Script[] = R"(
int RunTrimPreceding()
{
    FText T = FText::FromString("  hello");
    FText Trimmed = FText::TrimPreceding(T);
    if (Trimmed.IsEmpty())
    {
        return -1;
    }
    FText Expected = FText::FromString("hello");
    if (!Trimmed.EqualTo(Expected))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextTrimPreceding", Script, "int RunTrimPreceding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(TrimTrailing)
	{
		static const char Script[] = R"(
int RunTrimTrailing()
{
    FText T = FText::FromString("hello  ");
    FText Trimmed = FText::TrimTrailing(T);
    if (Trimmed.IsEmpty())
    {
        return -1;
    }
    FText Expected = FText::FromString("hello");
    if (!Trimmed.EqualTo(Expected))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextTrimTrailing", Script, "int RunTrimTrailing()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(TrimPrecedingAndTrailing)
	{
		static const char Script[] = R"(
int RunTrimPrecedingAndTrailing()
{
    FText T = FText::FromString("  hello  ");
    FText Trimmed = FText::TrimPrecedingAndTrailing(T);
    if (Trimmed.IsEmpty())
    {
        return -1;
    }
    FText Expected = FText::FromString("hello");
    if (!Trimmed.EqualTo(Expected))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextTrimPrecedingAndTrailing", Script, "int RunTrimPrecedingAndTrailing()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsNumberInt)
	{
		static const char Script[] = R"(
int RunAsNumberInt()
{
    FText T = FText::AsNumber(42);
    if (T.IsEmpty())
    {
        return -1;
    }
    if (!T.IsNumeric())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsNumberInt", Script, "int RunAsNumberInt()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsNumberInt64)
	{
		static const char Script[] = R"(
int RunAsNumberInt64()
{
    FText T = FText::AsNumber(int64(1000000000));
    if (T.IsEmpty())
    {
        return -1;
    }
    if (T.ToString() != "1,000,000,000")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsNumberInt64", Script, "int RunAsNumberInt64()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsNumberFloat)
	{
		static const char Script[] = R"(
int RunAsNumberFloat()
{
    FText T = FText::AsNumber(3.14f);
    if (T.IsEmpty())
    {
        return -1;
    }
    if (!T.IsNumeric())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsNumberFloat", Script, "int RunAsNumberFloat()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsNumberDouble)
	{
		static const char Script[] = R"(
int RunAsNumberDouble()
{
    FText T = FText::AsNumber(double(2.718281828));
    if (T.IsEmpty())
    {
        return -1;
    }
    if (!T.IsNumeric())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsNumberDouble", Script, "int RunAsNumberDouble()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsPercentFloat)
	{
		static const char Script[] = R"(
int RunAsPercentFloat()
{
    FText T = FText::AsPercent(0.5f);
    if (T.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsPercentFloat", Script, "int RunAsPercentFloat()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AsPercentDouble)
	{
		static const char Script[] = R"(
int RunAsPercentDouble()
{
    FText T = FText::AsPercent(double(0.75));
    if (T.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextAsPercentDouble", Script, "int RunAsPercentDouble()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Format1Text)
	{
		static const char Script[] = R"(
int RunFormat1Text()
{
    FText Pattern = FText::FromString("Hello {0}");
    FText Arg = FText::FromString("World");
    FText Result = FText::Format(Pattern, Arg);
    if (Result.IsEmpty())
    {
        return -1;
    }
    if (Result.ToString() != "Hello World")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFormat1Text", Script, "int RunFormat1Text()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Format2Text)
	{
		static const char Script[] = R"(
int RunFormat2Text()
{
    FText Pattern = FText::FromString("{0} and {1}");
    FText A = FText::FromString("Alpha");
    FText B = FText::FromString("Beta");
    FText Result = FText::Format(Pattern, A, B);
    if (Result.IsEmpty())
    {
        return -1;
    }
    if (Result.ToString() != "Alpha and Beta")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFormat2Text", Script, "int RunFormat2Text()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Format3Text)
	{
		static const char Script[] = R"(
int RunFormat3Text()
{
    FText Pattern = FText::FromString("{0}-{1}-{2}");
    FText A = FText::FromString("X");
    FText B = FText::FromString("Y");
    FText C = FText::FromString("Z");
    FText Result = FText::Format(Pattern, A, B, C);
    if (Result.IsEmpty())
    {
        return -1;
    }
    if (Result.ToString() != "X-Y-Z")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFormat3Text", Script, "int RunFormat3Text()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Format1Int)
	{
		static const char Script[] = R"(
int RunFormat1Int()
{
    FText Pattern = FText::FromString("Value: {0}");
    FText Result = FText::Format(Pattern, 7);
    if (Result.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFormat1Int", Script, "int RunFormat1Int()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Format1Float)
	{
		static const char Script[] = R"(
int RunFormat1Float()
{
    FText Pattern = FText::FromString("Pi: {0}");
    FText Result = FText::Format(Pattern, 3.14f);
    if (Result.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FTextFormat1Float", Script, "int RunFormat1Float()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
