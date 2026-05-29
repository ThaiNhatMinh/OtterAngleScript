// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Quat.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FQuat_DefaultConstruct(FQuat* Memory)
	{
		new (Memory) FQuat();
	}

	void FQuat_ConstructForceInit(EForceInit ForceInitValue, FQuat* Memory)
	{
		new (Memory) FQuat(ForceInitValue);
	}

	void FQuat_CopyConstruct(const FQuat& Other, FQuat* Memory)
	{
		new (Memory) FQuat(Other);
	}

	void FQuat_ConstructXYZW(double X, double Y, double Z, double W, FQuat* Memory)
	{
		new (Memory) FQuat(X, Y, Z, W);
	}

	void FQuat_ConstructRotator(const FRotator& Rotator, FQuat* Memory)
	{
		new (Memory) FQuat(Rotator);
	}

	void FQuat_ConstructAxisAngle(const FVector& Axis, double AngleRad, FQuat* Memory)
	{
		new (Memory) FQuat(Axis, AngleRad);
	}

	void FQuat_Destruct(FQuat* Memory)
	{
		Memory->~FQuat();
	}

	FQuat FQuat_MultiplyScaleReverse(const FQuat& Value, double Scale)
	{
		return Value * Scale;
	}

}

void Bind_FQuat(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FQuat_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FQuat_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(const FQuat &in Other)", asFUNCTION(FQuat_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(double X, double Y, double Z, double W)", asFUNCTION(FQuat_ConstructXYZW), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(const FRotator &in Rotator)", asFUNCTION(FQuat_ConstructRotator), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(const FVector &in Axis, double AngleRad)", asFUNCTION(FQuat_ConstructAxisAngle), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FQuat_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FQuat, "FQuat &opAssign(const FQuat &in Other)", asMETHODPR(FQuat, operator=, (const FQuat&), FQuat&), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opAdd(const FQuat &in Other) const", asMETHODPR(FQuat, operator+, (const FQuat&) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opSub(const FQuat &in Other) const", asMETHODPR(FQuat, operator-, (const FQuat&) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opMul(const FQuat &in Other) const", asMETHODPR(FQuat, operator*, (const FQuat&) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector opMul(const FVector &in Vector) const", asMETHODPR(FQuat, operator*, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opMul(double Scale) const", asMETHODPR(FQuat, operator*, (double) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opMul_r(double Scale) const", asFUNCTION(FQuat_MultiplyScaleReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opDiv(double Scale) const", asMETHODPR(FQuat, operator/, (double) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opAddAssign(const FQuat &in Other)", asMETHODPR(FQuat, operator+=, (const FQuat&), FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opSubAssign(const FQuat &in Other)", asMETHODPR(FQuat, operator-=, (const FQuat&), FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opMulAssign(const FQuat &in Other)", asMETHODPR(FQuat, operator*=, (const FQuat&), FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opMulAssign(double Scale)", asMETHODPR(FQuat, operator*=, (double), FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat opDivAssign(double Scale)", asMETHODPR(FQuat, operator/=, (double), FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "bool opEquals(const FQuat &in Other) const", asMETHODPR(FQuat, operator==, (const FQuat&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "double opOr(const FQuat &in Other) const", asMETHODPR(FQuat, operator|, (const FQuat&) const, double), asCALL_THISCALL);

	REGISTER_METHOD(FQuat, "bool Equals(const FQuat &in Other, double Tolerance = 0.0001) const", asMETHODPR(FQuat, Equals, (const FQuat&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "bool IsIdentity(double Tolerance = 0.00000001) const", asMETHODPR(FQuat, IsIdentity, (double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector Euler() const", asMETHODPR(FQuat, Euler, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "void Normalize(double Tolerance = 0.00000001)", asMETHODPR(FQuat, Normalize, (double), void), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat GetNormalized(double Tolerance = 0.00000001) const", asMETHODPR(FQuat, GetNormalized, (double) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "bool IsNormalized() const", asMETHODPR(FQuat, IsNormalized, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "double Size() const", asMETHODPR(FQuat, Size, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "double SizeSquared() const", asMETHODPR(FQuat, SizeSquared, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "double GetAngle() const", asMETHODPR(FQuat, GetAngle, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "void ToAxisAndAngle(FVector &out Axis, double &out Angle) const", asMETHODPR(FQuat, ToAxisAndAngle, (FVector&, double&) const, void), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "void ToSwingTwist(const FVector &in InTwistAxis, FQuat &out OutSwing, FQuat &out OutTwist) const", asMETHODPR(FQuat, ToSwingTwist, (const FVector&, FQuat&, FQuat&) const, void), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector RotateVector(const FVector &in Vector) const", asMETHODPR(FQuat, RotateVector, (FVector) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector UnrotateVector(const FVector &in Vector) const", asMETHODPR(FQuat, UnrotateVector, (FVector) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat Log() const", asMETHODPR(FQuat, Log, () const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat Exp() const", asMETHODPR(FQuat, Exp, () const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FQuat Inverse() const", asMETHODPR(FQuat, Inverse, () const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "void EnforceShortestArcWith(const FQuat &in OtherQuat)", asMETHODPR(FQuat, EnforceShortestArcWith, (const FQuat&), void), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetAxisX() const", asMETHODPR(FQuat, GetAxisX, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetAxisY() const", asMETHODPR(FQuat, GetAxisY, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetAxisZ() const", asMETHODPR(FQuat, GetAxisZ, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetForwardVector() const", asMETHODPR(FQuat, GetForwardVector, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetRightVector() const", asMETHODPR(FQuat, GetRightVector, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetUpVector() const", asMETHODPR(FQuat, GetUpVector, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector Vector() const", asMETHODPR(FQuat, Vector, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FRotator Rotator() const", asMETHODPR(FQuat, Rotator, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FVector GetRotationAxis() const", asMETHODPR(FQuat, GetRotationAxis, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "double AngularDistance(const FQuat &in Other) const", asMETHODPR(FQuat, AngularDistance, (const FQuat&) const, double), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "bool ContainsNaN() const", asMETHODPR(FQuat, ContainsNaN, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "FString ToString() const", asMETHODPR(FQuat, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FQuat, "bool InitFromString(const FString &in Source)", asMETHODPR(FQuat, InitFromString, (const FString&), bool), asCALL_THISCALL);

	REGISTER_PROPERTY(FQuat, "double X", X);
	REGISTER_PROPERTY(FQuat, "double Y", Y);
	REGISTER_PROPERTY(FQuat, "double Z", Z);
	REGISTER_PROPERTY(FQuat, "double W", W);

	Result = Engine->SetDefaultNamespace("FQuat");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat MakeFromEuler(const FVector &in Euler)", asFUNCTION(FQuat::MakeFromEuler), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FindBetween(const FVector &in Vector1, const FVector &in Vector2)", asFUNCTION(FQuat::FindBetween), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FindBetweenNormals(const FVector &in Normal1, const FVector &in Normal2)", asFUNCTION(FQuat::FindBetweenNormals), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FindBetweenVectors(const FVector &in Vector1, const FVector &in Vector2)", asFUNCTION(FQuat::FindBetweenVectors), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Error(const FQuat &in A, const FQuat &in B)", asFUNCTION(FQuat::Error), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double ErrorAutoNormalize(const FQuat &in A, const FQuat &in B)", asFUNCTION(FQuat::ErrorAutoNormalize), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FastLerp(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat::FastLerp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FastBilerp(const FQuat &in P00, const FQuat &in P10, const FQuat &in P01, const FQuat &in P11, double FracX, double FracY)", asFUNCTION(FQuat::FastBilerp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat Slerp_NotNormalized(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat::Slerp_NotNormalized), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat Slerp(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat::Slerp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat SlerpFullPath_NotNormalized(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat::SlerpFullPath_NotNormalized), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat SlerpFullPath(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat::SlerpFullPath), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat Squad(const FQuat &in Quat1, const FQuat &in Tangent1, const FQuat &in Quat2, const FQuat &in Tangent2, double Alpha)", asFUNCTION(FQuat::Squad), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat SquadFullPath(const FQuat &in Quat1, const FQuat &in Tangent1, const FQuat &in Quat2, const FQuat &in Tangent2, double Alpha)", asFUNCTION(FQuat::SquadFullPath), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("void CalcTangents(const FQuat &in PrevP, const FQuat &in P, const FQuat &in NextP, double Tension, FQuat &out OutTan)", asFUNCTION(FQuat::CalcTangents), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
