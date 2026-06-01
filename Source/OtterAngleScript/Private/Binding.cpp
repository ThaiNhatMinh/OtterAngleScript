#include "Binding.h"

#ifdef _MSC_VER
#pragma warning(disable:4191 4996)
#endif

#include "AlphaBlend.h"
#include "Math/Color.h"
#include "Math/Box2D.h"
#include "Math/Vector2D.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Misc/FrameTime.h"
#include "Misc/FrameRate.h"
#include "AI/Navigation/NavAgentSelector.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AudioParameter.h"
#include "Curves/RichCurve.h"
#include "angelscript.h"

void Declare_Types(asIScriptEngine* Engine)
{
    int Result = Engine->RegisterObjectType("UObject", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
    check(Result >= 0);
	
	Result = Engine->RegisterObjectType("UScriptStruct", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("UClass", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("UPhysicalMaterial", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FQuat", sizeof(FQuat), asOBJ_VALUE | asGetTypeTraits<FQuat>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

    Result = Engine->RegisterObjectType("FRotator", sizeof(FRotator), asOBJ_VALUE | asGetTypeTraits<FRotator>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
    check(Result >= 0);

	Result = Engine->RegisterObjectType("FVector4f", sizeof(FVector4f), asOBJ_VALUE | asGetTypeTraits<FVector4f>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FVector2D", sizeof(FVector2D), asOBJ_VALUE | asGetTypeTraits<FVector2D>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FBox2D", sizeof(FBox2D), asOBJ_VALUE | asGetTypeTraits<FBox2D>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
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

	Result = Engine->RegisterObjectType("FAlphaBlendArgs", sizeof(FAlphaBlendArgs), asOBJ_VALUE | asGetTypeTraits<FAlphaBlendArgs>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FKeyHandle", sizeof(FKeyHandle), asOBJ_VALUE | asGetTypeTraits<FKeyHandle>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FRichCurve", sizeof(FRichCurve), asOBJ_VALUE | asGetTypeTraits<FRichCurve>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FPhysicalMaterialStrength", sizeof(FPhysicalMaterialStrength), asOBJ_VALUE | asOBJ_POD);
	check(Result >= 0);

	Result = Engine->RegisterObjectType("FPhysicalMaterialDamageModifier", sizeof(FPhysicalMaterialDamageModifier), asOBJ_VALUE | asOBJ_POD);
	check(Result >= 0);

	Result = Engine->RegisterObjectType(
		"FSoftObjectPath",
		sizeof(FSoftObjectPath),
		asOBJ_VALUE | asGetTypeTraits<FSoftObjectPath>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType(
		"FText",
		sizeof(FText),
		asOBJ_VALUE | asGetTypeTraits<FText>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Result = Engine->RegisterObjectType(
		"FMatrix",
		sizeof(FMatrix),
		asOBJ_VALUE | asGetTypeTraits<FMatrix>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	Declare_TArray(Engine);
	Declare_TSet(Engine);
	Declare_TMap(Engine);
	Declare_TSoftObjectPtr(Engine);
	Declare_TWeakObjectPtr(Engine);
	Declare_TSubclassOf(Engine);
	Declare_FName(Engine);
}