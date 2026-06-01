// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Internationalization/Text.h"
#include "Misc/AssertionMacros.h"
#include "UObject/NameTypes.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FText_DefaultConstruct(FText* Memory)
	{
		new (Memory) FText();
	}

	void FText_CopyConstruct(const FText& Other, FText* Memory)
	{
		new (Memory) FText(Other);
	}

	void FText_Destruct(FText* Memory)
	{
		Memory->~FText();
	}

	FText& FText_Assign(FText& Value, const FText& Other)
	{
		Value = Other;
		return Value;
	}

	FString FText_ToString(const FText& Value)
	{
		return Value.ToString();
	}

	FString FText_BuildSourceString(const FText& Value)
	{
		return Value.BuildSourceString();
	}

	bool FText_IsNumeric(const FText& Value)
	{
		return Value.IsNumeric();
	}

	int32 FText_CompareTo(const FText& Value, const FText& Other)
	{
		return Value.CompareTo(Other);
	}

	int32 FText_CompareToCaseIgnored(const FText& Value, const FText& Other)
	{
		return Value.CompareToCaseIgnored(Other);
	}

	bool FText_EqualTo(const FText& Value, const FText& Other)
	{
		return Value.EqualTo(Other);
	}

	bool FText_EqualToCaseIgnored(const FText& Value, const FText& Other)
	{
		return Value.EqualToCaseIgnored(Other);
	}

	bool FText_IdenticalTo(const FText& Value, const FText& Other)
	{
		return Value.IdenticalTo(Other);
	}

	bool FText_IsEmpty(const FText& Value)
	{
		return Value.IsEmpty();
	}

	bool FText_IsEmptyOrWhitespace(const FText& Value)
	{
		return Value.IsEmptyOrWhitespace();
	}

	FText FText_ToLower(const FText& Value)
	{
		return Value.ToLower();
	}

	FText FText_ToUpper(const FText& Value)
	{
		return Value.ToUpper();
	}

	bool FText_IsTransient(const FText& Value)
	{
		return Value.IsTransient();
	}

	bool FText_IsCultureInvariant(const FText& Value)
	{
		return Value.IsCultureInvariant();
	}

	bool FText_IsInitializedFromString(const FText& Value)
	{
		return Value.IsInitializedFromString();
	}

	bool FText_IsFromStringTable(const FText& Value)
	{
		return Value.IsFromStringTable();
	}

	// Static helpers registered in the FText namespace

	FText FText_GetEmpty()
	{
		return FText::GetEmpty();
	}

	FText FText_FromString(const FString& Str)
	{
		return FText::FromString(Str);
	}

	FText FText_FromName(const FName& Name)
	{
		return FText::FromName(Name);
	}

	FText FText_AsCultureInvariantFromText(const FText& InText)
	{
		return FText::AsCultureInvariant(FText(InText));
	}

	FText FText_AsCultureInvariantFromString(const FString& Str)
	{
		return FText::AsCultureInvariant(FString(Str));
	}

	FText FText_TrimPreceding(const FText& InText)
	{
		return FText::TrimPreceding(InText);
	}

	FText FText_TrimTrailing(const FText& InText)
	{
		return FText::TrimTrailing(InText);
	}

	FText FText_TrimPrecedingAndTrailing(const FText& InText)
	{
		return FText::TrimPrecedingAndTrailing(InText);
	}

	FText FText_AsNumberInt(int32 Val)
	{
		return FText::AsNumber(Val);
	}

	FText FText_AsNumberInt64(int64 Val)
	{
		return FText::AsNumber(Val);
	}

	FText FText_AsNumberFloat(float Val)
	{
		return FText::AsNumber(Val);
	}

	FText FText_AsNumberDouble(double Val)
	{
		return FText::AsNumber(Val);
	}

	FText FText_AsPercentFloat(float Val)
	{
		return FText::AsPercent(Val);
	}

	FText FText_AsPercentDouble(double Val)
	{
		return FText::AsPercent(Val);
	}

	FText FText_Format1Text(const FText& Pattern, const FText& Arg0)
	{
		return FText::Format(FTextFormat(Pattern), FFormatOrderedArguments{ FFormatArgumentValue(Arg0) });
	}

	FText FText_Format2Text(const FText& Pattern, const FText& Arg0, const FText& Arg1)
	{
		return FText::Format(FTextFormat(Pattern), FFormatOrderedArguments{ FFormatArgumentValue(Arg0), FFormatArgumentValue(Arg1) });
	}

	FText FText_Format3Text(const FText& Pattern, const FText& Arg0, const FText& Arg1, const FText& Arg2)
	{
		return FText::Format(FTextFormat(Pattern), FFormatOrderedArguments{ FFormatArgumentValue(Arg0), FFormatArgumentValue(Arg1), FFormatArgumentValue(Arg2) });
	}

	FText FText_Format1Int(const FText& Pattern, int32 Arg0)
	{
		return FText::Format(FTextFormat(Pattern), FFormatOrderedArguments{ FFormatArgumentValue(Arg0) });
	}

	FText FText_Format1Float(const FText& Pattern, float Arg0)
	{
		return FText::Format(FTextFormat(Pattern), FFormatOrderedArguments{ FFormatArgumentValue(Arg0) });
	}
}

