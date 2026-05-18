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

	FQuat& FQuat_Assign(FQuat& Value, const FQuat& Other)
	{
		Value = Other;
		return Value;
	}

	FQuat FQuat_Add(const FQuat& Value, const FQuat& Other)
	{
		return Value + Other;
	}

	FQuat FQuat_Subtract(const FQuat& Value, const FQuat& Other)
	{
		return Value - Other;
	}

	FQuat FQuat_MultiplyQuat(const FQuat& Value, const FQuat& Other)
	{
		return Value * Other;
	}

	FVector FQuat_MultiplyVector(const FQuat& Value, const FVector& Vector)
	{
		return Value * Vector;
	}

	FQuat FQuat_MultiplyScale(const FQuat& Value, double Scale)
	{
		return Value * Scale;
	}

	FQuat FQuat_MultiplyScaleReverse(const FQuat& Value, double Scale)
	{
		return Value * Scale;
	}

	FQuat FQuat_DivideScale(const FQuat& Value, double Scale)
	{
		return Value / Scale;
	}

	FQuat FQuat_AddAssign(FQuat& Value, const FQuat& Other)
	{
		return Value += Other;
	}

	FQuat FQuat_SubtractAssign(FQuat& Value, const FQuat& Other)
	{
		return Value -= Other;
	}

	FQuat FQuat_MultiplyAssignQuat(FQuat& Value, const FQuat& Other)
	{
		return Value *= Other;
	}

	FQuat FQuat_MultiplyAssignScale(FQuat& Value, double Scale)
	{
		return Value *= Scale;
	}

	FQuat FQuat_DivideAssignScale(FQuat& Value, double Scale)
	{
		return Value /= Scale;
	}

	bool FQuat_EqualsExact(const FQuat& Value, const FQuat& Other)
	{
		return Value == Other;
	}

	double FQuat_DotOperator(const FQuat& Value, const FQuat& Other)
	{
		return Value | Other;
	}

	bool FQuat_Equals(const FQuat& Value, const FQuat& Other, double Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	bool FQuat_IsIdentity(const FQuat& Value, double Tolerance)
	{
		return Value.IsIdentity(Tolerance);
	}

	FVector FQuat_Euler(const FQuat& Value)
	{
		return Value.Euler();
	}

	void FQuat_Normalize(FQuat& Value, double Tolerance)
	{
		Value.Normalize(Tolerance);
	}

	FQuat FQuat_GetNormalized(const FQuat& Value, double Tolerance)
	{
		return Value.GetNormalized(Tolerance);
	}

	bool FQuat_IsNormalized(const FQuat& Value)
	{
		return Value.IsNormalized();
	}

	double FQuat_Size(const FQuat& Value)
	{
		return Value.Size();
	}

	double FQuat_SizeSquared(const FQuat& Value)
	{
		return Value.SizeSquared();
	}

	double FQuat_GetAngle(const FQuat& Value)
	{
		return Value.GetAngle();
	}

	void FQuat_ToAxisAndAngle(const FQuat& Value, FVector& Axis, double& Angle)
	{
		Value.ToAxisAndAngle(Axis, Angle);
	}

	void FQuat_ToSwingTwist(const FQuat& Value, const FVector& InTwistAxis, FQuat& OutSwing, FQuat& OutTwist)
	{
		Value.ToSwingTwist(InTwistAxis, OutSwing, OutTwist);
	}

	FVector FQuat_RotateVector(const FQuat& Value, const FVector& Vector)
	{
		return Value.RotateVector(Vector);
	}

	FVector FQuat_UnrotateVector(const FQuat& Value, const FVector& Vector)
	{
		return Value.UnrotateVector(Vector);
	}

	FQuat FQuat_Log(const FQuat& Value)
	{
		return Value.Log();
	}

	FQuat FQuat_Exp(const FQuat& Value)
	{
		return Value.Exp();
	}

	FQuat FQuat_Inverse(const FQuat& Value)
	{
		return Value.Inverse();
	}

	void FQuat_EnforceShortestArcWith(FQuat& Value, const FQuat& OtherQuat)
	{
		Value.EnforceShortestArcWith(OtherQuat);
	}

	FVector FQuat_GetAxisX(const FQuat& Value)
	{
		return Value.GetAxisX();
	}

	FVector FQuat_GetAxisY(const FQuat& Value)
	{
		return Value.GetAxisY();
	}

	FVector FQuat_GetAxisZ(const FQuat& Value)
	{
		return Value.GetAxisZ();
	}

	FVector FQuat_GetForwardVector(const FQuat& Value)
	{
		return Value.GetForwardVector();
	}

	FVector FQuat_GetRightVector(const FQuat& Value)
	{
		return Value.GetRightVector();
	}

	FVector FQuat_GetUpVector(const FQuat& Value)
	{
		return Value.GetUpVector();
	}

	FVector FQuat_Vector(const FQuat& Value)
	{
		return Value.Vector();
	}

	FRotator FQuat_Rotator(const FQuat& Value)
	{
		return Value.Rotator();
	}

	FVector FQuat_GetRotationAxis(const FQuat& Value)
	{
		return Value.GetRotationAxis();
	}

	double FQuat_AngularDistance(const FQuat& Value, const FQuat& Other)
	{
		return Value.AngularDistance(Other);
	}

	bool FQuat_ContainsNaN(const FQuat& Value)
	{
		return Value.ContainsNaN();
	}

	FString FQuat_ToString(const FQuat& Value)
	{
		return Value.ToString();
	}

	bool FQuat_InitFromString(FQuat& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	FQuat FQuat_MakeFromEuler(const FVector& Euler)
	{
		return FQuat::MakeFromEuler(Euler);
	}

	FQuat FQuat_FindBetween(const FVector& Vector1, const FVector& Vector2)
	{
		return FQuat::FindBetween(Vector1, Vector2);
	}

	FQuat FQuat_FindBetweenNormals(const FVector& Normal1, const FVector& Normal2)
	{
		return FQuat::FindBetweenNormals(Normal1, Normal2);
	}

	FQuat FQuat_FindBetweenVectors(const FVector& Vector1, const FVector& Vector2)
	{
		return FQuat::FindBetweenVectors(Vector1, Vector2);
	}

	double FQuat_Error(const FQuat& A, const FQuat& B)
	{
		return FQuat::Error(A, B);
	}

	double FQuat_ErrorAutoNormalize(const FQuat& A, const FQuat& B)
	{
		return FQuat::ErrorAutoNormalize(A, B);
	}

	FQuat FQuat_FastLerp(const FQuat& A, const FQuat& B, double Alpha)
	{
		return FQuat::FastLerp(A, B, Alpha);
	}

	FQuat FQuat_FastBilerp(const FQuat& P00, const FQuat& P10, const FQuat& P01, const FQuat& P11, double FracX, double FracY)
	{
		return FQuat::FastBilerp(P00, P10, P01, P11, FracX, FracY);
	}

	FQuat FQuat_Slerp_NotNormalized(const FQuat& A, const FQuat& B, double Alpha)
	{
		return FQuat::Slerp_NotNormalized(A, B, Alpha);
	}

	FQuat FQuat_Slerp(const FQuat& A, const FQuat& B, double Alpha)
	{
		return FQuat::Slerp(A, B, Alpha);
	}

	FQuat FQuat_SlerpFullPath_NotNormalized(const FQuat& A, const FQuat& B, double Alpha)
	{
		return FQuat::SlerpFullPath_NotNormalized(A, B, Alpha);
	}

	FQuat FQuat_SlerpFullPath(const FQuat& A, const FQuat& B, double Alpha)
	{
		return FQuat::SlerpFullPath(A, B, Alpha);
	}

	FQuat FQuat_Squad(const FQuat& Quat1, const FQuat& Tangent1, const FQuat& Quat2, const FQuat& Tangent2, double Alpha)
	{
		return FQuat::Squad(Quat1, Tangent1, Quat2, Tangent2, Alpha);
	}

	FQuat FQuat_SquadFullPath(const FQuat& Quat1, const FQuat& Tangent1, const FQuat& Quat2, const FQuat& Tangent2, double Alpha)
	{
		return FQuat::SquadFullPath(Quat1, Tangent1, Quat2, Tangent2, Alpha);
	}

	void FQuat_CalcTangents(const FQuat& PrevP, const FQuat& P, const FQuat& NextP, double Tension, FQuat& OutTan)
	{
		FQuat::CalcTangents(PrevP, P, NextP, Tension, OutTan);
	}
}

void Bind_FQuat(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FQuat",
		sizeof(FQuat),
		asOBJ_VALUE | asGetTypeTraits<FQuat>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FQuat_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FQuat_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(const FQuat &in Other)", asFUNCTION(FQuat_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(double X, double Y, double Z, double W)", asFUNCTION(FQuat_ConstructXYZW), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(const FRotator &in Rotator)", asFUNCTION(FQuat_ConstructRotator), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_CONSTRUCT, "void f(const FVector &in Axis, double AngleRad)", asFUNCTION(FQuat_ConstructAxisAngle), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FQuat, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FQuat_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FQuat, "FQuat &opAssign(const FQuat &in Other)", asFUNCTION(FQuat_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opAdd(const FQuat &in Other) const", asFUNCTION(FQuat_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opSub(const FQuat &in Other) const", asFUNCTION(FQuat_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opMul(const FQuat &in Other) const", asFUNCTION(FQuat_MultiplyQuat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector opMul(const FVector &in Vector) const", asFUNCTION(FQuat_MultiplyVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opMul(double Scale) const", asFUNCTION(FQuat_MultiplyScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opMul_r(double Scale) const", asFUNCTION(FQuat_MultiplyScaleReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opDiv(double Scale) const", asFUNCTION(FQuat_DivideScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opAddAssign(const FQuat &in Other)", asFUNCTION(FQuat_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opSubAssign(const FQuat &in Other)", asFUNCTION(FQuat_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opMulAssign(const FQuat &in Other)", asFUNCTION(FQuat_MultiplyAssignQuat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opMulAssign(double Scale)", asFUNCTION(FQuat_MultiplyAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat opDivAssign(double Scale)", asFUNCTION(FQuat_DivideAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "bool opEquals(const FQuat &in Other) const", asFUNCTION(FQuat_EqualsExact), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "double opOr(const FQuat &in Other) const", asFUNCTION(FQuat_DotOperator), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FQuat, "bool Equals(const FQuat &in Other, double Tolerance = 0.0001) const", asFUNCTION(FQuat_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "bool IsIdentity(double Tolerance = 0.00000001) const", asFUNCTION(FQuat_IsIdentity), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector Euler() const", asFUNCTION(FQuat_Euler), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "void Normalize(double Tolerance = 0.00000001)", asFUNCTION(FQuat_Normalize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat GetNormalized(double Tolerance = 0.00000001) const", asFUNCTION(FQuat_GetNormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "bool IsNormalized() const", asFUNCTION(FQuat_IsNormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "double Size() const", asFUNCTION(FQuat_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "double SizeSquared() const", asFUNCTION(FQuat_SizeSquared), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "double GetAngle() const", asFUNCTION(FQuat_GetAngle), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "void ToAxisAndAngle(FVector &out Axis, double &out Angle) const", asFUNCTION(FQuat_ToAxisAndAngle), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "void ToSwingTwist(const FVector &in InTwistAxis, FQuat &out OutSwing, FQuat &out OutTwist) const", asFUNCTION(FQuat_ToSwingTwist), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector RotateVector(const FVector &in Vector) const", asFUNCTION(FQuat_RotateVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector UnrotateVector(const FVector &in Vector) const", asFUNCTION(FQuat_UnrotateVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat Log() const", asFUNCTION(FQuat_Log), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat Exp() const", asFUNCTION(FQuat_Exp), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FQuat Inverse() const", asFUNCTION(FQuat_Inverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "void EnforceShortestArcWith(const FQuat &in OtherQuat)", asFUNCTION(FQuat_EnforceShortestArcWith), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetAxisX() const", asFUNCTION(FQuat_GetAxisX), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetAxisY() const", asFUNCTION(FQuat_GetAxisY), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetAxisZ() const", asFUNCTION(FQuat_GetAxisZ), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetForwardVector() const", asFUNCTION(FQuat_GetForwardVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetRightVector() const", asFUNCTION(FQuat_GetRightVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetUpVector() const", asFUNCTION(FQuat_GetUpVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector Vector() const", asFUNCTION(FQuat_Vector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FRotator Rotator() const", asFUNCTION(FQuat_Rotator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FVector GetRotationAxis() const", asFUNCTION(FQuat_GetRotationAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "double AngularDistance(const FQuat &in Other) const", asFUNCTION(FQuat_AngularDistance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "bool ContainsNaN() const", asFUNCTION(FQuat_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "FString ToString() const", asFUNCTION(FQuat_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FQuat, "bool InitFromString(const FString &in Source)", asFUNCTION(FQuat_InitFromString), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FQuat, "double X", X);
	REGISTER_PROPERTY(FQuat, "double Y", Y);
	REGISTER_PROPERTY(FQuat, "double Z", Z);
	REGISTER_PROPERTY(FQuat, "double W", W);

	Result = Engine->SetDefaultNamespace("FQuat");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat MakeFromEuler(const FVector &in Euler)", asFUNCTION(FQuat_MakeFromEuler), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FindBetween(const FVector &in Vector1, const FVector &in Vector2)", asFUNCTION(FQuat_FindBetween), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FindBetweenNormals(const FVector &in Normal1, const FVector &in Normal2)", asFUNCTION(FQuat_FindBetweenNormals), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FindBetweenVectors(const FVector &in Vector1, const FVector &in Vector2)", asFUNCTION(FQuat_FindBetweenVectors), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Error(const FQuat &in A, const FQuat &in B)", asFUNCTION(FQuat_Error), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double ErrorAutoNormalize(const FQuat &in A, const FQuat &in B)", asFUNCTION(FQuat_ErrorAutoNormalize), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FastLerp(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat_FastLerp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat FastBilerp(const FQuat &in P00, const FQuat &in P10, const FQuat &in P01, const FQuat &in P11, double FracX, double FracY)", asFUNCTION(FQuat_FastBilerp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat Slerp_NotNormalized(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat_Slerp_NotNormalized), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat Slerp(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat_Slerp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat SlerpFullPath_NotNormalized(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat_SlerpFullPath_NotNormalized), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat SlerpFullPath(const FQuat &in A, const FQuat &in B, double Alpha)", asFUNCTION(FQuat_SlerpFullPath), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat Squad(const FQuat &in Quat1, const FQuat &in Tangent1, const FQuat &in Quat2, const FQuat &in Tangent2, double Alpha)", asFUNCTION(FQuat_Squad), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FQuat SquadFullPath(const FQuat &in Quat1, const FQuat &in Tangent1, const FQuat &in Quat2, const FQuat &in Tangent2, double Alpha)", asFUNCTION(FQuat_SquadFullPath), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("void CalcTangents(const FQuat &in PrevP, const FQuat &in P, const FQuat &in NextP, double Tension, FQuat &out OutTan)", asFUNCTION(FQuat_CalcTangents), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
