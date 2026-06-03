// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/Guid.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FGuid_DefaultConstruct(FGuid* Memory)
	{
		new (Memory) FGuid();
	}

	static void FGuid_CopyConstruct(const FGuid& Other, FGuid* Memory)
	{
		new (Memory) FGuid(Other);
	}

	static void FGuid_ConstructComponents(uint32 A, uint32 B, uint32 C, uint32 D, FGuid* Memory)
	{
		new (Memory) FGuid(A, B, C, D);
	}

	static void FGuid_ConstructFromString(const FString& InGuidStr, FGuid* Memory)
	{
		new (Memory) FGuid(InGuidStr);
	}

	static void FGuid_Destruct(FGuid* Memory)
	{
		Memory->~FGuid();
	}

	// --- Operators ---

	static FGuid& FGuid_Assign(FGuid& Value, const FGuid& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FGuid_OpEquals(const FGuid& Value, const FGuid& Other)
	{
		return Value == Other;
	}

	static int FGuid_OpCmp(const FGuid& Value, const FGuid& Other)
	{
		if (Value < Other) return -1;
		if (Other < Value) return 1;
		return 0;
	}

	// --- Property accessors ---

	static uint32 FGuid_GetA(const FGuid& Value)
	{
		return Value.A;
	}

	static void FGuid_SetA(FGuid& Value, uint32 InA)
	{
		Value.A = InA;
	}

	static uint32 FGuid_GetB(const FGuid& Value)
	{
		return Value.B;
	}

	static void FGuid_SetB(FGuid& Value, uint32 InB)
	{
		Value.B = InB;
	}

	static uint32 FGuid_GetC(const FGuid& Value)
	{
		return Value.C;
	}

	static void FGuid_SetC(FGuid& Value, uint32 InC)
	{
		Value.C = InC;
	}

	static uint32 FGuid_GetD(const FGuid& Value)
	{
		return Value.D;
	}

	static void FGuid_SetD(FGuid& Value, uint32 InD)
	{
		Value.D = InD;
	}

	// --- Methods ---

	static bool FGuid_IsValid(const FGuid& Value)
	{
		return Value.IsValid();
	}

	static void FGuid_Invalidate(FGuid& Value)
	{
		Value.Invalidate();
	}

	static FString FGuid_ToString(const FGuid& Value, EGuidFormats Format)
	{
		return Value.ToString(Format);
	}

	// --- Static methods ---

	static FGuid FGuid_NewGuid()
	{
		return FGuid::NewGuid();
	}

	static bool FGuid_Parse(const FString& GuidString, FGuid& OutGuid)
	{
		return FGuid::Parse(GuidString, OutGuid);
	}

	static bool FGuid_ParseExact(const FString& GuidString, EGuidFormats Format, FGuid& OutGuid)
	{
		return FGuid::ParseExact(GuidString, Format, OutGuid);
	}
}

void Bind_FGuid(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Register the EGuidFormats enum
	Result = Engine->RegisterEnum("EGuidFormats");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "Digits", static_cast<int>(EGuidFormats::Digits));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "DigitsLower", static_cast<int>(EGuidFormats::DigitsLower));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "DigitsWithHyphens", static_cast<int>(EGuidFormats::DigitsWithHyphens));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "DigitsWithHyphensLower", static_cast<int>(EGuidFormats::DigitsWithHyphensLower));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "DigitsWithHyphensInBraces", static_cast<int>(EGuidFormats::DigitsWithHyphensInBraces));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "DigitsWithHyphensInParentheses", static_cast<int>(EGuidFormats::DigitsWithHyphensInParentheses));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "HexValuesInBraces", static_cast<int>(EGuidFormats::HexValuesInBraces));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "UniqueObjectGuid", static_cast<int>(EGuidFormats::UniqueObjectGuid));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "Short", static_cast<int>(EGuidFormats::Short));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EGuidFormats", "Base36Encoded", static_cast<int>(EGuidFormats::Base36Encoded));
	check(Result >= 0);

	// Register the type
	Result = Engine->RegisterObjectType("FGuid", sizeof(FGuid),
		asOBJ_VALUE | asGetTypeTraits<FGuid>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FGuid, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FGuid_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FGuid, asBEHAVE_CONSTRUCT, "void f(const FGuid &in Other)", asFUNCTION(FGuid_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FGuid, asBEHAVE_CONSTRUCT, "void f(uint A, uint B, uint C, uint D)", asFUNCTION(FGuid_ConstructComponents), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FGuid, asBEHAVE_CONSTRUCT, "void f(const FString &in GuidStr)", asFUNCTION(FGuid_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FGuid, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FGuid_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FGuid, "FGuid &opAssign(const FGuid &in Other)", asFUNCTION(FGuid_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "bool opEquals(const FGuid &in Other) const", asFUNCTION(FGuid_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "int opCmp(const FGuid &in Other) const", asFUNCTION(FGuid_OpCmp), asCALL_CDECL_OBJFIRST);

	// Property accessors
	REGISTER_METHOD(FGuid, "uint get_A() const", asFUNCTION(FGuid_GetA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "void set_A(uint InA)", asFUNCTION(FGuid_SetA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "uint get_B() const", asFUNCTION(FGuid_GetB), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "void set_B(uint InB)", asFUNCTION(FGuid_SetB), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "uint get_C() const", asFUNCTION(FGuid_GetC), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "void set_C(uint InC)", asFUNCTION(FGuid_SetC), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "uint get_D() const", asFUNCTION(FGuid_GetD), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "void set_D(uint InD)", asFUNCTION(FGuid_SetD), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FGuid, "bool IsValid() const", asFUNCTION(FGuid_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "void Invalidate()", asFUNCTION(FGuid_Invalidate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FGuid, "FString ToString(EGuidFormats Format = EGuidFormats::Digits) const", asFUNCTION(FGuid_ToString), asCALL_CDECL_OBJFIRST);

	// Static methods under the FGuid namespace
	Result = Engine->SetDefaultNamespace("FGuid");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FGuid NewGuid()", asFUNCTION(FGuid_NewGuid), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Parse(const FString &in GuidString, FGuid &out OutGuid)", asFUNCTION(FGuid_Parse), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool ParseExact(const FString &in GuidString, EGuidFormats Format, FGuid &out OutGuid)", asFUNCTION(FGuid_ParseExact), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
