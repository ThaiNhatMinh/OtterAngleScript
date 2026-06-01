// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	static double& SetVector2DIndexException(FVector2D& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.X;
	}

	// --- Constructors / destructor ---

	static void FVector2D_DefaultConstruct(FVector2D* Memory)
	{
		new (Memory) FVector2D();
	}

	static void FVector2D_CopyConstruct(const FVector2D& Other, FVector2D* Memory)
	{
		new (Memory) FVector2D(Other);
	}

	static void FVector2D_ConstructScalar(double InF, FVector2D* Memory)
	{
		new (Memory) FVector2D(InF);
	}

	static void FVector2D_ConstructXY(double InX, double InY, FVector2D* Memory)
	{
		new (Memory) FVector2D(InX, InY);
	}

	static void FVector2D_ConstructFromFVector(const FVector& V, FVector2D* Memory)
	{
		new (Memory) FVector2D(V);
	}

	static void FVector2D_Destruct(FVector2D* Memory)
	{
		Memory->~FVector2D();
	}

	// --- Assignment / comparison ---

	static FVector2D& FVector2D_Assign(FVector2D& Value, const FVector2D& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FVector2D_Equals_Op(const FVector2D& Value, const FVector2D& Other)
	{
		return Value == Other;
	}

	// --- Arithmetic operators ---

	static FVector2D FVector2D_Add(const FVector2D& Value, const FVector2D& Other)
	{
		return Value + Other;
	}

	static FVector2D FVector2D_AddScalar(const FVector2D& Value, double A)
	{
		return Value + A;
	}

	static FVector2D FVector2D_Subtract(const FVector2D& Value, const FVector2D& Other)
	{
		return Value - Other;
	}

	static FVector2D FVector2D_SubtractScalar(const FVector2D& Value, double A)
	{
		return Value - A;
	}

	static FVector2D FVector2D_MultiplyScale(const FVector2D& Value, double Scale)
	{
		return Value * Scale;
	}

	static FVector2D FVector2D_MultiplyComponents(const FVector2D& Value, const FVector2D& Other)
	{
		return Value * Other;
	}

	static FVector2D FVector2D_DivideScale(const FVector2D& Value, double Scale)
	{
		return Value / Scale;
	}

	static FVector2D FVector2D_DivideComponents(const FVector2D& Value, const FVector2D& Other)
	{
		return Value / Other;
	}

	static FVector2D FVector2D_Negate(const FVector2D& Value)
	{
		return -Value;
	}

	// operator| = dot product; operator^ = cross product (scalar for 2D)
	static double FVector2D_DotOperator(const FVector2D& Value, const FVector2D& Other)
	{
		return Value | Other;
	}

	static double FVector2D_CrossOperator(const FVector2D& Value, const FVector2D& Other)
	{
		return Value ^ Other;
	}

	// --- Compound assignment ---

	static FVector2D FVector2D_AddAssign(FVector2D& Value, const FVector2D& Other)
	{
		return Value += Other;
	}

	static FVector2D FVector2D_SubtractAssign(FVector2D& Value, const FVector2D& Other)
	{
		return Value -= Other;
	}

	static FVector2D FVector2D_MultiplyAssignScale(FVector2D& Value, double Scale)
	{
		return Value *= Scale;
	}

	static FVector2D FVector2D_DivideAssignScale(FVector2D& Value, double Scale)
	{
		return Value /= Scale;
	}

	static FVector2D FVector2D_MultiplyAssignComponents(FVector2D& Value, const FVector2D& Other)
	{
		return Value *= Other;
	}

	static FVector2D FVector2D_DivideAssignComponents(FVector2D& Value, const FVector2D& Other)
	{
		return Value /= Other;
	}

	// --- Index operators ---

	static double& FVector2D_IndexRef(FVector2D& Value, unsigned int Index)
	{
		if (Index > 1U)
		{
			return SetVector2DIndexException(Value, "FVector2D index out of range");
		}
		return Value[static_cast<int32>(Index)];
	}

	static double FVector2D_IndexValue(const FVector2D& Value, unsigned int Index)
	{
		if (Index > 1U)
		{
			SetScriptException("FVector2D index out of range");
			return 0.0;
		}
		return Value[static_cast<int32>(Index)];
	}

	// --- Component / set ---

	static void FVector2D_Set(FVector2D& Value, double InX, double InY)
	{
		Value.Set(InX, InY);
	}

	static bool FVector2D_IsValidIndex(const FVector2D& Value, int32 Index)
	{
		return Value.IsValidIndex(Index);
	}

	// --- Size / length ---

	static double FVector2D_Size(const FVector2D& Value)
	{
		return Value.Size();
	}

	static double FVector2D_Length(const FVector2D& Value)
	{
		return Value.Length();
	}

	static double FVector2D_SizeSquared(const FVector2D& Value)
	{
		return Value.SizeSquared();
	}

	static double FVector2D_SquaredLength(const FVector2D& Value)
	{
		return Value.SquaredLength();
	}

	// --- Component accessors ---

	static double FVector2D_GetMax(const FVector2D& Value)
	{
		return Value.GetMax();
	}

	static double FVector2D_GetAbsMax(const FVector2D& Value)
	{
		return Value.GetAbsMax();
	}

	static double FVector2D_GetMin(const FVector2D& Value)
	{
		return Value.GetMin();
	}

	static FVector2D FVector2D_ComponentMin(const FVector2D& Value, const FVector2D& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FVector2D FVector2D_ComponentMax(const FVector2D& Value, const FVector2D& Other)
	{
		return Value.ComponentMax(Other);
	}

	static FVector2D FVector2D_GetAbs(const FVector2D& Value)
	{
		return Value.GetAbs();
	}

	static FVector2D FVector2D_GetSignVector(const FVector2D& Value)
	{
		return Value.GetSignVector();
	}

	// --- Dot ---

	static double FVector2D_Dot(const FVector2D& Value, const FVector2D& V2)
	{
		return Value.Dot(V2);
	}

	// --- Normalization ---

	static bool FVector2D_NormalizeDefault(FVector2D& Value)
	{
		return Value.Normalize();
	}

	static bool FVector2D_NormalizeTolerance(FVector2D& Value, double Tolerance)
	{
		return Value.Normalize(Tolerance);
	}

	static FVector2D FVector2D_GetSafeNormalDefault(const FVector2D& Value)
	{
		return Value.GetSafeNormal();
	}

	static FVector2D FVector2D_GetSafeNormalTolerance(const FVector2D& Value, double Tolerance)
	{
		return Value.GetSafeNormal(Tolerance);
	}

	// --- Rotation ---

	static FVector2D FVector2D_GetRotated(const FVector2D& Value, double AngleDeg)
	{
		return Value.GetRotated(AngleDeg);
	}

	// --- Predicates ---

	static bool FVector2D_IsNearlyZeroDefault(const FVector2D& Value)
	{
		return Value.IsNearlyZero();
	}

	static bool FVector2D_IsNearlyZeroTolerance(const FVector2D& Value, double Tolerance)
	{
		return Value.IsNearlyZero(Tolerance);
	}

	static bool FVector2D_IsZero(const FVector2D& Value)
	{
		return Value.IsZero();
	}

	static bool FVector2D_EqualsDefault(const FVector2D& Value, const FVector2D& V)
	{
		return Value.Equals(V);
	}

	static bool FVector2D_EqualsTolerance(const FVector2D& Value, const FVector2D& V, double Tolerance)
	{
		return Value.Equals(V, Tolerance);
	}

	static bool FVector2D_ContainsNaN(const FVector2D& Value)
	{
		return Value.ContainsNaN();
	}

	static bool FVector2D_ComponentwiseAllLessThan(const FVector2D& Value, const FVector2D& Other)
	{
		return Value.ComponentwiseAllLessThan(Other);
	}

	static bool FVector2D_ComponentwiseAllGreaterThan(const FVector2D& Value, const FVector2D& Other)
	{
		return Value.ComponentwiseAllGreaterThan(Other);
	}

	static bool FVector2D_ComponentwiseAllLessOrEqual(const FVector2D& Value, const FVector2D& Other)
	{
		return Value.ComponentwiseAllLessOrEqual(Other);
	}

	static bool FVector2D_ComponentwiseAllGreaterOrEqual(const FVector2D& Value, const FVector2D& Other)
	{
		return Value.ComponentwiseAllGreaterOrEqual(Other);
	}

	// --- ToDirectionAndLength ---

	static void FVector2D_ToDirectionAndLength(const FVector2D& Value, FVector2D& OutDir, double& OutLength)
	{
		Value.ToDirectionAndLength(OutDir, OutLength);
	}

	// --- Clamping ---

	static FVector2D FVector2D_ClampAxes(const FVector2D& Value, double MinAxisVal, double MaxAxisVal)
	{
		return Value.ClampAxes(MinAxisVal, MaxAxisVal);
	}

	static FVector2D FVector2D_GetClampedToSize(const FVector2D& Value, double Min, double Max)
	{
		return Value.GetClampedToSize(Min, Max);
	}

	static FVector2D FVector2D_GetClampedToMaxSize(const FVector2D& Value, double MaxSize)
	{
		return Value.GetClampedToMaxSize(MaxSize);
	}

	// --- Rounding ---

	static FVector2D FVector2D_RoundToVector(const FVector2D& Value)
	{
		return Value.RoundToVector();
	}

	// --- String ---

	static FString FVector2D_ToString(const FVector2D& Value)
	{
		return Value.ToString();
	}

	static bool FVector2D_InitFromString(FVector2D& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	// --- Conversion ---

	static FVector FVector2D_SphericalToUnitCartesian(const FVector2D& Value)
	{
		return Value.SphericalToUnitCartesian();
	}

	// --- Static helpers ---

	static double FVector2D_DotProduct(const FVector2D& A, const FVector2D& B)
	{
		return FVector2D::DotProduct(A, B);
	}

	static double FVector2D_CrossProduct(const FVector2D& A, const FVector2D& B)
	{
		return FVector2D::CrossProduct(A, B);
	}

	static double FVector2D_Distance(const FVector2D& V1, const FVector2D& V2)
	{
		return FVector2D::Distance(V1, V2);
	}

	static double FVector2D_DistSquared(const FVector2D& V1, const FVector2D& V2)
	{
		return FVector2D::DistSquared(V1, V2);
	}

	static FVector2D FVector2D_Max(const FVector2D& A, const FVector2D& B)
	{
		return FVector2D::Max(A, B);
	}

	static FVector2D FVector2D_Min(const FVector2D& A, const FVector2D& B)
	{
		return FVector2D::Min(A, B);
	}

	static FVector2D FVector2D_Clamp(const FVector2D& V, const FVector2D& MinValue, const FVector2D& MaxValue)
	{
		return FVector2D::Clamp(V, MinValue, MaxValue);
	}

	static FVector2D FVector2D_Zero()  { return FVector2D::Zero(); }
	static FVector2D FVector2D_One()   { return FVector2D::One(); }
	static FVector2D FVector2D_UnitX() { return FVector2D::UnitX(); }
	static FVector2D FVector2D_UnitY() { return FVector2D::UnitY(); }
}

void Bind_FVector2D(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// --- Behaviors ---

	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector2D_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f(const FVector2D &in Other)", asFUNCTION(FVector2D_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f(double InF)", asFUNCTION(FVector2D_ConstructScalar), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f(double InX, double InY)", asFUNCTION(FVector2D_ConstructXY), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f(const FVector &in V)", asFUNCTION(FVector2D_ConstructFromFVector), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(FVector2D_Destruct), asCALL_CDECL_OBJLAST);

	// --- Properties ---

	REGISTER_PROPERTY(FVector2D, "double X", X);
	REGISTER_PROPERTY(FVector2D, "double Y", Y);

	// --- Assignment / comparison ---

	REGISTER_METHOD(FVector2D, "FVector2D &opAssign(const FVector2D &in Other)", asFUNCTION(FVector2D_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool opEquals(const FVector2D &in Other) const", asFUNCTION(FVector2D_Equals_Op), asCALL_CDECL_OBJFIRST);

	// --- Arithmetic operators ---

	REGISTER_METHOD(FVector2D, "FVector2D opAdd(const FVector2D &in Other) const", asFUNCTION(FVector2D_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opAdd(double A) const", asFUNCTION(FVector2D_AddScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opSub(const FVector2D &in Other) const", asFUNCTION(FVector2D_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opSub(double A) const", asFUNCTION(FVector2D_SubtractScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opMul(double Scale) const", asFUNCTION(FVector2D_MultiplyScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opMul(const FVector2D &in Other) const", asFUNCTION(FVector2D_MultiplyComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opDiv(double Scale) const", asFUNCTION(FVector2D_DivideScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opDiv(const FVector2D &in Other) const", asFUNCTION(FVector2D_DivideComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opNeg() const", asFUNCTION(FVector2D_Negate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double opOr(const FVector2D &in Other) const", asFUNCTION(FVector2D_DotOperator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double opXor(const FVector2D &in Other) const", asFUNCTION(FVector2D_CrossOperator), asCALL_CDECL_OBJFIRST);

	// --- Compound assignment ---

	REGISTER_METHOD(FVector2D, "FVector2D opAddAssign(const FVector2D &in Other)", asFUNCTION(FVector2D_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opSubAssign(const FVector2D &in Other)", asFUNCTION(FVector2D_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opMulAssign(double Scale)", asFUNCTION(FVector2D_MultiplyAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opDivAssign(double Scale)", asFUNCTION(FVector2D_DivideAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opMulAssign(const FVector2D &in Other)", asFUNCTION(FVector2D_MultiplyAssignComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D opDivAssign(const FVector2D &in Other)", asFUNCTION(FVector2D_DivideAssignComponents), asCALL_CDECL_OBJFIRST);

	// --- Index operators ---

	REGISTER_METHOD(FVector2D, "double &opIndex(uint Index)", asFUNCTION(FVector2D_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double opIndex(uint Index) const", asFUNCTION(FVector2D_IndexValue), asCALL_CDECL_OBJFIRST);

	// --- Component / set ---

	REGISTER_METHOD(FVector2D, "void Set(double InX, double InY)", asFUNCTION(FVector2D_Set), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool IsValidIndex(int Index) const", asFUNCTION(FVector2D_IsValidIndex), asCALL_CDECL_OBJFIRST);

	// --- Size / length ---

	REGISTER_METHOD(FVector2D, "double Size() const", asFUNCTION(FVector2D_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double Length() const", asFUNCTION(FVector2D_Length), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double SizeSquared() const", asFUNCTION(FVector2D_SizeSquared), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double SquaredLength() const", asFUNCTION(FVector2D_SquaredLength), asCALL_CDECL_OBJFIRST);

	// --- Component accessors ---

	REGISTER_METHOD(FVector2D, "double GetMax() const", asFUNCTION(FVector2D_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double GetAbsMax() const", asFUNCTION(FVector2D_GetAbsMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double GetMin() const", asFUNCTION(FVector2D_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D ComponentMin(const FVector2D &in Other) const", asFUNCTION(FVector2D_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D ComponentMax(const FVector2D &in Other) const", asFUNCTION(FVector2D_ComponentMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D GetAbs() const", asFUNCTION(FVector2D_GetAbs), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D GetSignVector() const", asFUNCTION(FVector2D_GetSignVector), asCALL_CDECL_OBJFIRST);

	// --- Dot ---

	REGISTER_METHOD(FVector2D, "double Dot(const FVector2D &in V2) const", asFUNCTION(FVector2D_Dot), asCALL_CDECL_OBJFIRST);

	// --- Normalization ---

	REGISTER_METHOD(FVector2D, "bool Normalize()", asFUNCTION(FVector2D_NormalizeDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool Normalize(double Tolerance)", asFUNCTION(FVector2D_NormalizeTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D GetSafeNormal() const", asFUNCTION(FVector2D_GetSafeNormalDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D GetSafeNormal(double Tolerance) const", asFUNCTION(FVector2D_GetSafeNormalTolerance), asCALL_CDECL_OBJFIRST);

	// --- Rotation ---

	REGISTER_METHOD(FVector2D, "FVector2D GetRotated(double AngleDeg) const", asFUNCTION(FVector2D_GetRotated), asCALL_CDECL_OBJFIRST);

	// --- Predicates ---

	REGISTER_METHOD(FVector2D, "bool IsNearlyZero() const", asFUNCTION(FVector2D_IsNearlyZeroDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool IsNearlyZero(double Tolerance) const", asFUNCTION(FVector2D_IsNearlyZeroTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool IsZero() const", asFUNCTION(FVector2D_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool Equals(const FVector2D &in V) const", asFUNCTION(FVector2D_EqualsDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool Equals(const FVector2D &in V, double Tolerance) const", asFUNCTION(FVector2D_EqualsTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool ContainsNaN() const", asFUNCTION(FVector2D_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool ComponentwiseAllLessThan(const FVector2D &in Other) const", asFUNCTION(FVector2D_ComponentwiseAllLessThan), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool ComponentwiseAllGreaterThan(const FVector2D &in Other) const", asFUNCTION(FVector2D_ComponentwiseAllGreaterThan), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool ComponentwiseAllLessOrEqual(const FVector2D &in Other) const", asFUNCTION(FVector2D_ComponentwiseAllLessOrEqual), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool ComponentwiseAllGreaterOrEqual(const FVector2D &in Other) const", asFUNCTION(FVector2D_ComponentwiseAllGreaterOrEqual), asCALL_CDECL_OBJFIRST);

	// --- Direction and length decomposition ---

	REGISTER_METHOD(FVector2D, "void ToDirectionAndLength(FVector2D &out OutDir, double &out OutLength) const", asFUNCTION(FVector2D_ToDirectionAndLength), asCALL_CDECL_OBJFIRST);

	// --- Clamping ---

	REGISTER_METHOD(FVector2D, "FVector2D ClampAxes(double MinAxisVal, double MaxAxisVal) const", asFUNCTION(FVector2D_ClampAxes), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D GetClampedToSize(double Min, double Max) const", asFUNCTION(FVector2D_GetClampedToSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "FVector2D GetClampedToMaxSize(double MaxSize) const", asFUNCTION(FVector2D_GetClampedToMaxSize), asCALL_CDECL_OBJFIRST);

	// --- Rounding ---

	REGISTER_METHOD(FVector2D, "FVector2D RoundToVector() const", asFUNCTION(FVector2D_RoundToVector), asCALL_CDECL_OBJFIRST);

	// --- String ---

	REGISTER_METHOD(FVector2D, "FString ToString() const", asFUNCTION(FVector2D_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool InitFromString(const FString &in Source)", asFUNCTION(FVector2D_InitFromString), asCALL_CDECL_OBJFIRST);

	// --- Conversion ---

	REGISTER_METHOD(FVector2D, "FVector SphericalToUnitCartesian() const", asFUNCTION(FVector2D_SphericalToUnitCartesian), asCALL_CDECL_OBJFIRST);

	// --- Static helpers under FVector2D namespace ---

	Result = Engine->SetDefaultNamespace("FVector2D");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("double DotProduct(const FVector2D &in A, const FVector2D &in B)", asFUNCTION(FVector2D_DotProduct), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double CrossProduct(const FVector2D &in A, const FVector2D &in B)", asFUNCTION(FVector2D_CrossProduct), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double Distance(const FVector2D &in V1, const FVector2D &in V2)", asFUNCTION(FVector2D_Distance), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("double DistSquared(const FVector2D &in V1, const FVector2D &in V2)", asFUNCTION(FVector2D_DistSquared), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D Max(const FVector2D &in A, const FVector2D &in B)", asFUNCTION(FVector2D_Max), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D Min(const FVector2D &in A, const FVector2D &in B)", asFUNCTION(FVector2D_Min), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D Clamp(const FVector2D &in V, const FVector2D &in MinValue, const FVector2D &in MaxValue)", asFUNCTION(FVector2D_Clamp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D Zero()",  asFUNCTION(FVector2D_Zero),  asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D One()",   asFUNCTION(FVector2D_One),   asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D UnitX()", asFUNCTION(FVector2D_UnitX), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector2D UnitY()", asFUNCTION(FVector2D_UnitY), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}

