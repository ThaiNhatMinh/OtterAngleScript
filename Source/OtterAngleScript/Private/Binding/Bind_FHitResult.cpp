// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/EngineTypes.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FHitResult_DefaultConstruct(FHitResult* Memory)
	{
		new (Memory) FHitResult();
	}

	static void FHitResult_CopyConstruct(const FHitResult& Other, FHitResult* Memory)
	{
		new (Memory) FHitResult(Other);
	}

	static void FHitResult_Destruct(FHitResult* Memory)
	{
		Memory->~FHitResult();
	}

	static FHitResult& FHitResult_Assign(FHitResult& Value, const FHitResult& Other)
	{
		Value = Other;
		return Value;
	}

	static void FHitResult_Init(FHitResult& Value, const FVector& Start, const FVector& End)
	{
		Value.Init(Start, End);
	}

	static void FHitResult_Reset(FHitResult& Value, float InTime, bool bPreserveTraceData)
	{
		Value.Reset(InTime, bPreserveTraceData);
	}

	static bool FHitResult_IsValidBlockingHit(const FHitResult& Value)
	{
		return Value.IsValidBlockingHit();
	}

	static FString FHitResult_ToString(const FHitResult& Value)
	{
		return Value.ToString();
	}

	static bool FHitResult_GetBlockingHit(const FHitResult& Value)
	{
		return Value.bBlockingHit;
	}

	static void FHitResult_SetBlockingHit(FHitResult& Value, bool bBlockingHit)
	{
		Value.bBlockingHit = bBlockingHit;
	}

	static bool FHitResult_GetStartPenetrating(const FHitResult& Value)
	{
		return Value.bStartPenetrating;
	}

	static void FHitResult_SetStartPenetrating(FHitResult& Value, bool bStartPenetrating)
	{
		Value.bStartPenetrating = bStartPenetrating;
	}

	static float FHitResult_GetTime(const FHitResult& Value)
	{
		return Value.Time;
	}

	static void FHitResult_SetTime(FHitResult& Value, float Time)
	{
		Value.Time = Time;
	}

	static float FHitResult_GetDistance(const FHitResult& Value)
	{
		return Value.Distance;
	}

	static void FHitResult_SetDistance(FHitResult& Value, float Distance)
	{
		Value.Distance = Distance;
	}

	static float FHitResult_GetPenetrationDepth(const FHitResult& Value)
	{
		return Value.PenetrationDepth;
	}

	static void FHitResult_SetPenetrationDepth(FHitResult& Value, float PenetrationDepth)
	{
		Value.PenetrationDepth = PenetrationDepth;
	}

	static int32 FHitResult_GetFaceIndex(const FHitResult& Value)
	{
		return Value.FaceIndex;
	}

	static void FHitResult_SetFaceIndex(FHitResult& Value, int32 FaceIndex)
	{
		Value.FaceIndex = FaceIndex;
	}

	static int32 FHitResult_GetItem(const FHitResult& Value)
	{
		return Value.Item;
	}

	static void FHitResult_SetItem(FHitResult& Value, int32 Item)
	{
		Value.Item = Item;
	}

	static int32 FHitResult_GetElementIndex(const FHitResult& Value)
	{
		return static_cast<int32>(Value.ElementIndex);
	}

	static void FHitResult_SetElementIndex(FHitResult& Value, int32 ElementIndex)
	{
		Value.ElementIndex = static_cast<uint8>(ElementIndex);
	}

	static FVector FHitResult_GetLocation(const FHitResult& Value)
	{
		return Value.Location;
	}

	static void FHitResult_SetLocation(FHitResult& Value, const FVector& Location)
	{
		Value.Location = Location;
	}

	static FVector FHitResult_GetImpactPoint(const FHitResult& Value)
	{
		return Value.ImpactPoint;
	}

	static void FHitResult_SetImpactPoint(FHitResult& Value, const FVector& ImpactPoint)
	{
		Value.ImpactPoint = ImpactPoint;
	}

	static FVector FHitResult_GetNormal(const FHitResult& Value)
	{
		return Value.Normal;
	}

	static void FHitResult_SetNormal(FHitResult& Value, const FVector& Normal)
	{
		Value.Normal = Normal;
	}

	static FVector FHitResult_GetImpactNormal(const FHitResult& Value)
	{
		return Value.ImpactNormal;
	}

	static void FHitResult_SetImpactNormal(FHitResult& Value, const FVector& ImpactNormal)
	{
		Value.ImpactNormal = ImpactNormal;
	}

	static FVector FHitResult_GetTraceStart(const FHitResult& Value)
	{
		return Value.TraceStart;
	}

	static void FHitResult_SetTraceStart(FHitResult& Value, const FVector& TraceStart)
	{
		Value.TraceStart = TraceStart;
	}

	static FVector FHitResult_GetTraceEnd(const FHitResult& Value)
	{
		return Value.TraceEnd;
	}

	static void FHitResult_SetTraceEnd(FHitResult& Value, const FVector& TraceEnd)
	{
		Value.TraceEnd = TraceEnd;
	}
}

