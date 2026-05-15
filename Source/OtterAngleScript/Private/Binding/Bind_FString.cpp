// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Containers/UnrealString.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "../../../ThirdParty/sdk/add_on/autowrapper/aswrappedcall.h"
#include <cstring>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void Construct(FString* Memory)
{
	new (Memory) FString();
}

static void ConstructCopy(FString* Memory, const FString& Other)
{
	new (Memory) FString(Other);
}

static void Destruct(FString* Memory)
{
	Memory->~FString();
}

static FString& FStringAssign(const FString& Other, FString& Self)
{
	Self = Other;
	return Self;
}

static bool FStringOpEquals(const FString& Other, const FString& Self)
{
	return Self == Other;
}

static int32 FStringCompare(const FString& Other, const FString& Self)
{
	if (Self < Other)
	{
		return -1;
	}
	if (Self > Other)
	{
		return 1;
	}
	return 0;
}

static FString FStringAppend(const FString& Other, const FString& Self)
{
	return Self + Other;
}

static FString& FStringAppendAssign(const FString& Other, FString& Self)
{
	Self += Other;
	return Self;
}

static FString FStringPathAppend(const FString& Other, const FString& Self)
{
	return Self / Other;
}

static FString& FStringPathAppendAssign(const FString& Other, FString& Self)
{
	Self /= Other;
	return Self;
}

static void FStringInsertAt(int32 Index, const FString& Chars, FString& Self)
{
	Self.InsertAt(Index, Chars);
}

static void FStringRemoveAt(int32 Index, int32 Count, FString& Self)
{
	Self.RemoveAt(Index, Count);
}

static FString FStringMidToEnd(int32 Start, const FString& Self)
{
	return Self.Mid(Start);
}

static int32 FStringFind(const FString& SubStr, const FString& Self)
{
	return Self.Find(SubStr);
}

static bool FStringContains(const FString& SubStr, const FString& Self)
{
	return Self.Contains(SubStr);
}

static bool FStringStartsWith(const FString& Prefix, const FString& Self)
{
	return Self.StartsWith(Prefix);
}

static bool FStringEndsWith(const FString& Suffix, const FString& Self)
{
	return Self.EndsWith(Suffix);
}

static bool FStringMatchesWildcard(const FString& Wildcard, const FString& Self)
{
	return Self.MatchesWildcard(Wildcard);
}

static bool FStringEquals(const FString& Other, const FString& Self)
{
	return Self.Equals(Other, ESearchCase::CaseSensitive);
}

static bool FStringEqualsIgnoreCase(const FString& Other, const FString& Self)
{
	return Self.Equals(Other, ESearchCase::IgnoreCase);
}

static int32 FStringCompareCaseSensitive(const FString& Other, const FString& Self)
{
	return Self.Compare(Other, ESearchCase::CaseSensitive);
}

static int32 FStringCompareIgnoreCase(const FString& Other, const FString& Self)
{
	return Self.Compare(Other, ESearchCase::IgnoreCase);
}

static FString FStringTrimQuotes(const FString& Self)
{
	return Self.TrimQuotes();
}

static FString FStringReplace(const FString& From, const FString& To, const FString& Self)
{
	return Self.Replace(*From, *To);
}

static int32 FStringReplaceInline(const FString& From, const FString& To, FString& Self)
{
	return Self.ReplaceInline(*From, *To);
}

static bool FStringRemoveFromStart(const FString& Prefix, FString& Self)
{
	return Self.RemoveFromStart(Prefix);
}

static bool FStringRemoveFromEnd(const FString& Suffix, FString& Self)
{
	return Self.RemoveFromEnd(Suffix);
}

static FString MakeFStringFromInt(int32 Num)
{
	return FString::FromInt(Num);
}

static FString MakeFStringSanitizeFloat(double Value)
{
	return FString::SanitizeFloat(Value);
}

// ---------------------------------------------------------------------------
// String factory – maps script string literals to FString objects
// ---------------------------------------------------------------------------

class FStringFactory : public asIStringFactory
{
	const void* GetStringConstant(const char* Data, asUINT Length) override
	{
		FUTF8ToTCHAR Convertor(Data, Length);
		FString* Str = new FString();
		Str->AppendChars(Convertor.Get(), Convertor.Length());
		return Str;
	}

	int ReleaseStringConstant(const void* Str) override
	{
		delete static_cast<const FString*>(Str);
		return 0;
	}

