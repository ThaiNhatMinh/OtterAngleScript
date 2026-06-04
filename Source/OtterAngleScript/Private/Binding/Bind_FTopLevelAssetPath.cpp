// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "UObject/TopLevelAssetPath.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FTopLevelAssetPath_DefaultConstruct(FTopLevelAssetPath* Memory)
	{
		new (Memory) FTopLevelAssetPath();
	}

	static void FTopLevelAssetPath_CopyConstruct(const FTopLevelAssetPath& Other, FTopLevelAssetPath* Memory)
	{
		new (Memory) FTopLevelAssetPath(Other);
	}

	static void FTopLevelAssetPath_ConstructComponents(const FName& InPackageName, const FName& InAssetName, FTopLevelAssetPath* Memory)
	{
		new (Memory) FTopLevelAssetPath(InPackageName, InAssetName);
	}

	static void FTopLevelAssetPath_ConstructFromString(const FString& Path, FTopLevelAssetPath* Memory)
	{
		new (Memory) FTopLevelAssetPath(Path);
	}

	static void FTopLevelAssetPath_Destruct(FTopLevelAssetPath* Memory)
	{
		Memory->~FTopLevelAssetPath();
	}

	// --- Operators ---

	static FTopLevelAssetPath& FTopLevelAssetPath_Assign(FTopLevelAssetPath& Value, const FTopLevelAssetPath& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FTopLevelAssetPath_OpEquals(const FTopLevelAssetPath& Value, const FTopLevelAssetPath& Other)
	{
		return Value == Other;
	}

	// --- Property accessors ---

	static FName FTopLevelAssetPath_GetPackageName(const FTopLevelAssetPath& Value)
	{
		return Value.GetPackageName();
	}

	static FName FTopLevelAssetPath_GetAssetName(const FTopLevelAssetPath& Value)
	{
		return Value.GetAssetName();
	}

	// --- Methods ---

	static bool FTopLevelAssetPath_IsValid(const FTopLevelAssetPath& Value)
	{
		return Value.IsValid();
	}

	static bool FTopLevelAssetPath_IsNull(const FTopLevelAssetPath& Value)
	{
		return Value.IsNull();
	}

	static void FTopLevelAssetPath_Reset(FTopLevelAssetPath& Value)
	{
		Value.Reset();
	}

	static FString FTopLevelAssetPath_ToString(const FTopLevelAssetPath& Value)
	{
		return Value.ToString();
	}

	static int32 FTopLevelAssetPath_Compare(const FTopLevelAssetPath& Value, const FTopLevelAssetPath& Other)
	{
		return Value.Compare(Other);
	}
}

void Bind_FTopLevelAssetPath(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FTopLevelAssetPath, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FTopLevelAssetPath_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTopLevelAssetPath, asBEHAVE_CONSTRUCT, "void f(const FTopLevelAssetPath &in Other)", asFUNCTION(FTopLevelAssetPath_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTopLevelAssetPath, asBEHAVE_CONSTRUCT, "void f(const FName &in PackageName, const FName &in AssetName)", asFUNCTION(FTopLevelAssetPath_ConstructComponents), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTopLevelAssetPath, asBEHAVE_CONSTRUCT, "void f(const FString &in Path)", asFUNCTION(FTopLevelAssetPath_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTopLevelAssetPath, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FTopLevelAssetPath_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FTopLevelAssetPath, "FTopLevelAssetPath &opAssign(const FTopLevelAssetPath &in Other)", asFUNCTION(FTopLevelAssetPath_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTopLevelAssetPath, "bool opEquals(const FTopLevelAssetPath &in Other) const", asFUNCTION(FTopLevelAssetPath_OpEquals), asCALL_CDECL_OBJFIRST);

	// Property accessors
	REGISTER_METHOD(FTopLevelAssetPath, "FName get_PackageName() const", asFUNCTION(FTopLevelAssetPath_GetPackageName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTopLevelAssetPath, "FName get_AssetName() const", asFUNCTION(FTopLevelAssetPath_GetAssetName), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FTopLevelAssetPath, "bool IsValid() const", asFUNCTION(FTopLevelAssetPath_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTopLevelAssetPath, "bool IsNull() const", asFUNCTION(FTopLevelAssetPath_IsNull), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTopLevelAssetPath, "void Reset()", asFUNCTION(FTopLevelAssetPath_Reset), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTopLevelAssetPath, "FString ToString() const", asFUNCTION(FTopLevelAssetPath_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTopLevelAssetPath, "int Compare(const FTopLevelAssetPath &in Other) const", asFUNCTION(FTopLevelAssetPath_Compare), asCALL_CDECL_OBJFIRST);
}
