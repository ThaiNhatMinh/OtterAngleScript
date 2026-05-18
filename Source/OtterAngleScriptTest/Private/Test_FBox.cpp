// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Box.h"
#include "Math/Transform.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FBox UnrealBuildBox(double MinX, double MinY, double MinZ, double MaxX, double MaxY, double MaxZ)
	{
		return FBox(FVector(MinX, MinY, MinZ), FVector(MaxX, MaxY, MaxZ));
	}

	bool UnrealAcceptsBox(const FBox& Value)
	{
		return Value.Equals(FBox(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0)), 0.0001);
	}

	double UnrealBoxVolume(const FBox& Value)
	{
		return Value.GetVolume();
	}

	FString UnrealBoxToString(const FBox& Value)
	{
		return Value.ToString();
	}

	FString UnrealBoxToCompactString(const FBox& Value)
	{
		return Value.ToCompactString();
	}

	FBox UnrealBoxWithNaN()
	{
		FBox Value(ForceInit);
		Value.Min = FVector(TNumericLimits<double>::Max(), 1.0, 2.0);
		Value.Max = FVector(TNumericLimits<double>::Max(), 4.0, 5.0);
		Value.IsValid = 1;
		return Value;
	}

	bool RegisterFBoxInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FBox UnrealBuildBox(double MinX, double MinY, double MinZ, double MaxX, double MaxY, double MaxZ)",
			asFUNCTION(UnrealBuildBox),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsBox(const FBox &in Value)",
			asFUNCTION(UnrealAcceptsBox),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealBoxVolume(const FBox &in Value)",
			asFUNCTION(UnrealBoxVolume),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FString UnrealBoxToString(const FBox &in Value)",
			asFUNCTION(UnrealBoxToString),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FString UnrealBoxToCompactString(const FBox &in Value)",
			asFUNCTION(UnrealBoxToCompactString),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FBox UnrealBoxWithNaN()",
			asFUNCTION(UnrealBoxWithNaN),
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
	FOtterAngleScriptFBoxTests,
	"OtterAngleScript.FBox",
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
		ASSERT_THAT(IsTrue(RegisterFBoxInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptBoxTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(ConstructorsOperatorsAndProperties)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FBox")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTransform")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));

		static const char Script[] = R"(
