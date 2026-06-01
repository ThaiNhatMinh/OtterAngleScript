// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPath.h"
#include "angelscript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FSoftObjectPath_DefaultConstruct(FSoftObjectPath* Memory)
	{
		new (Memory) FSoftObjectPath();
	}

	static void FSoftObjectPath_CopyConstruct(const FSoftObjectPath& Other, FSoftObjectPath* Memory)
	{
		new (Memory) FSoftObjectPath(Other);
	}

	static void FSoftObjectPath_ConstructFromString(const FString& PathString, FSoftObjectPath* Memory)
	{
		new (Memory) FSoftObjectPath(PathString);
	}

	static void FSoftObjectPath_ConstructFromObject(UObject* InObject, FSoftObjectPath* Memory)
	{
		new (Memory) FSoftObjectPath(InObject);
	}

	static void FSoftObjectPath_Destruct(FSoftObjectPath* Memory)
	{
		Memory->~FSoftObjectPath();
	}

	// --- TryLoad: has a default arg, can't use asMETHODPR directly ---
	static UObject* FSoftObjectPath_TryLoad(const FSoftObjectPath& Path)
	{
		return Path.TryLoad();
	}

	// --- Static factory ---
	static FSoftObjectPath FSoftObjectPath_GetOrCreateIDForObject(UObject* Object)
	{
		return FSoftObjectPath::GetOrCreateIDForObject(Object);
	}
}

void Bind_FSoftObjectPath(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Constructors / destructor
	REGISTER_BEHAVIOUR(FSoftObjectPath, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FSoftObjectPath_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftObjectPath, asBEHAVE_CONSTRUCT, "void f(const FSoftObjectPath &in Other)", asFUNCTION(FSoftObjectPath_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftObjectPath, asBEHAVE_CONSTRUCT, "void f(const FString &in PathString)", asFUNCTION(FSoftObjectPath_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftObjectPath, asBEHAVE_CONSTRUCT, "void f(UObject InObject)", asFUNCTION(FSoftObjectPath_ConstructFromObject), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftObjectPath, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FSoftObjectPath_Destruct), asCALL_CDECL_OBJLAST);

	// Assignment
	REGISTER_METHOD(FSoftObjectPath, "FSoftObjectPath &opAssign(const FSoftObjectPath &in Other)", asMETHODPR(FSoftObjectPath, operator=, (const FSoftObjectPath&), FSoftObjectPath&), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "FSoftObjectPath &opAssign(const FString &in Path)", asMETHODPR(FSoftObjectPath, operator=, (const FString&), FSoftObjectPath&), asCALL_THISCALL);

	// Equality
	REGISTER_METHOD(FSoftObjectPath, "bool opEquals(const FSoftObjectPath &in Other) const", asMETHODPR(FSoftObjectPath, operator==, (const FSoftObjectPath&) const, bool), asCALL_THISCALL);

	// Path accessors
	REGISTER_METHOD(FSoftObjectPath, "FString ToString() const", asMETHODPR(FSoftObjectPath, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "FString GetAssetPathString() const", asMETHODPR(FSoftObjectPath, GetAssetPathString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "FString GetLongPackageName() const", asMETHODPR(FSoftObjectPath, GetLongPackageName, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "FName GetLongPackageFName() const", asMETHODPR(FSoftObjectPath, GetLongPackageFName, () const, FName), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "FString GetAssetName() const", asMETHODPR(FSoftObjectPath, GetAssetName, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "FName GetAssetFName() const", asMETHODPR(FSoftObjectPath, GetAssetFName, () const, FName), asCALL_THISCALL);

	// Path mutation
	REGISTER_METHOD(FSoftObjectPath, "void SetPath(const FString &in Path)", asMETHODPR(FSoftObjectPath, SetPath, (const FString&), void), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "void Reset()", asMETHODPR(FSoftObjectPath, Reset, (), void), asCALL_THISCALL);

	// State queries
	REGISTER_METHOD(FSoftObjectPath, "bool IsValid() const", asMETHODPR(FSoftObjectPath, IsValid, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "bool IsNull() const", asMETHODPR(FSoftObjectPath, IsNull, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "bool IsAsset() const", asMETHODPR(FSoftObjectPath, IsAsset, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "bool IsSubobject() const", asMETHODPR(FSoftObjectPath, IsSubobject, () const, bool), asCALL_THISCALL);

	// Ordering
	REGISTER_METHOD(FSoftObjectPath, "bool LexicalLess(const FSoftObjectPath &in Other) const", asMETHODPR(FSoftObjectPath, LexicalLess, (const FSoftObjectPath&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "bool FastLess(const FSoftObjectPath &in Other) const", asMETHODPR(FSoftObjectPath, FastLess, (const FSoftObjectPath&) const, bool), asCALL_THISCALL);

	// Object resolution
	REGISTER_METHOD(FSoftObjectPath, "UObject ResolveObject() const", asMETHODPR(FSoftObjectPath, ResolveObject, () const, UObject*), asCALL_THISCALL);
	REGISTER_METHOD(FSoftObjectPath, "UObject TryLoad() const", asFUNCTION(FSoftObjectPath_TryLoad), asCALL_CDECL_OBJFIRST);

	// Static factories
	Result = Engine->SetDefaultNamespace("FSoftObjectPath");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction(
		"FSoftObjectPath GetOrCreateIDForObject(UObject InObject)",
		asFUNCTION(FSoftObjectPath_GetOrCreateIDForObject),
		asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
