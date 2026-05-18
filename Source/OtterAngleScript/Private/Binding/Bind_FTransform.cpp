// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Transform.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FTransform_DefaultConstruct(FTransform* Memory)
	{
		new (Memory) FTransform();
	}

	void FTransform_ConstructTranslation(const FVector& InTranslation, FTransform* Memory)
	{
		new (Memory) FTransform(InTranslation);
	}

	void FTransform_ConstructQuat(const FQuat& InRotation, FTransform* Memory)
	{
		new (Memory) FTransform(InRotation);
	}

	void FTransform_ConstructRotator(const FRotator& InRotation, FTransform* Memory)
	{
		new (Memory) FTransform(InRotation);
	}

	void FTransform_ConstructQuatTranslation(const FQuat& InRotation, const FVector& InTranslation, FTransform* Memory)
	{
		new (Memory) FTransform(InRotation, InTranslation);
	}

	void FTransform_ConstructQuatTranslationScale(const FQuat& InRotation, const FVector& InTranslation, const FVector& InScale3D, FTransform* Memory)
	{
		new (Memory) FTransform(InRotation, InTranslation, InScale3D);
	}

	void FTransform_ConstructRotatorTranslation(const FRotator& InRotation, const FVector& InTranslation, FTransform* Memory)
	{
		new (Memory) FTransform(InRotation, InTranslation);
	}

	void FTransform_ConstructRotatorTranslationScale(const FRotator& InRotation, const FVector& InTranslation, const FVector& InScale3D, FTransform* Memory)
	{
		new (Memory) FTransform(InRotation, InTranslation, InScale3D);
	}

	void FTransform_CopyConstruct(const FTransform& Other, FTransform* Memory)
	{
		new (Memory) FTransform(Other);
	}

	void FTransform_Destruct(FTransform* Memory)
	{
		Memory->~FTransform();
	}

	FTransform& FTransform_Assign(FTransform& Value, const FTransform& Other)
	{
		Value = Other;
		return Value;
	}

	FTransform FTransform_Inverse(const FTransform& Value)
	{
		return Value.Inverse();
	}

	void FTransform_Blend(FTransform& Value, const FTransform& Atom1, const FTransform& Atom2, double Alpha)
	{
		Value.Blend(Atom1, Atom2, static_cast<float>(Alpha));
	}

	void FTransform_BlendWith(FTransform& Value, const FTransform& OtherAtom, double Alpha)
	{
		Value.BlendWith(OtherAtom, static_cast<float>(Alpha));
	}

	FTransform FTransform_Add(const FTransform& Value, const FTransform& Other)
	{
		return Value + Other;
	}

	FTransform FTransform_AddAssign(FTransform& Value, const FTransform& Other)
	{
		Value += Other;
		return Value;
	}

	FTransform FTransform_MultiplyScalar(const FTransform& Value, double Scale)
	{
		return Value * ScalarRegister(static_cast<float>(Scale));
	}

	FTransform FTransform_MultiplyAssignScalar(FTransform& Value, double Scale)
	{
		Value *= ScalarRegister(static_cast<float>(Scale));
		return Value;
	}

	FTransform FTransform_MultiplyTransform(const FTransform& Value, const FTransform& Other)
	{
		return Value * Other;
	}

	FTransform FTransform_MultiplyAssignTransform(FTransform& Value, const FTransform& Other)
	{
		Value *= Other;
		return Value;
	}

	FTransform FTransform_MultiplyQuat(const FTransform& Value, const FQuat& Other)
	{
		return Value * Other;
	}

	FTransform FTransform_MultiplyAssignQuat(FTransform& Value, const FQuat& Other)
	{
		Value *= Other;
		return Value;
	}

	bool FTransform_AnyHasNegativeScale(const FVector& InScale3D, const FVector& InOtherScale3D)
	{
		return FTransform::AnyHasNegativeScale(InScale3D, InOtherScale3D);
	}

	void FTransform_ScaleTranslationVector(FTransform& Value, const FVector& InScale3D)
	{
		Value.ScaleTranslation(InScale3D);
	}

	void FTransform_ScaleTranslationScalar(FTransform& Value, double Scale)
	{
		Value.ScaleTranslation(static_cast<float>(Scale));
	}

	void FTransform_RemoveScaling(FTransform& Value, double Tolerance)
	{
		Value.RemoveScaling(static_cast<float>(Tolerance));
	}

	double FTransform_GetMaximumAxisScale(const FTransform& Value)
	{
		return Value.GetMaximumAxisScale();
	}

	double FTransform_GetMinimumAxisScale(const FTransform& Value)
	{
		return Value.GetMinimumAxisScale();
	}

	FTransform FTransform_GetRelativeTransform(const FTransform& Value, const FTransform& Other)
	{
		return Value.GetRelativeTransform(Other);
	}

	FTransform FTransform_GetRelativeTransformReverse(const FTransform& Value, const FTransform& Other)
	{
		return Value.GetRelativeTransformReverse(Other);
	}

	void FTransform_SetToRelativeTransform(FTransform& Value, const FTransform& ParentTransform)
	{
		Value.SetToRelativeTransform(ParentTransform);
	}

	FVector FTransform_TransformPosition(const FTransform& Value, const FVector& Vector)
	{
		return Value.TransformPosition(Vector);
	}

	FVector FTransform_TransformPositionNoScale(const FTransform& Value, const FVector& Vector)
	{
		return Value.TransformPositionNoScale(Vector);
	}

	FVector FTransform_InverseTransformPosition(const FTransform& Value, const FVector& Vector)
	{
		return Value.InverseTransformPosition(Vector);
	}

	FVector FTransform_InverseTransformPositionNoScale(const FTransform& Value, const FVector& Vector)
	{
		return Value.InverseTransformPositionNoScale(Vector);
	}

	FVector FTransform_TransformVector(const FTransform& Value, const FVector& Vector)
	{
		return Value.TransformVector(Vector);
	}

	FVector FTransform_TransformVectorNoScale(const FTransform& Value, const FVector& Vector)
	{
		return Value.TransformVectorNoScale(Vector);
	}

	FVector FTransform_InverseTransformVector(const FTransform& Value, const FVector& Vector)
	{
		return Value.InverseTransformVector(Vector);
	}

	FVector FTransform_InverseTransformVectorNoScale(const FTransform& Value, const FVector& Vector)
	{
		return Value.InverseTransformVectorNoScale(Vector);
	}

	FQuat FTransform_TransformRotation(const FTransform& Value, const FQuat& Rotation)
	{
		return Value.TransformRotation(Rotation);
	}

	FQuat FTransform_InverseTransformRotation(const FTransform& Value, const FQuat& Rotation)
	{
		return Value.InverseTransformRotation(Rotation);
	}

	FTransform FTransform_GetScaledScalar(const FTransform& Value, double Scale)
	{
		return Value.GetScaled(static_cast<float>(Scale));
	}

	FTransform FTransform_GetScaledVector(const FTransform& Value, const FVector& Scale)
	{
		return Value.GetScaled(Scale);
	}

	FVector FTransform_GetScaledAxis(const FTransform& Value, EAxis::Type Axis)
	{
		return Value.GetScaledAxis(Axis);
	}

	FVector FTransform_GetUnitAxis(const FTransform& Value, EAxis::Type Axis)
	{
		return Value.GetUnitAxis(Axis);
	}

	void FTransform_Mirror(FTransform& Value, EAxis::Type MirrorAxis, EAxis::Type FlipAxis)
	{
		Value.Mirror(MirrorAxis, FlipAxis);
	}

	FVector FTransform_GetSafeScaleReciprocal(const FVector& InScale, double Tolerance)
	{
		return FTransform::GetSafeScaleReciprocal(InScale, static_cast<float>(Tolerance));
	}

	FVector FTransform_GetLocation(const FTransform& Value)
	{
		return Value.GetLocation();
	}

	FRotator FTransform_Rotator(const FTransform& Value)
	{
		return Value.Rotator();
	}

	double FTransform_GetDeterminant(const FTransform& Value)
	{
		return Value.GetDeterminant();
	}

	void FTransform_SetLocation(FTransform& Value, const FVector& Origin)
	{
		Value.SetLocation(Origin);
	}

	bool FTransform_ContainsNaN(const FTransform& Value)
	{
		return Value.ContainsNaN();
	}

	bool FTransform_IsValid(const FTransform& Value)
	{
		return Value.IsValid();
	}

	bool FTransform_AreRotationsEqual(const FTransform& A, const FTransform& B, double Tolerance)
	{
		return FTransform::AreRotationsEqual(A, B, static_cast<float>(Tolerance));
	}

	bool FTransform_AreTranslationsEqual(const FTransform& A, const FTransform& B, double Tolerance)
	{
		return FTransform::AreTranslationsEqual(A, B, static_cast<float>(Tolerance));
	}

	bool FTransform_AreScale3DsEqual(const FTransform& A, const FTransform& B, double Tolerance)
	{
		return FTransform::AreScale3DsEqual(A, B, static_cast<float>(Tolerance));
	}

	bool FTransform_RotationEquals(const FTransform& Value, const FTransform& Other, double Tolerance)
	{
		return Value.RotationEquals(Other, static_cast<float>(Tolerance));
	}

	bool FTransform_TranslationEquals(const FTransform& Value, const FTransform& Other, double Tolerance)
	{
		return Value.TranslationEquals(Other, static_cast<float>(Tolerance));
	}

	bool FTransform_Scale3DEquals(const FTransform& Value, const FTransform& Other, double Tolerance)
	{
		return Value.Scale3DEquals(Other, static_cast<float>(Tolerance));
	}

	bool FTransform_Equals(const FTransform& Value, const FTransform& Other, double Tolerance)
	{
		return Value.Equals(Other, static_cast<float>(Tolerance));
	}

	bool FTransform_EqualsNoScale(const FTransform& Value, const FTransform& Other, double Tolerance)
	{
		return Value.EqualsNoScale(Other, static_cast<float>(Tolerance));
	}

	FTransform FTransform_MultiplyStatic(const FTransform& A, const FTransform& B)
	{
		FTransform Result;
		FTransform::Multiply(&Result, &A, &B);
		return Result;
	}

	void FTransform_SetComponents(FTransform& Value, const FQuat& InRotation, const FVector& InTranslation, const FVector& InScale3D)
	{
		Value.SetComponents(InRotation, InTranslation, InScale3D);
	}

	void FTransform_SetIdentity(FTransform& Value)
	{
		Value.SetIdentity();
	}

	void FTransform_MultiplyScale3D(FTransform& Value, const FVector& Scale3DMultiplier)
	{
		Value.MultiplyScale3D(Scale3DMultiplier);
	}

	void FTransform_SetTranslation(FTransform& Value, const FVector& NewTranslation)
	{
		Value.SetTranslation(NewTranslation);
	}

	void FTransform_CopyTranslation(FTransform& Value, const FTransform& Other)
	{
		Value.CopyTranslation(Other);
	}

	void FTransform_ConcatenateRotation(FTransform& Value, const FQuat& DeltaRotation)
	{
		Value.ConcatenateRotation(DeltaRotation);
	}

	void FTransform_AddToTranslation(FTransform& Value, const FVector& DeltaTranslation)
	{
		Value.AddToTranslation(DeltaTranslation);
	}

	FVector FTransform_AddTranslations(const FTransform& A, const FTransform& B)
	{
		return FTransform::AddTranslations(A, B);
	}

	FVector FTransform_SubtractTranslations(const FTransform& A, const FTransform& B)
	{
		return FTransform::SubtractTranslations(A, B);
	}

	void FTransform_SetRotation(FTransform& Value, const FQuat& NewRotation)
	{
		Value.SetRotation(NewRotation);
	}

	void FTransform_CopyRotation(FTransform& Value, const FTransform& Other)
	{
		Value.CopyRotation(Other);
	}

	void FTransform_SetScale3D(FTransform& Value, const FVector& NewScale3D)
	{
		Value.SetScale3D(NewScale3D);
	}

	void FTransform_CopyScale3D(FTransform& Value, const FTransform& Other)
	{
		Value.CopyScale3D(Other);
	}

	void FTransform_SetTranslationAndScale3D(FTransform& Value, const FVector& NewTranslation, const FVector& NewScale3D)
	{
		Value.SetTranslationAndScale3D(NewTranslation, NewScale3D);
	}

	void FTransform_Accumulate(FTransform& Value, const FTransform& SourceAtom)
	{
		Value.Accumulate(SourceAtom);
	}

	void FTransform_AccumulateWeighted(FTransform& Value, const FTransform& Atom, double BlendWeight)
	{
		Value.Accumulate(Atom, ScalarRegister(static_cast<float>(BlendWeight)));
	}

	void FTransform_AccumulateWithShortestRotation(FTransform& Value, const FTransform& DeltaAtom, double BlendWeight)
	{
		Value.AccumulateWithShortestRotation(DeltaAtom, ScalarRegister(static_cast<float>(BlendWeight)));
	}

	void FTransform_AccumulateWithAdditiveScale(FTransform& Value, const FTransform& Atom, double BlendWeight)
	{
		Value.AccumulateWithAdditiveScale(Atom, ScalarRegister(static_cast<float>(BlendWeight)));
	}

	void FTransform_LerpTranslationScale3D(FTransform& Value, const FTransform& SourceAtom1, const FTransform& SourceAtom2, double Alpha)
	{
		Value.LerpTranslationScale3D(SourceAtom1, SourceAtom2, ScalarRegister(static_cast<float>(Alpha)));
	}

	void FTransform_NormalizeRotation(FTransform& Value)
	{
		Value.NormalizeRotation();
	}

	bool FTransform_IsRotationNormalized(const FTransform& Value)
	{
		return Value.IsRotationNormalized();
	}

	void FTransform_BlendFromIdentityAndAccumulate(FTransform& FinalAtom, const FTransform& SourceAtom, double BlendWeight)
	{
		FTransform SourceCopy(SourceAtom);
		FTransform::BlendFromIdentityAndAccumulate(FinalAtom, SourceCopy, ScalarRegister(static_cast<float>(BlendWeight)));
	}

	FQuat FTransform_GetRotation(const FTransform& Value)
	{
		return Value.GetRotation();
	}

	FVector FTransform_GetTranslation(const FTransform& Value)
	{
		return Value.GetTranslation();
	}

	FVector FTransform_GetScale3D(const FTransform& Value)
	{
		return Value.GetScale3D();
	}

	void FTransform_CopyRotationPart(FTransform& Value, const FTransform& Other)
	{
		Value.CopyRotationPart(Other);
	}

	void FTransform_CopyTranslationAndScale3D(FTransform& Value, const FTransform& Other)
	{
		Value.CopyTranslationAndScale3D(Other);
	}

	FString FTransform_ToHumanReadableString(const FTransform& Value)
	{
		return Value.ToHumanReadableString();
	}

	FString FTransform_ToString(const FTransform& Value)
	{
		return Value.ToString();
	}

	bool FTransform_InitFromString(FTransform& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}
}

