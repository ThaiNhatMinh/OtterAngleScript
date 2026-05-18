// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

namespace
{
	bool UKismetSystemLibrary_IsValid(UObject* Object)
	{
		return UKismetSystemLibrary::IsValid(Object);
	}

	bool UKismetSystemLibrary_IsValidClass(UClass* Class)
	{
		return UKismetSystemLibrary::IsValidClass(Class);
	}

	FString UKismetSystemLibrary_GetObjectName(UObject* Object)
	{
		return UKismetSystemLibrary::GetObjectName(Object);
	}

	FString UKismetSystemLibrary_GetPathName(UObject* Object)
	{
		return UKismetSystemLibrary::GetPathName(Object);
	}

	FString UKismetSystemLibrary_GetDisplayName(UObject* Object)
	{
		return UKismetSystemLibrary::GetDisplayName(Object);
	}

	FString UKismetSystemLibrary_GetClassDisplayName(UClass* Class)
	{
		return UKismetSystemLibrary::GetClassDisplayName(Class);
	}

	UObject* UKismetSystemLibrary_GetOuterObject(UObject* Object)
	{
		return UKismetSystemLibrary::GetOuterObject(Object);
	}

	UObject* UKismetSystemLibrary_DuplicateObject(UObject* SourceObject, UObject* Outer, const FName& Name)
	{
		return UKismetSystemLibrary::DuplicateObject(SourceObject, Outer, Name);
	}

	FString UKismetSystemLibrary_GetEngineVersion()
	{
		return UKismetSystemLibrary::GetEngineVersion();
	}

	FString UKismetSystemLibrary_GetProjectDirectory()
	{
		return UKismetSystemLibrary::GetProjectDirectory();
	}

	FString UKismetSystemLibrary_ConvertToRelativePath(const FString& Filename)
	{
		return UKismetSystemLibrary::ConvertToRelativePath(Filename);
	}

	FString UKismetSystemLibrary_ConvertToAbsolutePath(const FString& Filename)
	{
		return UKismetSystemLibrary::ConvertToAbsolutePath(Filename);
	}

	FString UKismetSystemLibrary_NormalizeFilename(const FString& Filename)
	{
		return UKismetSystemLibrary::NormalizeFilename(Filename);
	}

	int64 UKismetSystemLibrary_GetFrameCount()
	{
		return UKismetSystemLibrary::GetFrameCount();
	}
}

void Bind_UKismetSystemLibrary(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->SetDefaultNamespace("UKismetSystemLibrary");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("bool IsValid(UObject@ Object)", asFUNCTION(UKismetSystemLibrary_IsValid), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool IsValidClass(UClass@ Class)", asFUNCTION(UKismetSystemLibrary_IsValidClass), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString GetObjectName(UObject@ Object)", asFUNCTION(UKismetSystemLibrary_GetObjectName), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString GetPathName(UObject@ Object)", asFUNCTION(UKismetSystemLibrary_GetPathName), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString GetDisplayName(UObject@ Object)", asFUNCTION(UKismetSystemLibrary_GetDisplayName), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString GetClassDisplayName(UClass@ Class)", asFUNCTION(UKismetSystemLibrary_GetClassDisplayName), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("UObject@ GetOuterObject(UObject@ Object)", asFUNCTION(UKismetSystemLibrary_GetOuterObject), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("UObject@ DuplicateObject(UObject@ SourceObject, UObject@ Outer, const FName &in Name)", asFUNCTION(UKismetSystemLibrary_DuplicateObject), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString GetEngineVersion()", asFUNCTION(UKismetSystemLibrary_GetEngineVersion), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString GetProjectDirectory()", asFUNCTION(UKismetSystemLibrary_GetProjectDirectory), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString ConvertToRelativePath(const FString &in Filename)", asFUNCTION(UKismetSystemLibrary_ConvertToRelativePath), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString ConvertToAbsolutePath(const FString &in Filename)", asFUNCTION(UKismetSystemLibrary_ConvertToAbsolutePath), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString NormalizeFilename(const FString &in Filename)", asFUNCTION(UKismetSystemLibrary_NormalizeFilename), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("int64 GetFrameCount()", asFUNCTION(UKismetSystemLibrary_GetFrameCount), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
