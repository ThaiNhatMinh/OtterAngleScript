// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "CoreMinimal.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FName_DefaultConstruct(FName* Memory)
	{
		new (Memory) FName();
	}

	static void FName_CopyConstruct(const FName& Other, FName* Memory)
	{
		new (Memory) FName(Other);
	}

	static void FName_ConstructString(const FString& Value, FName* Memory)
	{
		new (Memory) FName(*Value);
	}

	static void FName_Destruct(FName* Memory)
	{
		Memory->~FName();
	}

	static FName& FName_Assign(FName& Value, const FName& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FName_OpEquals(const FName& Value, const FName& Other)
	{
		return Value == Other;
	}

	static bool FName_IsNone(const FName& Value)
	{
		return Value.IsNone();
	}

	static FString FName_ToString(const FName& Value)
	{
		return Value.ToString();
	}
}

void Declare_FName(asIScriptEngine* Engine)
{
	int Result;
	REGISTER_BEHAVIOUR(FName, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FName_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FName, asBEHAVE_CONSTRUCT, "void f(const FName &in Other)", asFUNCTION(FName_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FName, asBEHAVE_CONSTRUCT, "void f(const FString &in Value)", asFUNCTION(FName_ConstructString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FName, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FName_Destruct), asCALL_CDECL_OBJLAST);
}

void Bind_FName(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_METHOD(FName, "FName &opAssign(const FName &in Other)", asFUNCTION(FName_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FName, "bool opEquals(const FName &in Other) const", asFUNCTION(FName_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FName, "bool IsNone() const", asFUNCTION(FName_IsNone), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FName, "FString ToString() const", asFUNCTION(FName_ToString), asCALL_CDECL_OBJFIRST);

	Result = Engine->RegisterGlobalProperty("const FName NAME_None", (void*)&NAME_None);
	check(Result >= 0);
}
