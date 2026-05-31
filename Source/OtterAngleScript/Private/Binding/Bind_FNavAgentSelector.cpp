// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "AI/Navigation/NavAgentSelector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FNavAgentSelector_DefaultConstruct(FNavAgentSelector* Memory)
	{
		new (Memory) FNavAgentSelector();
	}

	static void FNavAgentSelector_ConstructWithBits(uint32 InBits, FNavAgentSelector* Memory)
	{
		new (Memory) FNavAgentSelector(InBits);
	}

	static void FNavAgentSelector_CopyConstruct(const FNavAgentSelector& Other, FNavAgentSelector* Memory)
	{
		new (Memory) FNavAgentSelector(Other);
	}

	static void FNavAgentSelector_Destruct(FNavAgentSelector* Memory)
	{
		Memory->~FNavAgentSelector();
	}

	static FNavAgentSelector& FNavAgentSelector_Assign(FNavAgentSelector& Value, const FNavAgentSelector& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FNavAgentSelector_Contains(const FNavAgentSelector& Value, int32 AgentIndex)
	{
		return Value.Contains(AgentIndex);
	}

	static bool FNavAgentSelector_ContainsAnyAgent(const FNavAgentSelector& Value)
	{
		return Value.ContainsAnyAgent();
	}

	static void FNavAgentSelector_Set(FNavAgentSelector& Value, int32 AgentIndex)
	{
		Value.Set(AgentIndex);
	}

	static bool FNavAgentSelector_IsInitialized(const FNavAgentSelector& Value)
	{
		return Value.IsInitialized();
	}

	static void FNavAgentSelector_MarkInitialized(FNavAgentSelector& Value)
	{
		Value.MarkInitialized();
	}

	static void FNavAgentSelector_Empty(FNavAgentSelector& Value)
	{
		Value.Empty();
	}

	static bool FNavAgentSelector_IsSame(const FNavAgentSelector& Value, const FNavAgentSelector& Other)
	{
		return Value.IsSame(Other);
	}

	static uint32 FNavAgentSelector_GetAgentBits(const FNavAgentSelector& Value)
	{
		return Value.GetAgentBits();
	}
}

void Bind_FNavAgentSelector(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FNavAgentSelector, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FNavAgentSelector_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FNavAgentSelector, asBEHAVE_CONSTRUCT, "void f(uint InBits)", asFUNCTION(FNavAgentSelector_ConstructWithBits), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FNavAgentSelector, asBEHAVE_CONSTRUCT, "void f(const FNavAgentSelector &in Other)", asFUNCTION(FNavAgentSelector_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FNavAgentSelector, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FNavAgentSelector_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FNavAgentSelector, "FNavAgentSelector &opAssign(const FNavAgentSelector &in Other)", asFUNCTION(FNavAgentSelector_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "bool Contains(int AgentIndex) const", asFUNCTION(FNavAgentSelector_Contains), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "bool ContainsAnyAgent() const", asFUNCTION(FNavAgentSelector_ContainsAnyAgent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "void Set(int AgentIndex)", asFUNCTION(FNavAgentSelector_Set), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "bool IsInitialized() const", asFUNCTION(FNavAgentSelector_IsInitialized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "void MarkInitialized()", asFUNCTION(FNavAgentSelector_MarkInitialized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "void Empty()", asFUNCTION(FNavAgentSelector_Empty), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "bool IsSame(const FNavAgentSelector &in Other) const", asFUNCTION(FNavAgentSelector_IsSame), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FNavAgentSelector, "uint GetAgentBits() const", asFUNCTION(FNavAgentSelector_GetAgentBits), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FNavAgentSelector, "uint PackedBits", PackedBits);
}
