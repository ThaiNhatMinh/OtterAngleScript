// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScriptBuilder/OtterAngelScriptBuilder.h"
#include "OtterAngelScriptClass.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Containers/Set.h"
#include "Containers/Map.h"
#include "angelscript.h"
#include <string>
#include <vector>
#include <vector>
#include <algorithm>
#include <cctype>

// Overwrite all characters except line breaks with blanks
void OverwriteCode(std::string& modifiedScript, int start, int len)
{
	char* code = &modifiedScript[start];
	for (int n = 0; n < len; n++)
	{
		if (*code != '\n')
			*code = ' ';
		code++;
	}
}

bool IsValidFunctionSpecifiers(std::string& Spec)
{
	static std::vector<std::string> SupportedSpecifiers = 
	{
		"blueprintevent",
		"blueprintcallable",
		"meta",
	};
	std::transform(Spec.begin(), Spec.end(), Spec.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
	return std::find(SupportedSpecifiers.begin(), SupportedSpecifiers.end(), Spec) != SupportedSpecifiers.end();
}

int FOtterAngelScriptBuilder::SkipStatement(std::string& modifiedScript, int pos)
{
	asUINT len = 0;

	// Skip until ; or { whichever comes first
	while (pos < (int)modifiedScript.length() && modifiedScript[pos] != ';' && modifiedScript[pos] != '{')
	{
		Engine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
		pos += len;
	}

	// Skip entire statement block
	if (pos < (int)modifiedScript.length() && modifiedScript[pos] == '{')
	{
		pos += 1;

		// Find the end of the statement block
		int level = 1;
		while (level > 0 && pos < (int)modifiedScript.size())
		{
			asETokenClass t = Engine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
			if (t == asTC_KEYWORD)
			{
				if (modifiedScript[pos] == '{')
					level++;
				else if (modifiedScript[pos] == '}')
					level--;
			}

			pos += len;
		}
	}
	else
		pos += 1;

	return pos;
}

FString FOtterAngelScriptBuilder::Trim(const FString& S)
{
	int32 Start = 0;
	while (Start < S.Len() && (S[Start] == ' ' || S[Start] == '\t'))
		++Start;
	int32 End = S.Len();
	while (End > Start && (S[End - 1] == ' ' || S[End - 1] == '\t' || S[End - 1] == ';'))
		--End;
	return S.Mid(Start, End - Start);
}

TMap<FString, FString> FOtterAngelScriptBuilder::ParseSpecifiers(const FString& SpecText)
{
	TMap<FString, FString> Result;
	if (SpecText.IsEmpty())
	{
		return Result;
	}

	FString Clean = SpecText;
	// Remove parentheses pairs inside specifier values so that top-level
	// commas are meaningful.  This is a best-effort scan — we walk the
	// FString and skip parenthesised blocks.
	FString Flat;
	int32 Depth = 0;
	for (int32 i = 0; i < Clean.Len(); ++i)
	{
		TCHAR C = Clean[i];
		if (C == '(')
		{
			++Depth;
			// Keep '(' to reconstruct the value later
			Flat.AppendChar(C);
		}
		else if (C == ')')
		{
			--Depth;
			Flat.AppendChar(C);
		}
		else if (C == ',' && Depth == 0)
		{
			Flat.AppendChar(C);
		}
		else
		{
			Flat.AppendChar(C);
		}
	}

	// Split on top-level commas
	TArray<FString> Parts;
	Flat.ParseIntoArray(Parts, TEXT(","), true);
	for (const FString& Part : Parts)
	{
		FString Trimmed = Trim(Part);
		if (Trimmed.IsEmpty())
		{
			continue;
		}
		int32 EqIdx;
		if (Trimmed.FindChar(TCHAR('='), EqIdx))
		{
			FString Key = Trim(Trimmed.Left(EqIdx)).ToLower();
			FString Val = Trim(Trimmed.Mid(EqIdx + 1));
			Result.Add(Key, Val);
		}
		else
		{
			Result.Add(Trimmed.ToLower(), FString());
		}
	}
	return Result;
}

FString FOtterAngelScriptBuilder::StripAnnotations(const FString& Line)
{
	// Replace annotation macros (UCLASS, USTRUCT, UENUM, UPROPERTY, UFUNCTION
	// and parenthesised content) with spaces so line numbers stay stable.
	FString Result = Line;
	while (true)
	{
		int32 Pos = INDEX_NONE;
		int32 Len = 0;
		const TCHAR* Patterns[] = {
			TEXT("UCLASS("),
			TEXT("USTRUCT("),
			TEXT("UENUM("),
			TEXT("UPROPERTY("),
			TEXT("UFUNCTION(")
		};
		for (const TCHAR* Pat : Patterns)
		{
			int32 Found = Result.Find(Pat, ESearchCase::CaseSensitive);
			if (Found != INDEX_NONE && (Pos == INDEX_NONE || Found < Pos))
			{
				Pos = Found;
				Len = FCString::Strlen(Pat);
			}
		}
		if (Pos == INDEX_NONE)
		{
			break;
		}
		// Find matching close paren
		int32 Depth = 1;
		int32 End = Pos + Len - 1;
		while (++End < Result.Len())
		{
			if (Result[End] == '(')
			{
				++Depth;
			}
			else if (Result[End] == ')')
			{
				if (--Depth == 0)
				{
					break;
				}
			}
		}
		// Replace the entire annotation with spaces
		for (int32 i = Pos; i <= End; ++i)
		{
			Result[i] = ' ';
		}
	}
	return Result;
}

bool FOtterAngelScriptBuilder::ExtractBlock(const FString& Text, int32 StartPos,
										   int32& OutBlockStart, int32& OutBlockEnd)
{
	// Find the first '{' at or after StartPos
	/*int32 BraceOpen = Text.Find(TEXT('{'), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPos);
	if (BraceOpen == INDEX_NONE)
	{
		return false;
	}

	int32 Depth = 1;
	int32 i = BraceOpen;
	while (++i < Text.Len())
	{
		if (Text[i] == '{')
		{
			++Depth;
		}
		else if (Text[i] == '}')
		{
			if (--Depth == 0)
			{
				OutBlockStart = BraceOpen + 1;
				OutBlockEnd = i;
				return true;
			}
		}
	}*/
	return false;
}

// ── Enum parsing ────────────────────────────────────────────────────────────

bool FOtterAngelScriptBuilder::TryParseEnum(const FString& Text, int32& InOutPos, FScriptEnum& OutEnum)
{
	int32 Start = InOutPos;

	// Expect "UENUM" keyword
	int32 Pos = Text.Find(TEXT("UENUM"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Start);
	if (Pos == INDEX_NONE || Pos != Start)
	{
		return false;
	}
	Pos += 5;

	int32 ParenOpen = Pos;
	if (ParenOpen >= Text.Len() || Text[ParenOpen] != '(')
	{
		return false;
	}

	// Find matching close paren for specifiers
	int32 Depth = 1;
	int32 ParenEnd = ParenOpen;
	while (++ParenEnd < Text.Len())
	{
		if (Text[ParenEnd] == '(')
		{
			++Depth;
		}
		else if (Text[ParenEnd] == ')')
		{
			if (--Depth == 0)
			{
				break;
			}
		}
	}

	FString SpecContent = Text.Mid(ParenOpen + 1, ParenEnd - ParenOpen - 1);
	OutEnum.Specifiers = ParseSpecifiers(SpecContent);

	// Extract the declaration line: "enum <name>" or "enum class <name>"
	int32 DeclEnd = Text.Find(TEXT("{"), ESearchCase::IgnoreCase, ESearchDir::FromStart, ParenEnd + 1);
	if (DeclEnd == INDEX_NONE)
	{
		return false;
	}

	FString DeclLine = Text.Mid(ParenEnd + 1, DeclEnd - ParenEnd - 1);
	DeclLine.TrimStartAndEndInline();

	// Strip annotations from the declaration line
	FString CleanDecl = StripAnnotations(DeclLine);
	CleanDecl.TrimStartAndEndInline();

	// Parse "enum <name>" or "enum class <name>" or "enum <name> : <type>"
	const TCHAR* EnumPrefix = TEXT("enum");
	if (!CleanDecl.StartsWith(EnumPrefix))
	{
		return false;
	}
	FString AfterEnum = Trim(CleanDecl.Mid(4)); // skip "enum"

	// Optional "class" or "struct"
	int32 NameStart = 0;
	if (AfterEnum.StartsWith(TEXT("class")))
	{
		AfterEnum = Trim(AfterEnum.Mid(5));
	}

	// Name ends at space, colon, or brace
	int32 NameEnd = INDEX_NONE;
	AfterEnum.FindChar(TCHAR(' '), NameEnd);
	int32 ColonIdx;
	if (AfterEnum.FindChar(TCHAR(':'), ColonIdx) && (NameEnd == INDEX_NONE || ColonIdx < NameEnd))
	{
		NameEnd = ColonIdx;
	}
	if (NameEnd == INDEX_NONE)
	{
		OutEnum.Name = Trim(AfterEnum);
	}
	else
	{
		OutEnum.Name = Trim(AfterEnum.Left(NameEnd));
	}

	if (OutEnum.Name.IsEmpty())
	{
		return false;
	}

	// Extract enum body
	int32 BlockStart, BlockEnd;
	if (!ExtractBlock(Text, ParenEnd + 1, BlockStart, BlockEnd))
	{
		return false;
	}

	FString Body = Text.Mid(BlockStart, BlockEnd - BlockStart);

	// Split by commas
	TArray<FString> Lines;
	Body.ParseIntoArray(Lines, TEXT(","), true);
	for (FString& Line : Lines)
	{
		FString CleanLine = Trim(Line);
		if (CleanLine.IsEmpty())
		{
			continue;
		}
		// Strip trailing comment
		int32 CommentIdx = CleanLine.Find(TEXT("//"));
		if (CommentIdx != INDEX_NONE)
		{
			CleanLine = Trim(CleanLine.Left(CommentIdx));
		}
		if (CleanLine.IsEmpty())
		{
			continue;
		}

		FScriptEnumValue EV;
		int32 EqIdx;
		if (CleanLine.FindChar(TCHAR('='), EqIdx))
		{
			EV.Name = Trim(CleanLine.Left(EqIdx));
			FString ValStr = Trim(CleanLine.Mid(EqIdx + 1));
			// Handle hex (0x) and decimal
			if (ValStr.StartsWith(TEXT("0x")) || ValStr.StartsWith(TEXT("0X")))
			{
				EV.OverrideValue = FCString::Strtoi64(*ValStr, nullptr, 16);
			}
			else
			{
				EV.OverrideValue = FCString::Strtoi64(*ValStr, nullptr, 10);
			}
		}
		else
		{
			EV.Name = CleanLine;
		}
		OutEnum.Values.Add(EV);
	}

	// Set category from specifiers
	if (OutEnum.Specifiers.Contains(TEXT("category")))
	{
		OutEnum.Category = OutEnum.Specifiers[TEXT("category")];
	}

	InOutPos = BlockEnd + 1;
	return true;
}

// ── Struct parsing ──────────────────────────────────────────────────────────

bool FOtterAngelScriptBuilder::TryParseStruct(const FString& Text, int32& InOutPos, FScriptStruct& OutStruct)
{
	int32 Start = InOutPos;
	int32 Pos = Text.Find(TEXT("USTRUCT"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Start);
	if (Pos == INDEX_NONE || Pos != Start)
	{
		return false;
	}
	Pos += 7;

	if (Pos >= Text.Len() || Text[Pos] != '(')
	{
		return false;
	}

	// Find matching close paren for specifiers
	int32 Depth = 1;
	int32 ParenEnd = Pos;
	while (++ParenEnd < Text.Len())
	{
		if (Text[ParenEnd] == '(')
		{
			++Depth;
		}
		else if (Text[ParenEnd] == ')')
		{
			if (--Depth == 0)
			{
				break;
			}
		}
	}

	FString SpecContent = Text.Mid(Pos + 1, ParenEnd - Pos - 1);
	OutStruct.Specifiers = ParseSpecifiers(SpecContent);

	// Extract declaration line: "struct <name>"
	int32 BraceOpen = Text.Find(TEXT("{"), ESearchCase::IgnoreCase, ESearchDir::FromStart, ParenEnd + 1);
	if (BraceOpen == INDEX_NONE)
	{
		return false;
	}

	FString DeclLine = Text.Mid(ParenEnd + 1, BraceOpen - ParenEnd - 1);
	DeclLine.TrimStartAndEndInline();
	FString CleanDecl = StripAnnotations(DeclLine);
	CleanDecl.TrimStartAndEndInline();

	if (!CleanDecl.StartsWith(TEXT("struct ")))
	{
		return false;
	}
	OutStruct.Name = Trim(CleanDecl.Mid(7)); // skip "struct "

	int32 SpaceIdx;
	if (OutStruct.Name.FindChar(TCHAR(' '), SpaceIdx))
	{
		OutStruct.Name = OutStruct.Name.Left(SpaceIdx);
	}

	if (OutStruct.Name.IsEmpty())
	{
		return false;
	}

	// Extract body
	int32 BlockStart, BlockEnd;
	if (!ExtractBlock(Text, ParenEnd + 1, BlockStart, BlockEnd))
	{
		return false;
	}

	// Parse members
	FString Body = Text.Mid(BlockStart, BlockEnd - BlockStart);
	TArray<FString> BodyLines;
	Body.ParseIntoArrayLines(BodyLines);

	int32 LinePos = 0;
	while (LinePos < BodyLines.Num())
	{
		FString Line = Trim(BodyLines[LinePos]);
		if (Line.IsEmpty())
		{
			++LinePos;
			continue;
		}

		if (Line.StartsWith(TEXT("//")))
		{
			++LinePos;
			continue;
		}

		if (Line.StartsWith(TEXT("UPROPERTY"), ESearchCase::CaseSensitive))
		{
			FScriptProperty Prop;
			if (TryParseProperty(Body, LinePos, Prop))
			{
				OutStruct.Properties.Add(Prop);
			}
			continue;
		}

		if (Line.StartsWith(TEXT("UFUNCTION"), ESearchCase::CaseSensitive))
		{
			FScriptFunction Func;
			// if (TryParseFunction(Body, LinePos, Func))
			{
				OutStruct.Functions.Add(Func);
			}
			continue;
		}

		++LinePos;
	}

	// Set category from specifiers
	if (OutStruct.Specifiers.Contains(TEXT("category")))
	{
		OutStruct.Category = OutStruct.Specifiers[TEXT("category")];
	}

	InOutPos = BlockEnd + 1;
	return true;
}

// ── Class parsing ───────────────────────────────────────────────────────────

bool FOtterAngelScriptBuilder::TryParseClass(const FString& Text, int32& InOutPos, FScriptClass& OutClass)
{
	int32 Start = InOutPos;
	int32 Pos = Text.Find(TEXT("UCLASS"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Start);
	if (Pos == INDEX_NONE || Pos != Start)
	{
		return false;
	}
	Pos += 6;

	if (Pos >= Text.Len() || Text[Pos] != '(')
	{
		return false;
	}

	// Find matching close paren
	int32 Depth = 1;
	int32 ParenEnd = Pos;
	while (++ParenEnd < Text.Len())
	{
		if (Text[ParenEnd] == '(')
		{
			++Depth;
		}
		else if (Text[ParenEnd] == ')')
		{
			if (--Depth == 0)
			{
				break;
			}
		}
	}

	FString SpecContent = Text.Mid(Pos + 1, ParenEnd - Pos - 1);
	OutClass.Specifiers = ParseSpecifiers(SpecContent);

	// Extract class declaration: "class <name> [extends <parent>]"
	int32 BraceOpen = Text.Find(TEXT("{"), ESearchCase::IgnoreCase, ESearchDir::FromStart, ParenEnd + 1);
	if (BraceOpen == INDEX_NONE)
	{
		return false;
	}

	FString DeclLine = Text.Mid(ParenEnd + 1, BraceOpen - ParenEnd - 1);
	DeclLine.TrimStartAndEndInline();
	FString CleanDecl = StripAnnotations(DeclLine);
	CleanDecl.TrimStartAndEndInline();

	if (!CleanDecl.StartsWith(TEXT("class ")))
	{
		return false;
	}

	FString AfterClass = Trim(CleanDecl.Mid(6)); // skip "class "

	// Look for "extends" or ":"
	const TCHAR* ExtendsKeywords[] = { TEXT("extends "), TEXT(": ") };
	FString ExtendsFound;
	int32 ExtendsIdx = INDEX_NONE;
	for (const TCHAR* Kw : ExtendsKeywords)
	{
		ExtendsIdx = AfterClass.Find(Kw, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		if (ExtendsIdx != INDEX_NONE)
		{
			ExtendsFound = Kw;
			break;
		}
	}

	if (ExtendsIdx != INDEX_NONE)
	{
		OutClass.Name = Trim(AfterClass.Left(ExtendsIdx));
		OutClass.ParentClass = Trim(AfterClass.Mid(ExtendsIdx + ExtendsFound.Len()));
		// Remove any trailing brace
		int32 TrailingBrace;
		if (OutClass.ParentClass.FindChar(TCHAR('{'), TrailingBrace))
		{
			OutClass.ParentClass = Trim(OutClass.ParentClass.Left(TrailingBrace));
		}
	}
	else
	{
		OutClass.Name = AfterClass;
		int32 BracesIdx;
		if (OutClass.Name.FindChar(TCHAR('{'), BracesIdx))
		{
			OutClass.Name = Trim(OutClass.Name.Left(BracesIdx));
		}
	}

	if (OutClass.Name.IsEmpty())
	{
		return false;
	}

	// Extract body
	int32 BlockStart, BlockEnd;
	if (!ExtractBlock(Text, ParenEnd + 1, BlockStart, BlockEnd))
	{
		return false;
	}

	// Parse members
	FString Body = Text.Mid(BlockStart, BlockEnd - BlockStart);
	TArray<FString> BodyLines;
	Body.ParseIntoArrayLines(BodyLines);

	int32 LinePos = 0;
	while (LinePos < BodyLines.Num())
	{
		FString Line = Trim(BodyLines[LinePos]);
		if (Line.IsEmpty())
		{
			++LinePos;
			continue;
		}

		if (Line.StartsWith(TEXT("//")))
		{
			++LinePos;
			continue;
		}

		if (Line.StartsWith(TEXT("UPROPERTY"), ESearchCase::CaseSensitive))
		{
			FScriptProperty Prop;
			if (TryParseProperty(Body, LinePos, Prop))
			{
				OutClass.Properties.Add(Prop);
			}
			continue;
		}

		if (Line.StartsWith(TEXT("UFUNCTION"), ESearchCase::CaseSensitive))
		{
			FScriptFunction Func;
			// if (TryParseFunction(Body, LinePos, Func))
			{
				OutClass.Functions.Add(Func);
			}
			continue;
		}

		++LinePos;
	}

	// Set category from specifiers
	if (OutClass.Specifiers.Contains(TEXT("category")))
	{
		OutClass.Category = OutClass.Specifiers[TEXT("category")];
	}

	InOutPos = BlockEnd + 1;
	return true;
}

// ── Property parsing ────────────────────────────────────────────────────────

bool FOtterAngelScriptBuilder::TryParseProperty(const FString& Text, int32& InOutPos, FScriptProperty& OutProp)
{
	// Expect Text[InOutPos]... to contain UPROPERTY(...)
	FString Line = Text.Mid(InOutPos);
	if (!Line.StartsWith(TEXT("UPROPERTY"), ESearchCase::CaseSensitive))
	{
		return false;
	}

	// Find the matching close paren for the property specifiers
	int32 LocalPos = 9; // after "UPROPERTY"
	if (LocalPos >= Line.Len() || Line[LocalPos] != '(')
	{
		return false;
	}

	int32 Depth = 1;
	int32 ParenEnd = LocalPos;
	while (++ParenEnd < Line.Len())
	{
		if (Line[ParenEnd] == '(')
		{
			++Depth;
		}
		else if (Line[ParenEnd] == ')')
		{
			if (--Depth == 0)
			{
				break;
			}
		}
	}

	FString SpecContent = Line.Mid(LocalPos + 1, ParenEnd - LocalPos - 1);
	OutProp.Specifiers = ParseSpecifiers(SpecContent);

	// Advance past UPROPERTY(...)
	int32 CurrentInText = InOutPos + ParenEnd + 1;

	// Read the next declaration line: in the Text content, find the next newline
	// after the UPROPERTY and read the next non-empty line.
	int32 NewlinePos = Text.Find(TEXT("\n"), ESearchCase::IgnoreCase, ESearchDir::FromStart, CurrentInText);
	int32 NextLineStart = (NewlinePos != INDEX_NONE) ? NewlinePos + 1 : CurrentInText;
	while (NextLineStart < Text.Len())
	{
		int32 EOL = Text.Find(TEXT("\n"), ESearchCase::IgnoreCase, ESearchDir::FromStart, NextLineStart);
		if (EOL == INDEX_NONE)
		{
			EOL = Text.Len();
		}
		FString DeclLine = Text.Mid(NextLineStart, EOL - NextLineStart);
		FString CleanLine = Trim(DeclLine);
		// Strip comments
		int32 CommentIdx;
		if (CleanLine.FindChar(TCHAR('/'), CommentIdx) && CommentIdx + 1 < CleanLine.Len() && CleanLine[CommentIdx + 1] == TCHAR('/'))
		{
			CleanLine = Trim(CleanLine.Left(CommentIdx));
		}

		if (!CleanLine.IsEmpty() && !CleanLine.StartsWith(TEXT("//")))
		{
			// Parse "type Name [= Default];"
			FString NoSemi = CleanLine;
			if (NoSemi.EndsWith(TEXT(";")))
			{
				NoSemi = NoSemi.Left(NoSemi.Len() - 1);
			}
			NoSemi.TrimEndInline();

			// Split by '=' for default value
			int32 EqIdx;
			if (NoSemi.FindChar(TCHAR('='), EqIdx))
			{
				FString TypeAndName = Trim(NoSemi.Left(EqIdx));
				OutProp.DefaultValue = Trim(NoSemi.Mid(EqIdx + 1));

				int32 SpaceIdx;
				if (TypeAndName.FindLastChar(TCHAR(' '), SpaceIdx))
				{
					OutProp.Type = Trim(TypeAndName.Left(SpaceIdx));
					OutProp.Name = Trim(TypeAndName.Mid(SpaceIdx + 1));
				}
				else
				{
					// No space before name — unlikely but handle gracefully
					OutProp.Name = TypeAndName;
				}
			}
			else
			{
				int32 SpaceIdx;
				if (NoSemi.FindLastChar(TCHAR(' '), SpaceIdx))
				{
					OutProp.Type = Trim(NoSemi.Left(SpaceIdx));
					OutProp.Name = Trim(NoSemi.Mid(SpaceIdx + 1));
				}
				else
				{
					OutProp.Name = NoSemi;
				}
			}

			InOutPos = NextLineStart + EOL - NextLineStart + 1;
			return !OutProp.Name.IsEmpty();
		}

		NextLineStart = EOL + 1;
	}

	InOutPos = NextLineStart;
	return false;
}

// ── Function parsing ────────────────────────────────────────────────────────
uint32 FOtterAngelScriptBuilder::TryParseMeta(std::string& ModifiedContent, uint32 Pos, TMap<FString, FString>& OutMeta)
{
	std::string Meta_str("meta");
	OverwriteCode(ModifiedContent, Pos, Meta_str.length());
	Pos += Meta_str.length();
	bool bHasOpen = false;
	bool bHasClose = false;
	while( Pos < ModifiedContent.size() )
	{
		asUINT len = 0;
		asETokenClass TokenClass = Engine->ParseToken(&ModifiedContent[Pos], ModifiedContent.size() - Pos, &len);
		if (TokenClass == asTC_COMMENT || TokenClass == asTC_WHITESPACE)
		{
			Pos += len;
			continue;
		}
		
		std::string TokenStr;
		TokenStr.assign(&ModifiedContent[Pos], len);

		if (!bHasOpen)
		{
			if (TokenStr != "(" && TokenStr != "=")
			{
				// TODO: Report parse failed
				check(false);
			}
			else if (TokenStr == "(")
			{
				bHasOpen = true;
			}
			OverwriteCode(ModifiedContent, Pos, len);
			Pos += len;
			continue;
		}
		else if (!bHasClose)
		{
			if (TokenStr == ")")
			{
				bHasClose = true;
			}
			else
			{
				auto AssignPos = TokenStr.find('=');
				if (AssignPos == std::string::npos)
				{
					OutMeta.Add(TokenStr.c_str());
				}
				else
				{
					std::string Key = TokenStr.substr(0, AssignPos);
					std::string Val = TokenStr.substr(AssignPos + 1);
					OutMeta.Add(Key.c_str(), Val.c_str());	
				}
			}
			OverwriteCode(ModifiedContent, Pos, len);
			Pos += len;
			continue;
		}

		if (bHasClose)
		{
			break;
		}
		OverwriteCode(ModifiedContent, Pos, len);
		Pos += len;
	}

	if (!bHasClose)
	{
		// TODO: Report parse failed
		check(false);
	}

	return Pos;
}

uint32 FOtterAngelScriptBuilder::TryParseFunction(std::string& ModifiedContent, uint32 Pos, const std::string& CurrentClass, FScriptFunction& OutFunc)
{
	// Extract all metadata. They can be separated by , 
	std::string UFUNCTION_str("UFUNCTION");
	OverwriteCode(ModifiedContent, Pos, UFUNCTION_str.length());
	Pos += UFUNCTION_str.length();
	bool bHasOpen = false;
	bool bHasClose = false;
	bool bBeginParameteter = false;
	bool bEndParameteter = false;
	while( Pos < ModifiedContent.size() )
	{
		asUINT len = 0;
		asETokenClass TokenClass = Engine->ParseToken(&ModifiedContent[Pos], ModifiedContent.size() - Pos, &len);
		if (TokenClass == asTC_COMMENT || TokenClass == asTC_WHITESPACE)
		{
			Pos += len;
			continue;
		}
		std::string TokenStr;
		TokenStr.assign(&ModifiedContent[Pos], len);

		if (!bHasOpen)
		{
			if (TokenStr != "(")
			{
				// TODO: Report parse failed
				check(false);
			}
			else
			{
				bHasOpen = true;
			}
			OverwriteCode(ModifiedContent, Pos, len);
			Pos += len;
			continue;
		}
		else if (!bHasClose)
		{
			if (TokenStr == ")")
			{
				bHasClose = true;
			}
			else if (IsValidFunctionSpecifiers(TokenStr))
			{
				if (TokenStr == "meta")
				{
					Pos = TryParseMeta(ModifiedContent, Pos, OutFunc.Metas);
					continue;
				}
				else
				{
					OutFunc.Specifiers.Add(TokenStr.c_str());
				}
			}
			else if (TokenStr != ",")
			{
				// TODO: Report error
				check(false);
			}
			OverwriteCode(ModifiedContent, Pos, len);
			Pos += len;
			continue;
		}
		else if (OutFunc.ReturnType.IsEmpty())
		{
			OutFunc.ReturnType = TokenStr.c_str();
		}
		else if (OutFunc.Name.IsEmpty())
		{
			OutFunc.Name = TokenStr.c_str();
		}
		else if (!bBeginParameteter && TokenStr == "(") // Begin parameter
		{
			bBeginParameteter = true;
		}
		else if (bBeginParameteter && !bEndParameteter)
		{
			if (TokenStr == ",")
			{
				Pos += len;
				continue;
			}
			else if (TokenStr == ")")
			{
				Pos += len;
				bEndParameteter = true;
				continue;
			}
			if (OutFunc.ParamNames.Num() < OutFunc.ParamTypes.Num())
			{
				OutFunc.ParamNames.Add(TokenStr.c_str());
			}
			else
			{
				OutFunc.ParamTypes.Add(TokenStr.c_str());
			}
		}
		else if (bEndParameteter)
		{
			// Done parse function, skip the rest
			break;
		}

		Pos += len;

	}
	return Pos;
}

std::string FOtterAngelScriptBuilder::ParseSourceText(const FString& Content, const FString& SourceName)
{
	std::string ModifiedContent = TCHAR_TO_ANSI(*Content);
	std::string CurrentClass;
	std::string SectionName;
	std::vector<std::string> Includes;

	std::string                CurrentNamespace;
	std::vector<int>           CurrentNamespaceStack;

	// Then check for meta data and pre-processor directives
	uint32 pos = 0;
	while( pos < ModifiedContent.size() )
	{
		asUINT len = 0;
		asETokenClass TokenClass = Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);
		if( TokenClass == asTC_COMMENT || TokenClass == asTC_WHITESPACE )
		{
			pos += len;
			continue;
		}
		std::string TokenStr;
		TokenStr.assign(&ModifiedContent[pos], len);

		// Skip possible decorators before class and interface declarations
		if (TokenStr == "shared" || TokenStr == "abstract" || TokenStr == "mixin" || TokenStr == "external")
		{
			pos += len;
			continue;
		}

		// Check if class or interface so the metadata for members can be gathered
		if( CurrentClass == "" && (TokenStr == "class" || TokenStr == "interface") )
		{
			// Get the identifier after "class"
			do
			{
				pos += len;
				if( pos >= ModifiedContent.size() )
				{
					TokenClass = asTC_UNKNOWN;
					break;
				}
				TokenClass = Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);
			} while(TokenClass == asTC_COMMENT || TokenClass == asTC_WHITESPACE);

			if( TokenClass == asTC_IDENTIFIER )
			{
				CurrentClass = ModifiedContent.substr(pos,len);

				// Search until first { or ; is encountered
				while( pos < ModifiedContent.length() )
				{
					Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);

					// If start of class section encountered stop
					if( ModifiedContent[pos] == '{' )
					{
						pos += len;
						break;
					}
					else if (ModifiedContent[pos] == ';')
					{
						// The class declaration has ended and there are no children
						CurrentClass = "";
						pos += len;
						break;
					}

					// Check next symbol
					pos += len;
				}
			}

			continue;
		}

		// Check if end of class
		if( CurrentClass != "" && TokenStr == "}" )
		{
			CurrentClass = "";
			pos += len;
			continue;
		}

		// Check if namespace so the metadata for members can be gathered
		if( TokenStr == "namespace" )
		{
			// Get the scope after "namespace". It can be composed of multiple nested namespaces, e.g. A::B::C
			// Keep track of the number of nested namespace scopes are declared for each block
			int nestedNamespaces = 0;
			do
			{
				do
				{
					pos += len;
					TokenClass = Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);
				} while (TokenClass == asTC_COMMENT || TokenClass == asTC_WHITESPACE);

				if (TokenClass == asTC_IDENTIFIER)
				{
					if (CurrentNamespace != "")
						CurrentNamespace += "::";
					CurrentNamespace += ModifiedContent.substr(pos, len);
					nestedNamespaces++;
				}
			} while (TokenClass == asTC_IDENTIFIER || (TokenClass == asTC_KEYWORD && ModifiedContent.substr(pos, len) == "::"));
			CurrentNamespaceStack.push_back(nestedNamespaces);

			// Search until first { is encountered
			while( pos < ModifiedContent.length() )
			{
				Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);

				// If start of namespace section encountered stop
				if( ModifiedContent[pos] == '{' )
				{
					pos += len;
					break;
				}

				// Check next symbol
				pos += len;
			}

			continue;
		}

		// Check if end of namespace
		if( CurrentNamespace != "" && TokenStr == "}" )
		{
			check(CurrentNamespaceStack.size() > 0);
			int nestedNamespaces = CurrentNamespaceStack[CurrentNamespaceStack.size()-1];
			CurrentNamespaceStack.pop_back();
			while (nestedNamespaces-- > 0)
			{
				size_t found = CurrentNamespace.rfind("::");
				if (found != std::string::npos)
				{
					CurrentNamespace.erase(found);
				}
				else
				{
					CurrentNamespace = "";
				}
			}
			pos += len;
			continue;
		}

		// Is this the start of metadata?
		if (TokenStr == "UFUNCTION")
		{
			FScriptFunction outFunc;
			pos = TryParseFunction(ModifiedContent, pos, CurrentClass, outFunc);
			if (CurrentClass.size() == 0)
			{
				StaticFunctions.Add(outFunc);
			}
		}
		else
		// Is this a preprocessor directive?
		if( TokenStr == "#" && (pos + 1 < ModifiedContent.size()) )
		{
			int start = pos++;

			TokenClass = Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);
			if (TokenClass == asTC_IDENTIFIER)
			{
				TokenStr.assign(&ModifiedContent[pos], len);
				if (TokenStr == "include")
				{
					pos += len;
					TokenClass = Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);
					if (TokenClass == asTC_WHITESPACE)
					{
						pos += len;
						TokenClass = Engine->ParseToken(&ModifiedContent[pos], ModifiedContent.size() - pos, &len);
					}

					if (TokenClass == asTC_VALUE && len > 2 && (ModifiedContent[pos] == '"' || ModifiedContent[pos] == '\''))
					{
						// Get the include file
						std::string includefile;
						includefile.assign(&ModifiedContent[pos + 1], len - 2);
						pos += len;

						// Make sure the includeFile doesn't contain any line breaks
						size_t p = includefile.find('\n');
						if (p != std::string::npos)
						{
							// TODO: Show the correct line number for the error
							std::string str = "Invalid file name for #include; it contains a line-break: '" + includefile.substr(0, p) + "'";
							Engine->WriteMessage(SectionName.c_str(), 0, 0, asMSGTYPE_ERROR, str.c_str());
						}
						else
						{
							// Store it for later processing
							Includes.push_back(includefile);

							// Overwrite the include directive with space characters to avoid compiler error
							OverwriteCode(ModifiedContent, start, pos - start);
						}
					}
				}
				else if (TokenStr == "pragma")
				{
					// Read until the end of the line
					pos += len;
					//for (; pos < ModifiedContent.size() && ModifiedContent[pos] != '\n'; pos++);

					//// Call the pragma callback
					//FString pragmaText(&ModifiedContent[start + 7], pos - start - 7);
					//int r = pragmaCallback ? pragmaCallback(pragmaText, *this, pragmaParam) : -1;
					//if (r < 0)
					//{
					//	// TODO: Report the correct line number
					//	Engine->WriteMessage(SectionName.c_str(), 0, 0, asMSGTYPE_ERROR, "Invalid #pragma directive");
					//	return r;
					//}

					// Overwrite the pragma directive with space characters to avoid compiler error
					OverwriteCode(ModifiedContent, start, pos - start);
				}
			}
			else
			{
				// Check for lines starting with #!, e.g. shebang interpreter directive. These will be treated as comments and removed by the preprocessor
				if (ModifiedContent[pos] == '!')
				{
					// Read until the end of the line
					pos += len;
					for (; pos < ModifiedContent.size() && ModifiedContent[pos] != '\n'; pos++);

					// Overwrite the directive with space characters to avoid compiler error
					OverwriteCode(ModifiedContent, start, pos - start);
				}
			}
		}
		// Don't search for metadata/includes within statement blocks or between tokens in statements
		else
		{
			pos = SkipStatement(ModifiedContent, pos);
		}
	}

	return ModifiedContent;
}

