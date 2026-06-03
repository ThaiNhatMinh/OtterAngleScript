// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "InputCoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	static void FKey_DefaultConstruct(FKey* Memory)
	{
		new (Memory) FKey();
	}

	static void FKey_CopyConstruct(const FKey& Other, FKey* Memory)
	{
		new (Memory) FKey(Other);
	}

	static void FKey_ConstructFromFName(const FName& InName, FKey* Memory)
	{
		new (Memory) FKey(InName);
	}

	static void FKey_ConstructFromString(const FString& InName, FKey* Memory)
	{
		new (Memory) FKey(*InName);
	}

	static void FKey_Destruct(FKey* Memory)
	{
		Memory->~FKey();
	}

	static FKey& FKey_Assign(FKey& Value, const FKey& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FKey_OpEquals(const FKey& Value, const FKey& Other)
	{
		return Value == Other;
	}

	static int FKey_OpCmp(const FKey& Value, const FKey& Other)
	{
		if (Value < Other) return -1;
		if (Other < Value) return 1;
		return 0;
	}

	static bool FKey_IsValid(const FKey& Value)
	{
		return Value.IsValid();
	}

	static bool FKey_IsModifierKey(const FKey& Value)
	{
		return Value.IsModifierKey();
	}

	static bool FKey_IsGamepadKey(const FKey& Value)
	{
		return Value.IsGamepadKey();
	}

	static bool FKey_IsTouch(const FKey& Value)
	{
		return Value.IsTouch();
	}

	static bool FKey_IsMouseButton(const FKey& Value)
	{
		return Value.IsMouseButton();
	}

	static bool FKey_IsButtonAxis(const FKey& Value)
	{
		return Value.IsButtonAxis();
	}

	static bool FKey_IsAxis1D(const FKey& Value)
	{
		return Value.IsAxis1D();
	}

	static bool FKey_IsAxis2D(const FKey& Value)
	{
		return Value.IsAxis2D();
	}

	static bool FKey_IsAxis3D(const FKey& Value)
	{
		return Value.IsAxis3D();
	}

	static bool FKey_IsDigital(const FKey& Value)
	{
		return Value.IsDigital();
	}

	static bool FKey_IsAnalog(const FKey& Value)
	{
		return Value.IsAnalog();
	}

	static bool FKey_IsBindableInBlueprints(const FKey& Value)
	{
		return Value.IsBindableInBlueprints();
	}

	static bool FKey_ShouldUpdateAxisWithoutSamples(const FKey& Value)
	{
		return Value.ShouldUpdateAxisWithoutSamples();
	}

	static bool FKey_IsBindableToActions(const FKey& Value)
	{
		return Value.IsBindableToActions();
	}

	static bool FKey_IsDeprecated(const FKey& Value)
	{
		return Value.IsDeprecated();
	}

	static bool FKey_IsGesture(const FKey& Value)
	{
		return Value.IsGesture();
	}

	static FText FKey_GetDisplayName(const FKey& Value, bool bLongDisplayName)
	{
		return Value.GetDisplayName(bLongDisplayName);
	}

	static FString FKey_ToString(const FKey& Value)
	{
		return Value.ToString();
	}

	static FName FKey_GetFName(const FKey& Value)
	{
		return Value.GetFName();
	}

	static FName FKey_GetMenuCategory(const FKey& Value)
	{
		return Value.GetMenuCategory();
	}

	static EPairedAxis FKey_GetPairedAxis(const FKey& Value)
	{
		return Value.GetPairedAxis();
	}

	static FKey FKey_GetPairedAxisKey(const FKey& Value)
	{
		return Value.GetPairedAxisKey();
	}
}

void Bind_FKey(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	Result = Engine->RegisterObjectType("FKey", sizeof(FKey), asOBJ_VALUE | asGetTypeTraits<FKey>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FKey, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FKey_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKey, asBEHAVE_CONSTRUCT, "void f(const FKey &in Other)", asFUNCTION(FKey_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKey, asBEHAVE_CONSTRUCT, "void f(const FName &in Name)", asFUNCTION(FKey_ConstructFromFName), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKey, asBEHAVE_CONSTRUCT, "void f(const FString &in Name)", asFUNCTION(FKey_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKey, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FKey_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FKey, "FKey &opAssign(const FKey &in Other)", asFUNCTION(FKey_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool opEquals(const FKey &in Other) const", asFUNCTION(FKey_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "int opCmp(const FKey &in Other) const", asFUNCTION(FKey_OpCmp), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FKey, "bool IsValid() const", asFUNCTION(FKey_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsModifierKey() const", asFUNCTION(FKey_IsModifierKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsGamepadKey() const", asFUNCTION(FKey_IsGamepadKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsTouch() const", asFUNCTION(FKey_IsTouch), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsMouseButton() const", asFUNCTION(FKey_IsMouseButton), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsButtonAxis() const", asFUNCTION(FKey_IsButtonAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsAxis1D() const", asFUNCTION(FKey_IsAxis1D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsAxis2D() const", asFUNCTION(FKey_IsAxis2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsAxis3D() const", asFUNCTION(FKey_IsAxis3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsDigital() const", asFUNCTION(FKey_IsDigital), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsAnalog() const", asFUNCTION(FKey_IsAnalog), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsBindableInBlueprints() const", asFUNCTION(FKey_IsBindableInBlueprints), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool ShouldUpdateAxisWithoutSamples() const", asFUNCTION(FKey_ShouldUpdateAxisWithoutSamples), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsBindableToActions() const", asFUNCTION(FKey_IsBindableToActions), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsDeprecated() const", asFUNCTION(FKey_IsDeprecated), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "bool IsGesture() const", asFUNCTION(FKey_IsGesture), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FKey, "FText GetDisplayName(bool bLongDisplayName = true) const", asFUNCTION(FKey_GetDisplayName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "FString ToString() const", asFUNCTION(FKey_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "FName GetFName() const", asFUNCTION(FKey_GetFName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "FName GetMenuCategory() const", asFUNCTION(FKey_GetMenuCategory), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "EPairedAxis GetPairedAxis() const", asFUNCTION(FKey_GetPairedAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKey, "FKey GetPairedAxisKey() const", asFUNCTION(FKey_GetPairedAxisKey), asCALL_CDECL_OBJFIRST);
}
