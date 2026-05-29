// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"
#include "UObject/SoftObjectPath.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FSoftClassPath_DefaultConstruct(FSoftClassPath* Memory)
	{
		new (Memory) FSoftClassPath();
	}

	void FSoftClassPath_CopyConstruct(const FSoftClassPath& Other, FSoftClassPath* Memory)
	{
		new (Memory) FSoftClassPath(Other);
	}

	void FSoftClassPath_ConstructFromString(const FString& PathString, FSoftClassPath* Memory)
	{
		new (Memory) FSoftClassPath(PathString);
	}

	void FSoftClassPath_ConstructFromClass(UClass* InClass, FSoftClassPath* Memory)
	{
		new (Memory) FSoftClassPath(InClass);
	}

	void FSoftClassPath_Destruct(FSoftClassPath* Memory)
	{
		Memory->~FSoftClassPath();
	}

	// opEquals: base class operator== takes FSoftObjectPath const& — wrapper needed
	bool FSoftClassPath_Equals(const FSoftClassPath& A, const FSoftClassPath& B)
	{
		return static_cast<const FSoftObjectPath&>(A) == static_cast<const FSoftObjectPath&>(B);
	}

	// TryLoadClass<T> is a template method — expose as TryLoadClass<UObject>
	UClass* FSoftClassPath_TryLoadClass(const FSoftClassPath& Path)
	{
		return Path.TryLoadClass<UObject>();
	}

	// Static method wrapper — UClass* param for reference-type adaptation
	FSoftClassPath FSoftClassPath_GetOrCreateIDForClass(UClass* InClass)
	{
		return FSoftClassPath::GetOrCreateIDForClass(InClass);
	}
}

void Bind_FSoftClassPath(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;
	REGISTER_BEHAVIOUR(FSoftClassPath, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FSoftClassPath_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftClassPath, asBEHAVE_CONSTRUCT, "void f(const FSoftClassPath &in Other)", asFUNCTION(FSoftClassPath_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftClassPath, asBEHAVE_CONSTRUCT, "void f(const FString &in PathString)", asFUNCTION(FSoftClassPath_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftClassPath, asBEHAVE_CONSTRUCT, "void f(UClass InClass)", asFUNCTION(FSoftClassPath_ConstructFromClass), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSoftClassPath, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FSoftClassPath_Destruct), asCALL_CDECL_OBJLAST);

	// Assignment and equality
	REGISTER_METHOD(FSoftClassPath, "FSoftClassPath &opAssign(const FSoftClassPath &in Other)", asMETHODPR(FSoftClassPath, operator=, (const FSoftClassPath&), FSoftClassPath&), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "bool opEquals(const FSoftClassPath &in Other) const", asFUNCTION(FSoftClassPath_Equals), asCALL_CDECL_OBJFIRST);

	// Path accessors (inherited from FSoftObjectPath)
	REGISTER_METHOD(FSoftClassPath, "FString ToString() const", asMETHODPR(FSoftClassPath, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "FString GetAssetPathString() const", asMETHODPR(FSoftClassPath, GetAssetPathString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "FString GetLongPackageName() const", asMETHODPR(FSoftClassPath, GetLongPackageName, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "FName GetLongPackageFName() const", asMETHODPR(FSoftClassPath, GetLongPackageFName, () const, FName), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "FString GetAssetName() const", asMETHODPR(FSoftClassPath, GetAssetName, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "FName GetAssetFName() const", asMETHODPR(FSoftClassPath, GetAssetFName, () const, FName), asCALL_THISCALL);

	// Path mutation (inherited from FSoftObjectPath)
	REGISTER_METHOD(FSoftClassPath, "void SetPath(const FString &in Path)", asMETHODPR(FSoftClassPath, SetPath, (const FString&), void), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "void Reset()", asMETHODPR(FSoftClassPath, Reset, (), void), asCALL_THISCALL);

	// State queries (inherited from FSoftObjectPath)
	REGISTER_METHOD(FSoftClassPath, "bool IsValid() const", asMETHODPR(FSoftClassPath, IsValid, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "bool IsNull() const", asMETHODPR(FSoftClassPath, IsNull, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "bool IsAsset() const", asMETHODPR(FSoftClassPath, IsAsset, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "bool IsSubobject() const", asMETHODPR(FSoftClassPath, IsSubobject, () const, bool), asCALL_THISCALL);

	// Class resolution
	REGISTER_METHOD(FSoftClassPath, "UClass ResolveClass() const", asMETHODPR(FSoftClassPath, ResolveClass, () const, UClass*), asCALL_THISCALL);
	REGISTER_METHOD(FSoftClassPath, "UClass TryLoadClass() const", asFUNCTION(FSoftClassPath_TryLoadClass), asCALL_CDECL_OBJFIRST);

	// Static factories
	Result = Engine->SetDefaultNamespace("FSoftClassPath");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction(
		"FSoftClassPath GetOrCreateIDForClass(UClass InClass)",
		asFUNCTION(FSoftClassPath_GetOrCreateIDForClass),
		asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