// ── Public API ──────────────────────────────────────────────────────────────

bool FOtterAngelScriptBuilder::ParseDirectory(const FString& DirectoryPath)
{
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *DirectoryPath, TEXT("*.as"), true, false);
	if (Files.IsEmpty())
	{
		return false;
	}

	bool bAny = false;
	for (const FString& File : Files)
	{
		if (ParseFile(File))
		{
			bAny = true;
		}
	}
	return bAny;
}

bool FOtterAngelScriptBuilder::ParseFile(const FString& FilePath)
{
	FString Content;
	if (!FFileHelper::LoadFileToString(Content, *FilePath))
	{
		return false;
	}
	FString SourceName = FPaths::GetCleanFilename(FilePath);
	ParseSourceText(Content, SourceName);
	return !Enums.IsEmpty() || !Structs.IsEmpty() || !Classes.IsEmpty();
}

std::string FOtterAngelScriptBuilder::ParseContent(const FString& Content, const FString& SourceName)
{
	return ParseSourceText(Content, SourceName);
}

void FOtterAngelScriptBuilder::Reset()
{
	Enums.Reset();
	Structs.Reset();
	Classes.Reset();
}

UOtterAngelScriptClass* FOtterAngelScriptBuilder::CreateNewClass(const FString& ClassName, UClass* ParentClass)
{
	UOtterAngelScriptClass* NewClass = NewObject<UOtterAngelScriptClass>(GetTransientPackage(), *ClassName, RF_Public | RF_Transient);
	if (ParentClass)
	{
		NewClass->SetSuperStruct(ParentClass);
	}
	else
	{
		NewClass->SetSuperStruct(UObject::StaticClass());
	}
	NewClass->AddToRoot();

	// Finalize the class
	NewClass->Bind();
	NewClass->StaticLink(true);
	NewClass->AssembleReferenceTokenStream();
	NewClass->GetDefaultObject();
	return NewClass;
}

bool FOtterAngelScriptBuilder::Build(asIScriptModule* Module, const FString& PluginName)
{
	for (TObjectIterator<UFunction> It; It; ++It)
	{
		UFunction* Function = *It;
		auto Class = Cast<UClass>(Function->GetOuter());
		if (Function->HasAnyFunctionFlags(FUNC_Static))
		{
			int a = 9;
		}
	}

	auto FunctionLibraryClass = CreateNewClass(FString::Printf(TEXT("%s_FunctionLibrary"), *PluginName));
	for (auto& Function : StaticFunctions)
	{

	}
	return true;
}

