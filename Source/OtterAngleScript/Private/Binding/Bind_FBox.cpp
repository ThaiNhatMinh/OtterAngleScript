// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Box.h"
#include "Math/Transform.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	FVector& SetBoxIndexException(FBox& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.Min;
	}

	void FBox_DefaultConstruct(FBox* Memory)
	{
		new (Memory) FBox();
	}

	void FBox_ConstructForceInit(EForceInit ForceInitValue, FBox* Memory)
	{
		new (Memory) FBox(ForceInitValue);
	}

	void FBox_CopyConstruct(const FBox& Other, FBox* Memory)
	{
		new (Memory) FBox(Other);
	}

	void FBox_ConstructMinMax(const FVector& InMin, const FVector& InMax, FBox* Memory)
	{
		new (Memory) FBox(InMin, InMax);
	}

	void FBox_Destruct(FBox* Memory)
	{
		Memory->~FBox();
	}

	FBox& FBox_Assign(FBox& Value, const FBox& Other)
	{
		Value = Other;
		return Value;
	}

	bool FBox_EqualsExact(const FBox& Value, const FBox& Other)
	{
		return Value == Other;
	}

	bool FBox_EqualsTolerance(const FBox& Value, const FBox& Other, double Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	FBox FBox_AddPoint(const FBox& Value, const FVector& Other)
	{
		return Value + Other;
	}

	FBox FBox_AddBox(const FBox& Value, const FBox& Other)
	{
		return Value + Other;
	}

	FBox FBox_AddAssignPoint(FBox& Value, const FVector& Other)
	{
		return Value += Other;
	}

	FBox FBox_AddAssignBox(FBox& Value, const FBox& Other)
	{
		return Value += Other;
	}

	FVector& FBox_IndexRef(FBox& Value, unsigned int Index)
	{
		switch (Index)
		{
		case 0:
			return Value.Min;
		case 1:
			return Value.Max;
		default:
			return SetBoxIndexException(Value, "FBox index out of bounds");
		}
	}

	FVector FBox_IndexValue(const FBox& Value, unsigned int Index)
	{
		switch (Index)
		{
		case 0:
			return Value.Min;
		case 1:
			return Value.Max;
		default:
			SetScriptException("FBox index out of bounds");
			return Value.Min;
		}
	}

	bool FBox_ContainsNaN(const FBox& Value)
	{
		return Value.ContainsNaN();
	}

	double FBox_ComputeSquaredDistanceToPoint(const FBox& Value, const FVector& Point)
	{
		return Value.ComputeSquaredDistanceToPoint(Point);
	}

	double FBox_ComputeSquaredDistanceToBox(const FBox& Value, const FBox& Other)
	{
		return Value.ComputeSquaredDistanceToBox(Other);
	}

	FBox FBox_ExpandByScalar(const FBox& Value, double Amount)
	{
		return Value.ExpandBy(Amount);
	}

	FBox FBox_ExpandByVector(const FBox& Value, const FVector& Amount)
	{
		return Value.ExpandBy(Amount);
	}

	FBox FBox_ExpandByMinMax(const FBox& Value, const FVector& Negative, const FVector& Positive)
	{
		return Value.ExpandBy(Negative, Positive);
	}

	FBox FBox_ShiftBy(const FBox& Value, const FVector& Offset)
	{
		return Value.ShiftBy(Offset);
	}

	FBox FBox_MoveTo(const FBox& Value, const FVector& Destination)
	{
		return Value.MoveTo(Destination);
	}

	FVector FBox_GetCenter(const FBox& Value)
	{
		return Value.GetCenter();
	}

	void FBox_GetCenterAndExtents(const FBox& Value, FVector& Center, FVector& Extents)
	{
		Value.GetCenterAndExtents(Center, Extents);
	}

	FVector FBox_GetClosestPointTo(const FBox& Value, const FVector& Point)
	{
		return Value.GetClosestPointTo(Point);
	}

	void FBox_GetVertices(
		const FBox& Value,
		FVector& Vertex0,
		FVector& Vertex1,
		FVector& Vertex2,
		FVector& Vertex3,
		FVector& Vertex4,
		FVector& Vertex5,
		FVector& Vertex6,
		FVector& Vertex7)
	{
		FVector Vertices[8];
		Value.GetVertices(Vertices);
		Vertex0 = Vertices[0];
		Vertex1 = Vertices[1];
		Vertex2 = Vertices[2];
		Vertex3 = Vertices[3];
		Vertex4 = Vertices[4];
		Vertex5 = Vertices[5];
		Vertex6 = Vertices[6];
		Vertex7 = Vertices[7];
	}

	FVector FBox_GetExtent(const FBox& Value)
	{
		return Value.GetExtent();
	}

	FVector FBox_GetSize(const FBox& Value)
	{
		return Value.GetSize();
	}

	double FBox_GetVolume(const FBox& Value)
	{
		return Value.GetVolume();
	}

	void FBox_Init(FBox& Value)
	{
		Value.Init();
	}

	bool FBox_Intersect(const FBox& Value, const FBox& Other)
	{
		return Value.Intersect(Other);
	}

	bool FBox_IntersectXY(const FBox& Value, const FBox& Other)
	{
		return Value.IntersectXY(Other);
	}

	FBox FBox_Overlap(const FBox& Value, const FBox& Other)
	{
		return Value.Overlap(Other);
	}

	FBox FBox_InverseTransformBy(const FBox& Value, const FTransform& Transform)
	{
		return Value.InverseTransformBy(Transform);
	}

	bool FBox_IsInsidePoint(const FBox& Value, const FVector& Point)
	{
		return Value.IsInside(Point);
	}

	bool FBox_IsInsideOrOnPoint(const FBox& Value, const FVector& Point)
	{
		return Value.IsInsideOrOn(Point);
	}

	bool FBox_IsInsideBox(const FBox& Value, const FBox& Other)
	{
		return Value.IsInside(Other);
	}

	bool FBox_IsInsideOrOnBox(const FBox& Value, const FBox& Other)
	{
		return Value.IsInsideOrOn(Other);
	}

	bool FBox_IsInsideXYPoint(const FBox& Value, const FVector& Point)
	{
		return Value.IsInsideXY(Point);
	}

	bool FBox_IsInsideOrOnXYPoint(const FBox& Value, const FVector& Point)
	{
		return Value.IsInsideOrOnXY(Point);
	}

	bool FBox_IsInsideXYBox(const FBox& Value, const FBox& Other)
	{
		return Value.IsInsideXY(Other);
	}

	FBox FBox_TransformBy(const FBox& Value, const FTransform& Transform)
	{
		return Value.TransformBy(Transform);
	}

	FString FBox_ToString(const FBox& Value)
	{
		return Value.ToString();
	}

	FString FBox_ToCompactString(const FBox& Value)
	{
		return Value.ToCompactString();
	}

	bool FBox_GetIsValid(const FBox& Value)
	{
		return Value.IsValid != 0;
	}

	void FBox_SetIsValid(FBox& Value, bool bIsValid)
	{
		Value.IsValid = bIsValid ? 1 : 0;
	}

	FBox FBox_BuildAABB(const FVector& Origin, const FVector& Extent)
	{
		return FBox::BuildAABB(Origin, Extent);
	}
}

