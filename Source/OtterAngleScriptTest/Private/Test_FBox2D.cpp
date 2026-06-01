// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Box2D.h"
#include "Math/Vector2D.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FBox2D UnrealBuildBox2D(double MinX, double MinY, double MaxX, double MaxY)
	{
		return FBox2D(FVector2D(MinX, MinY), FVector2D(MaxX, MaxY));
	}

	bool UnrealAcceptsBox2D(const FBox2D& Value)
	{
		return Value.Equals(FBox2D(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0)), 0.0001);
	}

	double UnrealBox2DArea(const FBox2D& Value)
	{
		return Value.GetArea();
	}

	FString UnrealBox2DToString(const FBox2D& Value)
	{
		return Value.ToString();
	}

	bool RegisterFBox2DInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FBox2D UnrealBuildBox2D(double MinX, double MinY, double MaxX, double MaxY)",
			asFUNCTION(UnrealBuildBox2D),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsBox2D(const FBox2D &in Value)",
			asFUNCTION(UnrealAcceptsBox2D),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealBox2DArea(const FBox2D &in Value)",
			asFUNCTION(UnrealBox2DArea),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FString UnrealBox2DToString(const FBox2D &in Value)",
			asFUNCTION(UnrealBox2DToString),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFBox2DTests,
	"OtterAngleScript.FBox2D",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	asIScriptEngine* Engine = nullptr;
	asIScriptModule* ScriptModule = nullptr;
	asIScriptContext* Context = nullptr;

	asIScriptFunction* BuildFunction(const char* SectionName, const char* Script, const char* Declaration)
	{
		if (!Assert.IsTrue(ScriptModule->AddScriptSection(SectionName, Script) >= 0))
		{
			return nullptr;
		}
		if (!Assert.IsTrue(ScriptModule->Build() >= 0))
		{
			return nullptr;
		}

		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl(Declaration);
		if (!Assert.IsNotNull(Function))
		{
			return nullptr;
		}
		return Function;
	}

	int ExecuteFunction(asIScriptFunction* Function)
	{
		if (Context != nullptr)
		{
			Context->Release();
			Context = nullptr;
		}

		Context = Engine->CreateContext();
		if (!Assert.IsNotNull(Context))
		{
			return -1;
		}
		if (!Assert.IsTrue(Context->Prepare(Function) >= 0))
		{
			return -1;
		}

		return Context->Execute();
	}

	int32 ExecuteIntFunction(asIScriptFunction* Function)
	{
		const int Result = ExecuteFunction(Function);
		if (Result == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		if (!Assert.IsTrue(Result == asEXECUTION_FINISHED))
		{
			return -1;
		}

		return static_cast<int32>(Context->GetReturnDWord());
	}

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));
		ASSERT_THAT(IsTrue(RegisterFBox2DInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptBox2DTest", asGM_ALWAYS_CREATE);
		ASSERT_THAT(IsNotNull(ScriptModule));
	}

	AFTER_EACH()
	{
		if (Context != nullptr)
		{
			Context->Release();
			Context = nullptr;
		}

		if (ScriptModule != nullptr)
		{
			ScriptModule->Discard();
			ScriptModule = nullptr;
		}

		Engine = nullptr;
	}

	TEST_METHOD(TypesRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FBox2D")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector2D")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));
	}

	TEST_METHOD(ForceInitConstructor)
	{
		static const char Script[] = R"(
int RunForceInitConstructor()
{
    FBox2D Box(ForceInit);
    if (Box.bIsValid)
    {
        return -1;
    }
    if (!Box.Min.Equals(FVector2D(0.0, 0.0), 0.0001) || !Box.Max.Equals(FVector2D(0.0, 0.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DForceInitConstructor", Script, "int RunForceInitConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MinMaxConstructor)
	{
		static const char Script[] = R"(
int RunMinMaxConstructor()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    if (!Box.bIsValid)
    {
        return -1;
    }
    if (!Box.opEquals(UnrealBuildBox2D(1.0, 2.0, 4.0, 5.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DMinMaxConstructor", Script, "int RunMinMaxConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FBox2D Original(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    FBox2D Copy(Original);
    if (!Copy.opEquals(Original))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunAssignOperator()
{
    FBox2D Source(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    FBox2D Dest(ForceInit);
    Dest = Source;
    if (!Dest.opEquals(Source))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DAssignOperator", Script, "int RunAssignOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(EqualityOperator)
	{
		static const char Script[] = R"(
int RunEqualityOperator()
{
    FBox2D A(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    FBox2D B(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    FBox2D C(FVector2D(0.0, 0.0), FVector2D(1.0, 1.0));
    if (!A.opEquals(B) || A.opEquals(C))
    {
        return -1;
    }
    if (!A.Equals(B, 0.0001) || A.Equals(C, 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DEqualityOperator", Script, "int RunEqualityOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddPointOperator)
	{
		static const char Script[] = R"(
int RunAddPointOperator()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    FBox2D Result = Box + FVector2D(-1.0, 8.0);
    if (!Result.Min.Equals(FVector2D(-1.0, 2.0), 0.0001) || !Result.Max.Equals(FVector2D(4.0, 8.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DAddPointOperator", Script, "int RunAddPointOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddBoxOperator)
	{
		static const char Script[] = R"(
int RunAddBoxOperator()
{
    FBox2D A(FVector2D(0.0, 0.0), FVector2D(3.0, 3.0));
    FBox2D B(FVector2D(2.0, 2.0), FVector2D(6.0, 6.0));
    FBox2D Result = A + B;
    if (!Result.Min.Equals(FVector2D(0.0, 0.0), 0.0001) || !Result.Max.Equals(FVector2D(6.0, 6.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DAddBoxOperator", Script, "int RunAddBoxOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddAssignPointOperator)
	{
		static const char Script[] = R"(
int RunAddAssignPointOperator()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    Box += FVector2D(-2.0, 7.0);
    if (!Box.Min.Equals(FVector2D(-2.0, 2.0), 0.0001) || !Box.Max.Equals(FVector2D(4.0, 7.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DAddAssignPointOperator", Script, "int RunAddAssignPointOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddAssignBoxOperator)
	{
		static const char Script[] = R"(
int RunAddAssignBoxOperator()
{
    FBox2D A(FVector2D(0.0, 0.0), FVector2D(3.0, 3.0));
    FBox2D B(FVector2D(2.0, 2.0), FVector2D(6.0, 6.0));
    A += B;
    if (!A.Min.Equals(FVector2D(0.0, 0.0), 0.0001) || !A.Max.Equals(FVector2D(6.0, 6.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DAddAssignBoxOperator", Script, "int RunAddAssignBoxOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IndexOperator)
	{
		static const char Script[] = R"(
int RunIndexOperator()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    if (!Box[0].Equals(FVector2D(1.0, 2.0), 0.0001))
    {
        return -1;
    }
    if (!Box[1].Equals(FVector2D(4.0, 5.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DIndexOperator", Script, "int RunIndexOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ComputeSquaredDistanceToPoint)
	{
		static const char Script[] = R"(
int RunComputeSquaredDistanceToPoint()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    // Point inside box -> distance is 0
    double Dist = Box.ComputeSquaredDistanceToPoint(FVector2D(2.0, 2.0));
    if (Dist > 0.0001)
    {
        return -1;
    }
    // Point outside: (6, 0) -> closest is (4, 0), dist^2 = 4
    double DistOutside = Box.ComputeSquaredDistanceToPoint(FVector2D(6.0, 0.0));
    if (DistOutside < 3.9999 || DistOutside > 4.0001)
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DComputeSquaredDistanceToPoint", Script, "int RunComputeSquaredDistanceToPoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ExpandByScalar)
	{
		static const char Script[] = R"(
int RunExpandByScalar()
{
    FBox2D Box(FVector2D(1.0, 1.0), FVector2D(3.0, 3.0));
    FBox2D Expanded = Box.ExpandBy(1.0);
    if (!Expanded.Min.Equals(FVector2D(0.0, 0.0), 0.0001) || !Expanded.Max.Equals(FVector2D(4.0, 4.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DExpandByScalar", Script, "int RunExpandByScalar()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ExpandByVector)
	{
		static const char Script[] = R"(
int RunExpandByVector()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(5.0, 6.0));
    FBox2D Expanded = Box.ExpandBy(FVector2D(2.0, 1.0));
    if (!Expanded.Min.Equals(FVector2D(-1.0, 1.0), 0.0001) || !Expanded.Max.Equals(FVector2D(7.0, 7.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DExpandByVector", Script, "int RunExpandByVector()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetArea)
	{
		static const char Script[] = R"(
int RunGetArea()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(3.0, 4.0));
    double Area = Box.GetArea();
    if (Area < 11.9999 || Area > 12.0001)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DGetArea", Script, "int RunGetArea()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetCenter)
	{
		static const char Script[] = R"(
int RunGetCenter()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 6.0));
    FVector2D Center = Box.GetCenter();
    if (!Center.Equals(FVector2D(2.0, 3.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DGetCenter", Script, "int RunGetCenter()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetCenterAndExtents)
	{
		static const char Script[] = R"(
int RunGetCenterAndExtents()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 6.0));
    FVector2D Center;
    FVector2D Extents;
    Box.GetCenterAndExtents(Center, Extents);
    if (!Center.Equals(FVector2D(2.0, 3.0), 0.0001))
    {
        return -1;
    }
    if (!Extents.Equals(FVector2D(2.0, 3.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DGetCenterAndExtents", Script, "int RunGetCenterAndExtents()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetClosestPointTo)
	{
		static const char Script[] = R"(
int RunGetClosestPointTo()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    FVector2D Closest = Box.GetClosestPointTo(FVector2D(6.0, 2.0));
    if (!Closest.Equals(FVector2D(4.0, 2.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DGetClosestPointTo", Script, "int RunGetClosestPointTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetExtentAndSize)
	{
		static const char Script[] = R"(
int RunGetExtentAndSize()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 6.0));
    FVector2D Extent = Box.GetExtent();
    FVector2D Size   = Box.GetSize();
    if (!Extent.Equals(FVector2D(2.0, 3.0), 0.0001))
    {
        return -1;
    }
    if (!Size.Equals(FVector2D(4.0, 6.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DGetExtentAndSize", Script, "int RunGetExtentAndSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Init)
	{
		static const char Script[] = R"(
int RunInit()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    Box.Init();
    if (Box.bIsValid)
    {
        return -1;
    }
    if (!Box.Min.Equals(FVector2D(0.0, 0.0), 0.0001) || !Box.Max.Equals(FVector2D(0.0, 0.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DInit", Script, "int RunInit()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Overlap)
	{
		static const char Script[] = R"(
int RunOverlap()
{
    FBox2D A(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    FBox2D B(FVector2D(2.0, 2.0), FVector2D(6.0, 6.0));
    FBox2D O = A.Overlap(B);
    if (!O.Min.Equals(FVector2D(2.0, 2.0), 0.0001) || !O.Max.Equals(FVector2D(4.0, 4.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DOverlap", Script, "int RunOverlap()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Intersect)
	{
		static const char Script[] = R"(
int RunIntersect()
{
    FBox2D A(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    FBox2D B(FVector2D(2.0, 2.0), FVector2D(6.0, 6.0));
    FBox2D C(FVector2D(5.0, 5.0), FVector2D(8.0, 8.0));
    if (!A.Intersect(B))
    {
        return -1;
    }
    if (A.Intersect(C))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DIntersect", Script, "int RunIntersect()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsidePoint)
	{
		static const char Script[] = R"(
int RunIsInsidePoint()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    if (!Box.IsInside(FVector2D(2.0, 2.0)))
    {
        return -1;
    }
    if (Box.IsInside(FVector2D(5.0, 5.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DIsInsidePoint", Script, "int RunIsInsidePoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideOrOn)
	{
		static const char Script[] = R"(
int RunIsInsideOrOn()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    if (!Box.IsInsideOrOn(FVector2D(4.0, 4.0)))
    {
        return -1;
    }
    if (Box.IsInsideOrOn(FVector2D(5.0, 5.0)))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DIsInsideOrOn", Script, "int RunIsInsideOrOn()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideBox)
	{
		static const char Script[] = R"(
int RunIsInsideBox()
{
    FBox2D Outer(FVector2D(0.0, 0.0), FVector2D(10.0, 10.0));
    FBox2D Inner(FVector2D(1.0, 1.0), FVector2D(5.0, 5.0));
    FBox2D Outside(FVector2D(8.0, 8.0), FVector2D(12.0, 12.0));
    if (!Outer.IsInside(Inner))
    {
        return -1;
    }
    if (Outer.IsInside(Outside))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DIsInsideBox", Script, "int RunIsInsideBox()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ShiftBy)
	{
		static const char Script[] = R"(
int RunShiftBy()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    FBox2D Shifted = Box.ShiftBy(FVector2D(2.0, 3.0));
    if (!Shifted.Min.Equals(FVector2D(2.0, 3.0), 0.0001) || !Shifted.Max.Equals(FVector2D(6.0, 7.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DShiftBy", Script, "int RunShiftBy()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MoveTo)
	{
		static const char Script[] = R"(
int RunMoveTo()
{
    FBox2D Box(FVector2D(0.0, 0.0), FVector2D(4.0, 4.0));
    FBox2D Moved = Box.MoveTo(FVector2D(5.0, 5.0));
    if (!Moved.Min.Equals(FVector2D(3.0, 3.0), 0.0001) || !Moved.Max.Equals(FVector2D(7.0, 7.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DMoveTo", Script, "int RunMoveTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    FString Str = Box.ToString();
    FString Expected = UnrealBox2DToString(Box);
    if (!Str.Equals(Expected, false))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(BuildAABB)
	{
		static const char Script[] = R"(
int RunBuildAABB()
{
    FVector2D Origin(5.0, 5.0);
    FVector2D Extent(2.0, 3.0);
    FBox2D Box = FBox2D::BuildAABB(Origin, Extent);
    if (!Box.Min.Equals(FVector2D(3.0, 2.0), 0.0001) || !Box.Max.Equals(FVector2D(7.0, 8.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DBuildAABB", Script, "int RunBuildAABB()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Interop)
	{
		static const char Script[] = R"(
int RunInterop()
{
    FBox2D Box(FVector2D(1.0, 2.0), FVector2D(4.0, 5.0));
    if (!UnrealAcceptsBox2D(Box))
    {
        return -1;
    }
    double Area = UnrealBox2DArea(Box);
    if (Area < 8.9999 || Area > 9.0001)
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBox2DInterop", Script, "int RunInterop()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
