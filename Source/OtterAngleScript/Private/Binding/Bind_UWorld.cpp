// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/World.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"

#ifdef _MSC_VER
#pragma warning(disable:4191 4996)
#endif

#include "angelscript.h"
#include "AI/NavigationSystemBase.h"

namespace
{
	// --- UObject base method wrappers ---

	UClass* UWorld_GetClass(const UWorld* Value)
	{
		return Value->GetClass();
	}

	FString UWorld_GetName(const UWorld* Value)
	{
		return Value->GetName();
	}

	bool UWorld_IsA(const UWorld* Value, const UClass* SomeBase)
	{
		return Value->IsA(SomeBase);
	}

	// --- Level access ---

	UObject* UWorld_GetPersistentLevel(const UWorld* World)
	{
		return World->PersistentLevel;
	}

	int32 UWorld_GetNumLevels(const UWorld* World)
	{
		return World->GetNumLevels();
	}

	UObject* UWorld_GetLevel(const UWorld* World, int32 Index)
	{
		return World->GetLevel(Index);
	}

	// --- Navigation ---

	const class UNavigationSystemBase* UWorld_GetNavigationSystem(const UWorld* World)
	{
		return World->GetNavigationSystem();
	}

	// --- World type queries ---

	bool UWorld_IsGameWorld(const UWorld* World)
	{
		return World->IsGameWorld();
	}

	bool UWorld_IsEditorWorld(const UWorld* World)
	{
		return World->IsEditorWorld();
	}

	bool UWorld_IsPreviewWorld(const UWorld* World)
	{
		return World->IsPreviewWorld();
	}

	// --- Game state queries ---

	bool UWorld_GetBegunPlay(const UWorld* World)
	{
		return World->GetBegunPlay();
	}

	bool UWorld_IsMatchStarted(const UWorld* World)
	{
		return World->bMatchStarted;
	}

	bool UWorld_AreActorsInitialized(const UWorld* World)
	{
		return World->bActorsInitialized;
	}

	// --- Time methods ---

	float UWorld_GetAudioTimeSeconds(const UWorld* World)
	{
		return static_cast<float>(World->GetAudioTimeSeconds());
	}

	// --- Initialization ---

	bool UWorld_IsInitialized(const UWorld* World)
	{
		return World->bIsWorldInitialized;
	}

	// --- UWorld-specific string methods ---

	FString UWorld_GetMapName(const UWorld* World)
	{
		return World->GetMapName();
	}

	// --- Time-delta methods ---

	float UWorld_GetDeltaSeconds(const UWorld* World)
	{
		return World->GetDeltaSeconds();
	}

	float UWorld_GetRealTimeSeconds(const UWorld* World)
	{
		return World->GetRealTimeSeconds();
	}

	float UWorld_GetTimeSeconds(const UWorld* World)
	{
		return World->GetTimeSeconds();
	}

	float UWorld_GetUnpausedTimeSeconds(const UWorld* World)
	{
		return World->GetUnpausedTimeSeconds();
	}

	// --- Additional world state booleans ---

	bool UWorld_IsPaused(const UWorld* World)
	{
		return World->IsPaused();
	}

	bool UWorld_IsPlayInEditor(const UWorld* World)
	{
		return World->IsPlayInEditor();
	}

	bool UWorld_IsInSeamlessTravel(const UWorld* World)
	{
		return World->IsInSeamlessTravel();
	}

	bool UWorld_IsPreparingMapChange(const UWorld* World)
	{
		return World->IsPreparingMapChange();
	}

	bool UWorld_HasBegunPlay(const UWorld* World)
	{
		return World->HasBegunPlay();
	}

	bool UWorld_IsNavigationRebuilt(const UWorld* World)
	{
		return World->IsNavigationRebuilt();
	}

	bool UWorld_AllowAudioPlayback(const UWorld* World)
	{
		return World->AllowAudioPlayback();
	}

	bool UWorld_AreAlwaysLoadedLevelsLoaded(const UWorld* World)
	{
		return World->AreAlwaysLoadedLevelsLoaded();
	}

	bool UWorld_AllowLevelLoadRequests(const UWorld* World)
	{
		return World->AllowLevelLoadRequests();
	}

	// --- Additional object accessors ---

	UObject* UWorld_GetFirstPlayerController(const UWorld* World)
	{
		return World->GetFirstPlayerController();
	}

	UObject* UWorld_GetWorldSettings(const UWorld* World)
	{
		return World->GetWorldSettings();
	}

	UObject* UWorld_GetCurrentLevel(const UWorld* World)
	{
		return World->GetCurrentLevel();
	}

	AWorldDataLayers* UWorld_GetWorldDataLayers(const UWorld* World)
	{
		return World->GetWorldDataLayers();
	}

	// --- Additional int/float accessors ---

	int32 UWorld_GetActorCount(const UWorld* World)
	{
		return World->GetActorCount();
	}

	int32 UWorld_GetNumControllers(const UWorld* World)
	{
		return World->GetNumControllers();
	}

	int32 UWorld_GetNumPlayerControllers(const UWorld* World)
	{
		return World->GetNumPlayerControllers();
	}

	float UWorld_GetDefaultGravityZ(const UWorld* World)
	{
		return World->GetDefaultGravityZ();
	}

	float UWorld_GetGravityZ(const UWorld* World)
	{
		return World->GetGravityZ();
	}

	// --- Additional string methods ---

	FString UWorld_GetLocalURL(const UWorld* World)
	{
		return World->GetLocalURL();
	}

