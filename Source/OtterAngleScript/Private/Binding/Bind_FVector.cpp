// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Internationalization/Text.h"
#include "Math/Plane.h"
#include "Math/Quat.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	double& SetVectorIndexException(FVector& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.X;
	}

	void FVector_ConstructScalar(double InF, FVector* Memory)
	{
		new (Memory) FVector(InF);
	}

	void FVector_ConstructXYZ(double X, double Y, double Z, FVector* Memory)
	{
		new (Memory) FVector(X, Y, Z);
	}

	void FVector_ConstructVector2D(const FVector2D& Value, double Z, FVector* Memory)
	{
		new (Memory) FVector(Value, Z);
	}

	void FVector_DefaultConstruct(FVector* Memory)
	{
		new (Memory) FVector();
	}

	void FVector_CopyConstruct(const FVector& Other, FVector* Memory)
	{
		new (Memory) FVector(Other);
	}

	void FVector_Destruct(FVector* Memory)
	{
		Memory->~FVector();
	}

	FVector& FVector_Assign(FVector& Value, const FVector& Other)
	{
		Value = Other;
		return Value;
	}

	bool FVector_Equals(const FVector& Value, const FVector& Other)
	{
		return Value == Other;
	}

	FVector FVector_CrossOperator(const FVector& Value, const FVector& Other)
	{
		return Value ^ Other;
	}

	FVector FVector_Cross(const FVector& Value, const FVector& Other)
	{
		return Value.Cross(Other);
	}

	double FVector_DotOperator(const FVector& Value, const FVector& Other)
	{
		return Value | Other;
	}

	double FVector_Dot(const FVector& Value, const FVector& Other)
	{
		return Value.Dot(Other);
	}

	FVector FVector_Add(const FVector& Value, const FVector& Other)
	{
		return Value + Other;
	}

	FVector FVector_Subtract(const FVector& Value, const FVector& Other)
	{
		return Value - Other;
	}

	FVector FVector_SubtractBias(const FVector& Value, double Bias)
	{
		return Value - Bias;
	}

	FVector FVector_AddBias(const FVector& Value, double Bias)
	{
		return Value + Bias;
	}

	FVector FVector_MultiplyScale(const FVector& Value, double Scale)
	{
		return Value * Scale;
	}

	FVector FVector_DivideScale(const FVector& Value, double Scale)
	{
		return Value / Scale;
	}

	FVector FVector_MultiplyComponents(const FVector& Value, const FVector& Other)
	{
		return Value * Other;
	}

	FVector FVector_DivideComponents(const FVector& Value, const FVector& Other)
	{
		return Value / Other;
	}

	FVector FVector_Negate(const FVector& Value)
	{
		return -Value;
	}

	FVector FVector_AddAssign(FVector& Value, const FVector& Other)
	{
		return Value += Other;
	}

	FVector FVector_SubtractAssign(FVector& Value, const FVector& Other)
	{
		return Value -= Other;
	}

	FVector FVector_MultiplyAssignScale(FVector& Value, double Scale)
	{
		return Value *= Scale;
	}

	FVector FVector_DivideAssignScale(FVector& Value, double Scale)
	{
		return Value /= Scale;
	}

	FVector FVector_MultiplyAssignComponents(FVector& Value, const FVector& Other)
	{
		return Value *= Other;
	}

	FVector FVector_DivideAssignComponents(FVector& Value, const FVector& Other)
	{
		return Value /= Other;
	}

	double& FVector_IndexRef(FVector& Value, unsigned int Index)
	{
		if (Index > 2U)
		{
			return SetVectorIndexException(Value, "FVector index out of range");
		}

		return Value[static_cast<int32>(Index)];
	}

	double FVector_IndexValue(const FVector& Value, unsigned int Index)
	{
		if (Index > 2U)
		{
			SetScriptException("FVector index out of range");
			return 0.0;
		}

		return Value[static_cast<int32>(Index)];
	}

	double& FVector_ComponentRef(FVector& Value, int32 Index)
	{
		if (!Value.IsValidIndex(Index))
		{
			return SetVectorIndexException(Value, "FVector component index out of range");
		}

		return Value.Component(Index);
	}

	double FVector_ComponentValue(const FVector& Value, int32 Index)
	{
		if (!Value.IsValidIndex(Index))
		{
			SetScriptException("FVector component index out of range");
			return 0.0;
		}

		return Value.Component(Index);
	}

	bool FVector_IsValidIndex(const FVector& Value, int32 Index)
	{
		return Value.IsValidIndex(Index);
	}

	double FVector_GetComponentForAxis(const FVector& Value, EAxis::Type Axis)
	{
		return Value.GetComponentForAxis(Axis);
	}

	void FVector_SetComponentForAxis(FVector& Value, EAxis::Type Axis, double Component)
	{
		Value.SetComponentForAxis(Axis, Component);
	}

	void FVector_Set(FVector& Value, double X, double Y, double Z)
	{
		Value.Set(X, Y, Z);
	}

	double FVector_GetMax(const FVector& Value)
	{
		return Value.GetMax();
	}

	double FVector_GetAbsMax(const FVector& Value)
	{
		return Value.GetAbsMax();
	}

	double FVector_GetMin(const FVector& Value)
	{
		return Value.GetMin();
	}

	double FVector_GetAbsMin(const FVector& Value)
	{
		return Value.GetAbsMin();
	}

	FVector FVector_ComponentMin(const FVector& Value, const FVector& Other)
	{
		return Value.ComponentMin(Other);
	}

	FVector FVector_ComponentMax(const FVector& Value, const FVector& Other)
	{
		return Value.ComponentMax(Other);
	}

	FVector FVector_GetAbs(const FVector& Value)
	{
		return Value.GetAbs();
	}

	double FVector_Size(const FVector& Value)
	{
		return Value.Size();
	}

	double FVector_Length(const FVector& Value)
	{
		return Value.Length();
	}

	double FVector_SizeSquared(const FVector& Value)
	{
		return Value.SizeSquared();
	}

	double FVector_SquaredLength(const FVector& Value)
	{
		return Value.SquaredLength();
	}

	double FVector_Size2D(const FVector& Value)
	{
		return Value.Size2D();
	}

	double FVector_SizeSquared2D(const FVector& Value)
	{
		return Value.SizeSquared2D();
	}

	bool FVector_IsNearlyZeroDefault(const FVector& Value)
	{
		return Value.IsNearlyZero();
	}

	bool FVector_IsNearlyZeroTolerance(const FVector& Value, double Tolerance)
	{
		return Value.IsNearlyZero(Tolerance);
	}

	bool FVector_IsZero(const FVector& Value)
	{
		return Value.IsZero();
	}

	bool FVector_IsUnitDefault(const FVector& Value)
	{
		return Value.IsUnit();
	}

	bool FVector_IsUnitTolerance(const FVector& Value, double Tolerance)
	{
		return Value.IsUnit(Tolerance);
	}

	bool FVector_IsNormalized(const FVector& Value)
	{
		return Value.IsNormalized();
	}

	bool FVector_NormalizeDefault(FVector& Value)
	{
		return Value.Normalize();
	}

	bool FVector_NormalizeTolerance(FVector& Value, double Tolerance)
	{
		return Value.Normalize(Tolerance);
	}

	FVector FVector_GetUnsafeNormal(const FVector& Value)
	{
		return Value.GetUnsafeNormal();
	}

	FVector FVector_GetSafeNormalDefault(const FVector& Value)
	{
		return Value.GetSafeNormal();
	}

	FVector FVector_GetSafeNormalTolerance(const FVector& Value, double Tolerance)
	{
		return Value.GetSafeNormal(Tolerance);
	}

	FVector FVector_GetSafeNormalResult(const FVector& Value, double Tolerance, const FVector& ResultIfZero)
	{
		return Value.GetSafeNormal(Tolerance, ResultIfZero);
	}

	FVector FVector_GetSafeNormal2DDefault(const FVector& Value)
	{
		return Value.GetSafeNormal2D();
	}

	FVector FVector_GetSafeNormal2DTolerance(const FVector& Value, double Tolerance)
	{
		return Value.GetSafeNormal2D(Tolerance);
	}

	FVector FVector_GetSafeNormal2DResult(const FVector& Value, double Tolerance, const FVector& ResultIfZero)
	{
		return Value.GetSafeNormal2D(Tolerance, ResultIfZero);
	}

	void FVector_ToDirectionAndLengthDouble(const FVector& Value, FVector& OutDir, double& OutLength)
	{
		Value.ToDirectionAndLength(OutDir, OutLength);
	}

	void FVector_ToDirectionAndLengthFloat(const FVector& Value, FVector& OutDir, float& OutLength)
	{
		Value.ToDirectionAndLength(OutDir, OutLength);
	}

	FVector FVector_GetSignVector(const FVector& Value)
	{
		return Value.GetSignVector();
	}

	FVector FVector_Projection(const FVector& Value)
	{
		return Value.Projection();
	}

	FVector FVector_GetUnsafeNormal2D(const FVector& Value)
	{
		return Value.GetUnsafeNormal2D();
	}

	FVector FVector_GridSnap(const FVector& Value, double GridSize)
	{
		return Value.GridSnap(GridSize);
	}

	FVector FVector_BoundToCube(const FVector& Value, double Radius)
	{
		return Value.BoundToCube(Radius);
	}

	FVector FVector_BoundToBox(const FVector& Value, const FVector& Min, const FVector& Max)
	{
		return Value.BoundToBox(Min, Max);
	}

	FVector FVector_GetClampedToSize(const FVector& Value, double Min, double Max)
	{
		return Value.GetClampedToSize(Min, Max);
	}

	FVector FVector_GetClampedToSize2D(const FVector& Value, double Min, double Max)
	{
		return Value.GetClampedToSize2D(Min, Max);
	}

	FVector FVector_GetClampedToMaxSize(const FVector& Value, double MaxSize)
	{
		return Value.GetClampedToMaxSize(MaxSize);
	}

	FVector FVector_GetClampedToMaxSize2D(const FVector& Value, double MaxSize)
	{
		return Value.GetClampedToMaxSize2D(MaxSize);
	}

	void FVector_AddBoundedDefault(FVector& Value, const FVector& Other)
	{
		Value.AddBounded(Other);
	}

	void FVector_AddBoundedRadius(FVector& Value, const FVector& Other, double Radius)
	{
		Value.AddBounded(Other, Radius);
	}

	FVector FVector_Reciprocal(const FVector& Value)
	{
		return Value.Reciprocal();
	}

	bool FVector_IsUniformDefault(const FVector& Value)
	{
		return Value.IsUniform();
	}

	bool FVector_IsUniformTolerance(const FVector& Value, double Tolerance)
	{
		return Value.IsUniform(Tolerance);
	}

	FVector FVector_MirrorByVector(const FVector& Value, const FVector& MirrorNormal)
	{
		return Value.MirrorByVector(MirrorNormal);
	}

	FVector FVector_MirrorByPlane(const FVector& Value, const FPlane& Plane)
	{
		return Value.MirrorByPlane(Plane);
	}

	FVector FVector_RotateAngleAxis(const FVector& Value, double AngleDegrees, const FVector& Axis)
	{
		return Value.RotateAngleAxis(AngleDegrees, Axis);
	}

	FVector FVector_RotateAngleAxisRad(const FVector& Value, double AngleRadians, const FVector& Axis)
	{
		return Value.RotateAngleAxisRad(AngleRadians, Axis);
	}

	double FVector_CosineAngle2D(const FVector& Value, FVector Other)
	{
		return Value.CosineAngle2D(Other);
	}

	FVector FVector_ProjectOnTo(const FVector& Value, const FVector& Other)
	{
		return Value.ProjectOnTo(Other);
	}

	FVector FVector_ProjectOnToNormal(const FVector& Value, const FVector& Normal)
	{
		return Value.ProjectOnToNormal(Normal);
	}

	FRotator FVector_ToOrientationRotator(const FVector& Value)
	{
		return Value.ToOrientationRotator();
	}

	FQuat FVector_ToOrientationQuat(const FVector& Value)
	{
		return Value.ToOrientationQuat();
	}

	FRotator FVector_Rotation(const FVector& Value)
	{
		return Value.Rotation();
	}

	void FVector_FindBestAxisVectors(const FVector& Value, FVector& Axis1, FVector& Axis2)
	{
		Value.FindBestAxisVectors(Axis1, Axis2);
	}

	void FVector_UnwindEuler(FVector& Value)
	{
		Value.UnwindEuler();
	}

	bool FVector_ContainsNaN(const FVector& Value)
	{
		return Value.ContainsNaN();
	}

	FString FVector_ToString(const FVector& Value)
	{
		return Value.ToString();
	}

	FText FVector_ToText(const FVector& Value)
	{
		return Value.ToText();
	}

	FString FVector_ToCompactString(const FVector& Value)
	{
		return Value.ToCompactString();
	}

	FText FVector_ToCompactText(const FVector& Value)
	{
		return Value.ToCompactText();
	}

	bool FVector_InitFromString(FVector& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	bool FVector_InitFromCompactString(FVector& Value, const FString& Source)
	{
		return Value.InitFromCompactString(Source);
	}

	FVector2D FVector_UnitCartesianToSpherical(const FVector& Value)
	{
		return Value.UnitCartesianToSpherical();
	}

	double FVector_HeadingAngle(const FVector& Value)
	{
		return Value.HeadingAngle();
	}

	FVector FVector_CrossProduct(const FVector& A, const FVector& B)
	{
		return FVector::CrossProduct(A, B);
	}

	double FVector_DotProduct(const FVector& A, const FVector& B)
	{
		return FVector::DotProduct(A, B);
	}

	FVector FVector_SlerpVectorToDirection(const FVector& Value, const FVector& Direction, double Alpha)
	{
		return FVector::SlerpVectorToDirection(Value, Direction, Alpha);
	}

	FVector FVector_SlerpNormals(const FVector& NormalA, const FVector& NormalB, double Alpha)
	{
		return FVector::SlerpNormals(NormalA, NormalB, Alpha);
	}

	void FVector_CreateOrthonormalBasis(FVector& XAxis, FVector& YAxis, FVector& ZAxis)
	{
		FVector::CreateOrthonormalBasis(XAxis, YAxis, ZAxis);
	}

	bool FVector_PointsAreSame(const FVector& A, const FVector& B)
	{
		return FVector::PointsAreSame(A, B);
	}

	bool FVector_PointsAreNear(const FVector& A, const FVector& B, double Distance)
	{
		return FVector::PointsAreNear(A, B, Distance);
	}

	double FVector_PointPlaneDist(const FVector& Point, const FVector& PlaneBase, const FVector& PlaneNormal)
	{
		return FVector::PointPlaneDist(Point, PlaneBase, PlaneNormal);
	}

	FVector FVector_PointPlaneProjectPlane(const FVector& Point, const FPlane& Plane)
	{
		return FVector::PointPlaneProject(Point, Plane);
	}

	FVector FVector_PointPlaneProjectTriangle(const FVector& Point, const FVector& A, const FVector& B, const FVector& C)
	{
		return FVector::PointPlaneProject(Point, A, B, C);
	}

	FVector FVector_PointPlaneProjectBaseNormal(const FVector& Point, const FVector& PlaneBase, const FVector& PlaneNormal)
	{
		return FVector::PointPlaneProject(Point, PlaneBase, PlaneNormal);
	}

	FVector FVector_VectorPlaneProject(const FVector& Value, const FVector& PlaneNormal)
	{
		return FVector::VectorPlaneProject(Value, PlaneNormal);
	}

	double FVector_Dist(const FVector& A, const FVector& B)
	{
		return FVector::Dist(A, B);
	}

	double FVector_Distance(const FVector& A, const FVector& B)
	{
		return FVector::Distance(A, B);
	}

	double FVector_DistXY(const FVector& A, const FVector& B)
	{
		return FVector::DistXY(A, B);
	}

	double FVector_Dist2D(const FVector& A, const FVector& B)
	{
		return FVector::Dist2D(A, B);
	}

	double FVector_DistSquared(const FVector& A, const FVector& B)
	{
		return FVector::DistSquared(A, B);
	}

	double FVector_DistSquaredXY(const FVector& A, const FVector& B)
	{
		return FVector::DistSquaredXY(A, B);
	}

	double FVector_DistSquared2D(const FVector& A, const FVector& B)
	{
		return FVector::DistSquared2D(A, B);
	}

	double FVector_BoxPushOut(const FVector& Normal, const FVector& Size)
	{
		return FVector::BoxPushOut(Normal, Size);
	}

	FVector FVector_Min(const FVector& A, const FVector& B)
	{
		return FVector::Min(A, B);
	}

	FVector FVector_Max(const FVector& A, const FVector& B)
	{
		return FVector::Max(A, B);
	}

	FVector FVector_Min3(const FVector& A, const FVector& B, const FVector& C)
	{
		return FVector::Min3(A, B, C);
	}

	FVector FVector_Max3(const FVector& A, const FVector& B, const FVector& C)
	{
		return FVector::Max3(A, B, C);
	}

	bool FVector_ParallelDefault(const FVector& NormalA, const FVector& NormalB)
	{
		return FVector::Parallel(NormalA, NormalB);
	}

	bool FVector_ParallelThreshold(const FVector& NormalA, const FVector& NormalB, double Threshold)
	{
		return FVector::Parallel(NormalA, NormalB, Threshold);
	}

	bool FVector_CoincidentDefault(const FVector& NormalA, const FVector& NormalB)
	{
		return FVector::Coincident(NormalA, NormalB);
	}

	bool FVector_CoincidentThreshold(const FVector& NormalA, const FVector& NormalB, double Threshold)
	{
		return FVector::Coincident(NormalA, NormalB, Threshold);
	}

	bool FVector_OrthogonalDefault(const FVector& NormalA, const FVector& NormalB)
	{
		return FVector::Orthogonal(NormalA, NormalB);
	}

	bool FVector_OrthogonalThreshold(const FVector& NormalA, const FVector& NormalB, double Threshold)
	{
		return FVector::Orthogonal(NormalA, NormalB, Threshold);
	}

	bool FVector_CoplanarDefault(const FVector& Base1, const FVector& Normal1, const FVector& Base2, const FVector& Normal2)
	{
		return FVector::Coplanar(Base1, Normal1, Base2, Normal2);
	}

	bool FVector_CoplanarThreshold(const FVector& Base1, const FVector& Normal1, const FVector& Base2, const FVector& Normal2, double Threshold)
	{
		return FVector::Coplanar(Base1, Normal1, Base2, Normal2, Threshold);
	}

	double FVector_Triple(const FVector& X, const FVector& Y, const FVector& Z)
	{
		return FVector::Triple(X, Y, Z);
	}

	FVector FVector_RadiansToDegrees(const FVector& Value)
	{
		return FVector::RadiansToDegrees(Value);
	}

	FVector FVector_DegreesToRadians(const FVector& Value)
	{
		return FVector::DegreesToRadians(Value);
	}
}