	int GetRawStringData(const void* Str, char* Data, asUINT* Length) const override
	{
		const FString* UnrealString = static_cast<const FString*>(Str);
		if (UnrealString->Len() == 0)
		{
			if (Length != nullptr)
			{
				*Length = 0;
			}
			return 0;
		}

		FTCHARToUTF8 Convertor(&(*UnrealString)[0], UnrealString->Len());
		if (Length != nullptr)
		{
			*Length = Convertor.Length();
		}
		if (Data != nullptr)
		{
			FMemory::Memcpy(Data, Convertor.Get(), Convertor.Length());
		}

		return 0;
	}
};

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void Bind_FString(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType("FString", sizeof(FString), asOBJ_VALUE | asGetTypeTraits<FString>());
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("FString", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectBehaviour("FString", asBEHAVE_CONSTRUCT, "void f(const FString &in)", asFUNCTION(ConstructCopy), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectBehaviour("FString", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Destruct), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "FString &opAssign(const FString &in)", asFUNCTION(FStringAssign), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool opEquals(const FString &in) const", asFUNCTION(FStringOpEquals), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "int opCmp(const FString &in) const", asFUNCTION(FStringCompare), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString opAdd(const FString &in) const", asFUNCTION(FStringAppend), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString &opAddAssign(const FString &in)", asFUNCTION(FStringAppendAssign), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString opDiv(const FString &in) const", asFUNCTION(FStringPathAppend), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString &opDivAssign(const FString &in)", asFUNCTION(FStringPathAppendAssign), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "int Len() const", asMETHODPR(FString, Len, () const, int), asCALL_GENERIC); check(Result >= 0);
	//Result = Engine->RegisterObjectMethod("FString", "bool IsEmpty() const", asMETHOD(FString, IsEmpty), asCALL_GENERIC); check(Result >= 0);
	//Result = Engine->RegisterObjectMethod("FString", "bool IsValidIndex(int Index) const", asMETHOD(FString, IsValidIndex), asCALL_GENERIC); check(Result >= 0);
	//Result = Engine->RegisterObjectMethod("FString", "bool IsNumeric() const", asMETHOD(FString, IsNumeric), asCALL_GENERIC); check(Result >= 0);
	//Result = Engine->RegisterObjectMethod("FString", "bool ToBool() const", asMETHOD(FString, ToBool), asCALL_GENERIC); check(Result >= 0);

	/*Result = Engine->RegisterObjectMethod("FString", "void Empty()", asMETHOD(FString, Empty), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "void Reset(int NewReservedSize = 0)", asMETHOD(FString, Reset), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "void Shrink()", WRAP_MFN(FString, Shrink), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "void Reserve(int CharacterCount)", WRAP_MFN(FString, Reserve), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "void AppendInt(int Num)", WRAP_MFN(FString, AppendInt), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "void InsertAt(int Index, const FString &in Chars)", WRAP_OBJ_LAST(FStringInsertAt), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "void RemoveAt(int Index, int Count = 1)", WRAP_OBJ_LAST(FStringRemoveAt), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "void RemoveSpacesInline()", WRAP_MFN(FString, RemoveSpacesInline), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "FString Left(int Count) const", WRAP_MFN_PR(FString, Left, (int32) const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString LeftChop(int Count) const", WRAP_MFN_PR(FString, LeftChop, (int32) const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString Right(int Count) const", WRAP_MFN_PR(FString, Right, (int32) const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString RightChop(int Count) const", WRAP_MFN_PR(FString, RightChop, (int32) const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString Mid(int Start, int Count) const", WRAP_MFN_PR(FString, Mid, (int32, int32) const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString MidToEnd(int Start) const", WRAP_OBJ_LAST(FStringMidToEnd), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "FString LeftPad(int Count) const", WRAP_MFN(FString, LeftPad), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString RightPad(int Count) const", WRAP_MFN(FString, RightPad), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "int Find(const FString &in SubStr) const", WRAP_OBJ_LAST(FStringFind), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool Contains(const FString &in SubStr) const", WRAP_OBJ_LAST(FStringContains), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool StartsWith(const FString &in Prefix) const", WRAP_OBJ_LAST(FStringStartsWith), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool EndsWith(const FString &in Suffix) const", WRAP_OBJ_LAST(FStringEndsWith), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool MatchesWildcard(const FString &in Wildcard) const", WRAP_OBJ_LAST(FStringMatchesWildcard), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "bool Equals(const FString &in Other) const", WRAP_OBJ_LAST(FStringEquals), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool EqualsIgnoreCase(const FString &in Other) const", WRAP_OBJ_LAST(FStringEqualsIgnoreCase), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "int Compare(const FString &in Other) const", WRAP_OBJ_LAST(FStringCompareCaseSensitive), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "int CompareIgnoreCase(const FString &in Other) const", WRAP_OBJ_LAST(FStringCompareIgnoreCase), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "FString ToUpper() const", WRAP_MFN_PR(FString, ToUpper, () const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString ToLower() const", WRAP_MFN_PR(FString, ToLower, () const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString TrimStartAndEnd() const", WRAP_MFN_PR(FString, TrimStartAndEnd, () const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString TrimStart() const", WRAP_MFN_PR(FString, TrimStart, () const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString TrimEnd() const", WRAP_MFN_PR(FString, TrimEnd, () const, FString), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString TrimQuotes() const", WRAP_OBJ_LAST(FStringTrimQuotes), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterObjectMethod("FString", "FString Replace(const FString &in From, const FString &in To) const", WRAP_OBJ_LAST(FStringReplace), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "int ReplaceInline(const FString &in From, const FString &in To)", WRAP_OBJ_LAST(FStringReplaceInline), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool RemoveFromStart(const FString &in Prefix)", WRAP_OBJ_LAST(FStringRemoveFromStart), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "bool RemoveFromEnd(const FString &in Suffix)", WRAP_OBJ_LAST(FStringRemoveFromEnd), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterObjectMethod("FString", "FString Reverse() const", WRAP_MFN_PR(FString, Reverse, () const, FString), asCALL_GENERIC); check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FString FStringFromInt(int Num)", WRAP_FN(MakeFStringFromInt), asCALL_GENERIC); check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FString FStringSanitizeFloat(double Value)", WRAP_FN(MakeFStringSanitizeFloat), asCALL_GENERIC); check(Result >= 0);*/

	Result = Engine->RegisterStringFactory("FString", new FStringFactory());
	check(Result >= 0);
}
