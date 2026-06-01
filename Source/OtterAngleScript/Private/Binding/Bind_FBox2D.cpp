// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Box2D.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	static FVector2D& SetBox2DIndexException(FBox2D& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.Min;
	}

	// --- Constructors / destructor ---

	static void FBox2D_DefaultConstruct(FBox2D* Memory)
	{
		new (Memory) FBox2D();
	}

	static void FBox2D_ConstructForceInit(EForceInit ForceInitValue, FBox2D* Memory)
	{
		new (Memory) FBox2D(ForceInitValue);
	}

	static void FBox2D_CopyConstruct(const FBox2D& Other, FBox2D* Memory)
	{
		new (Memory) FBox2D(Other);
	}

	static void FBox2D_ConstructMinMax(const FVector2D& InMin, const FVector2D& InMax, FBox2D* Memory)
	{
		new (Memory) FBox2D(InMin, InMax);
	}

	static void FBox2D_Destruct(FBox2D* Memory)
	{
		Memory->~FBox2D();
	}

	// --- Assignment / comparison ---

	static FBox2D& FBox2D_Assign(FBox2D& Value, const FBox2D& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FBox2D_Equals_Op(const FBox2D& Value, const FBox2D& Other)
	{
		return Value == Other;
	}

	// --- Operators: extend box ---

	static FBox2D FBox2D_AddPoint(const FBox2D& Value, const FVector2D& Point)
	{
		return Value + Point;
	}

	static FBox2D FBox2D_AddBox(const FBox2D& Value, const FBox2D& Other)
	{
		return Value + Other;
	}

	static FBox2D& FBox2D_AddAssignPoint(FBox2D& Value, const FVector2D& Point)
	{
		return Value += Point;
	}

	static FBox2D& FBox2D_AddAssignBox(FBox2D& Value, const FBox2D& Other)
	{
		return Value += Other;
	}

	// --- Index operator ---

	static FVector2D& FBox2D_IndexRef(FBox2D& Value, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return Value.Min;
			case 1: return Value.Max;
			default: return SetBox2DIndexException(Value, "FBox2D index out of range");
		}
	}

	static FVector2D FBox2D_IndexValue(const FBox2D& Value, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return Value.Min;
			case 1: return Value.Max;
			default:
				SetScriptException("FBox2D index out of range");
				return FVector2D();
		}
	}

	// --- Geometry methods ---

	static double FBox2D_ComputeSquaredDistanceToPoint(const FBox2D& Value, const FVector2D& Point)
	{
		return Value.ComputeSquaredDistanceToPoint(Point);
	}

	static FBox2D FBox2D_ExpandByScalar(const FBox2D& Value, double W)
	{
		return Value.ExpandBy(W);
	}

	static FBox2D FBox2D_ExpandByVector(const FBox2D& Value, const FVector2D& V)
	{
		return Value.ExpandBy(V);
	}

	static double FBox2D_GetArea(const FBox2D& Value)
	{
		return Value.GetArea();
	}

	static FVector2D FBox2D_GetCenter(const FBox2D& Value)
	{
		return Value.GetCenter();
	}

	static void FBox2D_GetCenterAndExtents(const FBox2D& Value, FVector2D& Center, FVector2D& Extents)
	{
		Value.GetCenterAndExtents(Center, Extents);
	}

	static FVector2D FBox2D_GetClosestPointTo(const FBox2D& Value, const FVector2D& Point)
	{
		return Value.GetClosestPointTo(Point);
	}

	static FVector2D FBox2D_GetExtent(const FBox2D& Value)
	{
		return Value.GetExtent();
	}

	static FVector2D FBox2D_GetSize(const FBox2D& Value)
	{
		return Value.GetSize();
	}

	static void FBox2D_Init(FBox2D& Value)
	{
		Value.Init();
	}

	static FBox2D FBox2D_Overlap(const FBox2D& Value, const FBox2D& Other)
	{
		return Value.Overlap(Other);
	}

	static bool FBox2D_Intersect(const FBox2D& Value, const FBox2D& Other)
	{
		return Value.Intersect(Other);
	}

	static bool FBox2D_IsInsidePoint(const FBox2D& Value, const FVector2D& TestPoint)
	{
		return Value.IsInside(TestPoint);
	}

	static bool FBox2D_IsInsideOrOn(const FBox2D& Value, const FVector2D& TestPoint)
	{
		return Value.IsInsideOrOn(TestPoint);
	}

	static bool FBox2D_IsInsideBox(const FBox2D& Value, const FBox2D& Other)
	{
		return Value.IsInside(Other);
	}

	static FBox2D FBox2D_ShiftBy(const FBox2D& Value, const FVector2D& Offset)
	{
		return Value.ShiftBy(Offset);
	}

	static FBox2D FBox2D_MoveTo(const FBox2D& Value, const FVector2D& Destination)
	{
		return Value.MoveTo(Destination);
	}

	static bool FBox2D_EqualsDefault(const FBox2D& Value, const FBox2D& Other)
	{
		return Value.Equals(Other);
	}

	static bool FBox2D_EqualsTolerance(const FBox2D& Value, const FBox2D& Other, double Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	static FString FBox2D_ToString(const FBox2D& Value)
	{
		return Value.ToString();
	}

	// --- Static helper ---

	static FBox2D FBox2D_BuildAABB(const FVector2D& Origin, const FVector2D& Extent)
	{
		return FBox2D::BuildAABB(Origin, Extent);
	}
}

