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

	bool FBox_GetIsValid(const FBox& Value)
	{
		return Value.IsValid != 0;
	}

	void FBox_SetIsValid(FBox& Value, bool bIsValid)
	{
		Value.IsValid = bIsValid ? 1 : 0;
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

	REGISTER_METHOD(FBox, "FBox &opAssign(const FBox &in Other)", asMETHODPR(FBox, operator=, (const FBox&), FBox&), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool opEquals(const FBox &in Other) const", asMETHODPR(FBox, operator==, (const FBox&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool Equals(const FBox &in Other, double Tolerance = 0.0001) const", asMETHODPR(FBox, Equals, (const FBox&, double) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox opAdd(const FVector &in Other) const", asMETHODPR(FBox, operator+, (const FVector&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox opAdd(const FBox &in Other) const", asMETHODPR(FBox, operator+, (const FBox&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox opAddAssign(const FVector &in Other)", asMETHODPR(FBox, operator+=, (const FVector&), FBox&), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox opAddAssign(const FBox &in Other)", asMETHODPR(FBox, operator+=, (const FBox&), FBox&), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FVector &opIndex(uint Index)", asFUNCTION(FBox_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector opIndex(uint Index) const", asFUNCTION(FBox_IndexValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "bool ContainsNaN() const", asMETHODPR(FBox, ContainsNaN, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "double ComputeSquaredDistanceToPoint(const FVector &in Point) const", asMETHODPR(FBox, ComputeSquaredDistanceToPoint, (const FVector&) const, double), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "double ComputeSquaredDistanceToBox(const FBox &in Other) const", asMETHODPR(FBox, ComputeSquaredDistanceToBox, (const FBox&) const, double), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox ExpandBy(double Amount) const", asMETHODPR(FBox, ExpandBy, (double) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox ExpandBy(const FVector &in Amount) const", asMETHODPR(FBox, ExpandBy, (const FVector&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox ExpandBy(const FVector &in Negative, const FVector &in Positive) const", asMETHODPR(FBox, ExpandBy, (const FVector&, const FVector&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox ShiftBy(const FVector &in Offset) const", asMETHODPR(FBox, ShiftBy, (const FVector&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox MoveTo(const FVector &in Destination) const", asMETHODPR(FBox, MoveTo, (const FVector&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FVector GetCenter() const", asMETHODPR(FBox, GetCenter, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "void GetCenterAndExtents(FVector &out Center, FVector &out Extents) const", asMETHODPR(FBox, GetCenterAndExtents, (FVector&, FVector&) const, void), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FVector GetClosestPointTo(const FVector &in Point) const", asMETHODPR(FBox, GetClosestPointTo, (const FVector&) const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "void GetVertices(FVector &out Vertex0, FVector &out Vertex1, FVector &out Vertex2, FVector &out Vertex3, FVector &out Vertex4, FVector &out Vertex5, FVector &out Vertex6, FVector &out Vertex7) const", asFUNCTION(FBox_GetVertices), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "FVector GetExtent() const", asMETHODPR(FBox, GetExtent, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FVector GetSize() const", asMETHODPR(FBox, GetSize, () const, FVector), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "double GetVolume() const", asMETHODPR(FBox, GetVolume, () const, double), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "void Init()", asMETHODPR(FBox, Init, (), void), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool Intersect(const FBox &in Other) const", asMETHODPR(FBox, Intersect, (const FBox&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IntersectXY(const FBox &in Other) const", asMETHODPR(FBox, IntersectXY, (const FBox&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox Overlap(const FBox &in Other) const", asMETHODPR(FBox, Overlap, (const FBox&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox InverseTransformBy(const FTransform &in Transform) const", asMETHODPR(FBox, InverseTransformBy, (const FTransform&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInside(const FVector &in Point) const", asMETHODPR(FBox, IsInside, (const FVector&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInsideOrOn(const FVector &in Point) const", asMETHODPR(FBox, IsInsideOrOn, (const FVector&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInside(const FBox &in Other) const", asMETHODPR(FBox, IsInside, (const FBox&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInsideOrOn(const FBox &in Other) const", asMETHODPR(FBox, IsInsideOrOn, (const FBox&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInsideXY(const FVector &in Point) const", asMETHODPR(FBox, IsInsideXY, (const FVector&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInsideOrOnXY(const FVector &in Point) const", asMETHODPR(FBox, IsInsideOrOnXY, (const FVector&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool IsInsideXY(const FBox &in Other) const", asMETHODPR(FBox, IsInsideXY, (const FBox&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FBox TransformBy(const FTransform &in Transform) const", asMETHODPR(FBox, TransformBy, (const FTransform&) const, FBox), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FString ToString() const", asMETHODPR(FBox, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "FString ToCompactString() const", asMETHODPR(FBox, ToCompactString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FBox, "bool get_IsValid() const property", asFUNCTION(FBox_GetIsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FBox, "void set_IsValid(bool Value) property", asFUNCTION(FBox_SetIsValid), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FBox, "FVector Min", Min);
	REGISTER_PROPERTY(FBox, "FVector Max", Max);

	Result = Engine->SetDefaultNamespace("FBox");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FBox BuildAABB(const FVector &in Origin, const FVector &in Extent)", asFUNCTION(FBox::BuildAABB), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
