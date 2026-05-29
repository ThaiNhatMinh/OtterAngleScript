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

	FRotator FRotator_MultiplyReverse(const FRotator& Value, double Scale)
	{
		return Scale * Value;
	}

}

void Bind_FRotator(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FRotator_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(const FRotator &in Other)", asFUNCTION(FRotator_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(double Value)", asFUNCTION(FRotator_ConstructScalar), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(double Pitch, double Yaw, double Roll)", asFUNCTION(FRotator_ConstructPitchYawRoll), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FRotator_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_CONSTRUCT, "void f(const FQuat &in Quat)", asFUNCTION(FRotator_ConstructQuat), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRotator, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FRotator_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FRotator, "FRotator &opAssign(const FRotator &in Other)", asMETHODPR(FRotator, operator=, (const FRotator&), FRotator&), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator opAdd(const FRotator &in Other) const", asMETHODPR(FRotator, operator+, (const FRotator&) const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator opSub(const FRotator &in Other) const", asMETHODPR(FRotator, operator-, (const FRotator&) const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator opMul(double Scale) const", asMETHODPR(FRotator, operator*, (double) const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator opMul_r(double Scale) const", asFUNCTION(FRotator_MultiplyReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRotator, "FRotator opAddAssign(const FRotator &in Other)", asMETHODPR(FRotator, operator+=, (const FRotator&), FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator opSubAssign(const FRotator &in Other)", asMETHODPR(FRotator, operator-=, (const FRotator&), FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator opMulAssign(double Scale)", asMETHODPR(FRotator, operator*=, (double), FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "bool opEquals(const FRotator &in Other) const", asMETHODPR(FRotator, operator==, (const FRotator&) const, bool), asCALL_THISCALL);

	REGISTER_METHOD(FRotator, "bool IsNearlyZero(double Tolerance = 0.0001) const", asMETHODPR(FRotator, IsNearlyZero, (double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "bool IsZero() const", asMETHODPR(FRotator, IsZero, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "bool Equals(const FRotator &in Other, double Tolerance = 0.0001) const", asMETHODPR(FRotator, Equals, (const FRotator&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "bool EqualsOrientation(const FRotator &in Other, double Tolerance = 0.0001) const", asMETHODPR(FRotator, EqualsOrientation, (const FRotator&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator Add(double DeltaPitch, double DeltaYaw, double DeltaRoll)", asMETHODPR(FRotator, Add, (double, double, double), FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator GetInverse() const", asMETHODPR(FRotator, GetInverse, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator GridSnap(const FRotator &in RotGrid) const", asMETHODPR(FRotator, GridSnap, (const FRotator&) const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FVector Vector() const", asMETHODPR(FRotator, Vector, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FQuat Quaternion() const", asMETHODPR(FRotator, Quaternion, () const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FVector Euler() const", asMETHODPR(FRotator, Euler, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FVector RotateVector(const FVector &in Vector) const", asMETHODPR(FRotator, RotateVector, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FVector UnrotateVector(const FVector &in Vector) const", asMETHODPR(FRotator, UnrotateVector, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator Clamp() const", asMETHODPR(FRotator, Clamp, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator GetNormalized() const", asMETHODPR(FRotator, GetNormalized, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator GetDenormalized() const", asMETHODPR(FRotator, GetDenormalized, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "double GetComponentForAxis(EAxisType Axis) const", asMETHODPR(FRotator, GetComponentForAxis, (EAxis::Type) const, double), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "void SetComponentForAxis(EAxisType Axis, double Component)", asMETHODPR(FRotator, SetComponentForAxis, (EAxis::Type, double), void), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "void Normalize()", asMETHODPR(FRotator, Normalize, (), void), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "void GetWindingAndRemainder(FRotator &out Winding, FRotator &out Remainder) const", asMETHODPR(FRotator, GetWindingAndRemainder, (FRotator&, FRotator&) const, void), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "double GetManhattanDistance(const FRotator &in Other) const", asMETHODPR(FRotator, GetManhattanDistance, (const FRotator&) const, double), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FRotator GetEquivalentRotator() const", asMETHODPR(FRotator, GetEquivalentRotator, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "void SetClosestToMe(FRotator &out MakeClosest) const", asMETHODPR(FRotator, SetClosestToMe, (FRotator&) const, void), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FString ToString() const", asMETHODPR(FRotator, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "FString ToCompactString() const", asMETHODPR(FRotator, ToCompactString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "bool InitFromString(const FString &in Source)", asMETHODPR(FRotator, InitFromString, (const FString&), bool), asCALL_THISCALL);
	REGISTER_METHOD(FRotator, "bool ContainsNaN() const", asMETHODPR(FRotator, ContainsNaN, () const, bool), asCALL_THISCALL);

	REGISTER_PROPERTY(FRotator, "double Pitch", Pitch);
	REGISTER_PROPERTY(FRotator, "double Yaw", Yaw);
	REGISTER_PROPERTY(FRotator, "double Roll", Roll);

	Result = Engine->SetDefaultNamespace("FRotator");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double ClampAxis(double Angle)", asFUNCTION(FRotator::ClampAxis), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double NormalizeAxis(double Angle)", asFUNCTION(FRotator::NormalizeAxis), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("uint8 CompressAxisToByte(double Angle)", asFUNCTION(FRotator::CompressAxisToByte), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DecompressAxisFromByte(uint8 Angle)", asFUNCTION(FRotator::DecompressAxisFromByte), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("uint16 CompressAxisToShort(double Angle)", asFUNCTION(FRotator::CompressAxisToShort), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DecompressAxisFromShort(uint16 Angle)", asFUNCTION(FRotator::DecompressAxisFromShort), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FRotator MakeFromEuler(const FVector &in Euler)", asFUNCTION(FRotator::MakeFromEuler), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