void Bind_FBox(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FBox",
		sizeof(FBox),
		asOBJ_VALUE | asGetTypeTraits<FBox>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FBox, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FBox_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FBox_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox, asBEHAVE_CONSTRUCT, "void f(const FBox &in Other)", asFUNCTION(FBox_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox, asBEHAVE_CONSTRUCT, "void f(const FVector &in InMin, const FVector &in InMax)", asFUNCTION(FBox_ConstructMinMax), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FBox, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FBox_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FBox, "FBox &opAssign(const FBox &in Other)", asFUNCTION(FBox_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool opEquals(const FBox &in Other) const", asFUNCTION(FBox_EqualsExact), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool Equals(const FBox &in Other, double Tolerance = 0.0001) const", asFUNCTION(FBox_EqualsTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox opAdd(const FVector &in Other) const", asFUNCTION(FBox_AddPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox opAdd(const FBox &in Other) const", asFUNCTION(FBox_AddBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox opAddAssign(const FVector &in Other)", asFUNCTION(FBox_AddAssignPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox opAddAssign(const FBox &in Other)", asFUNCTION(FBox_AddAssignBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector &opIndex(uint Index)", asFUNCTION(FBox_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector opIndex(uint Index) const", asFUNCTION(FBox_IndexValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool ContainsNaN() const", asFUNCTION(FBox_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "double ComputeSquaredDistanceToPoint(const FVector &in Point) const", asFUNCTION(FBox_ComputeSquaredDistanceToPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "double ComputeSquaredDistanceToBox(const FBox &in Other) const", asFUNCTION(FBox_ComputeSquaredDistanceToBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox ExpandBy(double Amount) const", asFUNCTION(FBox_ExpandByScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox ExpandBy(const FVector &in Amount) const", asFUNCTION(FBox_ExpandByVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox ExpandBy(const FVector &in Negative, const FVector &in Positive) const", asFUNCTION(FBox_ExpandByMinMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox ShiftBy(const FVector &in Offset) const", asFUNCTION(FBox_ShiftBy), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox MoveTo(const FVector &in Destination) const", asFUNCTION(FBox_MoveTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector GetCenter() const", asFUNCTION(FBox_GetCenter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "void GetCenterAndExtents(FVector &out Center, FVector &out Extents) const", asFUNCTION(FBox_GetCenterAndExtents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector GetClosestPointTo(const FVector &in Point) const", asFUNCTION(FBox_GetClosestPointTo), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "void GetVertices(FVector &out Vertex0, FVector &out Vertex1, FVector &out Vertex2, FVector &out Vertex3, FVector &out Vertex4, FVector &out Vertex5, FVector &out Vertex6, FVector &out Vertex7) const", asFUNCTION(FBox_GetVertices), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector GetExtent() const", asFUNCTION(FBox_GetExtent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector GetSize() const", asFUNCTION(FBox_GetSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "double GetVolume() const", asFUNCTION(FBox_GetVolume), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "void Init()", asFUNCTION(FBox_Init), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool Intersect(const FBox &in Other) const", asFUNCTION(FBox_Intersect), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IntersectXY(const FBox &in Other) const", asFUNCTION(FBox_IntersectXY), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox Overlap(const FBox &in Other) const", asFUNCTION(FBox_Overlap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox InverseTransformBy(const FTransform &in Transform) const", asFUNCTION(FBox_InverseTransformBy), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInside(const FVector &in Point) const", asFUNCTION(FBox_IsInsidePoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInsideOrOn(const FVector &in Point) const", asFUNCTION(FBox_IsInsideOrOnPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInside(const FBox &in Other) const", asFUNCTION(FBox_IsInsideBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInsideOrOn(const FBox &in Other) const", asFUNCTION(FBox_IsInsideOrOnBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInsideXY(const FVector &in Point) const", asFUNCTION(FBox_IsInsideXYPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInsideOrOnXY(const FVector &in Point) const", asFUNCTION(FBox_IsInsideOrOnXYPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool IsInsideXY(const FBox &in Other) const", asFUNCTION(FBox_IsInsideXYBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FBox TransformBy(const FTransform &in Transform) const", asFUNCTION(FBox_TransformBy), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FString ToString() const", asFUNCTION(FBox_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FString ToCompactString() const", asFUNCTION(FBox_ToCompactString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool get_IsValid() const property", asFUNCTION(FBox_GetIsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "void set_IsValid(bool Value) property", asFUNCTION(FBox_SetIsValid), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FBox, "FVector Min", Min);
	REGISTER_PROPERTY(FBox, "FVector Max", Max);

	Result = Engine->SetDefaultNamespace("FBox");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FBox BuildAABB(const FVector &in Origin, const FVector &in Extent)", asFUNCTION(FBox_BuildAABB), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
