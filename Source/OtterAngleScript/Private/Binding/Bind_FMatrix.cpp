// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Plane.h"
#include "Math/Rotator.h"
#include "Math/Quat.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	void FMatrix_DefaultConstruct(FMatrix* Memory)
	{
		new (Memory) FMatrix();
	}

	void FMatrix_ConstructForceInit(EForceInit ForceInitValue, FMatrix* Memory)
	{
		new (Memory) FMatrix(ForceInitValue);
	}

	void FMatrix_ConstructFromVectors(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW, FMatrix* Memory)
	{
		new (Memory) FMatrix(InX, InY, InZ, InW);
	}

	void FMatrix_ConstructFromPlanes(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW, FMatrix* Memory)
	{
		new (Memory) FMatrix(InX, InY, InZ, InW);
	}

	void FMatrix_CopyConstruct(const FMatrix& Other, FMatrix* Memory)
	{
		new (Memory) FMatrix(Other);
	}

	void FMatrix_Destruct(FMatrix* Memory)
	{
		Memory->~FMatrix();
	}

	// opMulAssign / opAddAssign wrappers (native operators return void)
	FMatrix FMatrix_MultiplyAssign(FMatrix& Value, const FMatrix& Other)
	{
		Value *= Other;
		return Value;
	}

	FMatrix FMatrix_AddAssign(FMatrix& Value, const FMatrix& Other)
	{
		Value += Other;
		return Value;
	}

	FMatrix FMatrix_MultiplyAssignScalar(FMatrix& Value, double Scale)
	{
		Value *= Scale;
		return Value;
	}

	// Index access with bounds checking
	double& FMatrix_Index(FMatrix& Value, uint32 Row, uint32 Col)
	{
		if (Row >= 4 || Col >= 4)
		{
			SetScriptException("FMatrix index out of bounds");
			return Value.M[0][0];
		}
		return Value.M[Row][Col];
	}

	double FMatrix_IndexConst(const FMatrix& Value, uint32 Row, uint32 Col)
	{
		if (Row >= 4 || Col >= 4)
		{
			SetScriptException("FMatrix index out of bounds");
			return 0.0;
		}
		return Value.M[Row][Col];
	}

	// Out-param wrappers
	void FMatrix_GetScaledAxes(const FMatrix& Value, FVector& X, FVector& Y, FVector& Z)
	{
		Value.GetScaledAxes(X, Y, Z);
	}

	void FMatrix_GetUnitAxes(const FMatrix& Value, FVector& X, FVector& Y, FVector& Z)
	{
		Value.GetUnitAxes(X, Y, Z);
	}

	// Static Identity getter
	FMatrix FMatrix_GetIdentity()
	{
		return FMatrix::Identity;
	}
}

