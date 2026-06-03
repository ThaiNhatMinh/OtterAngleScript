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
	// --- Constructors / destructor ---

	static void FVector3f_DefaultConstruct(FVector3f* Memory)
	{
		new (Memory) FVector3f();
	}

	static void FVector3f_CopyConstruct(const FVector3f& Other, FVector3f* Memory)
	{
		new (Memory) FVector3f(Other);
	}

	static void FVector3f_ConstructScalar(float InF, FVector3f* Memory)
	{
		new (Memory) FVector3f(InF);
	}

	static void FVector3f_ConstructXYZ(float X, float Y, float Z, FVector3f* Memory)
	{
		new (Memory) FVector3f(X, Y, Z);
	}

	static void FVector3f_ConstructVector2D(const FVector2D& Value, float Z, FVector3f* Memory)
	{
		//new (Memory) FVector3f(Value, Z);
	}

	static void FVector3f_Destruct(FVector3f* Memory)
	{
		Memory->~FVector3f();
	}

	// --- Assignment / comparison ---

	static FVector3f& FVector3f_Assign(FVector3f& Value, const FVector3f& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FVector3f_Equals(const FVector3f& Value, const FVector3f& Other)
	{
		return Value == Other;
	}

	static bool FVector3f_IsNearEquals(const FVector3f& Value, const FVector3f& Other, float Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	static FVector3f FVector3f_CrossOperator(const FVector3f& Value, const FVector3f& Other)
	{
		return Value ^ Other;
	}

	static float FVector3f_DotOperator(const FVector3f& Value, const FVector3f& Other)
	{
		return Value | Other;
	}

	// --- Arithmetic operators ---

	static FVector3f FVector3f_Add(const FVector3f& Value, const FVector3f& Other)
	{
		return Value + Other;
	}

	static FVector3f FVector3f_Subtract(const FVector3f& Value, const FVector3f& Other)
	{
		return Value - Other;
	}

	static FVector3f FVector3f_AddBias(const FVector3f& Value, float Bias)
	{
		return Value + Bias;
	}

	static FVector3f FVector3f_SubtractBias(const FVector3f& Value, float Bias)
	{
		return Value - Bias;
	}

	static FVector3f FVector3f_MultiplyScale(const FVector3f& Value, float Scale)
	{
		return Value * Scale;
	}

	static FVector3f FVector3f_MultiplyComponents(const FVector3f& Value, const FVector3f& Other)
	{
		return Value * Other;
	}

	static FVector3f FVector3f_DivideScale(const FVector3f& Value, float Scale)
	{
		return Value / Scale;
	}

	static FVector3f FVector3f_DivideComponents(const FVector3f& Value, const FVector3f& Other)
	{
		return Value / Other;
	}

	static FVector3f FVector3f_Negate(const FVector3f& Value)
	{
		return -Value;
	}

	// --- Compound assignment ---

	static void FVector3f_AddAssign(FVector3f& Value, const FVector3f& Other)
	{
		Value += Other;
	}

	static void FVector3f_SubtractAssign(FVector3f& Value, const FVector3f& Other)
	{
		Value -= Other;
	}

	static void FVector3f_MultiplyAssignScale(FVector3f& Value, float Scale)
	{
		Value *= Scale;
	}

	static void FVector3f_MultiplyAssignComponents(FVector3f& Value, const FVector3f& Other)
	{
		Value *= Other;
	}

	static void FVector3f_DivideAssignScale(FVector3f& Value, float Scale)
	{
		Value /= Scale;
	}

	static void FVector3f_DivideAssignComponents(FVector3f& Value, const FVector3f& Other)
	{
		Value /= Other;
	}

	// --- Index operators ---

	static float& SetVector3fIndexException(FVector3f& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.X;
	}

	static float& FVector3f_IndexRef(FVector3f& Value, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return Value.X;
			case 1: return Value.Y;
			case 2: return Value.Z;
			default: return SetVector3fIndexException(Value, "FVector3f index out of range");
		}
	}

	static float FVector3f_IndexValue(const FVector3f& Value, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return Value.X;
			case 1: return Value.Y;
			case 2: return Value.Z;
			default:
				SetScriptException("FVector3f index out of range");
				return 0.0f;
		}
	}

	// --- Component / set ---

	static bool FVector3f_IsValidIndex(const FVector3f& Value, int32 Index)
	{
		return Value.IsValidIndex(Index);
	}

	static void FVector3f_Set(FVector3f& Value, float InX, float InY, float InZ)
	{
		Value.Set(InX, InY, InZ);
	}

	static float FVector3f_GetMax(const FVector3f& Value)
	{
		return Value.GetMax();
	}

	static float FVector3f_GetAbsMax(const FVector3f& Value)
	{
		return Value.GetAbsMax();
	}

	static float FVector3f_GetMin(const FVector3f& Value)
	{
		return Value.GetMin();
	}

	static float FVector3f_GetAbsMin(const FVector3f& Value)
	{
		return Value.GetAbsMin();
	}

	static FVector3f FVector3f_ComponentMin(const FVector3f& Value, const FVector3f& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FVector3f FVector3f_ComponentMax(const FVector3f& Value, const FVector3f& Other)
	{
		return Value.ComponentMax(Other);
	}

	static FVector3f FVector3f_GetAbs(const FVector3f& Value)
	{
		return Value.GetAbs();
	}

	// --- Size / length ---

	static float FVector3f_Size(const FVector3f& Value)
	{
		return Value.Size();
	}

	static float FVector3f_SizeSquared(const FVector3f& Value)
	{
		return Value.SizeSquared();
	}

	static float FVector3f_Size2D(const FVector3f& Value)
	{
		return Value.Size2D();
	}

	static float FVector3f_SizeSquared2D(const FVector3f& Value)
	{
		return Value.SizeSquared2D();
	}

	// --- Predicates ---

	static bool FVector3f_IsNearlyZeroDefault(const FVector3f& Value)
	{
		return Value.IsNearlyZero();
	}

	static bool FVector3f_IsNearlyZeroTolerance(const FVector3f& Value, float Tolerance)
	{
		return Value.IsNearlyZero(Tolerance);
	}

	static bool FVector3f_IsZero(const FVector3f& Value)
	{
		return Value.IsZero();
	}

	static bool FVector3f_IsUnitDefault(const FVector3f& Value)
	{
		return Value.IsUnit();
	}

	static bool FVector3f_IsUnitTolerance(const FVector3f& Value, float LengthSquaredTolerance)
	{
		return Value.IsUnit(LengthSquaredTolerance);
	}

	static bool FVector3f_IsNormalized(const FVector3f& Value)
	{
		return Value.IsNormalized();
	}

	static bool FVector3f_ContainsNaN(const FVector3f& Value)
	{
		return Value.ContainsNaN();
	}

	// --- Normalization ---

	static bool FVector3f_NormalizeDefault(FVector3f& Value)
	{
		return Value.Normalize();
	}

	static bool FVector3f_NormalizeTolerance(FVector3f& Value, float Tolerance)
	{
		return Value.Normalize(Tolerance);
	}

	static FVector3f FVector3f_GetUnsafeNormal(const FVector3f& Value)
	{
		return Value.GetUnsafeNormal();
	}

	static FVector3f FVector3f_GetSafeNormalDefault(const FVector3f& Value)
	{
		return Value.GetSafeNormal();
	}

	static FVector3f FVector3f_GetSafeNormalTolerance(const FVector3f& Value, float Tolerance)
	{
		return Value.GetSafeNormal(Tolerance);
	}

	// --- Clamping ---

	static FVector3f FVector3f_GetClampedToSize(const FVector3f& Value, float Min, float Max)
	{
		return Value.GetClampedToSize(Min, Max);
	}

	static FVector3f FVector3f_GetClampedToMaxSize(const FVector3f& Value, float MaxSize)
	{
		return Value.GetClampedToMaxSize(MaxSize);
	}

	// --- Projection ---

	static FVector3f FVector3f_ProjectOnTo(const FVector3f& Value, const FVector3f& Other)
	{
		return Value.ProjectOnTo(Other);
	}

	static FVector3f FVector3f_ProjectOnToNormal(const FVector3f& Value, const FVector3f& Normal)
	{
		return Value.ProjectOnToNormal(Normal);
	}

	// --- String ---

	static FString FVector3f_ToString(const FVector3f& Value)
	{
		return Value.ToString();
	}

	static bool FVector3f_InitFromString(FVector3f& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	// --- Static helpers ---

	static FVector3f FVector3f_Zero()
	{
		return FVector3f::Zero();
	}

	static FVector3f FVector3f_One()
	{
		return FVector3f::One();
	}

	static FVector3f FVector3f_UnitX()
	{
		return FVector3f::UnitX();
	}

	static FVector3f FVector3f_UnitY()
	{
		return FVector3f::UnitY();
	}

	static FVector3f FVector3f_UnitZ()
	{
		return FVector3f::UnitZ();
	}

	static float FVector3f_DotProduct(const FVector3f& A, const FVector3f& B)
	{
		return FVector3f::DotProduct(A, B);
	}

	static FVector3f FVector3f_CrossProduct(const FVector3f& A, const FVector3f& B)
	{
		return FVector3f::CrossProduct(A, B);
	}

	static float FVector3f_Dist(const FVector3f& A, const FVector3f& B)
	{
		return FVector3f::Dist(A, B);
	}

	static float FVector3f_Distance(const FVector3f& A, const FVector3f& B)
	{
		return FVector3f::Distance(A, B);
	}
}

void Bind_FVector3f(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// --- Behaviors ---

	REGISTER_BEHAVIOUR(FVector3f, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector3f_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector3f, asBEHAVE_CONSTRUCT, "void f(const FVector3f &in Other)", asFUNCTION(FVector3f_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector3f, asBEHAVE_CONSTRUCT, "void f(float InF)", asFUNCTION(FVector3f_ConstructScalar), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector3f, asBEHAVE_CONSTRUCT, "void f(float X, float Y, float Z)", asFUNCTION(FVector3f_ConstructXYZ), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector3f, asBEHAVE_CONSTRUCT, "void f(const FVector2D &in Value, float Z)", asFUNCTION(FVector3f_ConstructVector2D), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector3f, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FVector3f_Destruct), asCALL_CDECL_OBJLAST);

	// --- Properties ---

	REGISTER_PROPERTY(FVector3f, "float X", X);
	REGISTER_PROPERTY(FVector3f, "float Y", Y);
	REGISTER_PROPERTY(FVector3f, "float Z", Z);

	// --- Assignment / comparison ---

	REGISTER_METHOD(FVector3f, "FVector3f &opAssign(const FVector3f &in Other)", asFUNCTION(FVector3f_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool opEquals(const FVector3f &in Other) const", asFUNCTION(FVector3f_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool Equals(const FVector3f &in Other, float Tolerance = 0.0001f) const", asFUNCTION(FVector3f_IsNearEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opXor(const FVector3f &in Other) const", asFUNCTION(FVector3f_CrossOperator), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float opOr(const FVector3f &in Other) const", asFUNCTION(FVector3f_DotOperator), asCALL_CDECL_OBJFIRST);

	// --- Arithmetic operators ---

	REGISTER_METHOD(FVector3f, "FVector3f opAdd(const FVector3f &in Other) const", asFUNCTION(FVector3f_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opSub(const FVector3f &in Other) const", asFUNCTION(FVector3f_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opSub(float Bias) const", asFUNCTION(FVector3f_SubtractBias), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opAdd(float Bias) const", asFUNCTION(FVector3f_AddBias), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opMul(float Scale) const", asFUNCTION(FVector3f_MultiplyScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opDiv(float Scale) const", asFUNCTION(FVector3f_DivideScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opMul(const FVector3f &in Other) const", asFUNCTION(FVector3f_MultiplyComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opDiv(const FVector3f &in Other) const", asFUNCTION(FVector3f_DivideComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f opNeg() const", asFUNCTION(FVector3f_Negate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f& opAddAssign(const FVector3f &in Other)", asFUNCTION(FVector3f_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f& opSubAssign(const FVector3f &in Other)", asFUNCTION(FVector3f_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f& opMulAssign(float Scale)", asFUNCTION(FVector3f_MultiplyAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f& opDivAssign(float Scale)", asFUNCTION(FVector3f_DivideAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f& opMulAssign(const FVector3f &in Other)", asFUNCTION(FVector3f_MultiplyAssignComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f& opDivAssign(const FVector3f &in Other)", asFUNCTION(FVector3f_DivideAssignComponents), asCALL_CDECL_OBJFIRST);

	// --- Index operators ---

	REGISTER_METHOD(FVector3f, "float &opIndex(uint Index)", asFUNCTION(FVector3f_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float opIndex(uint Index) const", asFUNCTION(FVector3f_IndexValue), asCALL_CDECL_OBJFIRST);

	// --- Component / set ---

	REGISTER_METHOD(FVector3f, "bool IsValidIndex(int Index) const", asFUNCTION(FVector3f_IsValidIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "void Set(float X, float Y, float Z)", asFUNCTION(FVector3f_Set), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float GetMax() const", asFUNCTION(FVector3f_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float GetAbsMax() const", asFUNCTION(FVector3f_GetAbsMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float GetMin() const", asFUNCTION(FVector3f_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float GetAbsMin() const", asFUNCTION(FVector3f_GetAbsMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f ComponentMin(const FVector3f &in Other) const", asFUNCTION(FVector3f_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f ComponentMax(const FVector3f &in Other) const", asFUNCTION(FVector3f_ComponentMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f GetAbs() const", asFUNCTION(FVector3f_GetAbs), asCALL_CDECL_OBJFIRST);

	// --- Size / length ---

	REGISTER_METHOD(FVector3f, "float Size() const", asFUNCTION(FVector3f_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float SizeSquared() const", asFUNCTION(FVector3f_SizeSquared), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float Size2D() const", asFUNCTION(FVector3f_Size2D), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "float SizeSquared2D() const", asFUNCTION(FVector3f_SizeSquared2D), asCALL_CDECL_OBJFIRST);

	// --- Predicates ---

	REGISTER_METHOD(FVector3f, "bool IsNearlyZero() const", asFUNCTION(FVector3f_IsNearlyZeroDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool IsNearlyZero(float Tolerance) const", asFUNCTION(FVector3f_IsNearlyZeroTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool IsZero() const", asFUNCTION(FVector3f_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool IsUnit() const", asFUNCTION(FVector3f_IsUnitDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool IsUnit(float LengthSquaredTolerance) const", asFUNCTION(FVector3f_IsUnitTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool IsNormalized() const", asFUNCTION(FVector3f_IsNormalized), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool ContainsNaN() const", asFUNCTION(FVector3f_ContainsNaN), asCALL_CDECL_OBJFIRST);

	// --- Normalization ---

	REGISTER_METHOD(FVector3f, "bool Normalize()", asFUNCTION(FVector3f_NormalizeDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool Normalize(float Tolerance)", asFUNCTION(FVector3f_NormalizeTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f GetUnsafeNormal() const", asFUNCTION(FVector3f_GetUnsafeNormal), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f GetSafeNormal() const", asFUNCTION(FVector3f_GetSafeNormalDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f GetSafeNormal(float Tolerance) const", asFUNCTION(FVector3f_GetSafeNormalTolerance), asCALL_CDECL_OBJFIRST);

	// --- Clamping ---

	REGISTER_METHOD(FVector3f, "FVector3f GetClampedToSize(float Min, float Max) const", asFUNCTION(FVector3f_GetClampedToSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f GetClampedToMaxSize(float MaxSize) const", asFUNCTION(FVector3f_GetClampedToMaxSize), asCALL_CDECL_OBJFIRST);

	// --- Projection ---

	REGISTER_METHOD(FVector3f, "FVector3f ProjectOnTo(const FVector3f &in Other) const", asFUNCTION(FVector3f_ProjectOnTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "FVector3f ProjectOnToNormal(const FVector3f &in Normal) const", asFUNCTION(FVector3f_ProjectOnToNormal), asCALL_CDECL_OBJFIRST);

	// --- String ---

	REGISTER_METHOD(FVector3f, "FString ToString() const", asFUNCTION(FVector3f_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector3f, "bool InitFromString(const FString &in Source)", asFUNCTION(FVector3f_InitFromString), asCALL_CDECL_OBJFIRST);

	// --- Static helpers under FVector3f namespace ---

	Result = Engine->SetDefaultNamespace("FVector3f");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FVector3f Zero()", asFUNCTION(FVector3f_Zero), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector3f One()", asFUNCTION(FVector3f_One), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector3f UnitX()", asFUNCTION(FVector3f_UnitX), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector3f UnitY()", asFUNCTION(FVector3f_UnitY), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector3f UnitZ()", asFUNCTION(FVector3f_UnitZ), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("float DotProduct(const FVector3f &in A, const FVector3f &in B)", asFUNCTION(FVector3f_DotProduct), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector3f CrossProduct(const FVector3f &in A, const FVector3f &in B)", asFUNCTION(FVector3f_CrossProduct), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("float Dist(const FVector3f &in A, const FVector3f &in B)", asFUNCTION(FVector3f_Dist), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("float Distance(const FVector3f &in A, const FVector3f &in B)", asFUNCTION(FVector3f_Distance), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