void Bind_FBox2D(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// --- Behaviors ---

	REGISTER_BEHAVIOUR(FBox2D, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FBox2D_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox2D, asBEHAVE_CONSTRUCT, "void f(EForceInitType)", asFUNCTION(FBox2D_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox2D, asBEHAVE_CONSTRUCT, "void f(const FBox2D &in Other)", asFUNCTION(FBox2D_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox2D, asBEHAVE_CONSTRUCT, "void f(const FVector2D &in InMin, const FVector2D &in InMax)", asFUNCTION(FBox2D_ConstructMinMax), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox2D, asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(FBox2D_Destruct), asCALL_CDECL_OBJLAST);

	// --- Properties ---

	REGISTER_PROPERTY(FBox2D, "FVector2D Min", Min);
	REGISTER_PROPERTY(FBox2D, "FVector2D Max", Max);
	REGISTER_PROPERTY(FBox2D, "bool bIsValid", bIsValid);

	// --- Assignment / comparison ---

	REGISTER_METHOD(FBox2D, "FBox2D &opAssign(const FBox2D &in Other)", asFUNCTION(FBox2D_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool opEquals(const FBox2D &in Other) const", asFUNCTION(FBox2D_Equals_Op), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool Equals(const FBox2D &in Other) const", asFUNCTION(FBox2D_EqualsDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool Equals(const FBox2D &in Other, double Tolerance) const", asFUNCTION(FBox2D_EqualsTolerance), asCALL_CDECL_OBJFIRST);

	// --- Extend operators ---

	REGISTER_METHOD(FBox2D, "FBox2D opAdd(const FVector2D &in Point) const", asFUNCTION(FBox2D_AddPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D opAdd(const FBox2D &in Other) const", asFUNCTION(FBox2D_AddBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D &opAddAssign(const FVector2D &in Point)", asFUNCTION(FBox2D_AddAssignPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D &opAddAssign(const FBox2D &in Other)", asFUNCTION(FBox2D_AddAssignBox), asCALL_CDECL_OBJFIRST);

	// --- Index operator ---

	REGISTER_METHOD(FBox2D, "FVector2D &opIndex(uint Index)", asFUNCTION(FBox2D_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FVector2D opIndex(uint Index) const", asFUNCTION(FBox2D_IndexValue), asCALL_CDECL_OBJFIRST);

	// --- Geometry methods ---

	REGISTER_METHOD(FBox2D, "double ComputeSquaredDistanceToPoint(const FVector2D &in Point) const", asFUNCTION(FBox2D_ComputeSquaredDistanceToPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D ExpandBy(double W) const", asFUNCTION(FBox2D_ExpandByScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D ExpandBy(const FVector2D &in V) const", asFUNCTION(FBox2D_ExpandByVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "double GetArea() const", asFUNCTION(FBox2D_GetArea), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FVector2D GetCenter() const", asFUNCTION(FBox2D_GetCenter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "void GetCenterAndExtents(FVector2D &out Center, FVector2D &out Extents) const", asFUNCTION(FBox2D_GetCenterAndExtents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FVector2D GetClosestPointTo(const FVector2D &in Point) const", asFUNCTION(FBox2D_GetClosestPointTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FVector2D GetExtent() const", asFUNCTION(FBox2D_GetExtent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FVector2D GetSize() const", asFUNCTION(FBox2D_GetSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "void Init()", asFUNCTION(FBox2D_Init), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D Overlap(const FBox2D &in Other) const", asFUNCTION(FBox2D_Overlap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool Intersect(const FBox2D &in Other) const", asFUNCTION(FBox2D_Intersect), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool IsInside(const FVector2D &in TestPoint) const", asFUNCTION(FBox2D_IsInsidePoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool IsInsideOrOn(const FVector2D &in TestPoint) const", asFUNCTION(FBox2D_IsInsideOrOn), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "bool IsInside(const FBox2D &in Other) const", asFUNCTION(FBox2D_IsInsideBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D ShiftBy(const FVector2D &in Offset) const", asFUNCTION(FBox2D_ShiftBy), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FBox2D MoveTo(const FVector2D &in Destination) const", asFUNCTION(FBox2D_MoveTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox2D, "FString ToString() const", asFUNCTION(FBox2D_ToString), asCALL_CDECL_OBJFIRST);

	// --- Static helper under FBox2D namespace ---

	Result = Engine->SetDefaultNamespace("FBox2D");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"FBox2D BuildAABB(const FVector2D &in Origin, const FVector2D &in Extent)",
		asFUNCTION(FBox2D_BuildAABB), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
