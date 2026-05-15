// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	struct FCapturedLogEntry
	{
		ELogVerbosity::Type Verbosity;
		FString Message;
	};

	class FOtterAngleScriptLogCapture final : public FOutputDevice
	{
	public:
		void Attach()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		void Detach()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		bool Contains(ELogVerbosity::Type Verbosity, const TCHAR* Message) const
		{
			for (const FCapturedLogEntry& Entry : Entries)
			{
				if (Entry.Verbosity == Verbosity && Entry.Message == Message)
				{
					return true;
				}
			}

			return false;
		}

	protected:
		void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override
		{
			static const FName CategoryName(TEXT("LogOtterAngleScript"));
			if (Category == CategoryName)
			{
				Entries.Add({Verbosity, V});
			}
		}

	private:
		TArray<FCapturedLogEntry> Entries;
	};
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptLoggingTests,
	"OtterAngleScript.Logging",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	TEST_METHOD(GlobalFunctionsAreRegisteredAndCallable)
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		asIScriptEngine* Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));

		ASSERT_THAT(IsNotNull(Engine->GetGlobalFunctionByDecl("void Log(const FString &in Message)")));
		ASSERT_THAT(IsNotNull(Engine->GetGlobalFunctionByDecl("void LogWarning(const FString &in Message)")));
		ASSERT_THAT(IsNotNull(Engine->GetGlobalFunctionByDecl("void LogError(const FString &in Message)")));

		asIScriptModule* ScriptModule = Engine->GetModule("OtterAngleScriptLoggingTest", asGM_ALWAYS_CREATE);
		ASSERT_THAT(IsNotNull(ScriptModule));

		static const char Script[] = R"(
void RunLoggingTest()
{
    Log("Info from script");
    LogWarning("Warning from script");
    LogError("Error from script");
}
)";

		ASSERT_THAT(IsTrue(ScriptModule->AddScriptSection("LoggingTests", Script) >= 0));
		ASSERT_THAT(IsTrue(ScriptModule->Build() >= 0));

		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl("void RunLoggingTest()");
		ASSERT_THAT(IsNotNull(Function));

		asIScriptContext* Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));

		FOtterAngleScriptLogCapture Capture;
		
		TestRunner->AddExpectedError(TEXT("Error from script")); // Expect the error log to be captured as an error by the test framework
		Capture.Attach();

		const int Result = Context->Execute();

		Capture.Detach();

		if (Result == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		ASSERT_THAT(IsTrue(Result == asEXECUTION_FINISHED));
		ASSERT_THAT(IsTrue(Capture.Contains(ELogVerbosity::Log, TEXT("Info from script"))));
		ASSERT_THAT(IsTrue(Capture.Contains(ELogVerbosity::Warning, TEXT("Warning from script"))));
		ASSERT_THAT(IsTrue(Capture.Contains(ELogVerbosity::Verbose, TEXT("Error from script"))));

		Context->Release();
		ScriptModule->Discard();
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