void Bind_FMatrix(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result = 0;

	// Constructors and destructor
	REGISTER_BEHAVIOUR(FMatrix, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FMatrix_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FMatrix, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FMatrix_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FMatrix, asBEHAVE_CONSTRUCT, "void f(const FVector &in InX, const FVector &in InY, const FVector &in InZ, const FVector &in InW)", asFUNCTION(FMatrix_ConstructFromVectors), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FMatrix, asBEHAVE_CONSTRUCT, "void f(const FPlane &in InX, const FPlane &in InY, const FPlane &in InZ, const FPlane &in InW)", asFUNCTION(FMatrix_ConstructFromPlanes), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FMatrix, asBEHAVE_CONSTRUCT, "void f(const FMatrix &in Other)", asFUNCTION(FMatrix_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FMatrix, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FMatrix_Destruct), asCALL_CDECL_OBJLAST);

	// Assignment and comparison
	REGISTER_METHOD(FMatrix, "FMatrix &opAssign(const FMatrix &in Other)", asMETHODPR(FMatrix, operator=, (const FMatrix&), FMatrix&), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "bool opEquals(const FMatrix &in Other) const", asMETHODPR(FMatrix, operator==, (const FMatrix&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "bool opNotEquals(const FMatrix &in Other) const", asMETHODPR(FMatrix, operator!=, (const FMatrix&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "bool Equals(const FMatrix &in Other, double Tolerance = 0.00001) const", asMETHODPR(FMatrix, Equals, (const FMatrix&, double) const, bool), asCALL_THISCALL);

	// Arithmetic operators
	REGISTER_METHOD(FMatrix, "FMatrix opMul(const FMatrix &in Other) const", asMETHODPR(FMatrix, operator*, (const FMatrix&) const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix opMulAssign(const FMatrix &in Other)", asFUNCTION(FMatrix_MultiplyAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FMatrix, "FMatrix opAdd(const FMatrix &in Other) const", asMETHODPR(FMatrix, operator+, (const FMatrix&) const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix opAddAssign(const FMatrix &in Other)", asFUNCTION(FMatrix_AddAssign), asCALL_CDECL_OBJFIRST);

	// Scalar arithmetic
	REGISTER_METHOD(FMatrix, "FMatrix opMul(double Scale) const", asMETHODPR(FMatrix, operator*, (double) const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix opMulAssign(double Scale)", asFUNCTION(FMatrix_MultiplyAssignScalar), asCALL_CDECL_OBJFIRST);

	// Transform methods
	// Note: TransformPosition and TransformVector return FVector4 (double),
	// which is not registered as a script type yet, so they are omitted here.

	REGISTER_METHOD(FMatrix, "FVector InverseTransformPosition(const FVector &in V) const", asMETHODPR(FMatrix, InverseTransformPosition, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FVector InverseTransformVector(const FVector &in V) const", asMETHODPR(FMatrix, InverseTransformVector, (const FVector&) const, FVector), asCALL_THISCALL);

	// Transpose
	REGISTER_METHOD(FMatrix, "FMatrix GetTransposed() const", asMETHODPR(FMatrix, GetTransposed, () const, FMatrix), asCALL_THISCALL);

	// Determinant
	REGISTER_METHOD(FMatrix, "double Determinant() const", asMETHODPR(FMatrix, Determinant, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "double RotDeterminant() const", asMETHODPR(FMatrix, RotDeterminant, () const, double), asCALL_THISCALL);

	// Inverse
	REGISTER_METHOD(FMatrix, "FMatrix Inverse() const", asMETHODPR(FMatrix, Inverse, () const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix InverseFast() const", asMETHODPR(FMatrix, InverseFast, () const, FMatrix), asCALL_THISCALL);

	// Scaling
	REGISTER_METHOD(FMatrix, "void RemoveScaling(double Tolerance = 0.00000001)", asMETHODPR(FMatrix, RemoveScaling, (double), void), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix GetMatrixWithoutScale(double Tolerance = 0.00000001) const", asMETHODPR(FMatrix, GetMatrixWithoutScale, (double) const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FVector ExtractScaling(double Tolerance = 0.00000001)", asMETHODPR(FMatrix, ExtractScaling, (double), FVector), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FVector GetScaleVector(double Tolerance = 0.00000001) const", asMETHODPR(FMatrix, GetScaleVector, (double) const, FVector), asCALL_THISCALL);

	// Translation
	REGISTER_METHOD(FMatrix, "FMatrix RemoveTranslation() const", asMETHODPR(FMatrix, RemoveTranslation, () const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix ConcatTranslation(const FVector &in Translation) const", asMETHODPR(FMatrix, ConcatTranslation, (const FVector&) const, FMatrix), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "void ScaleTranslation(const FVector &in Scale3D)", asMETHODPR(FMatrix, ScaleTranslation, (const FVector&), void), asCALL_THISCALL);

	// Scale queries
	REGISTER_METHOD(FMatrix, "double GetMinimumAxisScale() const", asMETHODPR(FMatrix, GetMinimumAxisScale, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "double GetMaximumAxisScale() const", asMETHODPR(FMatrix, GetMaximumAxisScale, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FMatrix ApplyScale(double Scale) const", asMETHODPR(FMatrix, ApplyScale, (double) const, FMatrix), asCALL_THISCALL);

	// Origin / axis access
	REGISTER_METHOD(FMatrix, "FVector GetOrigin() const", asMETHODPR(FMatrix, GetOrigin, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "void SetOrigin(const FVector &in NewOrigin)", asMETHODPR(FMatrix, SetOrigin, (const FVector&), void), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FVector GetScaledAxis(EAxisType Axis) const", asMETHODPR(FMatrix, GetScaledAxis, (EAxis::Type) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FVector GetUnitAxis(EAxisType Axis) const", asMETHODPR(FMatrix, GetUnitAxis, (EAxis::Type) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "void GetScaledAxes(FVector &out X, FVector &out Y, FVector &out Z) const", asFUNCTION(FMatrix_GetScaledAxes), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FMatrix, "void GetUnitAxes(FVector &out X, FVector &out Y, FVector &out Z) const", asFUNCTION(FMatrix_GetUnitAxes), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FMatrix, "void SetAxis(int32 i, const FVector &in Axis)", asMETHODPR(FMatrix, SetAxis, (int32, const FVector&), void), asCALL_THISCALL);

	// Column access
	REGISTER_METHOD(FMatrix, "FVector GetColumn(int32 i) const", asMETHODPR(FMatrix, GetColumn, (int32) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "void SetColumn(int32 i, FVector Value)", asMETHODPR(FMatrix, SetColumn, (int32, FVector), void), asCALL_THISCALL);

	// Rotation conversion
	REGISTER_METHOD(FMatrix, "FRotator Rotator() const", asMETHODPR(FMatrix, Rotator, () const, FRotator), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FQuat ToQuat() const", asMETHODPR(FMatrix, ToQuat, () const, FQuat), asCALL_THISCALL);

	// Utility
	REGISTER_METHOD(FMatrix, "bool ContainsNaN() const", asMETHODPR(FMatrix, ContainsNaN, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "void Mirror(EAxisType MirrorAxis, EAxisType FlipAxis)", asMETHODPR(FMatrix, Mirror, (EAxis::Type, EAxis::Type), void), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "void SetIdentity()", asMETHODPR(FMatrix, SetIdentity, (), void), asCALL_THISCALL);
	REGISTER_METHOD(FMatrix, "FString ToString() const", asMETHODPR(FMatrix, ToString, () const, FString), asCALL_THISCALL);

	// Index access
	REGISTER_METHOD(FMatrix, "double &opIndex(uint Row, uint Col)", asFUNCTION(FMatrix_Index), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FMatrix, "double opIndex(uint Row, uint Col) const", asFUNCTION(FMatrix_IndexConst), asCALL_CDECL_OBJFIRST);

	// Static: Identity
	Result = Engine->SetDefaultNamespace("FMatrix");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FMatrix get_Identity() property", asFUNCTION(FMatrix_GetIdentity), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