void Bind_FTransform(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FTransform",
		sizeof(FTransform),
		asOBJ_VALUE | asGetTypeTraits<FTransform>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FTransform_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FVector &in InTranslation)", asFUNCTION(FTransform_ConstructTranslation), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FQuat &in InRotation)", asFUNCTION(FTransform_ConstructQuat), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FRotator &in InRotation)", asFUNCTION(FTransform_ConstructRotator), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FQuat &in InRotation, const FVector &in InTranslation)", asFUNCTION(FTransform_ConstructQuatTranslation), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FQuat &in InRotation, const FVector &in InTranslation, const FVector &in InScale3D)", asFUNCTION(FTransform_ConstructQuatTranslationScale), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FRotator &in InRotation, const FVector &in InTranslation)", asFUNCTION(FTransform_ConstructRotatorTranslation), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FRotator &in InRotation, const FVector &in InTranslation, const FVector &in InScale3D)", asFUNCTION(FTransform_ConstructRotatorTranslationScale), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_CONSTRUCT, "void f(const FTransform &in Other)", asFUNCTION(FTransform_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTransform, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FTransform_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FTransform, "FTransform &opAssign(const FTransform &in Other)", asFUNCTION(FTransform_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform Inverse() const", asFUNCTION(FTransform_Inverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void Blend(const FTransform &in Atom1, const FTransform &in Atom2, double Alpha)", asFUNCTION(FTransform_Blend), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void BlendWith(const FTransform &in OtherAtom, double Alpha)", asFUNCTION(FTransform_BlendWith), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opAdd(const FTransform &in Other) const", asFUNCTION(FTransform_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opAddAssign(const FTransform &in Other)", asFUNCTION(FTransform_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMul(double Scale) const", asFUNCTION(FTransform_MultiplyScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMulAssign(double Scale)", asFUNCTION(FTransform_MultiplyAssignScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMul(const FTransform &in Other) const", asFUNCTION(FTransform_MultiplyTransform), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMulAssign(const FTransform &in Other)", asFUNCTION(FTransform_MultiplyAssignTransform), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMul(const FQuat &in Other) const", asFUNCTION(FTransform_MultiplyQuat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMulAssign(const FQuat &in Other)", asFUNCTION(FTransform_MultiplyAssignQuat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void ScaleTranslation(const FVector &in InScale3D)", asFUNCTION(FTransform_ScaleTranslationVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void ScaleTranslation(double Scale)", asFUNCTION(FTransform_ScaleTranslationScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void RemoveScaling(double Tolerance = 0.00000001)", asFUNCTION(FTransform_RemoveScaling), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "double GetMaximumAxisScale() const", asFUNCTION(FTransform_GetMaximumAxisScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "double GetMinimumAxisScale() const", asFUNCTION(FTransform_GetMinimumAxisScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform GetRelativeTransform(const FTransform &in Other) const", asFUNCTION(FTransform_GetRelativeTransform), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform GetRelativeTransformReverse(const FTransform &in Other) const", asFUNCTION(FTransform_GetRelativeTransformReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetToRelativeTransform(const FTransform &in ParentTransform)", asFUNCTION(FTransform_SetToRelativeTransform), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector TransformPosition(const FVector &in Vector) const", asFUNCTION(FTransform_TransformPosition), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector TransformPositionNoScale(const FVector &in Vector) const", asFUNCTION(FTransform_TransformPositionNoScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector InverseTransformPosition(const FVector &in Vector) const", asFUNCTION(FTransform_InverseTransformPosition), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector InverseTransformPositionNoScale(const FVector &in Vector) const", asFUNCTION(FTransform_InverseTransformPositionNoScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector TransformVector(const FVector &in Vector) const", asFUNCTION(FTransform_TransformVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector TransformVectorNoScale(const FVector &in Vector) const", asFUNCTION(FTransform_TransformVectorNoScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector InverseTransformVector(const FVector &in Vector) const", asFUNCTION(FTransform_InverseTransformVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector InverseTransformVectorNoScale(const FVector &in Vector) const", asFUNCTION(FTransform_InverseTransformVectorNoScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FQuat TransformRotation(const FQuat &in Rotation) const", asFUNCTION(FTransform_TransformRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FQuat InverseTransformRotation(const FQuat &in Rotation) const", asFUNCTION(FTransform_InverseTransformRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform GetScaled(double Scale) const", asFUNCTION(FTransform_GetScaledScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform GetScaled(const FVector &in Scale) const", asFUNCTION(FTransform_GetScaledVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector GetScaledAxis(EAxisType Axis) const", asFUNCTION(FTransform_GetScaledAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector GetUnitAxis(EAxisType Axis) const", asFUNCTION(FTransform_GetUnitAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void Mirror(EAxisType MirrorAxis, EAxisType FlipAxis)", asFUNCTION(FTransform_Mirror), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector GetLocation() const", asFUNCTION(FTransform_GetLocation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FRotator Rotator() const", asFUNCTION(FTransform_Rotator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "double GetDeterminant() const", asFUNCTION(FTransform_GetDeterminant), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetLocation(const FVector &in Origin)", asFUNCTION(FTransform_SetLocation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool ContainsNaN() const", asFUNCTION(FTransform_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool IsValid() const", asFUNCTION(FTransform_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool RotationEquals(const FTransform &in Other, double Tolerance = 0.0001) const", asFUNCTION(FTransform_RotationEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool TranslationEquals(const FTransform &in Other, double Tolerance = 0.0001) const", asFUNCTION(FTransform_TranslationEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool Scale3DEquals(const FTransform &in Other, double Tolerance = 0.0001) const", asFUNCTION(FTransform_Scale3DEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool Equals(const FTransform &in Other, double Tolerance = 0.0001) const", asFUNCTION(FTransform_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool EqualsNoScale(const FTransform &in Other, double Tolerance = 0.0001) const", asFUNCTION(FTransform_EqualsNoScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetComponents(const FQuat &in InRotation, const FVector &in InTranslation, const FVector &in InScale3D)", asFUNCTION(FTransform_SetComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetIdentity()", asFUNCTION(FTransform_SetIdentity), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void MultiplyScale3D(const FVector &in Scale3DMultiplier)", asFUNCTION(FTransform_MultiplyScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetTranslation(const FVector &in NewTranslation)", asFUNCTION(FTransform_SetTranslation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void CopyTranslation(const FTransform &in Other)", asFUNCTION(FTransform_CopyTranslation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void ConcatenateRotation(const FQuat &in DeltaRotation)", asFUNCTION(FTransform_ConcatenateRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void AddToTranslation(const FVector &in DeltaTranslation)", asFUNCTION(FTransform_AddToTranslation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetRotation(const FQuat &in NewRotation)", asFUNCTION(FTransform_SetRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void CopyRotation(const FTransform &in Other)", asFUNCTION(FTransform_CopyRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetScale3D(const FVector &in NewScale3D)", asFUNCTION(FTransform_SetScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void CopyScale3D(const FTransform &in Other)", asFUNCTION(FTransform_CopyScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void SetTranslationAndScale3D(const FVector &in NewTranslation, const FVector &in NewScale3D)", asFUNCTION(FTransform_SetTranslationAndScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void Accumulate(const FTransform &in SourceAtom)", asFUNCTION(FTransform_Accumulate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void Accumulate(const FTransform &in Atom, double BlendWeight)", asFUNCTION(FTransform_AccumulateWeighted), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void AccumulateWithShortestRotation(const FTransform &in DeltaAtom, double BlendWeight)", asFUNCTION(FTransform_AccumulateWithShortestRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void AccumulateWithAdditiveScale(const FTransform &in Atom, double BlendWeight)", asFUNCTION(FTransform_AccumulateWithAdditiveScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void LerpTranslationScale3D(const FTransform &in SourceAtom1, const FTransform &in SourceAtom2, double Alpha)", asFUNCTION(FTransform_LerpTranslationScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void NormalizeRotation()", asFUNCTION(FTransform_NormalizeRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool IsRotationNormalized() const", asFUNCTION(FTransform_IsRotationNormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FQuat GetRotation() const", asFUNCTION(FTransform_GetRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector GetTranslation() const", asFUNCTION(FTransform_GetTranslation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FVector GetScale3D() const", asFUNCTION(FTransform_GetScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void CopyRotationPart(const FTransform &in Other)", asFUNCTION(FTransform_CopyRotationPart), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void CopyTranslationAndScale3D(const FTransform &in Other)", asFUNCTION(FTransform_CopyTranslationAndScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FString ToHumanReadableString() const", asFUNCTION(FTransform_ToHumanReadableString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FString ToString() const", asFUNCTION(FTransform_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "bool InitFromString(const FString &in Source)", asFUNCTION(FTransform_InitFromString), asCALL_CDECL_OBJFIRST);

	Result = Engine->SetDefaultNamespace("FTransform");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool AnyHasNegativeScale(const FVector &in InScale3D, const FVector &in InOtherScale3D)", asFUNCTION(FTransform_AnyHasNegativeScale), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector GetSafeScaleReciprocal(const FVector &in InScale, double Tolerance = 0.00000001)", asFUNCTION(FTransform_GetSafeScaleReciprocal), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool AreRotationsEqual(const FTransform &in A, const FTransform &in B, double Tolerance = 0.0001)", asFUNCTION(FTransform_AreRotationsEqual), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool AreTranslationsEqual(const FTransform &in A, const FTransform &in B, double Tolerance = 0.0001)", asFUNCTION(FTransform_AreTranslationsEqual), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool AreScale3DsEqual(const FTransform &in A, const FTransform &in B, double Tolerance = 0.0001)", asFUNCTION(FTransform_AreScale3DsEqual), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTransform Multiply(const FTransform &in A, const FTransform &in B)", asFUNCTION(FTransform_MultiplyStatic), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector AddTranslations(const FTransform &in A, const FTransform &in B)", asFUNCTION(FTransform_AddTranslations), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector SubtractTranslations(const FTransform &in A, const FTransform &in B)", asFUNCTION(FTransform_SubtractTranslations), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("void BlendFromIdentityAndAccumulate(FTransform &inout FinalAtom, const FTransform &in SourceAtom, double BlendWeight)", asFUNCTION(FTransform_BlendFromIdentityAndAccumulate), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