void Bind_FVector(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FVector",
		sizeof(FVector),
		asOBJ_VALUE | asGetTypeTraits<FVector>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f(const FVector &in Other)", asFUNCTION(FVector_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f(double InF)", asFUNCTION(FVector_ConstructScalar), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f(double X, double Y, double Z)", asFUNCTION(FVector_ConstructXYZ), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f(const FVector2D &in Value, double Z)", asFUNCTION(FVector_ConstructVector2D), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FVector_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FVector, "FVector &opAssign(const FVector &in Other)", asFUNCTION(FVector_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool opEquals(const FVector &in Other) const", asFUNCTION(FVector_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opXor(const FVector &in Other) const", asFUNCTION(FVector_CrossOperator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector Cross(const FVector &in Other) const", asFUNCTION(FVector_Cross), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double opOr(const FVector &in Other) const", asFUNCTION(FVector_DotOperator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double Dot(const FVector &in Other) const", asFUNCTION(FVector_Dot), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opAdd(const FVector &in Other) const", asFUNCTION(FVector_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opSub(const FVector &in Other) const", asFUNCTION(FVector_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opSub(double Bias) const", asFUNCTION(FVector_SubtractBias), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opAdd(double Bias) const", asFUNCTION(FVector_AddBias), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opMul(double Scale) const", asFUNCTION(FVector_MultiplyScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opDiv(double Scale) const", asFUNCTION(FVector_DivideScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opMul(const FVector &in Other) const", asFUNCTION(FVector_MultiplyComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opDiv(const FVector &in Other) const", asFUNCTION(FVector_DivideComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opNeg() const", asFUNCTION(FVector_Negate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opAddAssign(const FVector &in Other)", asFUNCTION(FVector_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opSubAssign(const FVector &in Other)", asFUNCTION(FVector_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opMulAssign(double Scale)", asFUNCTION(FVector_MultiplyAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opDivAssign(double Scale)", asFUNCTION(FVector_DivideAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opMulAssign(const FVector &in Other)", asFUNCTION(FVector_MultiplyAssignComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector opDivAssign(const FVector &in Other)", asFUNCTION(FVector_DivideAssignComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double &opIndex(uint Index)", asFUNCTION(FVector_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double opIndex(uint Index) const", asFUNCTION(FVector_IndexValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double &Component(int Index)", asFUNCTION(FVector_ComponentRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double Component(int Index) const", asFUNCTION(FVector_ComponentValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsValidIndex(int Index) const", asFUNCTION(FVector_IsValidIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double GetComponentForAxis(EAxisType Axis) const", asFUNCTION(FVector_GetComponentForAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void SetComponentForAxis(EAxisType Axis, double Component)", asFUNCTION(FVector_SetComponentForAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void Set(double X, double Y, double Z)", asFUNCTION(FVector_Set), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double GetMax() const", asFUNCTION(FVector_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double GetAbsMax() const", asFUNCTION(FVector_GetAbsMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double GetMin() const", asFUNCTION(FVector_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double GetAbsMin() const", asFUNCTION(FVector_GetAbsMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector ComponentMin(const FVector &in Other) const", asFUNCTION(FVector_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector ComponentMax(const FVector &in Other) const", asFUNCTION(FVector_ComponentMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetAbs() const", asFUNCTION(FVector_GetAbs), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double Size() const", asFUNCTION(FVector_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double Length() const", asFUNCTION(FVector_Length), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double SizeSquared() const", asFUNCTION(FVector_SizeSquared), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double SquaredLength() const", asFUNCTION(FVector_SquaredLength), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double Size2D() const", asFUNCTION(FVector_Size2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double SizeSquared2D() const", asFUNCTION(FVector_SizeSquared2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsNearlyZero() const", asFUNCTION(FVector_IsNearlyZeroDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsNearlyZero(double Tolerance) const", asFUNCTION(FVector_IsNearlyZeroTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsZero() const", asFUNCTION(FVector_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsUnit() const", asFUNCTION(FVector_IsUnitDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsUnit(double LengthSquaredTolerance) const", asFUNCTION(FVector_IsUnitTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsNormalized() const", asFUNCTION(FVector_IsNormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool Normalize()", asFUNCTION(FVector_NormalizeDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool Normalize(double Tolerance)", asFUNCTION(FVector_NormalizeTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetUnsafeNormal() const", asFUNCTION(FVector_GetUnsafeNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSafeNormal() const", asFUNCTION(FVector_GetSafeNormalDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSafeNormal(double Tolerance) const", asFUNCTION(FVector_GetSafeNormalTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSafeNormal(double Tolerance, const FVector &in ResultIfZero) const", asFUNCTION(FVector_GetSafeNormalResult), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSafeNormal2D() const", asFUNCTION(FVector_GetSafeNormal2DDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSafeNormal2D(double Tolerance) const", asFUNCTION(FVector_GetSafeNormal2DTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSafeNormal2D(double Tolerance, const FVector &in ResultIfZero) const", asFUNCTION(FVector_GetSafeNormal2DResult), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void ToDirectionAndLength(FVector &out OutDir, double &out OutLength) const", asFUNCTION(FVector_ToDirectionAndLengthDouble), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void ToDirectionAndLength(FVector &out OutDir, float &out OutLength) const", asFUNCTION(FVector_ToDirectionAndLengthFloat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetSignVector() const", asFUNCTION(FVector_GetSignVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector Projection() const", asFUNCTION(FVector_Projection), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetUnsafeNormal2D() const", asFUNCTION(FVector_GetUnsafeNormal2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GridSnap(double GridSize) const", asFUNCTION(FVector_GridSnap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector BoundToCube(double Radius) const", asFUNCTION(FVector_BoundToCube), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector BoundToBox(const FVector &in Min, const FVector &in Max) const", asFUNCTION(FVector_BoundToBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetClampedToSize(double Min, double Max) const", asFUNCTION(FVector_GetClampedToSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetClampedToSize2D(double Min, double Max) const", asFUNCTION(FVector_GetClampedToSize2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetClampedToMaxSize(double MaxSize) const", asFUNCTION(FVector_GetClampedToMaxSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector GetClampedToMaxSize2D(double MaxSize) const", asFUNCTION(FVector_GetClampedToMaxSize2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void AddBounded(const FVector &in Other)", asFUNCTION(FVector_AddBoundedDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void AddBounded(const FVector &in Other, double Radius)", asFUNCTION(FVector_AddBoundedRadius), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector Reciprocal() const", asFUNCTION(FVector_Reciprocal), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsUniform() const", asFUNCTION(FVector_IsUniformDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool IsUniform(double Tolerance) const", asFUNCTION(FVector_IsUniformTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector MirrorByVector(const FVector &in MirrorNormal) const", asFUNCTION(FVector_MirrorByVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector MirrorByPlane(const FPlane &in Plane) const", asFUNCTION(FVector_MirrorByPlane), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector RotateAngleAxis(double AngleDegrees, const FVector &in Axis) const", asFUNCTION(FVector_RotateAngleAxis), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector RotateAngleAxisRad(double AngleRadians, const FVector &in Axis) const", asFUNCTION(FVector_RotateAngleAxisRad), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double CosineAngle2D(FVector Other) const", asFUNCTION(FVector_CosineAngle2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector ProjectOnTo(const FVector &in Other) const", asFUNCTION(FVector_ProjectOnTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector ProjectOnToNormal(const FVector &in Normal) const", asFUNCTION(FVector_ProjectOnToNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FRotator ToOrientationRotator() const", asFUNCTION(FVector_ToOrientationRotator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FQuat ToOrientationQuat() const", asFUNCTION(FVector_ToOrientationQuat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FRotator Rotation() const", asFUNCTION(FVector_Rotation), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void FindBestAxisVectors(FVector &out Axis1, FVector &out Axis2) const", asFUNCTION(FVector_FindBestAxisVectors), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void UnwindEuler()", asFUNCTION(FVector_UnwindEuler), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool ContainsNaN() const", asFUNCTION(FVector_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FString ToString() const", asFUNCTION(FVector_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FText ToText() const", asFUNCTION(FVector_ToText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FString ToCompactString() const", asFUNCTION(FVector_ToCompactString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FText ToCompactText() const", asFUNCTION(FVector_ToCompactText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool InitFromString(const FString &in Source)", asFUNCTION(FVector_InitFromString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool InitFromCompactString(const FString &in Source)", asFUNCTION(FVector_InitFromCompactString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "FVector2D UnitCartesianToSpherical() const", asFUNCTION(FVector_UnitCartesianToSpherical), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double HeadingAngle() const", asFUNCTION(FVector_HeadingAngle), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FVector, "double X", X);
	REGISTER_PROPERTY(FVector, "double Y", Y);
	REGISTER_PROPERTY(FVector, "double Z", Z);

	Result = Engine->SetDefaultNamespace("FVector");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector CrossProduct(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_CrossProduct), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DotProduct(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_DotProduct), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector SlerpVectorToDirection(const FVector &in Value, const FVector &in Direction, double Alpha)", asFUNCTION(FVector_SlerpVectorToDirection), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector SlerpNormals(const FVector &in NormalA, const FVector &in NormalB, double Alpha)", asFUNCTION(FVector_SlerpNormals), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("void CreateOrthonormalBasis(FVector &inout XAxis, FVector &inout YAxis, FVector &inout ZAxis)", asFUNCTION(FVector_CreateOrthonormalBasis), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool PointsAreSame(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_PointsAreSame), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool PointsAreNear(const FVector &in A, const FVector &in B, double Distance)", asFUNCTION(FVector_PointsAreNear), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double PointPlaneDist(const FVector &in Point, const FVector &in PlaneBase, const FVector &in PlaneNormal)", asFUNCTION(FVector_PointPlaneDist), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector PointPlaneProject(const FVector &in Point, const FPlane &in Plane)", asFUNCTION(FVector_PointPlaneProjectPlane), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector PointPlaneProject(const FVector &in Point, const FVector &in A, const FVector &in B, const FVector &in C)", asFUNCTION(FVector_PointPlaneProjectTriangle), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector PointPlaneProject(const FVector &in Point, const FVector &in PlaneBase, const FVector &in PlaneNormal)", asFUNCTION(FVector_PointPlaneProjectBaseNormal), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector VectorPlaneProject(const FVector &in Value, const FVector &in PlaneNormal)", asFUNCTION(FVector_VectorPlaneProject), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Dist(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_Dist), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Distance(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_Distance), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DistXY(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_DistXY), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Dist2D(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_Dist2D), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DistSquared(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_DistSquared), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DistSquaredXY(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_DistSquaredXY), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DistSquared2D(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_DistSquared2D), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double BoxPushOut(const FVector &in Normal, const FVector &in Size)", asFUNCTION(FVector_BoxPushOut), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector Min(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_Min), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector Max(const FVector &in A, const FVector &in B)", asFUNCTION(FVector_Max), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector Min3(const FVector &in A, const FVector &in B, const FVector &in C)", asFUNCTION(FVector_Min3), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector Max3(const FVector &in A, const FVector &in B, const FVector &in C)", asFUNCTION(FVector_Max3), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Parallel(const FVector &in NormalA, const FVector &in NormalB)", asFUNCTION(FVector_ParallelDefault), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Parallel(const FVector &in NormalA, const FVector &in NormalB, double Threshold)", asFUNCTION(FVector_ParallelThreshold), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Coincident(const FVector &in NormalA, const FVector &in NormalB)", asFUNCTION(FVector_CoincidentDefault), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Coincident(const FVector &in NormalA, const FVector &in NormalB, double Threshold)", asFUNCTION(FVector_CoincidentThreshold), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Orthogonal(const FVector &in NormalA, const FVector &in NormalB)", asFUNCTION(FVector_OrthogonalDefault), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Orthogonal(const FVector &in NormalA, const FVector &in NormalB, double Threshold)", asFUNCTION(FVector_OrthogonalThreshold), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Coplanar(const FVector &in Base1, const FVector &in Normal1, const FVector &in Base2, const FVector &in Normal2)", asFUNCTION(FVector_CoplanarDefault), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Coplanar(const FVector &in Base1, const FVector &in Normal1, const FVector &in Base2, const FVector &in Normal2, double Threshold)", asFUNCTION(FVector_CoplanarThreshold), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Triple(const FVector &in X, const FVector &in Y, const FVector &in Z)", asFUNCTION(FVector_Triple), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector RadiansToDegrees(const FVector &in Value)", asFUNCTION(FVector_RadiansToDegrees), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector DegreesToRadians(const FVector &in Value)", asFUNCTION(FVector_DegreesToRadians), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
