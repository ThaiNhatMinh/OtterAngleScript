// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Framework/Commands/InputChord.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FInputChord_DefaultConstruct(FInputChord* Memory)
	{
		new (Memory) FInputChord();
	}

	static void FInputChord_CopyConstruct(const FInputChord& Other, FInputChord* Memory)
	{
		new (Memory) FInputChord(Other);
	}

	static void FInputChord_ConstructKey(const FKey& InKey, FInputChord* Memory)
	{
		new (Memory) FInputChord(InKey);
	}

	static void FInputChord_ConstructFull(const FKey& InKey, bool bInShift, bool bInCtrl, bool bInAlt, bool bInCmd, FInputChord* Memory)
	{
		new (Memory) FInputChord(InKey, bInShift, bInCtrl, bInAlt, bInCmd);
	}

	static void FInputChord_Destruct(FInputChord* Memory)
	{
		Memory->~FInputChord();
	}

	// --- Operators ---

	static FInputChord& FInputChord_Assign(FInputChord& Value, const FInputChord& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FInputChord_OpEquals(const FInputChord& Value, const FInputChord& Other)
	{
		return Value == Other;
	}

	// --- Property accessors ---

	static FKey FInputChord_GetKey(const FInputChord& Value)
	{
		return Value.Key;
	}

	static void FInputChord_SetKey(FInputChord& Value, const FKey& InKey)
	{
		Value.Key = InKey;
	}

	static bool FInputChord_GetShift(const FInputChord& Value)
	{
		return Value.bShift;
	}

	static void FInputChord_SetShift(FInputChord& Value, bool bInShift)
	{
		Value.bShift = bInShift;
	}

	static bool FInputChord_GetCtrl(const FInputChord& Value)
	{
		return Value.bCtrl;
	}

	static void FInputChord_SetCtrl(FInputChord& Value, bool bInCtrl)
	{
		Value.bCtrl = bInCtrl;
	}

	static bool FInputChord_GetAlt(const FInputChord& Value)
	{
		return Value.bAlt;
	}

	static void FInputChord_SetAlt(FInputChord& Value, bool bInAlt)
	{
		Value.bAlt = bInAlt;
	}

	static bool FInputChord_GetCmd(const FInputChord& Value)
	{
		return Value.bCmd;
	}

	static void FInputChord_SetCmd(FInputChord& Value, bool bInCmd)
	{
		Value.bCmd = bInCmd;
	}

	// --- Methods ---

	static FInputChord::ERelationshipType FInputChord_GetRelationship(const FInputChord& Value, const FInputChord& OtherChord)
	{
		return Value.GetRelationship(OtherChord);
	}

	static bool FInputChord_NeedsControl(const FInputChord& Value)
	{
		return Value.NeedsControl();
	}

	static bool FInputChord_NeedsCommand(const FInputChord& Value)
	{
		return Value.NeedsCommand();
	}

	static bool FInputChord_NeedsAlt(const FInputChord& Value)
	{
		return Value.NeedsAlt();
	}

	static bool FInputChord_NeedsShift(const FInputChord& Value)
	{
		return Value.NeedsShift();
	}

	static FText FInputChord_GetInputText(const FInputChord& Value, bool bLongDisplayName)
	{
		return Value.GetInputText(bLongDisplayName);
	}

	static FText FInputChord_GetKeyText(const FInputChord& Value, bool bLongDisplayName)
	{
		return Value.GetKeyText(bLongDisplayName);
	}

	static FText FInputChord_GetModifierText(const FInputChord& Value)
	{
		return Value.GetModifierText();
	}

	static bool FInputChord_HasAnyModifierKeys(const FInputChord& Value)
	{
		return Value.HasAnyModifierKeys();
	}

	static bool FInputChord_IsValidChord(const FInputChord& Value)
	{
		return Value.IsValidChord();
	}

	static void FInputChord_Set(FInputChord& Value, const FInputChord& InTemplate)
	{
		Value.Set(InTemplate);
	}
}

void Bind_FInputChord(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Register nested enum
	Result = Engine->RegisterEnum("ERelationshipType");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERelationshipType", "None", static_cast<int>(FInputChord::ERelationshipType::None));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERelationshipType", "Same", static_cast<int>(FInputChord::ERelationshipType::Same));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERelationshipType", "Masked", static_cast<int>(FInputChord::ERelationshipType::Masked));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERelationshipType", "Masks", static_cast<int>(FInputChord::ERelationshipType::Masks));
	check(Result >= 0);

	// Register the type
	Result = Engine->RegisterObjectType("FInputChord", sizeof(FInputChord),
		asOBJ_VALUE | asGetTypeTraits<FInputChord>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FInputChord, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FInputChord_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputChord, asBEHAVE_CONSTRUCT, "void f(const FInputChord &in Other)", asFUNCTION(FInputChord_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputChord, asBEHAVE_CONSTRUCT, "void f(const FKey &in Key)", asFUNCTION(FInputChord_ConstructKey), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputChord, asBEHAVE_CONSTRUCT, "void f(const FKey &in Key, bool bShift, bool bCtrl, bool bAlt, bool bCmd)", asFUNCTION(FInputChord_ConstructFull), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputChord, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FInputChord_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FInputChord, "FInputChord &opAssign(const FInputChord &in Other)", asFUNCTION(FInputChord_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool opEquals(const FInputChord &in Other) const", asFUNCTION(FInputChord_OpEquals), asCALL_CDECL_OBJFIRST);

	// Properties (via getter/setter for bitfield safety)
	REGISTER_METHOD(FInputChord, "FKey get_Key() const", asFUNCTION(FInputChord_GetKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "void set_Key(const FKey &in InKey)", asFUNCTION(FInputChord_SetKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool get_bShift() const", asFUNCTION(FInputChord_GetShift), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "void set_bShift(bool bInShift)", asFUNCTION(FInputChord_SetShift), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool get_bCtrl() const", asFUNCTION(FInputChord_GetCtrl), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "void set_bCtrl(bool bInCtrl)", asFUNCTION(FInputChord_SetCtrl), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool get_bAlt() const", asFUNCTION(FInputChord_GetAlt), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "void set_bAlt(bool bInAlt)", asFUNCTION(FInputChord_SetAlt), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool get_bCmd() const", asFUNCTION(FInputChord_GetCmd), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "void set_bCmd(bool bInCmd)", asFUNCTION(FInputChord_SetCmd), asCALL_CDECL_OBJFIRST);

	// Methods
	REGISTER_METHOD(FInputChord, "ERelationshipType GetRelationship(const FInputChord &in OtherChord) const", asFUNCTION(FInputChord_GetRelationship), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool NeedsControl() const", asFUNCTION(FInputChord_NeedsControl), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool NeedsCommand() const", asFUNCTION(FInputChord_NeedsCommand), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool NeedsAlt() const", asFUNCTION(FInputChord_NeedsAlt), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool NeedsShift() const", asFUNCTION(FInputChord_NeedsShift), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "FText GetInputText(bool bLongDisplayName = true) const", asFUNCTION(FInputChord_GetInputText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "FText GetKeyText(bool bLongDisplayName = true) const", asFUNCTION(FInputChord_GetKeyText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "FText GetModifierText() const", asFUNCTION(FInputChord_GetModifierText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool HasAnyModifierKeys() const", asFUNCTION(FInputChord_HasAnyModifierKeys), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "bool IsValidChord() const", asFUNCTION(FInputChord_IsValidChord), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputChord, "void Set(const FInputChord &in InTemplate)", asFUNCTION(FInputChord_Set), asCALL_CDECL_OBJFIRST);
}
