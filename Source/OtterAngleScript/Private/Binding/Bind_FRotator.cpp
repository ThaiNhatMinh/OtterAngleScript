// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/CoreMiscDefines.h"
#include "Math/Quat.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FRotator_DefaultConstruct(FRotator* Memory)
	{
		new (Memory) FRotator();
	}

	void FRotator_CopyConstruct(const FRotator& Other, FRotator* Memory)
	{
		new (Memory) FRotator(Other);
	}

	void FRotator_ConstructScalar(double Value, FRotator* Memory)
	{
		new (Memory) FRotator(Value);
	}

	void FRotator_ConstructPitchYawRoll(double Pitch, double Yaw, double Roll, FRotator* Memory)
	{
		new (Memory) FRotator(Pitch, Yaw, Roll);
	}

	void FRotator_ConstructForceInit(EForceInit ForceInitValue, FRotator* Memory)
	{
		new (Memory) FRotator(ForceInitValue);
	}

	void FRotator_ConstructQuat(const FQuat& Quat, FRotator* Memory)
	{
		new (Memory) FRotator(Quat);
	}

	void FRotator_Destruct(FRotator* Memory)
	{
		Memory->~FRotator();
	}

	FRotator& FRotator_Assign(FRotator& Value, const FRotator& Other)
	{
		Value = Other;
		return Value;
	}

	FRotator FRotator_Add(const FRotator& Value, const FRotator& Other)
	{
		return Value + Other;
	}

	FRotator FRotator_Subtract(const FRotator& Value, const FRotator& Other)
	{
		return Value - Other;
	}

	FRotator FRotator_Multiply(const FRotator& Value, double Scale)
	{
		return Value * Scale;
	}

	FRotator FRotator_MultiplyReverse(const FRotator& Value, double Scale)
	{
		return Scale * Value;
	}

	FRotator FRotator_AddAssign(FRotator& Value, const FRotator& Other)
	{
		return Value += Other;
	}

	FRotator FRotator_SubtractAssign(FRotator& Value, const FRotator& Other)
	{
		return Value -= Other;
	}

	FRotator FRotator_MultiplyAssign(FRotator& Value, double Scale)
	{
		return Value *= Scale;
	}

	bool FRotator_EqualsExact(const FRotator& Value, const FRotator& Other)
	{
		return Value == Other;
	}

	bool FRotator_Equals(const FRotator& Value, const FRotator& Other, double Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	bool FRotator_EqualsOrientation(const FRotator& Value, const FRotator& Other, double Tolerance)
	{
		return Value.EqualsOrientation(Other, Tolerance);
	}

	bool FRotator_IsNearlyZero(const FRotator& Value, double Tolerance)
	{
		return Value.IsNearlyZero(Tolerance);
	}

	bool FRotator_IsZero(const FRotator& Value)
	{
		return Value.IsZero();
	}

	FRotator FRotator_AddDelta(FRotator& Value, double DeltaPitch, double DeltaYaw, double DeltaRoll)
	{
		return Value.Add(DeltaPitch, DeltaYaw, DeltaRoll);
	}

	FRotator FRotator_GetInverse(const FRotator& Value)
	{
		return Value.GetInverse();
	}

	FRotator FRotator_GridSnap(const FRotator& Value, const FRotator& Grid)
	{
		return Value.GridSnap(Grid);
	}

	FVector FRotator_Vector(const FRotator& Value)
	{
		return Value.Vector();
	}

	FQuat FRotator_Quaternion(const FRotator& Value)
	{
		return Value.Quaternion();
	}

	FVector FRotator_Euler(const FRotator& Value)
	{
		return Value.Euler();
	}

	FVector FRotator_RotateVector(const FRotator& Value, const FVector& Vector)
	{
		return Value.RotateVector(Vector);
	}

	FVector FRotator_UnrotateVector(const FRotator& Value, const FVector& Vector)
	{
		return Value.UnrotateVector(Vector);
	}

	FRotator FRotator_Clamp(const FRotator& Value)
	{
		return Value.Clamp();
	}

	FRotator FRotator_GetNormalized(const FRotator& Value)
	{
		return Value.GetNormalized();
	}

	FRotator FRotator_GetDenormalized(const FRotator& Value)
	{
		return Value.GetDenormalized();
	}

	double FRotator_GetComponentForAxis(const FRotator& Value, EAxis::Type Axis)
	{
		return Value.GetComponentForAxis(Axis);
	}

	void FRotator_SetComponentForAxis(FRotator& Value, EAxis::Type Axis, double Component)
	{
		Value.SetComponentForAxis(Axis, Component);
	}

	void FRotator_Normalize(FRotator& Value)
	{
		Value.Normalize();
	}

	void FRotator_GetWindingAndRemainder(const FRotator& Value, FRotator& Winding, FRotator& Remainder)
	{
		Value.GetWindingAndRemainder(Winding, Remainder);
	}

	double FRotator_GetManhattanDistance(const FRotator& Value, const FRotator& Other)
	{
		return Value.GetManhattanDistance(Other);
	}

	FRotator FRotator_GetEquivalentRotator(const FRotator& Value)
	{
		return Value.GetEquivalentRotator();
	}

	void FRotator_SetClosestToMe(const FRotator& Value, FRotator& MakeClosest)
	{
		Value.SetClosestToMe(MakeClosest);
	}

	FString FRotator_ToString(const FRotator& Value)
	{
		return Value.ToString();
	}

	FString FRotator_ToCompactString(const FRotator& Value)
	{
		return Value.ToCompactString();
	}

	bool FRotator_InitFromString(FRotator& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	bool FRotator_ContainsNaN(const FRotator& Value)
	{
		return Value.ContainsNaN();
	}

	double FRotator_ClampAxis(double Angle)
	{
		return FRotator::ClampAxis(Angle);
	}

	double FRotator_NormalizeAxis(double Angle)
	{
		return FRotator::NormalizeAxis(Angle);
	}

	uint8 FRotator_CompressAxisToByte(double Angle)
	{
		return FRotator::CompressAxisToByte(Angle);
	}

	double FRotator_DecompressAxisFromByte(uint8 Angle)
	{
		return FRotator::DecompressAxisFromByte(Angle);
	}

	uint16 FRotator_CompressAxisToShort(double Angle)
	{
		return FRotator::CompressAxisToShort(Angle);
	}

	double FRotator_DecompressAxisFromShort(uint16 Angle)
	{
		return FRotator::DecompressAxisFromShort(Angle);
	}

	FRotator FRotator_MakeFromEuler(const FVector& Euler)
	{
		return FRotator::MakeFromEuler(Euler);
	}
}

void Bind_FRotator(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FRotator",
		sizeof(FRotator),
		asOBJ_VALUE | asGetTypeTraits<FRotator>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FRotator_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(const FRotator &in Other)", asFUNCTION(FRotator_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(double Value)", asFUNCTION(FRotator_ConstructScalar), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(double Pitch, double Yaw, double Roll)", asFUNCTION(FRotator_ConstructPitchYawRoll), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FRotator_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(const FQuat &in Quat)", asFUNCTION(FRotator_ConstructQuat), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FRotator_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FRotator, "FRotator &opAssign(const FRotator &in Other)", asFUNCTION(FRotator_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opAdd(const FRotator &in Other) const", asFUNCTION(FRotator_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opSub(const FRotator &in Other) const", asFUNCTION(FRotator_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opMul(double Scale) const", asFUNCTION(FRotator_Multiply), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opMul_r(double Scale) const", asFUNCTION(FRotator_MultiplyReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opAddAssign(const FRotator &in Other)", asFUNCTION(FRotator_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opSubAssign(const FRotator &in Other)", asFUNCTION(FRotator_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opMulAssign(double Scale)", asFUNCTION(FRotator_MultiplyAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "bool opEquals(const FRotator &in Other) const", asFUNCTION(FRotator_EqualsExact), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FRotator, "bool IsNearlyZero(double Tolerance = 0.0001) const", asFUNCTION(FRotator_IsNearlyZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "bool IsZero() const", asFUNCTION(FRotator_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "bool Equals(const FRotator &in Other, double Tolerance = 0.0001) const", asFUNCTION(FRotator_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "bool EqualsOrientation(const FRotator &in Other, double Tolerance = 0.0001) const", asFUNCTION(FRotator_EqualsOrientation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator Add(double DeltaPitch, double DeltaYaw, double DeltaRoll)", asFUNCTION(FRotator_AddDelta), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator GetInverse() const", asFUNCTION(FRotator_GetInverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator GridSnap(const FRotator &in RotGrid) const", asFUNCTION(FRotator_GridSnap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FVector Vector() const", asFUNCTION(FRotator_Vector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FQuat Quaternion() const", asFUNCTION(FRotator_Quaternion), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FVector Euler() const", asFUNCTION(FRotator_Euler), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FVector RotateVector(const FVector &in Vector) const", asFUNCTION(FRotator_RotateVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FVector UnrotateVector(const FVector &in Vector) const", asFUNCTION(FRotator_UnrotateVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator Clamp() const", asFUNCTION(FRotator_Clamp), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator GetNormalized() const", asFUNCTION(FRotator_GetNormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator GetDenormalized() const", asFUNCTION(FRotator_GetDenormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "double GetComponentForAxis(EAxisType Axis) const", asFUNCTION(FRotator_GetComponentForAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "void SetComponentForAxis(EAxisType Axis, double Component)", asFUNCTION(FRotator_SetComponentForAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "void Normalize()", asFUNCTION(FRotator_Normalize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "void GetWindingAndRemainder(FRotator &out Winding, FRotator &out Remainder) const", asFUNCTION(FRotator_GetWindingAndRemainder), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "double GetManhattanDistance(const FRotator &in Other) const", asFUNCTION(FRotator_GetManhattanDistance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator GetEquivalentRotator() const", asFUNCTION(FRotator_GetEquivalentRotator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "void SetClosestToMe(FRotator &inout MakeClosest) const", asFUNCTION(FRotator_SetClosestToMe), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FString ToString() const", asFUNCTION(FRotator_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FString ToCompactString() const", asFUNCTION(FRotator_ToCompactString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "bool InitFromString(const FString &in Source)", asFUNCTION(FRotator_InitFromString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "bool ContainsNaN() const", asFUNCTION(FRotator_ContainsNaN), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FRotator, "double Pitch", Pitch);
	REGISTER_PROPERTY(FRotator, "double Yaw", Yaw);
	REGISTER_PROPERTY(FRotator, "double Roll", Roll);

	Result = Engine->SetDefaultNamespace("FRotator");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double ClampAxis(double Angle)", asFUNCTION(FRotator_ClampAxis), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double NormalizeAxis(double Angle)", asFUNCTION(FRotator_NormalizeAxis), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("uint8 CompressAxisToByte(double Angle)", asFUNCTION(FRotator_CompressAxisToByte), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DecompressAxisFromByte(uint8 Angle)", asFUNCTION(FRotator_DecompressAxisFromByte), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("uint16 CompressAxisToShort(double Angle)", asFUNCTION(FRotator_CompressAxisToShort), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DecompressAxisFromShort(uint16 Angle)", asFUNCTION(FRotator_DecompressAxisFromShort), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FRotator MakeFromEuler(const FVector &in Euler)", asFUNCTION(FRotator_MakeFromEuler), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
