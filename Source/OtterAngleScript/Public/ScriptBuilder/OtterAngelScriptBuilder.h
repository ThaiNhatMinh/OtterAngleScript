// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include <string>

class asIScriptEngine;

/**
 * Structured data for a single UENUM value.
 */
struct OTTERANGLESCRIPT_API FScriptEnumValue
{
	FString Name;
	FString DisplayName;
	FString ToolTip;
	int64 OverrideValue = INDEX_NONE;	//< Set if the enum value has an explicit = N assignment.
};

/**
 * Structured data for a UENUM() block parsed from an .as file.
 */
struct OTTERANGLESCRIPT_API FScriptEnum
{
	FString Name;
	FString Category;
	/** Specifier flags. Keys are lower-cased (e.g. "blueprinttype"). */
	TMap<FString, FString> Specifiers;
	TArray<FScriptEnumValue> Values;
};

/**
 * UPROPERTY() metadata extracted from an annotated .as struct/class member.
 */
struct OTTERANGLESCRIPT_API FScriptProperty
{
	FString Type;					// e.g. "int32", "float", "FString"
	FString Name;					// e.g. "Score", "PlayerName"
	FString DefaultValue;
	TMap<FString, FString> Specifiers;
};

/**
 * UFUNCTION() metadata extracted from an annotated .as struct/class method.
 */
struct OTTERANGLESCRIPT_API FScriptFunction
{
	FString ReturnType;				// e.g. "void", "float", "bool"
	FString Name;					// e.g. "TakeDamage", "IsAlive"
	TArray<FString> ParamTypes;
	TArray<FString> ParamNames;
	TArray<FString> Specifiers;
	TMap<FString, FString> Metas;
	bool bIsConst = false;
};

/**
 * Structured data for a UCLASS() block parsed from an .as file.
 */
struct OTTERANGLESCRIPT_API FScriptClass
{
	FString Name;					// e.g. "AActor", "UGameMode"
	FString ParentClass;			// e.g. "UObject" (empty for no parent)
	FString Category;
	TMap<FString, FString> Specifiers;
	TArray<FScriptProperty> Properties;
	TArray<FScriptFunction> Functions;
};

/**
 * Structured data for a USTRUCT() block parsed from an .as file.
 */
struct OTTERANGLESCRIPT_API FScriptStruct
{
	FString Name;
	FString Category;
	TMap<FString, FString> Specifiers;
	TArray<FScriptProperty> Properties;
	TArray<FScriptFunction> Functions;
};

/**
 * Parses AngelScript (.as) files that contain UE-style annotation macros
 * (UCLASS, USTRUCT, UENUM, UPROPERTY, UFUNCTION) and produces structured
 * type information that can be used to generate C++ binding registration code.
 *
 * Usage:
 *   FOtterAngelScriptBuilder Builder;
 *   if (Builder.ParseFile(TEXT("Scripts/MyTypes.as")))
 *   {
 *       for (const FScriptClass& Cls : Builder.GetClasses()) { ... }
 *       FString Code = Builder.GenerateBindingCode();
 *   }
 */
class OTTERANGLESCRIPT_API FOtterAngelScriptBuilder
{
public:
	FOtterAngelScriptBuilder(asIScriptEngine* InEngine) : Engine(InEngine) {}

	/** Parse all .as files in the given directory (recursive). */
	bool ParseDirectory(const FString& DirectoryPath);

	/** Parse a single .as file from disk. */
	bool ParseFile(const FString& FilePath);

	/** Parse a block of .as source text. */
	std::string ParseContent(const FString& Content, const FString& SourceName = TEXT("memory"));

	// ── Accessors ────────────────────────────────────────────────────────

	const TArray<FScriptEnum>&   GetEnums()   const { return Enums; }
	const TArray<FScriptStruct>& GetStructs() const { return Structs; }
	const TArray<FScriptClass>&  GetClasses() const { return Classes; }

	/** Clear all parsed data. */
	void Reset();

	class UOtterAngelScriptClass* CreateNewClass(const FString& ClassName, UClass* ParentClass = nullptr);

	bool Build(asIScriptModule* Module, const FString& PluginName);

private:
	asIScriptEngine* Engine;
	TArray<FScriptEnum>   Enums;
	TArray<FScriptStruct> Structs;
	TArray<FScriptClass>  Classes;
	TArray<FScriptFunction> StaticFunctions;

	/** Parse source text and populate the arrays. */
	std::string ParseSourceText(const FString& Content, const FString& SourceName);

	// ── Internal parsing helpers ─────────────────────────────────────────
	int SkipStatement(std::string& modifiedScript, int pos);

	/** Extract content within the first set of balanced {} braces starting at Pos. */
	static bool ExtractBlock(const FString& Text, int32 StartPos, int32& OutBlockStart, int32& OutBlockEnd);

	/** Parse a UENUM(...) block starting at the given position. */
	static bool TryParseEnum(const FString& Text, int32& InOutPos, FScriptEnum& OutEnum);

	/** Parse a USTRUCT(...) block starting at the given position. */
	static bool TryParseStruct(const FString& Text, int32& InOutPos, FScriptStruct& OutStruct);

	/** Parse a UCLASS(...) block starting at the given position. */
	static bool TryParseClass(const FString& Text, int32& InOutPos, FScriptClass& OutClass);

	/** Parse a UPROPERTY(...) line. Returns the specifier map and advances InOutPos past the line. */
	static bool TryParseProperty(const FString& Text, int32& InOutPos, FScriptProperty& OutProp);

	/** Parse a UFUNCTION(...) line + the following method declaration. */
	uint32 TryParseFunction(std::string& ScriptContent, uint32 Pos, const std::string& CurrentClass, FScriptFunction& OutFunc);
	uint32 TryParseMeta(std::string& ModifiedContent, uint32 Pos, TMap<FString, FString>& OutMeta);

	/** Parse specifiers from a UENUM(...)/UCLASS(...)/etc. annotation line. */
	static TMap<FString, FString> ParseSpecifiers(const FString& SpecText);

	/** Remove annotation macros from a line (replace with spaces for AS compilation). */
	static FString StripAnnotations(const FString& Line);

	/** Trim whitespace from left and right. */
	static FString Trim(const FString& S);
};
