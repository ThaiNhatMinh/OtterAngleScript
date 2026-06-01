// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "AudioParameter.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Object.h"
#include "angelscript.h"

#include <new>

namespace
{
	// --- FAudioParameter constructors / destructor ---

	static void FAudioParameter_DefaultConstruct(FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter();
	}

	static void FAudioParameter_CopyConstruct(const FAudioParameter& Other, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(Other);
	}

	static void FAudioParameter_ConstructFromName(const FName& InName, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName);
	}

	static void FAudioParameter_ConstructFromNameFloat(const FName& InName, float InValue, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName, InValue);
	}

	static void FAudioParameter_ConstructFromNameBool(const FName& InName, bool InValue, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName, InValue);
	}

	static void FAudioParameter_ConstructFromNameInt(const FName& InName, int32 InValue, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName, InValue);
	}

	static void FAudioParameter_ConstructFromNameObject(const FName& InName, UObject* InValue, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName, InValue);
	}

	static void FAudioParameter_ConstructFromNameString(const FName& InName, const FString& InValue, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName, InValue);
	}

	static void FAudioParameter_ConstructFromNameType(const FName& InName, int InType, FAudioParameter* Memory)
	{
		new (Memory) FAudioParameter(InName, static_cast<EAudioParameterType>(InType));
	}

	static void FAudioParameter_Destruct(FAudioParameter* Memory)
	{
		Memory->~FAudioParameter();
	}

	// --- ParamType getter/setter (EAudioParameterType is enum class : uint8, not int32) ---

	static int FAudioParameter_GetParamType(const FAudioParameter& Self)
	{
		return static_cast<int>(Self.ParamType);
	}

	static void FAudioParameter_SetParamType(FAudioParameter& Self, int Value)
	{
		Self.ParamType = static_cast<EAudioParameterType>(Value);
	}

	// --- ObjectParam getter/setter (TObjectPtr<UObject> cannot be bound directly) ---

	static UObject* FAudioParameter_GetObjectParam(const FAudioParameter& Self)
	{
		return Self.ObjectParam.Get();
	}

	static void FAudioParameter_SetObjectParam(FAudioParameter& Self, UObject* Value)
	{
		Self.ObjectParam = Value;
	}

	// --- CreateDefaultArray wrapper (static factory returning FAudioParameter by value) ---

	static FAudioParameter FAudioParameter_CreateDefaultArray(const FName& InName, int32 InNum)
	{
		return FAudioParameter::CreateDefaultArray(InName, InNum);
	}
}

void Bind_FAudioParameter(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;

	// --- EAudioParameterType enum ---

	Result = Engine->RegisterEnum("EAudioParameterType");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "None",         static_cast<int>(EAudioParameterType::None));         check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "Boolean",      static_cast<int>(EAudioParameterType::Boolean));      check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "Integer",      static_cast<int>(EAudioParameterType::Integer));      check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "Float",        static_cast<int>(EAudioParameterType::Float));        check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "String",       static_cast<int>(EAudioParameterType::String));       check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "Object",       static_cast<int>(EAudioParameterType::Object));       check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "NoneArray",    static_cast<int>(EAudioParameterType::NoneArray));    check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "BooleanArray", static_cast<int>(EAudioParameterType::BooleanArray)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "IntegerArray", static_cast<int>(EAudioParameterType::IntegerArray)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "FloatArray",   static_cast<int>(EAudioParameterType::FloatArray));   check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "StringArray",  static_cast<int>(EAudioParameterType::StringArray));  check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "ObjectArray",  static_cast<int>(EAudioParameterType::ObjectArray));  check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAudioParameterType", "Trigger",      static_cast<int>(EAudioParameterType::Trigger));      check(Result >= 0);

	// --- FAudioParameter behaviors ---

	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f()",
		asFUNCTION(FAudioParameter_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FAudioParameter &in Other)",
		asFUNCTION(FAudioParameter_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName)",
		asFUNCTION(FAudioParameter_ConstructFromName), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName, float InValue)",
		asFUNCTION(FAudioParameter_ConstructFromNameFloat), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName, bool InValue)",
		asFUNCTION(FAudioParameter_ConstructFromNameBool), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName, int InValue)",
		asFUNCTION(FAudioParameter_ConstructFromNameInt), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName, UObject@ InValue)",
		asFUNCTION(FAudioParameter_ConstructFromNameObject), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName, const FString &in InValue)",
		asFUNCTION(FAudioParameter_ConstructFromNameString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_CONSTRUCT, "void f(const FName &in InName, EAudioParameterType InType)",
		asFUNCTION(FAudioParameter_ConstructFromNameType), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAudioParameter, asBEHAVE_DESTRUCT, "void f()",
		asFUNCTION(FAudioParameter_Destruct), asCALL_CDECL_OBJLAST);

	// --- Assignment ---

	REGISTER_METHOD(FAudioParameter, "FAudioParameter &opAssign(const FAudioParameter &in Other)",
		asMETHODPR(FAudioParameter, operator=, (const FAudioParameter&), FAudioParameter&), asCALL_THISCALL);

	// --- Scalar properties (direct offset binding) ---

	REGISTER_PROPERTY(FAudioParameter, "FName ParamName",   ParamName);
	REGISTER_PROPERTY(FAudioParameter, "float FloatParam",  FloatParam);
	REGISTER_PROPERTY(FAudioParameter, "bool BoolParam",    BoolParam);
	REGISTER_PROPERTY(FAudioParameter, "int IntParam",      IntParam);
	REGISTER_PROPERTY(FAudioParameter, "FString StringParam", StringParam);
	REGISTER_PROPERTY(FAudioParameter, "FName TypeName",    TypeName);

	// --- Virtual properties for non-int32-backed or wrapped fields ---
	// EAudioParameterType is enum class : uint8; TObjectPtr<UObject> is a smart pointer wrapper.

	REGISTER_METHOD(FAudioParameter, "EAudioParameterType get_ParamType() const",
		asFUNCTION(FAudioParameter_GetParamType), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAudioParameter, "void set_ParamType(EAudioParameterType Value)",
		asFUNCTION(FAudioParameter_SetParamType), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FAudioParameter, "UObject@ get_ObjectParam() const",
		asFUNCTION(FAudioParameter_GetObjectParam), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAudioParameter, "void set_ObjectParam(UObject@ Value)",
		asFUNCTION(FAudioParameter_SetObjectParam), asCALL_CDECL_OBJFIRST);

	// --- Instance methods ---

	REGISTER_METHOD(FAudioParameter,
		"void Merge(const FAudioParameter &in InParameter, bool bInTakeName, bool bInTakeType, bool bInMergeArrayTypes)",
		asMETHODPR(FAudioParameter, Merge, (const FAudioParameter&, bool, bool, bool), void), asCALL_THISCALL);

	// --- Static functions under FAudioParameter namespace ---

	Result = Engine->SetDefaultNamespace("FAudioParameter");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"FAudioParameter CreateDefaultArray(const FName &in InName, int InNum)",
		asFUNCTION(FAudioParameter_CreateDefaultArray),
		asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
