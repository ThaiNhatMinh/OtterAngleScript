// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Containers/UnrealString.h"
#include <asbind20/asbind.hpp>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string FStringToStdString(const FString& Str)
{
    return std::string(TCHAR_TO_UTF8(*Str));
}

// ---------------------------------------------------------------------------
// Free function wrappers – OBJLAST convention (Self is always the last arg)
// ---------------------------------------------------------------------------

static FString FStringAppend(const FString& Other, const FString& Self)
{
    return Self + Other;
}

static int32 FStringCompare(const FString& Other, const FString& Self)
{
    if (Self < Other) return -1;
    if (Self > Other) return 1;
    return 0;
}

static FString FStringPathAppend(const FString& Rhs, const FString& Self)
{
    return Self / Rhs;
}

static FString& FStringPathAppendAssign(const FString& Rhs, FString& Self)
{
    Self /= Rhs;
    return Self;
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

static void FStringInsertAt(int32 Index, const FString& Chars, FString& Self)
{
    Self.InsertAt(Index, Chars);
}

static void FStringRemoveAt(int32 Index, int32 Count, FString& Self)
{
    Self.RemoveAt(Index, Count);
}

// ---------------------------------------------------------------------------
// String factory – maps script string literals to FString objects
// ---------------------------------------------------------------------------


class FStringFactory : public asIStringFactory
{
    const void* GetStringConstant(const char* Data, asUINT Length) override
    {
        FUTF8ToTCHAR Convertor(Data, Length);
        auto* Str = new FString();
        Str->AppendChars(Convertor.Get(), Convertor.Length());
        return Str;
    }

    int ReleaseStringConstant(const void* Str) override
    {
        delete (FString*)Str;
        return 0;
    }

    int GetRawStringData(const void* Str, char* Data, asUINT* Length) const override
    {
        FString* UnrealString = (FString*)Str;
        if (UnrealString->Len() == 0)
        {
            if (Length != nullptr)
                *Length = 0;
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
    FString DDD;
    DDD += "ASDS";
    asbind20::value_class<FString>(Engine, "FString")
        // Default constructor, copy constructor, destructor, opAssign via type traits
        .behaviours_by_traits()

        // ---- Comparison operators ----------------------------------------
        .opEquals()
        .method("int opCmp(const FString &in) const",
            asbind20::fp<&FStringCompare>)

        // ---- Concatenation operators -------------------------------------
        .method("FString opAdd(const FString &in) const",
            asbind20::fp<&FStringAppend>)
        .method("FString& opAddAssign(const FString &in)",
            [](const FString& Other, FString& Self) -> FString&
            {
                Self += Other;
                return Self;
            })

        // ---- Path-join operators (/) -------------------------------------
        .method("FString opDiv(const FString &in) const",
            asbind20::fp<&FStringPathAppend>)
        .method("FString& opDivAssign(const FString &in)",
            asbind20::fp<&FStringPathAppendAssign>)

        // ---- Query / state -----------------------------------------------
        .method("int Len() const",
            asbind20::fp<&FString::Len>)
        .method("bool IsEmpty() const",
            asbind20::fp<&FString::IsEmpty>)
        .method("bool IsValidIndex(int Index) const",
            asbind20::fp<&FString::IsValidIndex>)
        .method("bool IsNumeric() const",
            asbind20::fp<&FString::IsNumeric>)
        .method("bool ToBool() const",
            asbind20::fp<&FString::ToBool>)

        // ---- Memory management -------------------------------------------
        .method("void Empty()",
            asbind20::fp<static_cast<void (FString::*)()>(&FString::Empty)>)
        .method("void Reset(int NewReservedSize = 0)",
            asbind20::fp<&FString::Reset>)
        .method("void Shrink()",
            asbind20::fp<&FString::Shrink>)
        .method("void Reserve(int CharacterCount)",
            asbind20::fp<&FString::Reserve>)

        // ---- Append / insert / remove ------------------------------------
        .method("void AppendInt(int Num)",
            asbind20::fp<&FString::AppendInt>)
        .method("void InsertAt(int Index, const FString &in Chars)",
            asbind20::fp<&FStringInsertAt>)
        .method("void RemoveAt(int Index, int Count = 1)",
            asbind20::fp<&FStringRemoveAt>)
        .method("void RemoveSpacesInline()",
            asbind20::fp<&FString::RemoveSpacesInline>)

        // ---- Substring extraction ----------------------------------------
        .method("FString Left(int Count) const",
            asbind20::fp<static_cast<FString (FString::*)(int32) const &>(&FString::Left)>)
        .method("FString LeftChop(int Count) const",
            asbind20::fp<static_cast<FString (FString::*)(int32) const &>(&FString::LeftChop)>)
        .method("FString Right(int Count) const",
            asbind20::fp<static_cast<FString (FString::*)(int32) const &>(&FString::Right)>)
        .method("FString RightChop(int Count) const",
            asbind20::fp<static_cast<FString (FString::*)(int32) const &>(&FString::RightChop)>)
        .method("FString Mid(int Start, int Count) const",
            asbind20::fp<static_cast<FString (FString::*)(int32, int32) const &>(&FString::Mid)>)
        .method("FString MidToEnd(int Start) const",
            [](int32 Start, const FString& Self) -> FString
            {
                return Self.Mid(Start);
            })

        // ---- Padding -----------------------------------------------------
        .method("FString LeftPad(int Count) const", asbind20::fp<&FString::LeftPad>)
        .method("FString RightPad(int Count) const", asbind20::fp<&FString::RightPad>)

        // ---- Search ------------------------------------------------------
        .method("int Find(const FString &in SubStr) const",
            [](const FString& SubStr, const FString& Self) -> int32
            {
                return Self.Find(SubStr);
            })
        .method("bool Contains(const FString &in SubStr) const",
            [](const FString& SubStr, const FString& Self) -> bool
            {
                return Self.Contains(SubStr);
            })
        .method("bool StartsWith(const FString &in Prefix) const",
            [](const FString& Prefix, const FString& Self) -> bool
            {
                return Self.StartsWith(Prefix);
            })
        .method("bool EndsWith(const FString &in Suffix) const",
            [](const FString& Suffix, const FString& Self) -> bool
            {
                return Self.EndsWith(Suffix);
            })
        .method("bool MatchesWildcard(const FString &in Wildcard) const",
            [](const FString& Wildcard, const FString& Self) -> bool
            {
                return Self.MatchesWildcard(Wildcard);
            })

        // ---- Comparison --------------------------------------------------
        .method("bool Equals(const FString &in Other) const",
            [](const FString& Other, const FString& Self) -> bool
            {
                return Self.Equals(Other, ESearchCase::CaseSensitive);
            })
        .method("bool EqualsIgnoreCase(const FString &in Other) const",
            [](const FString& Other, const FString& Self) -> bool
            {
                return Self.Equals(Other, ESearchCase::IgnoreCase);
            })
        .method("int Compare(const FString &in Other) const",
            [](const FString& Other, const FString& Self) -> int32
            {
                return Self.Compare(Other, ESearchCase::CaseSensitive);
            })
        .method("int CompareIgnoreCase(const FString &in Other) const",
            [](const FString& Other, const FString& Self) -> int32
            {
                return Self.Compare(Other, ESearchCase::IgnoreCase);
            })

        // ---- Case conversion ---------------------------------------------
        .method("FString ToUpper() const",
            asbind20::fp<static_cast<FString (FString::*)() const &>(&FString::ToUpper)>)
        .method("FString ToLower() const",
            asbind20::fp<static_cast<FString (FString::*)() const &>(&FString::ToLower)>)

        // ---- Trimming ----------------------------------------------------
        .method("FString TrimStartAndEnd() const",
            asbind20::fp<static_cast<FString (FString::*)() const &>(&FString::TrimStartAndEnd)>)
        .method("FString TrimStart() const",
            asbind20::fp<static_cast<FString (FString::*)() const &>(&FString::TrimStart)>)
        .method("FString TrimEnd() const",
            asbind20::fp<static_cast<FString (FString::*)() const &>(&FString::TrimEnd)>)
        .method("FString TrimQuotes() const",
            [](const FString& Self) -> FString
            {
                return Self.TrimQuotes();
            })

        // ---- Modification ------------------------------------------------
        .method("FString Replace(const FString &in From, const FString &in To) const",
            asbind20::fp<&FStringReplace>)
        .method("int ReplaceInline(const FString &in From, const FString &in To)",
            asbind20::fp<&FStringReplaceInline>)
        .method("bool RemoveFromStart(const FString &in Prefix)",
            asbind20::fp<&FStringRemoveFromStart>)
        .method("bool RemoveFromEnd(const FString &in Suffix)",
            asbind20::fp<&FStringRemoveFromEnd>)
        .method("FString Reverse() const",
            asbind20::fp<static_cast<FString (FString::*)() const &>(&FString::Reverse)>)
        ;

    // Register string factory
    Engine->RegisterStringFactory("FString", new FStringFactory());

    // Static utility functions exposed as globals
    asbind20::global(Engine)
        .function("FString FStringFromInt(int Num)",
            [](int32 Num) -> FString
            {
                return FString::FromInt(Num);
            })
        .function("FString FStringSanitizeFloat(double Value)",
            [](double Value) -> FString
            {
                return FString::SanitizeFloat(Value);
            });
}