void Bind_FText(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FText, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FText_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FText, asBEHAVE_CONSTRUCT, "void f(const FText &in Other)", asFUNCTION(FText_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FText, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FText_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FText, "FText &opAssign(const FText &in Other)", asFUNCTION(FText_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "FString ToString() const", asFUNCTION(FText_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "FString BuildSourceString() const", asFUNCTION(FText_BuildSourceString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsNumeric() const", asFUNCTION(FText_IsNumeric), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "int CompareTo(const FText &in Other) const", asFUNCTION(FText_CompareTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "int CompareToCaseIgnored(const FText &in Other) const", asFUNCTION(FText_CompareToCaseIgnored), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool EqualTo(const FText &in Other) const", asFUNCTION(FText_EqualTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool EqualToCaseIgnored(const FText &in Other) const", asFUNCTION(FText_EqualToCaseIgnored), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IdenticalTo(const FText &in Other) const", asFUNCTION(FText_IdenticalTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsEmpty() const", asFUNCTION(FText_IsEmpty), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsEmptyOrWhitespace() const", asFUNCTION(FText_IsEmptyOrWhitespace), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "FText ToLower() const", asFUNCTION(FText_ToLower), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "FText ToUpper() const", asFUNCTION(FText_ToUpper), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsTransient() const", asFUNCTION(FText_IsTransient), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsCultureInvariant() const", asFUNCTION(FText_IsCultureInvariant), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsInitializedFromString() const", asFUNCTION(FText_IsInitializedFromString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "bool IsFromStringTable() const", asFUNCTION(FText_IsFromStringTable), asCALL_CDECL_OBJFIRST);

	// Register static helpers under FText namespace
	Result = Engine->SetDefaultNamespace("FText");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FText GetEmpty()", asFUNCTION(FText_GetEmpty), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText FromString(const FString &in Str)", asFUNCTION(FText_FromString), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText FromName(const FName &in Name)", asFUNCTION(FText_FromName), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsCultureInvariant(const FText &in Text)", asFUNCTION(FText_AsCultureInvariantFromText), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsCultureInvariant(const FString &in Str)", asFUNCTION(FText_AsCultureInvariantFromString), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText TrimPreceding(const FText &in Text)", asFUNCTION(FText_TrimPreceding), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText TrimTrailing(const FText &in Text)", asFUNCTION(FText_TrimTrailing), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText TrimPrecedingAndTrailing(const FText &in Text)", asFUNCTION(FText_TrimPrecedingAndTrailing), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsNumber(int Val)", asFUNCTION(FText_AsNumberInt), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsNumber(int64 Val)", asFUNCTION(FText_AsNumberInt64), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsNumber(float Val)", asFUNCTION(FText_AsNumberFloat), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsNumber(double Val)", asFUNCTION(FText_AsNumberDouble), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsPercent(float Val)", asFUNCTION(FText_AsPercentFloat), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText AsPercent(double Val)", asFUNCTION(FText_AsPercentDouble), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText Format(const FText &in Pattern, const FText &in Arg0)", asFUNCTION(FText_Format1Text), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText Format(const FText &in Pattern, const FText &in Arg0, const FText &in Arg1)", asFUNCTION(FText_Format2Text), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText Format(const FText &in Pattern, const FText &in Arg0, const FText &in Arg1, const FText &in Arg2)", asFUNCTION(FText_Format3Text), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText Format(const FText &in Pattern, int Arg0)", asFUNCTION(FText_Format1Int), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FText Format(const FText &in Pattern, float Arg0)", asFUNCTION(FText_Format1Float), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
