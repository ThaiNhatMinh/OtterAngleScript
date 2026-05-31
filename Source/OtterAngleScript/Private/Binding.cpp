#include "Binding.h"

#ifdef _MSC_VER
#pragma warning(disable:4191 4996)
#endif

#include "AlphaBlend.h"
#include "Misc/FrameTime.h"
#include "Misc/FrameRate.h"
#include "AI/Navigation/NavAgentSelector.h"
#include "angelscript.h"

void Declare_Types(asIScriptEngine* Engine)
{
    int Result = Engine->RegisterObjectType("UObject", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
    check(Result >= 0);

	Result = Engine->RegisterObjectType("UClass", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FQuat", sizeof(FQuat), asOBJ_VALUE | asGetTypeTraits<FQuat>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

    Result = Engine->RegisterObjectType("FRotator", sizeof(FRotator), asOBJ_VALUE | asGetTypeTraits<FRotator>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
    check(Result >= 0);

	Result = Engine->RegisterObjectType("FVector", sizeof(FVector), asOBJ_VALUE | asGetTypeTraits<FVector>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);
	
	Result = Engine->RegisterObjectType("FName", sizeof(FName), asOBJ_VALUE | asGetTypeTraits<FName>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FColor", sizeof(FColor), asOBJ_VALUE | asGetTypeTraits<FColor>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FSoftClassPath", sizeof(FSoftClassPath), asOBJ_VALUE | asGetTypeTraits<FSoftClassPath>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FTransform", sizeof(FTransform), asOBJ_VALUE | asGetTypeTraits<FTransform>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FString", sizeof(FString), asOBJ_VALUE | asGetTypeTraits<FString>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FFrameNumber", sizeof(FFrameNumber), asOBJ_VALUE | asGetTypeTraits<FFrameNumber>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FFrameTime", sizeof(FFrameTime), asOBJ_VALUE | asGetTypeTraits<FFrameTime>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FFrameRate", sizeof(FFrameRate), asOBJ_VALUE | asGetTypeTraits<FFrameRate>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FNavAgentSelector", sizeof(FNavAgentSelector), asOBJ_VALUE | asGetTypeTraits<FNavAgentSelector>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FAlphaBlend", sizeof(FAlphaBlend), asOBJ_VALUE | asGetTypeTraits<FAlphaBlend>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FAlphaBlendArgs", sizeof(FAlphaBlendArgs), asOBJ_VALUE | asGetTypeTraits<FAlphaBlendArgs>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Declare_TArray(Engine);
	Declare_TSoftObjectPtr(Engine);
	Declare_TWeakObjectPtr(Engine);
}