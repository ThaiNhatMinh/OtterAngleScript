// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Containers\UnrealString.h"
#include "Misc\AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static ESearchCase::Type ToSearchCase(bool bCaseSensitive)
	{
		return bCaseSensitive ? ESearchCase::CaseSensitive : ESearchCase::IgnoreCase;
	}

	static ESearchDir::Type ToSearchDir(bool bSearchFromEnd)
	{
		return bSearchFromEnd ? ESearchDir::FromEnd : ESearchDir::FromStart;
	}

	static void SetScriptException(const char* Message)
	{
		if (asIScriptContext* Context = asGetActiveContext())
		{
			Context->SetException(Message);
		}
	}

	class FStringFactory : public asIStringFactory
	{
	public:
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

	static void FString_DefaultConstruct(FString* Memory)
	{
		new (Memory) FString();
	}

	static void FString_CopyConstruct(const FString& Other, FString* Memory)
	{
		new (Memory) FString(Other);
	}

	static void FString_Destruct(FString* Memory)
	{
		Memory->~FString();
	}

	static FString& FString_Assign(FString& Value, const FString& Other)
	{
		Value = Other;
		return Value;
	}

	static FString& FString_AddAssign(FString& Value, const FString& Other)
	{
		Value += Other;
		return Value;
	}

	static FString FString_Add(const FString& Value, const FString& Other)
	{
		return Value + Other;
	}

	static bool FString_OpEquals(const FString& Value, const FString& Other)
	{
		return Value == Other;
	}

	static int32 FString_OpCmp(const FString& Value, const FString& Other)
	{
		return Value.Compare(Other, ESearchCase::CaseSensitive);
	}

	static asUINT FString_OpIndex(const FString& Value, asUINT Index)
	{
		if (Index >= static_cast<asUINT>(Value.Len()))
		{
			SetScriptException("FString index out of range");
			return 0;
		}

		return static_cast<asUINT>(Value[static_cast<int32>(Index)]);
	}

	static int32 FString_Len(const FString& Value)
	{
		return Value.Len();
	}

	static bool FString_IsEmpty(const FString& Value)
	{
		return Value.IsEmpty();
	}

	static bool FString_IsNumeric(const FString& Value)
	{
		return Value.IsNumeric();
	}

	static void FString_Empty(FString& Value)
	{
		Value.Empty();
	}

	static FString FString_Left(const FString& Value, int32 Count)
	{
		return Value.Left(Count);
	}

	static FString FString_LeftChop(const FString& Value, int32 Count)
	{
		return Value.LeftChop(Count);
	}

	static FString FString_Right(const FString& Value, int32 Count)
	{
		return Value.Right(Count);
	}

	static FString FString_RightChop(const FString& Value, int32 Count)
	{
		return Value.RightChop(Count);
	}

	static FString FString_Mid(const FString& Value, int32 Start)
	{
		return Value.Mid(Start);
	}

	static FString FString_MidCount(const FString& Value, int32 Start, int32 Count)
	{
		return Value.Mid(Start, Count);
	}

	static bool FString_Equals(const FString& Value, const FString& Other, bool bCaseSensitive)
	{
		return Value.Equals(Other, ToSearchCase(bCaseSensitive));
	}

	static int32 FString_Compare(const FString& Value, const FString& Other, bool bCaseSensitive)
	{
		return Value.Compare(Other, ToSearchCase(bCaseSensitive));
	}

	static bool FString_StartsWith(const FString& Value, const FString& Prefix, bool bCaseSensitive)
	{
		return Value.StartsWith(Prefix, ToSearchCase(bCaseSensitive));
	}

	static bool FString_EndsWith(const FString& Value, const FString& Suffix, bool bCaseSensitive)
	{
		return Value.EndsWith(Suffix, ToSearchCase(bCaseSensitive));
	}

	static bool FString_Contains(const FString& Value, const FString& Substring, bool bCaseSensitive, bool bSearchFromEnd)
	{
		return Value.Contains(Substring, ToSearchCase(bCaseSensitive), ToSearchDir(bSearchFromEnd));
	}

	static int32 FString_Find(const FString& Value, const FString& Substring, bool bCaseSensitive, bool bSearchFromEnd, int32 StartPosition)
	{
		return Value.Find(Substring, ToSearchCase(bCaseSensitive), ToSearchDir(bSearchFromEnd), StartPosition);
	}

	static bool FString_FindChar(const FString& Value, asUINT Character, int32& Index)
	{
		return Value.FindChar(static_cast<TCHAR>(Character), Index);
	}

	static bool FString_FindLastChar(const FString& Value, asUINT Character, int32& Index)
	{
		return Value.FindLastChar(static_cast<TCHAR>(Character), Index);
	}

	static FString FString_Replace(const FString& Value, const FString& From, const FString& To, bool bCaseSensitive)
	{
		FString Result = Value;
		Result.ReplaceInline(*From, *To, ToSearchCase(bCaseSensitive));
		return Result;
	}

	static int32 FString_ReplaceInline(FString& Value, const FString& From, const FString& To, bool bCaseSensitive)
	{
		return Value.ReplaceInline(*From, *To, ToSearchCase(bCaseSensitive));
	}

	static FString FString_ToLower(const FString& Value)
	{
		FString Result = Value;
		Result.ToLowerInline();
		return Result;
	}

	static FString FString_ToUpper(const FString& Value)
	{
		FString Result = Value;
		Result.ToUpperInline();
		return Result;
	}

	static void FString_ToLowerInline(FString& Value)
	{
		Value.ToLowerInline();
	}

	static void FString_ToUpperInline(FString& Value)
	{
		Value.ToUpperInline();
	}

	static FString FString_TrimStart(const FString& Value)
	{
		FString Result = Value;
		Result.TrimStartInline();
		return Result;
	}

	static FString FString_TrimEnd(const FString& Value)
	{
		FString Result = Value;
		Result.TrimEndInline();
		return Result;
	}

	static FString FString_TrimStartAndEnd(const FString& Value)
	{
		FString Result = Value;
		Result.TrimStartAndEndInline();
		return Result;
	}

	static void FString_TrimStartInline(FString& Value)
	{
		Value.TrimStartInline();
	}

	static void FString_TrimEndInline(FString& Value)
	{
		Value.TrimEndInline();
	}

	static void FString_TrimStartAndEndInline(FString& Value)
	{
		Value.TrimStartAndEndInline();
	}

	static void FString_ReverseString(FString& Value)
	{
		Value.ReverseString();
	}

	static bool FString_RemoveFromStart(FString& Value, const FString& Prefix, bool bCaseSensitive)
	{
		return Value.RemoveFromStart(Prefix, ToSearchCase(bCaseSensitive));
	}

	static bool FString_RemoveFromEnd(FString& Value, const FString& Suffix, bool bCaseSensitive)
	{
		return Value.RemoveFromEnd(Suffix, ToSearchCase(bCaseSensitive));
	}

	static bool FString_Split(const FString& Value, const FString& Delimiter, FString& LeftPart, FString& RightPart, bool bCaseSensitive, bool bSearchFromEnd)
	{
		return Value.Split(Delimiter, &LeftPart, &RightPart, ToSearchCase(bCaseSensitive), ToSearchDir(bSearchFromEnd));
	}
}

