// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Containers/UnrealString.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FString UnrealEchoFString(const FString& Value)
	{
		return FString::Printf(TEXT("Unreal<%s>"), *Value);
	}

	bool UnrealAcceptsFString(const FString& Value)
	{
		return Value == TEXT("ScriptToUnreal");
	}

	void UnrealFillFString(FString& OutValue)
	{
		OutValue = TEXT("FilledByUnreal");
	}

	FString UnrealJoinFString(const FString& Prefix, const FString& Value, const FString& Suffix)
	{
		return Prefix + Value + Suffix;
	}

	bool RegisterFStringInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FString UnrealEchoFString(const FString &in Value)",
			asFUNCTION(UnrealEchoFString),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsFString(const FString &in Value)",
			asFUNCTION(UnrealAcceptsFString),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"void UnrealFillFString(FString &out Value)",
			asFUNCTION(UnrealFillFString),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FString UnrealJoinFString(const FString &in Prefix, const FString &in Value, const FString &in Suffix)",
			asFUNCTION(UnrealJoinFString),
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
	FOtterAngleScriptFStringTests,
	"OtterAngleScript.FString",
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
        };
        if (!Assert.IsTrue(ScriptModule->Build() >= 0))
        {
            return nullptr;
        };

		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl(Declaration);
        if (!Assert.IsNotNull(Function))
        {
            return nullptr;
        };
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
        };
        if (!Assert.IsTrue(Context->Prepare(Function) >= 0))
        {
            return -1;
        };
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
        };
		return static_cast<int32>(Context->GetReturnDWord());
	}

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));
		ASSERT_THAT(IsTrue(RegisterFStringInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(BasicOperations)
	{
        AddInfo("Checking FString type info.");
        ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FString")));

		static const char Script[] = R"(
int RunConstructorsAndOperators()
{
    FString EmptyValue;
    if (!EmptyValue.IsEmpty())
    {
        return -1;
    }

    FString Source = "Otter";
    FString Copy(Source);
    if (!Copy.opEquals(Source))
    {
        return -2;
    }

    FString Assigned;
    Assigned = Source;
    if (!Assigned.opEquals(Source))
    {
        return -3;
    }

    FString Added = Source + "Angle";
    if (Added != "OtterAngle")
    {
        return -4;
    }

    Assigned += "Script";
    if (Assigned != "OtterScript")
    {
        return -5;
    }

    if (Source.opCmp("Otter") != 0)
    {
        return -6;
    }

    if (Source[1] != 116)
    {
        return -7;
    }
    
    return Added.Len() + Assigned.Len();
}
)";
		AddInfo("Testing basic constructors, operators, and indexing.");
		asIScriptFunction* Function = BuildFunction("FStringOperators", Script, "int RunConstructorsAndOperators()");
        // OtterAngleOtterScript
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 21));
	}

	TEST_METHOD(QueryMethods)
	{
		static const char Script[] = R"(
int RunQueryMethods()
{
    FString Value = "  AlphaBeta  ";
    if (Value.Len() != 13)
    {
        return -1;
    }

    if (!"12345".IsNumeric())
    {
        return -2;
    }

    FString Cleared = Value;
    Cleared.Empty();
    if (!Cleared.IsEmpty())
    {
        return -3;
    }

    FString Trimmed = Value.TrimStartAndEnd();
    if (Trimmed.Left(5) != "Alpha")
    {
        return -4;
    }

    if (Trimmed.LeftChop(4) != "Alpha")
    {
        return -5;
    }

    if (Trimmed.Right(4) != "Beta")
    {
        return -6;
    }

    if (Trimmed.RightChop(5) != "Beta")
    {
        return -7;
    }

    if (Trimmed.Mid(5) != "Beta")
    {
        return -8;
    }

    if (Trimmed.Mid(2, 3) != "pha")
    {
        return -9;
    }

    if (!Trimmed.Equals("alphabeta", false))
    {
        return -10;
    }

    if (Trimmed.Compare("ALPHABETA", false) != 0)
    {
        return -11;
    }

    if (!Trimmed.StartsWith("alpha", false))
    {
        return -12;
    }

    if (!Trimmed.EndsWith("BETA", false))
    {
        return -13;
    }

    if (!Trimmed.Contains("PHAB", false))
    {
        return -14;
    }

    if (Trimmed.Find("Beta") != 5)
    {
        return -15;
    }

    if (Trimmed.Find("a", false, true) != 8)
    {
        return -16;
    }

    int Position = -1;
    if (!Trimmed.FindChar(66, Position) || Position != 5)
    {
        return -17;
    }

    if (!Trimmed.FindLastChar(97, Position) || Position != 8)
    {
        return -18;
    }

    FString LeftPart;
    FString RightPart;
    if (!Trimmed.Split("ha", LeftPart, RightPart))
    {
        return -19;
    }

    if (LeftPart != "Alp" || RightPart != "Beta")
    {
        return -20;
    }

    return RightPart.Len();
}
)";

		asIScriptFunction* Function = BuildFunction("FStringQueries", Script, "int RunQueryMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 4));
	}

	TEST_METHOD(TransformMethods)
	{
		static const char Script[] = R"(
int RunTransformMethods()
{
    FString Value = "  OtterAngle  ";
    if (Value.TrimStart() != "OtterAngle  ")
    {
        return -1;
    }

    if (Value.TrimEnd() != "  OtterAngle")
    {
        return -2;
    }

    if (Value.TrimStartAndEnd() != "OtterAngle")
    {
        return -3;
    }

    Value.TrimStartInline();
    if (Value != "OtterAngle  ")
    {
        return -4;
    }

    Value.TrimEndInline();
    if (Value != "OtterAngle")
    {
        return -5;
    }

    if (Value.ToUpper() != "OTTERANGLE")
    {
        return -6;
    }

    if (Value.ToLower() != "otterangle")
    {
        return -7;
    }

    Value.ToUpperInline();
    if (Value != "OTTERANGLE")
    {
        return -8;
    }

    Value.ToLowerInline();
    if (Value != "otterangle")
    {
        return -9;
    }

    if (Value.Replace("angle", "Script", false) != "otterScript")
    {
        return -10;
    }

    Value.ReplaceInline("otter", "Mini", false);
    if (Value != "Miniangle")
    {
        return -11;
    }

    if (!Value.RemoveFromStart("mini", false))
    {
        return -12;
    }

    if (!Value.RemoveFromEnd("ANGLE", false))
    {
        return -13;
    }

    if (!Value.IsEmpty())
    {
        return -14;
    }

    FString Sample = "  turnip  ";
    Sample.TrimStartAndEndInline();
    Sample.ReverseString();
    if (Sample != "pinrut")
    {
        return -15;
    }

    return Sample.Len();
}
)";

		asIScriptFunction* Function = BuildFunction("FStringTransforms", Script, "int RunTransformMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 6));
	}

	TEST_METHOD(IndexOutOfRangeThrows)
	{
		static const char Script[] = R"(
int RunOutOfRange()
{
    FString Value = "A";
    return Value[1];
}
)";

		asIScriptFunction* Function = BuildFunction("FStringOutOfRange", Script, "int RunOutOfRange()");
		const int Result = ExecuteFunction(Function);
		ASSERT_THAT(IsTrue(Result == asEXECUTION_EXCEPTION));
		ASSERT_THAT(IsTrue(FString(Context->GetExceptionString()) == FString("FString index out of range")));
	}

	TEST_METHOD(ScriptCallsUnrealWithFString)
	{
		static const char Script[] = R"(
int RunScriptToUnrealRoundTrip()
{
    FString Value = "ScriptToUnreal";
    if (!UnrealAcceptsFString(Value))
    {
        return -1;
    }

    FString Echoed = UnrealEchoFString(Value);
    if (Echoed != "Unreal<ScriptToUnreal>")
    {
        return -2;
    }

    FString Filled;
    UnrealFillFString(Filled);
    if (Filled != "FilledByUnreal")
    {
        return -3;
    }

    FString Joined = UnrealJoinFString("[", Filled, "]");
    if (Joined != "[FilledByUnreal]")
    {
        return -4;
    }

    return Echoed.Len() + Joined.Len();
}
)";

		asIScriptFunction* Function = BuildFunction("FStringScriptToUnreal", Script, "int RunScriptToUnrealRoundTrip()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 38));
	}

	TEST_METHOD(UnrealCallsScriptWithFString)
	{
		static const char Script[] = R"(
FString BuildStringForUnreal(const FString &in Prefix, const FString &in Value)
{
    FString Result = Prefix + Value;
    Result += "!";
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FStringUnrealToScript", Script, "FString BuildStringForUnreal(const FString &in Prefix, const FString &in Value)");
		ASSERT_THAT(IsNotNull(Function));

		FString Prefix = TEXT("Unreal-");
		FString Value = TEXT("Input");

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));
		ASSERT_THAT(IsTrue(Context->SetArgObject(0, &Prefix) >= 0));
		ASSERT_THAT(IsTrue(Context->SetArgObject(1, &Value) >= 0));

		const int ExecuteResult = Context->Execute();
		if (ExecuteResult == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		ASSERT_THAT(IsTrue(ExecuteResult == asEXECUTION_FINISHED));

		const FString* ReturnValue = static_cast<const FString*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(*ReturnValue == FString(TEXT("Unreal-Input!"))));
	}

	TEST_METHOD(AppendInt)
	{
		static const char Script[] = R"(
int RunAppendInt()
{
    FString Value = "Count: ";
    Value.AppendInt(42);
    if (Value != "Count: 42")
    {
        return -1;
    }

    FString Negative = "Num: ";
    Negative.AppendInt(-5);
    if (Negative != "Num: -5")
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringAppendInt", Script, "int RunAppendInt()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsValidIndex)
	{
		static const char Script[] = R"(
int RunIsValidIndex()
{
    FString Value = "ABC";
    if (!Value.IsValidIndex(0))
    {
        return -1;
    }

    if (!Value.IsValidIndex(2))
    {
        return -2;
    }

    if (Value.IsValidIndex(3))
    {
        return -3;
    }

    if (Value.IsValidIndex(-1))
    {
        return -4;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringIsValidIndex", Script, "int RunIsValidIndex()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToBool)
	{
		static const char Script[] = R"(
int RunToBool()
{
    if (!"true".ToBool())
    {
        return -1;
    }

    if (!"True".ToBool())
    {
        return -2;
    }

    if (!"1".ToBool())
    {
        return -3;
    }

    if ("false".ToBool())
    {
        return -4;
    }

    if ("0".ToBool())
    {
        return -5;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringToBool", Script, "int RunToBool()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(LeftPad)
	{
		static const char Script[] = R"(
int RunLeftPad()
{
    FString Value = "Hi";
    FString Padded = Value.LeftPad(5);
    if (Padded != "   Hi")
    {
        return -1;
    }

    FString NoPad = Value.LeftPad(1);
    if (NoPad != "Hi")
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringLeftPad", Script, "int RunLeftPad()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RightPad)
	{
		static const char Script[] = R"(
int RunRightPad()
{
    FString Value = "Hi";
    FString Padded = Value.RightPad(5);
    if (Padded != "Hi   ")
    {
        return -1;
    }

    FString NoPad = Value.RightPad(1);
    if (NoPad != "Hi")
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringRightPad", Script, "int RunRightPad()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(TrimQuotes)
	{
		static const char Script[] = R"(
int RunTrimQuotes()
{
    FString Quoted = "\"Hello\"";
    FString Trimmed = Quoted.TrimQuotes();
    if (Trimmed != "Hello")
    {
        return -1;
    }

    FString Unquoted = "NoQuotes";
    if (Unquoted.TrimQuotes() != "NoQuotes")
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringTrimQuotes", Script, "int RunTrimQuotes()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(TrimQuotesInline)
	{
		static const char Script[] = R"(
int RunTrimQuotesInline()
{
    FString Value = "\"World\"";
    Value.TrimQuotesInline();
    if (Value != "World")
    {
        return -1;
    }

    FString Plain = "NoBrackets";
    Plain.TrimQuotesInline();
    if (Plain != "NoBrackets")
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringTrimQuotesInline", Script, "int RunTrimQuotesInline()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RemoveSpacesInline)
	{
		static const char Script[] = R"(
int RunRemoveSpacesInline()
{
    FString Value = "a b c";
    Value.RemoveSpacesInline();

    if (Value != "abc")
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringRemoveSpacesInline", Script, "int RunRemoveSpacesInline()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Shrink)
	{
		static const char Script[] = R"(
int RunShrink()
{
    FString Value = "Hello World";
    Value.Empty();
    Value.Shrink();
    if (!Value.IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringShrink", Script, "int RunShrink()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ConvertTabsToSpacesInline)
	{
		static const char Script[] = R"(
int RunConvertTabsToSpacesInline()
{
    FString Value = "A	B";
    Value.ConvertTabsToSpacesInline(4);
    if (Value != "A   B")
    {
        return -1;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FStringConvertTabs", Script, "int RunConvertTabsToSpacesInline()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
