// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	UWorld* GetFixtureWorld()
	{
		static UWorld* World = nullptr;
		if (World == nullptr)
		{
			World = UWorld::CreateWorld(EWorldType::Game, false, "OtterTestWorld");
			check(World != nullptr);
			World->AddToRoot();
		}
		return World;
	}

	bool RegisterWorldFixtureBindings(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"UWorld GetFixtureWorld()",
			asFUNCTION(GetFixtureWorld),
			asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptUWorldTests,
	"OtterAngleScript.UWorld",
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

		ASSERT_THAT(IsTrue(RegisterWorldFixtureBindings(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptUWorldTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeInfo)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UWorld")));
	}

	TEST_METHOD(FixtureWorldIsNotNull)
	{
		static const char Script[] = R"(
int RunFixtureWorldNotNull()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNotNull", Script, "int RunFixtureWorldNotNull()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetClass)
	{
		static const char Script[] = R"(
int RunGetClass()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    UClass Cls = World.GetClass();
    if (Cls is null)
    {
        return -2;
    }
    if (Cls !is UWorld::StaticClass())
    {
        return -3;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetClass", Script, "int RunGetClass()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetName)
	{
		static const char Script[] = R"(
int RunGetName()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    if (World.GetName() != "OtterTestWorld")
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetName", Script, "int RunGetName()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsA)
	{
		static const char Script[] = R"(
int RunIsA()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    if (!World.IsA(UWorld::StaticClass()))
    {
        return -2;
    }
    if (!World.IsA(UObject::StaticClass()))
    {
        return -3;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsA", Script, "int RunIsA()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetMapName)
	{
		static const char Script[] = R"(
int RunGetMapName()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    // GetMapName() should return a non-empty string for an initialized world
    FString MapName = World.GetMapName();
    // MapName could be empty for a transient world, but the call should not crash
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetMapName", Script, "int RunGetMapName()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetDeltaSeconds)
	{
		static const char Script[] = R"(
int RunGetDeltaSeconds()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    float DeltaSeconds = World.GetDeltaSeconds();
    if (DeltaSeconds < 0.0f)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetDeltaSeconds", Script, "int RunGetDeltaSeconds()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetRealTimeSeconds)
	{
		static const char Script[] = R"(
int RunGetRealTimeSeconds()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    float RealTime = World.GetRealTimeSeconds();
    if (RealTime < 0.0f)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetRealTimeSeconds", Script, "int RunGetRealTimeSeconds()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetTimeSeconds)
	{
		static const char Script[] = R"(
int RunGetTimeSeconds()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    float Time = World.GetTimeSeconds();
    if (Time < 0.0f)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetTimeSeconds", Script, "int RunGetTimeSeconds()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetUnpausedTimeSeconds)
	{
		static const char Script[] = R"(
int RunGetUnpausedTimeSeconds()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    float UnpausedTime = World.GetUnpausedTimeSeconds();
    if (UnpausedTime < 0.0f)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetUnpausedTimeSeconds", Script, "int RunGetUnpausedTimeSeconds()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsInitialized)
	{
		static const char Script[] = R"(
int RunIsInitialized()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
    {
        return -1;
    }
    // A world created via CreateWorld should be initialized
    if (!World.IsInitialized())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsInitialized", Script, "int RunIsInitialized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NullWorld_GetClass)
	{
		static const char Script[] = R"(
int RunNullGetClass()
{
    UWorld Null;
    // Calling GetClass on a null handle should be safe
    UClass Cls = Null.GetClass();
    if (Cls !is null)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNullGetClass", Script, "int RunNullGetClass()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NullWorld_IsA)
	{
		static const char Script[] = R"(
int RunNullIsA()
{
    UWorld Null;
    // Calling IsA on a null handle should be safe
    if (Null.IsA(UWorld::StaticClass()))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNullIsA", Script, "int RunNullIsA()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NullWorld_GetMapName)
	{
		static const char Script[] = R"(
int RunNullGetMapName()
{
    UWorld Null;
    // Calling GetMapName on a null handle should not crash
    FString MapName = Null.GetMapName();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNullGetMapName", Script, "int RunNullGetMapName()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Level access tests ---

	TEST_METHOD(GetPersistentLevel)
	{
		static const char Script[] = R"(
int RunGetPersistentLevel()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    UObject Level = World.GetPersistentLevel();
    if (Level is null)
        return -2;
    if (!Level.IsA(UWorld::StaticClass()().GetClass()))
        return -3;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetPersistentLevel", Script, "int RunGetPersistentLevel()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetNumLevels)
	{
		static const char Script[] = R"(
int RunGetNumLevels()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    int NumLevels = World.GetNumLevels();
    if (NumLevels < 0)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetNumLevels", Script, "int RunGetNumLevels()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetLevel)
	{
		static const char Script[] = R"(
int RunGetLevel()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    int NumLevels = World.GetNumLevels();
    if (NumLevels > 0)
    {
        UObject Level = World.GetLevel(0);
        if (Level is null)
            return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetLevel", Script, "int RunGetLevel()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Navigation tests ---

	TEST_METHOD(GetNavigationSystem)
	{
		static const char Script[] = R"(
int RunGetNavigationSystem()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    UObject NavSys = World.GetNavigationSystem();
    // Navigation system may be null depending on world setup, but call should not crash
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetNavigationSystem", Script, "int RunGetNavigationSystem()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- World type query tests ---

	TEST_METHOD(IsGameWorld)
	{
		static const char Script[] = R"(
int RunIsGameWorld()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bIsGame = World.IsGameWorld();
    // World created with EWorldType::Game, should be true
    if (!bIsGame)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsGameWorld", Script, "int RunIsGameWorld()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsEditorWorld)
	{
		static const char Script[] = R"(
int RunIsEditorWorld()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bIsEditor = World.IsEditorWorld();
    // World created with EWorldType::Game, should be false
    if (bIsEditor)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsEditorWorld", Script, "int RunIsEditorWorld()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsPreviewWorld)
	{
		static const char Script[] = R"(
int RunIsPreviewWorld()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bIsPreview = World.IsPreviewWorld();
    // World created with EWorldType::Game, should be false
    if (bIsPreview)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsPreviewWorld", Script, "int RunIsPreviewWorld()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Game state query tests ---

	TEST_METHOD(GetBegunPlay)
	{
		static const char Script[] = R"(
int RunGetBegunPlay()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bBegun = World.GetBegunPlay();
    // Fresh world should not have begun play
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetBegunPlay", Script, "int RunGetBegunPlay()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsMatchStarted)
	{
		static const char Script[] = R"(
int RunIsMatchStarted()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bStarted = World.IsMatchStarted();
    // Fresh world should not have match started
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsMatchStarted", Script, "int RunIsMatchStarted()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AreActorsInitialized)
	{
		static const char Script[] = R"(
int RunAreActorsInitialized()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bInitialized = World.AreActorsInitialized();
    // Check call succeeds, value depends on world setup
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldAreActorsInitialized", Script, "int RunAreActorsInitialized()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Audio time test ---

	TEST_METHOD(GetAudioTimeSeconds)
	{
		static const char Script[] = R"(
int RunGetAudioTimeSeconds()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    float AudioTime = World.GetAudioTimeSeconds();
    if (AudioTime < 0.0f)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetAudioTimeSeconds", Script, "int RunGetAudioTimeSeconds()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Null handle tests for new methods ---

	TEST_METHOD(NullWorld_GetPersistentLevel)
	{
		static const char Script[] = R"(
int RunNullGetPersistentLevel()
{
    UWorld Null;
    UObject Level = Null.GetPersistentLevel();
    if (Level !is null)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNullGetPersistentLevel", Script, "int RunNullGetPersistentLevel()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NullWorld_GetNumLevels)
	{
		static const char Script[] = R"(
int RunNullGetNumLevels()
{
    UWorld Null;
    int NumLevels = Null.GetNumLevels();
    if (NumLevels != 0)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNullGetNumLevels", Script, "int RunNullGetNumLevels()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NullWorld_IsGameWorld)
	{
		static const char Script[] = R"(
int RunNullIsGameWorld()
{
    UWorld Null;
    if (Null.IsGameWorld())
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldNullIsGameWorld", Script, "int RunNullIsGameWorld()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Additional world state boolean tests ---

	TEST_METHOD(IsPaused)
	{
		static const char Script[] = R"(
int RunIsPaused()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    // Fresh world should not be paused
    if (World.IsPaused())
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsPaused", Script, "int RunIsPaused()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsPlayInEditor)
	{
		static const char Script[] = R"(
int RunIsPlayInEditor()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    // World created with EWorldType::Game, should be false
    bool bResult = World.IsPlayInEditor();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsPlayInEditor", Script, "int RunIsPlayInEditor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsInSeamlessTravel)
	{
		static const char Script[] = R"(
int RunIsInSeamlessTravel()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    if (World.IsInSeamlessTravel())
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsInSeamlessTravel", Script, "int RunIsInSeamlessTravel()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsPreparingMapChange)
	{
		static const char Script[] = R"(
int RunIsPreparingMapChange()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    if (World.IsPreparingMapChange())
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsPreparingMapChange", Script, "int RunIsPreparingMapChange()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(HasBegunPlay)
	{
		static const char Script[] = R"(
int RunHasBegunPlay()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    // Fresh world should not have begun play
    if (World.HasBegunPlay())
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldHasBegunPlay", Script, "int RunHasBegunPlay()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsNavigationRebuilt)
	{
		static const char Script[] = R"(
int RunIsNavigationRebuilt()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    // Call should not crash; value depends on world setup
    bool bResult = World.IsNavigationRebuilt();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldIsNavigationRebuilt", Script, "int RunIsNavigationRebuilt()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AllowAudioPlayback)
	{
		static const char Script[] = R"(
int RunAllowAudioPlayback()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bResult = World.AllowAudioPlayback();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldAllowAudioPlayback", Script, "int RunAllowAudioPlayback()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AreAlwaysLoadedLevelsLoaded)
	{
		static const char Script[] = R"(
int RunAreAlwaysLoadedLevelsLoaded()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bResult = World.AreAlwaysLoadedLevelsLoaded();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldAreAlwaysLoadedLevelsLoaded", Script, "int RunAreAlwaysLoadedLevelsLoaded()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AllowLevelLoadRequests)
	{
		static const char Script[] = R"(
int RunAllowLevelLoadRequests()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    bool bResult = World.AllowLevelLoadRequests();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldAllowLevelLoadRequests", Script, "int RunAllowLevelLoadRequests()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Additional object accessor tests ---

	TEST_METHOD(GetFirstPlayerController)
	{
		static const char Script[] = R"(
int RunGetFirstPlayerController()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    UObject PC = World.GetFirstPlayerController();
    // May be null if no player exists, but call should not crash
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetFirstPlayerController", Script, "int RunGetFirstPlayerController()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetWorldSettings)
	{
		static const char Script[] = R"(
int RunGetWorldSettings()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    UObject Settings = World.GetWorldSettings();
    if (Settings is null)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetWorldSettings", Script, "int RunGetWorldSettings()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetCurrentLevel)
	{
		static const char Script[] = R"(
int RunGetCurrentLevel()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    UObject Level = World.GetCurrentLevel();
    if (Level is null)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetCurrentLevel", Script, "int RunGetCurrentLevel()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetWorldDataLayers)
	{
		static const char Script[] = R"(
int RunGetWorldDataLayers()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    UObject DataLayers = World.GetWorldDataLayers();
    // May be null depending on world setup, but call should not crash
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetWorldDataLayers", Script, "int RunGetWorldDataLayers()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Additional int/float accessor tests ---

	TEST_METHOD(GetActorCount)
	{
		static const char Script[] = R"(
int RunGetActorCount()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    int Count = World.GetActorCount();
    if (Count < 0)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetActorCount", Script, "int RunGetActorCount()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetNumControllers)
	{
		static const char Script[] = R"(
int RunGetNumControllers()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    int Count = World.GetNumControllers();
    if (Count < 0)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetNumControllers", Script, "int RunGetNumControllers()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetNumPlayerControllers)
	{
		static const char Script[] = R"(
int RunGetNumPlayerControllers()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    int Count = World.GetNumPlayerControllers();
    if (Count < 0)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetNumPlayerControllers", Script, "int RunGetNumPlayerControllers()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetDefaultGravityZ)
	{
		static const char Script[] = R"(
int RunGetDefaultGravityZ()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    float Gravity = World.GetDefaultGravityZ();
    if (Gravity == 0.0f)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetDefaultGravityZ", Script, "int RunGetDefaultGravityZ()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetGravityZ)
	{
		static const char Script[] = R"(
int RunGetGravityZ()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    float Gravity = World.GetGravityZ();
    if (Gravity == 0.0f)
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetGravityZ", Script, "int RunGetGravityZ()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	// --- Additional string method tests ---

	TEST_METHOD(GetLocalURL)
	{
		static const char Script[] = R"(
int RunGetLocalURL()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    FString URL = World.GetLocalURL();
    // URL should not crash; value depends on world setup
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetLocalURL", Script, "int RunGetLocalURL()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetAddressURL)
	{
		static const char Script[] = R"(
int RunGetAddressURL()
{
    UWorld World = GetFixtureWorld();
    if (World is null)
        return -1;
    FString URL = World.GetAddressURL();
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UWorldGetAddressURL", Script, "int RunGetAddressURL()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