#define REGISTER_FSTRING_BEHAVIOUR(Behaviour, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectBehaviour("FString", Behaviour, Declaration, Function, CallConv); \
	check(Result >= 0)

#define REGISTER_FSTRING_METHOD(Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod("FString", Declaration, Function, CallConv); \
	check(Result >= 0)

void Bind_FString(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FString",
		sizeof(FString),
		asOBJ_VALUE | asGetTypeTraits<FString>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_FSTRING_BEHAVIOUR(asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FString_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_FSTRING_BEHAVIOUR(asBEHAVE_CONSTRUCT, "void f(const FString &in Other)", asFUNCTION(FString_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_FSTRING_BEHAVIOUR(asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FString_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_FSTRING_METHOD("FString &opAssign(const FString &in Other)", asFUNCTION(FString_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString &opAddAssign(const FString &in Other)", asFUNCTION(FString_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString opAdd(const FString &in Other) const", asFUNCTION(FString_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool opEquals(const FString &in Other) const", asFUNCTION(FString_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("int opCmp(const FString &in Other) const", asFUNCTION(FString_OpCmp), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("uint opIndex(uint Index) const", asFUNCTION(FString_OpIndex), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("int Len() const", asFUNCTION(FString_Len), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool IsEmpty() const", asFUNCTION(FString_IsEmpty), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool IsNumeric() const", asFUNCTION(FString_IsNumeric), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("void Empty()", asFUNCTION(FString_Empty), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("FString Left(int Count) const", asFUNCTION(FString_Left), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString LeftChop(int Count) const", asFUNCTION(FString_LeftChop), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString Right(int Count) const", asFUNCTION(FString_Right), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString RightChop(int Count) const", asFUNCTION(FString_RightChop), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString Mid(int Start) const", asFUNCTION(FString_Mid), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString Mid(int Start, int Count) const", asFUNCTION(FString_MidCount), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("bool Equals(const FString &in Other, bool bCaseSensitive = false) const", asFUNCTION(FString_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("int Compare(const FString &in Other, bool bCaseSensitive = false) const", asFUNCTION(FString_Compare), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool StartsWith(const FString &in Prefix, bool bCaseSensitive = false) const", asFUNCTION(FString_StartsWith), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool EndsWith(const FString &in Suffix, bool bCaseSensitive = false) const", asFUNCTION(FString_EndsWith), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool Contains(const FString &in Substring, bool bCaseSensitive = false, bool bSearchFromEnd = false) const", asFUNCTION(FString_Contains), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("int Find(const FString &in Substring, bool bCaseSensitive = false, bool bSearchFromEnd = false, int StartPosition = -1) const", asFUNCTION(FString_Find), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool FindChar(uint Character, int &out Index) const", asFUNCTION(FString_FindChar), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool FindLastChar(uint Character, int &out Index) const", asFUNCTION(FString_FindLastChar), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool Split(const FString &in Delimiter, FString &out LeftPart, FString &out RightPart, bool bCaseSensitive = false, bool bSearchFromEnd = false) const", asFUNCTION(FString_Split), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("FString Replace(const FString &in From, const FString &in To, bool bCaseSensitive = false) const", asFUNCTION(FString_Replace), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("int ReplaceInline(const FString &in From, const FString &in To, bool bCaseSensitive = false)", asFUNCTION(FString_ReplaceInline), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("FString ToLower() const", asFUNCTION(FString_ToLower), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString ToUpper() const", asFUNCTION(FString_ToUpper), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("void ToLowerInline()", asFUNCTION(FString_ToLowerInline), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("void ToUpperInline()", asFUNCTION(FString_ToUpperInline), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("FString TrimStart() const", asFUNCTION(FString_TrimStart), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString TrimEnd() const", asFUNCTION(FString_TrimEnd), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("FString TrimStartAndEnd() const", asFUNCTION(FString_TrimStartAndEnd), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("void TrimStartInline()", asFUNCTION(FString_TrimStartInline), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("void TrimEndInline()", asFUNCTION(FString_TrimEndInline), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("void TrimStartAndEndInline()", asFUNCTION(FString_TrimStartAndEndInline), asCALL_CDECL_OBJFIRST);

	REGISTER_FSTRING_METHOD("void ReverseString()", asFUNCTION(FString_ReverseString), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool RemoveFromStart(const FString &in Prefix, bool bCaseSensitive = false)", asFUNCTION(FString_RemoveFromStart), asCALL_CDECL_OBJFIRST);
	REGISTER_FSTRING_METHOD("bool RemoveFromEnd(const FString &in Suffix, bool bCaseSensitive = false)", asFUNCTION(FString_RemoveFromEnd), asCALL_CDECL_OBJFIRST);

	Result = Engine->RegisterStringFactory("FString", new FStringFactory());
	check(Result >= 0);
}

#undef REGISTER_FSTRING_METHOD
#undef REGISTER_FSTRING_BEHAVIOUR