#define REGISTER_FHITRESULT_BEHAVIOUR(Behaviour, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectBehaviour("FHitResult", Behaviour, Declaration, Function, CallConv); \
	check(Result >= 0)

#define REGISTER_FHITRESULT_METHOD(Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod("FHitResult", Declaration, Function, CallConv); \
	check(Result >= 0)

void Bind_FHitResult(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FHitResult",
		sizeof(FHitResult),
		asOBJ_VALUE | asGetTypeTraits<FHitResult>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_FHITRESULT_BEHAVIOUR(asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FHitResult_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_FHITRESULT_BEHAVIOUR(asBEHAVE_CONSTRUCT, "void f(const FHitResult &in Other)", asFUNCTION(FHitResult_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_FHITRESULT_BEHAVIOUR(asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FHitResult_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_FHITRESULT_METHOD("FHitResult &opAssign(const FHitResult &in Other)", asFUNCTION(FHitResult_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void Init(const FVector &in Start, const FVector &in End)", asFUNCTION(FHitResult_Init), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void Reset(float InTime = 1.0f, bool bPreserveTraceData = false)", asFUNCTION(FHitResult_Reset), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("bool IsValidBlockingHit() const", asFUNCTION(FHitResult_IsValidBlockingHit), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FString ToString() const", asFUNCTION(FHitResult_ToString), asCALL_CDECL_OBJFIRST);

	REGISTER_FHITRESULT_METHOD("bool get_BlockingHit() const property", asFUNCTION(FHitResult_GetBlockingHit), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_BlockingHit(bool Value) property", asFUNCTION(FHitResult_SetBlockingHit), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("bool get_StartPenetrating() const property", asFUNCTION(FHitResult_GetStartPenetrating), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_StartPenetrating(bool Value) property", asFUNCTION(FHitResult_SetStartPenetrating), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("float get_Time() const property", asFUNCTION(FHitResult_GetTime), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_Time(float Value) property", asFUNCTION(FHitResult_SetTime), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("float get_Distance() const property", asFUNCTION(FHitResult_GetDistance), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_Distance(float Value) property", asFUNCTION(FHitResult_SetDistance), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("float get_PenetrationDepth() const property", asFUNCTION(FHitResult_GetPenetrationDepth), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_PenetrationDepth(float Value) property", asFUNCTION(FHitResult_SetPenetrationDepth), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("int get_FaceIndex() const property", asFUNCTION(FHitResult_GetFaceIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_FaceIndex(int Value) property", asFUNCTION(FHitResult_SetFaceIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("int get_Item() const property", asFUNCTION(FHitResult_GetItem), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_Item(int Value) property", asFUNCTION(FHitResult_SetItem), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("int get_ElementIndex() const property", asFUNCTION(FHitResult_GetElementIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_ElementIndex(int Value) property", asFUNCTION(FHitResult_SetElementIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FVector get_Location() const property", asFUNCTION(FHitResult_GetLocation), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_Location(const FVector &in Value) property", asFUNCTION(FHitResult_SetLocation), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FVector get_ImpactPoint() const property", asFUNCTION(FHitResult_GetImpactPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_ImpactPoint(const FVector &in Value) property", asFUNCTION(FHitResult_SetImpactPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FVector get_Normal() const property", asFUNCTION(FHitResult_GetNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_Normal(const FVector &in Value) property", asFUNCTION(FHitResult_SetNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FVector get_ImpactNormal() const property", asFUNCTION(FHitResult_GetImpactNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_ImpactNormal(const FVector &in Value) property", asFUNCTION(FHitResult_SetImpactNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FVector get_TraceStart() const property", asFUNCTION(FHitResult_GetTraceStart), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_TraceStart(const FVector &in Value) property", asFUNCTION(FHitResult_SetTraceStart), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("FVector get_TraceEnd() const property", asFUNCTION(FHitResult_GetTraceEnd), asCALL_CDECL_OBJFIRST);
	REGISTER_FHITRESULT_METHOD("void set_TraceEnd(const FVector &in Value) property", asFUNCTION(FHitResult_SetTraceEnd), asCALL_CDECL_OBJFIRST);
}

#undef REGISTER_FHITRESULT_METHOD
#undef REGISTER_FHITRESULT_BEHAVIOUR