	FString UWorld_GetAddressURL(const UWorld* World)
	{
		return World->GetAddressURL();
	}

	// --- Mutators ---

	bool UWorld_ServerTravel(UWorld* World, const FString& InURL, bool bAbsolute)
	{
		return World->ServerTravel(InURL, bAbsolute);
	}
}

void Bind_UWorld(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;

	// --- Level access ---

	REGISTER_METHOD(UWorld, "UObject GetPersistentLevel() const", asFUNCTION(UWorld_GetPersistentLevel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "int GetNumLevels() const", asFUNCTION(UWorld_GetNumLevels), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "UObject GetLevel(int Index) const", asFUNCTION(UWorld_GetLevel), asCALL_CDECL_OBJFIRST);

	// --- Navigation ---

	REGISTER_METHOD(UWorld, "const UNavigationSystemBase GetNavigationSystem() const", asFUNCTION(UWorld_GetNavigationSystem), asCALL_CDECL_OBJFIRST);

	// --- World type queries ---

	REGISTER_METHOD(UWorld, "bool IsGameWorld() const", asFUNCTION(UWorld_IsGameWorld), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsEditorWorld() const", asFUNCTION(UWorld_IsEditorWorld), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsPreviewWorld() const", asFUNCTION(UWorld_IsPreviewWorld), asCALL_CDECL_OBJFIRST);

	// --- Game state queries ---

	REGISTER_METHOD(UWorld, "bool GetBegunPlay() const", asFUNCTION(UWorld_GetBegunPlay), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsMatchStarted() const", asFUNCTION(UWorld_IsMatchStarted), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool AreActorsInitialized() const", asFUNCTION(UWorld_AreActorsInitialized), asCALL_CDECL_OBJFIRST);

	// --- Time methods ---

	REGISTER_METHOD(UWorld, "float GetAudioTimeSeconds() const", asFUNCTION(UWorld_GetAudioTimeSeconds), asCALL_CDECL_OBJFIRST);

	// --- Initialization ---

	REGISTER_METHOD(UWorld, "bool IsInitialized() const", asFUNCTION(UWorld_IsInitialized), asCALL_CDECL_OBJFIRST);

	// --- UWorld-specific string methods ---

	REGISTER_METHOD(UWorld, "FString GetMapName() const", asFUNCTION(UWorld_GetMapName), asCALL_CDECL_OBJFIRST);

	// --- Time-delta methods ---

	REGISTER_METHOD(UWorld, "float GetDeltaSeconds() const", asFUNCTION(UWorld_GetDeltaSeconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "float GetRealTimeSeconds() const", asFUNCTION(UWorld_GetRealTimeSeconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "float GetTimeSeconds() const", asFUNCTION(UWorld_GetTimeSeconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "float GetUnpausedTimeSeconds() const", asFUNCTION(UWorld_GetUnpausedTimeSeconds), asCALL_CDECL_OBJFIRST);

	// --- Additional world state booleans ---

	REGISTER_METHOD(UWorld, "bool IsPaused() const", asFUNCTION(UWorld_IsPaused), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsPlayInEditor() const", asFUNCTION(UWorld_IsPlayInEditor), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsInSeamlessTravel() const", asFUNCTION(UWorld_IsInSeamlessTravel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsPreparingMapChange() const", asFUNCTION(UWorld_IsPreparingMapChange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool HasBegunPlay() const", asFUNCTION(UWorld_HasBegunPlay), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool IsNavigationRebuilt() const", asFUNCTION(UWorld_IsNavigationRebuilt), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool AllowAudioPlayback() const", asFUNCTION(UWorld_AllowAudioPlayback), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool AreAlwaysLoadedLevelsLoaded() const", asFUNCTION(UWorld_AreAlwaysLoadedLevelsLoaded), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "bool AllowLevelLoadRequests() const", asFUNCTION(UWorld_AllowLevelLoadRequests), asCALL_CDECL_OBJFIRST);

	// --- Additional object accessors ---

	REGISTER_METHOD(UWorld, "UObject GetFirstPlayerController() const", asFUNCTION(UWorld_GetFirstPlayerController), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "UObject GetWorldSettings() const", asFUNCTION(UWorld_GetWorldSettings), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "UObject GetCurrentLevel() const", asFUNCTION(UWorld_GetCurrentLevel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "AWorldDataLayers GetWorldDataLayers() const", asFUNCTION(UWorld_GetWorldDataLayers), asCALL_CDECL_OBJFIRST);

	// --- Additional int/float accessors ---

	REGISTER_METHOD(UWorld, "int GetActorCount() const", asFUNCTION(UWorld_GetActorCount), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "int GetNumControllers() const", asFUNCTION(UWorld_GetNumControllers), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "int GetNumPlayerControllers() const", asFUNCTION(UWorld_GetNumPlayerControllers), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "float GetDefaultGravityZ() const", asFUNCTION(UWorld_GetDefaultGravityZ), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "float GetGravityZ() const", asFUNCTION(UWorld_GetGravityZ), asCALL_CDECL_OBJFIRST);

	// --- Additional string methods ---

	REGISTER_METHOD(UWorld, "FString GetLocalURL() const", asFUNCTION(UWorld_GetLocalURL), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UWorld, "FString GetAddressURL() const", asFUNCTION(UWorld_GetAddressURL), asCALL_CDECL_OBJFIRST);

	// --- Mutators ---

	REGISTER_METHOD(UWorld, "bool ServerTravel(const FString &in URL, bool bAbsolute = false)", asFUNCTION(UWorld_ServerTravel), asCALL_CDECL_OBJFIRST);
}
