// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Transform.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static constexpr uint32 Align = alignof(FTransform);
	struct FTransform_noalign
	{
		/** Rotation of this transformation, as a quaternion */
		TPersistentVectorRegisterType<double> Rotation;
		/** Translation of this transformation, as a vector */
		TPersistentVectorRegisterType<double> Translation;
		/** 3D scale (always applied in local space) as a vector */
		TPersistentVectorRegisterType<double> Scale3D;
	};

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

	void FTransform_Blend(FTransform& Value, const FTransform& Atom1, const FTransform& Atom2, double Alpha)
	{
		Value.Blend(Atom1, Atom2, static_cast<float>(Alpha));
	}

	void FTransform_BlendWith(FTransform& Value, const FTransform& OtherAtom, double Alpha)
	{
		Value.BlendWith(OtherAtom, static_cast<float>(Alpha));
	}

	FTransform FTransform_MultiplyScalar(const FTransform& Value, double Scale)
	{
		return Value * ScalarRegister(static_cast<float>(Scale));
	}

	FTransform& FTransform_MultiplyAssignScalar(FTransform_noalign& Value, double Scale)
	{
		(FTransform&)Value *= ScalarRegister(static_cast<float>(Scale));
		return (FTransform&)Value;
	}

	FTransform& FTransform_MultiplyAssignTransform(FTransform_noalign& Value, const FTransform& Other)
	{
		(FTransform&)Value *= Other;
		return (FTransform&)Value;
	}

	FTransform& FTransform_MultiplyAssignQuat(FTransform_noalign& Value, const FQuat& Other)
	{
		(FTransform&)Value *= Other;
		return (FTransform&)Value;
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

	FTransform FTransform_GetScaledScalar(const FTransform& Value, double Scale)
	{
		return Value.GetScaled(static_cast<float>(Scale));
	}

	FVector FTransform_GetSafeScaleReciprocal(const FVector& InScale, double Tolerance)
	{
		return FTransform::GetSafeScaleReciprocal(InScale, static_cast<float>(Tolerance));
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

	FTransform FTransform_MultiplyStatic(const FTransform& A, const FTransform& B)
	{
		FTransform Result;
		FTransform::Multiply(&Result, &A, &B);
		return Result;
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

	void FTransform_BlendFromIdentityAndAccumulate(FTransform& FinalAtom, const FTransform& SourceAtom, double BlendWeight)
	{
		FTransform SourceCopy(SourceAtom);
		FTransform::BlendFromIdentityAndAccumulate(FinalAtom, SourceCopy, ScalarRegister(static_cast<float>(BlendWeight)));
	}

}

void Bind_FTransform(asIScriptEngine* Engine)
{
	check(sizeof(FTransform) == sizeof(FTransform_noalign));

	check(Engine != nullptr);

	int Result = 0;

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

	REGISTER_METHOD(FTransform, "FTransform &opAssign(const FTransform &in Other)", asMETHODPR(FTransform, operator=, (const FTransform&), FTransform&), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform Inverse() const", asMETHODPR(FTransform, Inverse, () const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void Blend(const FTransform &in Atom1, const FTransform &in Atom2, double Alpha)", asFUNCTION(FTransform_Blend), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void BlendWith(const FTransform &in OtherAtom, double Alpha)", asFUNCTION(FTransform_BlendWith), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opAdd(const FTransform &in Other) const", asMETHODPR(FTransform, operator+, (const FTransform&) const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform opMul(double Scale) const", asFUNCTION(FTransform_MultiplyScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform opMul(const FTransform &in Other) const", asMETHODPR(FTransform, operator*, (const FTransform&) const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform opMul(const FQuat &in Other) const", asMETHODPR(FTransform, operator*, (const FQuat&) const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform& opAddAssign(const FTransform &in Other)", asMETHODPR(FTransform, operator+=, (const FTransform&), FTransform&), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform& opMulAssign(double Scale)", asFUNCTION(FTransform_MultiplyAssignScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform& opMulAssign(const FTransform &in Other)", asFUNCTION(FTransform_MultiplyAssignTransform), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform& opMulAssign(const FQuat &in Other)", asFUNCTION(FTransform_MultiplyAssignQuat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void ScaleTranslation(const FVector &in InScale3D)", asMETHODPR(FTransform, ScaleTranslation, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void ScaleTranslation(double Scale)", asFUNCTION(FTransform_ScaleTranslationScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void RemoveScaling(double Tolerance = 0.00000001)", asFUNCTION(FTransform_RemoveScaling), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "double GetMaximumAxisScale() const", asFUNCTION(FTransform_GetMaximumAxisScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "double GetMinimumAxisScale() const", asFUNCTION(FTransform_GetMinimumAxisScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform GetRelativeTransform(const FTransform &in Other) const", asMETHODPR(FTransform, GetRelativeTransform, (const FTransform&) const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform GetRelativeTransformReverse(const FTransform &in Other) const", asMETHODPR(FTransform, GetRelativeTransformReverse, (const FTransform&) const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetToRelativeTransform(const FTransform &in ParentTransform)", asMETHODPR(FTransform, SetToRelativeTransform, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector TransformPosition(const FVector &in Vector) const", asMETHODPR(FTransform, TransformPosition, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector TransformPositionNoScale(const FVector &in Vector) const", asMETHODPR(FTransform, TransformPositionNoScale, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector InverseTransformPosition(const FVector &in Vector) const", asMETHODPR(FTransform, InverseTransformPosition, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector InverseTransformPositionNoScale(const FVector &in Vector) const", asMETHODPR(FTransform, InverseTransformPositionNoScale, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector TransformVector(const FVector &in Vector) const", asMETHODPR(FTransform, TransformVector, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector TransformVectorNoScale(const FVector &in Vector) const", asMETHODPR(FTransform, TransformVectorNoScale, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector InverseTransformVector(const FVector &in Vector) const", asMETHODPR(FTransform, InverseTransformVector, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector InverseTransformVectorNoScale(const FVector &in Vector) const", asMETHODPR(FTransform, InverseTransformVectorNoScale, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FQuat TransformRotation(const FQuat &in Rotation) const", asMETHODPR(FTransform, TransformRotation, (const FQuat&) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FQuat InverseTransformRotation(const FQuat &in Rotation) const", asMETHODPR(FTransform, InverseTransformRotation, (const FQuat&) const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FTransform GetScaled(double Scale) const", asFUNCTION(FTransform_GetScaledScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "FTransform GetScaled(const FVector &in Scale) const", asMETHODPR(FTransform, GetScaled, (FVector) const, FTransform), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector GetScaledAxis(EAxisType Axis) const", asMETHODPR(FTransform, GetScaledAxis, (EAxis::Type) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector GetUnitAxis(EAxisType Axis) const", asMETHODPR(FTransform, GetUnitAxis, (EAxis::Type) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void Mirror(EAxisType MirrorAxis, EAxisType FlipAxis)", asMETHODPR(FTransform, Mirror, (EAxis::Type, EAxis::Type), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector GetLocation() const", asMETHODPR(FTransform, GetLocation, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FRotator Rotator() const", asMETHODPR(FTransform, Rotator, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "double GetDeterminant() const", asMETHODPR(FTransform, GetDeterminant, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetLocation(const FVector &in Origin)", asMETHODPR(FTransform, SetLocation, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool ContainsNaN() const", asMETHODPR(FTransform, ContainsNaN, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool IsValid() const", asMETHODPR(FTransform, IsValid, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool RotationEquals(const FTransform &in Other, double Tolerance = 0.0001) const", asMETHODPR(FTransform, RotationEquals, (const FTransform&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool TranslationEquals(const FTransform &in Other, double Tolerance = 0.0001) const", asMETHODPR(FTransform, TranslationEquals, (const FTransform&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool Scale3DEquals(const FTransform &in Other, double Tolerance = 0.0001) const", asMETHODPR(FTransform, Scale3DEquals, (const FTransform&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool Equals(const FTransform &in Other, double Tolerance = 0.0001) const", asMETHODPR(FTransform, Equals, (const FTransform&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool EqualsNoScale(const FTransform &in Other, double Tolerance = 0.0001) const", asMETHODPR(FTransform, EqualsNoScale, (const FTransform&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetComponents(const FQuat &in InRotation, const FVector &in InTranslation, const FVector &in InScale3D)", asMETHODPR(FTransform, SetComponents, (const FQuat&, const FVector&, const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetIdentity()", asMETHODPR(FTransform, SetIdentity, (), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void MultiplyScale3D(const FVector &in Scale3DMultiplier)", asMETHODPR(FTransform, MultiplyScale3D, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetTranslation(const FVector &in NewTranslation)", asMETHODPR(FTransform, SetTranslation, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void CopyTranslation(const FTransform &in Other)", asMETHODPR(FTransform, CopyTranslation, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void ConcatenateRotation(const FQuat &in DeltaRotation)", asMETHODPR(FTransform, ConcatenateRotation, (const FQuat&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void AddToTranslation(const FVector &in DeltaTranslation)", asMETHODPR(FTransform, AddToTranslation, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetRotation(const FQuat &in NewRotation)", asMETHODPR(FTransform, SetRotation, (const FQuat&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void CopyRotation(const FTransform &in Other)", asMETHODPR(FTransform, CopyRotation, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetScale3D(const FVector &in NewScale3D)", asMETHODPR(FTransform, SetScale3D, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void CopyScale3D(const FTransform &in Other)", asMETHODPR(FTransform, CopyScale3D, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void SetTranslationAndScale3D(const FVector &in NewTranslation, const FVector &in NewScale3D)", asMETHODPR(FTransform, SetTranslationAndScale3D, (const FVector&, const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void Accumulate(const FTransform &in SourceAtom)", asMETHODPR(FTransform, Accumulate, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void Accumulate(const FTransform &in Atom, double BlendWeight)", asFUNCTION(FTransform_AccumulateWeighted), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void AccumulateWithShortestRotation(const FTransform &in DeltaAtom, double BlendWeight)", asFUNCTION(FTransform_AccumulateWithShortestRotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void AccumulateWithAdditiveScale(const FTransform &in Atom, double BlendWeight)", asFUNCTION(FTransform_AccumulateWithAdditiveScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void LerpTranslationScale3D(const FTransform &in SourceAtom1, const FTransform &in SourceAtom2, double Alpha)", asFUNCTION(FTransform_LerpTranslationScale3D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTransform, "void NormalizeRotation()", asMETHODPR(FTransform, NormalizeRotation, (), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool IsRotationNormalized() const", asMETHODPR(FTransform, IsRotationNormalized, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FQuat GetRotation() const", asMETHODPR(FTransform, GetRotation, () const, FQuat), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector GetTranslation() const", asMETHODPR(FTransform, GetTranslation, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FVector GetScale3D() const", asMETHODPR(FTransform, GetScale3D, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void CopyRotationPart(const FTransform &in Other)", asMETHODPR(FTransform, CopyRotationPart, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "void CopyTranslationAndScale3D(const FTransform &in Other)", asMETHODPR(FTransform, CopyTranslationAndScale3D, (const FTransform&), void), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FString ToHumanReadableString() const", asMETHODPR(FTransform, ToHumanReadableString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "FString ToString() const", asMETHODPR(FTransform, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FTransform, "bool InitFromString(const FString &in Source)", asMETHODPR(FTransform, InitFromString, (const FString&), bool), asCALL_THISCALL);
	
	Result = Engine->SetDefaultNamespace("FTransform");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool AnyHasNegativeScale(const FVector &in InScale3D, const FVector &in InOtherScale3D)", asFUNCTION(FTransform::AnyHasNegativeScale), asCALL_CDECL);
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
	Result = Engine->RegisterGlobalFunction("FVector AddTranslations(const FTransform &in A, const FTransform &in B)", asFUNCTION(FTransform::AddTranslations), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector SubtractTranslations(const FTransform &in A, const FTransform &in B)", asFUNCTION(FTransform::SubtractTranslations), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("void BlendFromIdentityAndAccumulate(FTransform &out FinalAtom, const FTransform &in SourceAtom, double BlendWeight)", asFUNCTION(FTransform_BlendFromIdentityAndAccumulate), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