int RunConstructorsOperatorsAndProperties()
{
    FBox DefaultBox;
    if (DefaultBox.IsValid)
    {
        return -1;
    }

    DefaultBox += FVector(1.0, 2.0, 3.0);
    if (!DefaultBox.IsValid || !DefaultBox.Min.Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !DefaultBox.Max.Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -2;
    }

    FBox ForceInitBox(ForceInit);
    if (ForceInitBox.IsValid || !ForceInitBox.Min.Equals(FVector(0.0, 0.0, 0.0), 0.0001) || !ForceInitBox.Max.Equals(FVector(0.0, 0.0, 0.0), 0.0001))
    {
        return -3;
    }

    FBox MinMax(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    if (!MinMax.IsValid || !MinMax.opEquals(UnrealBuildBox(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)))
    {
        return -4;
    }

    FBox Copy(MinMax);
    if (!Copy.opEquals(MinMax))
    {
        return -5;
    }

    FBox Assigned;
    Assigned = MinMax;
    if (!Assigned.opEquals(MinMax))
    {
        return -6;
    }

    FBox AddedPoint = MinMax + FVector(-2.0, 7.0, 5.0);
    if (!AddedPoint.Min.Equals(FVector(-2.0, 2.0, 3.0), 0.0001) || !AddedPoint.Max.Equals(FVector(4.0, 7.0, 6.0), 0.0001))
    {
        return -7;
    }

    FBox AddedBox = MinMax + FBox(FVector(-1.0, 0.0, 1.0), FVector(3.0, 6.0, 9.0));
    if (!AddedBox.Min.Equals(FVector(-1.0, 0.0, 1.0), 0.0001) || !AddedBox.Max.Equals(FVector(4.0, 6.0, 9.0), 0.0001))
    {
        return -8;
    }

    FBox AddAssignPoint(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    AddAssignPoint += FVector(0.0, 10.0, 2.0);
    if (!AddAssignPoint.Min.Equals(FVector(0.0, 2.0, 2.0), 0.0001) || !AddAssignPoint.Max.Equals(FVector(4.0, 10.0, 6.0), 0.0001))
    {
        return -9;
    }

    FBox AddAssignBox(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    AddAssignBox += FBox(FVector(-3.0, 3.0, 2.0), FVector(2.0, 6.0, 9.0));
    if (!AddAssignBox.Min.Equals(FVector(-3.0, 2.0, 2.0), 0.0001) || !AddAssignBox.Max.Equals(FVector(4.0, 6.0, 9.0), 0.0001))
    {
        return -10;
    }

    FBox Indexed(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    Indexed[0] = FVector(-1.0, -2.0, -3.0);
    if (!Indexed[0].Equals(FVector(-1.0, -2.0, -3.0), 0.0001))
    {
        return -11;
    }

    const FBox ConstIndexed(Indexed);
    if (!ConstIndexed[1].Equals(FVector(4.0, 5.0, 6.0), 0.0001))
    {
        return -12;
    }

    if (!Indexed.Equals(FBox(FVector(-1.0, -2.0, -3.0), FVector(4.0, 5.0, 6.0)), 0.0001))
    {
        return -13;
    }

    Indexed.IsValid = false;
    if (Indexed.IsValid)
    {
        return -14;
    }

    Indexed.IsValid = true;
    Indexed.Min = FVector(1.0, 2.0, 3.0);
    Indexed.Max = FVector(4.0, 5.0, 6.0);
    if (!Indexed.opEquals(MinMax))
    {
        return -15;
    }

    if (!UnrealAcceptsBox(Indexed))
    {
        return -16;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxConstructorsOperatorsAndProperties", Script, "int RunConstructorsOperatorsAndProperties()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GeometryQueriesAndTransforms)
	{
		static const char Script[] = R"(
int RunGeometryQueriesAndTransforms()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!FMath::IsNearlyEqual(Box.ComputeSquaredDistanceToPoint(FVector(3.0, 4.0, 1.0)), 5.0, 0.0001))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(Box.ComputeSquaredDistanceToBox(FBox(FVector(4.0, 0.0, 0.0), FVector(6.0, 2.0, 2.0))), 4.0, 0.0001))
    {
        return -2;
    }

    FBox ScalarExpanded = Box.ExpandBy(2.0);
    if (!ScalarExpanded.Min.Equals(FVector(-2.0, -2.0, -2.0), 0.0001) || !ScalarExpanded.Max.Equals(FVector(4.0, 4.0, 4.0), 0.0001))
    {
        return -3;
    }

    FBox VectorExpanded = Box.ExpandBy(FVector(1.0, 2.0, 3.0));
    if (!VectorExpanded.Min.Equals(FVector(-1.0, -2.0, -3.0), 0.0001) || !VectorExpanded.Max.Equals(FVector(3.0, 4.0, 5.0), 0.0001))
    {
        return -4;
    }

    FBox MinMaxExpanded = Box.ExpandBy(FVector(1.0, 0.5, 0.0), FVector(0.0, 1.5, 2.0));
    if (!MinMaxExpanded.Min.Equals(FVector(-1.0, -0.5, 0.0), 0.0001) || !MinMaxExpanded.Max.Equals(FVector(2.0, 3.5, 4.0), 0.0001))
    {
        return -5;
    }

    if (!Box.ShiftBy(FVector(3.0, 4.0, 5.0)).opEquals(FBox(FVector(3.0, 4.0, 5.0), FVector(5.0, 6.0, 7.0))))
    {
        return -6;
    }

    if (!Box.MoveTo(FVector(10.0, 20.0, 30.0)).opEquals(FBox(FVector(9.0, 19.0, 29.0), FVector(11.0, 21.0, 31.0))))
    {
        return -7;
    }

    if (!Box.GetCenter().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -8;
    }

    FVector Center;
    FVector Extents;
    Box.GetCenterAndExtents(Center, Extents);
    if (!Center.Equals(FVector(1.0, 1.0, 1.0), 0.0001) || !Extents.Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -9;
    }

    if (!Box.GetClosestPointTo(FVector(3.0, -1.0, 1.0)).Equals(FVector(2.0, 0.0, 1.0), 0.0001))
    {
        return -10;
    }

    FVector V0;
    FVector V1;
    FVector V2;
    FVector V3;
    FVector V4;
    FVector V5;
    FVector V6;
    FVector V7;
    Box.GetVertices(V0, V1, V2, V3, V4, V5, V6, V7);
    if (!V0.Equals(FVector(0.0, 0.0, 0.0), 0.0001) || !V7.Equals(FVector(2.0, 2.0, 2.0), 0.0001))
    {
        return -11;
    }

    if (!Box.GetExtent().Equals(FVector(1.0, 1.0, 1.0), 0.0001) || !Box.GetSize().Equals(FVector(2.0, 2.0, 2.0), 0.0001))
    {
        return -12;
    }

    if (!FMath::IsNearlyEqual(Box.GetVolume(), 8.0, 0.0001) || !FMath::IsNearlyEqual(UnrealBoxVolume(Box), 8.0, 0.0001))
    {
        return -13;
    }

    if (!Box.Intersect(FBox(FVector(1.0, 1.0, 1.0), FVector(3.0, 3.0, 3.0))))
    {
        return -14;
    }

    if (Box.Intersect(FBox(FVector(3.1, 0.0, 0.0), FVector(4.0, 1.0, 1.0))))
    {
        return -15;
    }

    if (!Box.IntersectXY(FBox(FVector(1.0, 1.0, 100.0), FVector(3.0, 3.0, 200.0))))
    {
        return -16;
    }

    FBox Overlap = Box.Overlap(FBox(FVector(1.0, -1.0, 1.0), FVector(3.0, 1.5, 4.0)));
    if (!Overlap.opEquals(FBox(FVector(1.0, 0.0, 1.0), FVector(2.0, 1.5, 2.0))))
    {
        return -17;
    }

    FTransform Translation(FVector(5.0, 6.0, 7.0));
    FBox Transformed = Box.TransformBy(Translation);
    if (!Transformed.opEquals(FBox(FVector(5.0, 6.0, 7.0), FVector(7.0, 8.0, 9.0))))
    {
        return -18;
    }

    if (!Transformed.InverseTransformBy(Translation).opEquals(Box))
    {
        return -19;
    }

    if (!Box.IsInside(FVector(1.0, 1.0, 1.0)) || Box.IsInside(FVector(0.0, 0.0, 0.0)))
    {
        return -20;
    }

    if (!Box.IsInsideOrOn(FVector(0.0, 0.0, 0.0)))
    {
        return -21;
    }

    if (!Box.IsInside(FBox(FVector(0.5, 0.5, 0.5), FVector(1.5, 1.5, 1.5))) || Box.IsInside(FBox(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0))))
    {
        return -22;
    }

    if (!Box.IsInsideOrOn(FBox(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0))))
    {
        return -23;
    }

    if (!Box.IsInsideXY(FVector(1.0, 1.0, 99.0)) || Box.IsInsideXY(FVector(0.0, 0.0, 0.0)))
    {
        return -24;
    }

    if (!Box.IsInsideOrOnXY(FVector(0.0, 2.0, -50.0)))
    {
        return -25;
    }

    if (!Box.IsInsideXY(FBox(FVector(0.5, 0.5, -10.0), FVector(1.5, 1.5, 10.0))) || Box.IsInsideXY(FBox(FVector(0.0, 0.0, -10.0), FVector(2.0, 2.0, 10.0))))
    {
        return -26;
    }

    FBox ResetBox(FVector(3.0, 4.0, 5.0), FVector(6.0, 7.0, 8.0));
    ResetBox.Init();
    if (ResetBox.IsValid || !ResetBox.Min.Equals(FVector(0.0, 0.0, 0.0), 0.0001) || !ResetBox.Max.Equals(FVector(0.0, 0.0, 0.0), 0.0001))
    {
        return -27;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGeometryQueriesAndTransforms", Script, "int RunGeometryQueriesAndTransforms()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(StringsNamespaceHelpersAndNaN)
	{
		static const char Script[] = R"(
int RunStringsNamespaceHelpersAndNaN()
{
    FBox Box = FBox::BuildAABB(FVector(5.0, 6.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (!Box.opEquals(FBox(FVector(4.0, 4.0, 4.0), FVector(6.0, 8.0, 10.0))))
    {
        return -1;
    }

    if (Box.ToString() != UnrealBoxToString(Box))
    {
        return -2;
    }

    if (Box.ToCompactString() != UnrealBoxToCompactString(Box))
    {
        return -3;
    }

    if (Box.ContainsNaN())
    {
        return -4;
    }

    if (!UnrealBoxWithNaN().ContainsNaN())
    {
        return -5;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxStringsNamespaceHelpersAndNaN", Script, "int RunStringsNamespaceHelpersAndNaN()");
		ASSERT_THAT(IsNotNull(Function));
        ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif
#endif
