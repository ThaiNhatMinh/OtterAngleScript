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
		Value.Min = FVector(std::numeric_limits<float>::quiet_NaN(), 1.0, 2.0);
		Value.Max = FVector(std::numeric_limits<float>::quiet_NaN(), 4.0, 5.0);
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

	TEST_METHOD(TypesRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FBox")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTransform")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EForceInitType")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FBox Box;
    if (Box.IsValid)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ForceInitConstructor)
	{
		static const char Script[] = R"(
int RunForceInitConstructor()
{
    FBox Box(ForceInit);
    if (Box.IsValid || !Box.Min.Equals(FVector(0.0, 0.0, 0.0), 0.0001) || !Box.Max.Equals(FVector(0.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxForceInitConstructor", Script, "int RunForceInitConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MinMaxConstructor)
	{
		static const char Script[] = R"(
int RunMinMaxConstructor()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    if (!Box.IsValid || !Box.opEquals(UnrealBuildBox(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxMinMaxConstructor", Script, "int RunMinMaxConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FBox Original(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    FBox Copy(Original);
    if (!Copy.opEquals(Original))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AssignmentOperator)
	{
		static const char Script[] = R"(
int RunAssignmentOperator()
{
    FBox Source(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    FBox Dest;
    Dest = Source;
    if (!Dest.opEquals(Source))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxAssignmentOperator", Script, "int RunAssignmentOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(EqualityOperator)
	{
		static const char Script[] = R"(
int RunEqualityOperator()
{
    FBox A(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    FBox B(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    FBox C(FVector(0.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0));
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

		asIScriptFunction* Function = BuildFunction("FBoxEqualityOperator", Script, "int RunEqualityOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddPointOperator)
	{
		static const char Script[] = R"(
int RunAddPointOperator()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    FBox Result = Box + FVector(-2.0, 7.0, 5.0);
    if (!Result.Min.Equals(FVector(-2.0, 2.0, 3.0), 0.0001) || !Result.Max.Equals(FVector(4.0, 7.0, 6.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxAddPointOperator", Script, "int RunAddPointOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddBoxOperator)
	{
		static const char Script[] = R"(
int RunAddBoxOperator()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    FBox Result = Box + FBox(FVector(-1.0, 0.0, 1.0), FVector(3.0, 6.0, 9.0));
    if (!Result.Min.Equals(FVector(-1.0, 0.0, 1.0), 0.0001) || !Result.Max.Equals(FVector(4.0, 6.0, 9.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxAddBoxOperator", Script, "int RunAddBoxOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddAssignPointOperator)
	{
		static const char Script[] = R"(
int RunAddAssignPointOperator()
{
    FBox Default;
    Default += FVector(1.0, 2.0, 3.0);
    if (!Default.IsValid || !Default.Min.Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !Default.Max.Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -1;
    }

    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    Box += FVector(0.0, 10.0, 2.0);
    if (!Box.Min.Equals(FVector(0.0, 2.0, 2.0), 0.0001) || !Box.Max.Equals(FVector(4.0, 10.0, 6.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxAddAssignPointOperator", Script, "int RunAddAssignPointOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AddAssignBoxOperator)
	{
		static const char Script[] = R"(
int RunAddAssignBoxOperator()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    Box += FBox(FVector(-3.0, 3.0, 2.0), FVector(2.0, 6.0, 9.0));
    if (!Box.Min.Equals(FVector(-3.0, 2.0, 2.0), 0.0001) || !Box.Max.Equals(FVector(4.0, 6.0, 9.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxAddAssignBoxOperator", Script, "int RunAddAssignBoxOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IndexOperator)
	{
		static const char Script[] = R"(
int RunIndexOperator()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    Box[0] = FVector(-1.0, -2.0, -3.0);
    if (!Box[0].Equals(FVector(-1.0, -2.0, -3.0), 0.0001))
    {
        return -1;
    }
    const FBox ConstBox(Box);
    if (!ConstBox[1].Equals(FVector(4.0, 5.0, 6.0), 0.0001))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIndexOperator", Script, "int RunIndexOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsValidProperty)
	{
		static const char Script[] = R"(
int RunIsValidProperty()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    Box.IsValid = false;
    if (Box.IsValid)
    {
        return -1;
    }
    Box.IsValid = true;
    if (!Box.IsValid)
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsValidProperty", Script, "int RunIsValidProperty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MinMaxProperties)
	{
		static const char Script[] = R"(
int RunMinMaxProperties()
{
    FBox Box(FVector(1.0, 2.0, 3.0), FVector(4.0, 5.0, 6.0));
    Box.Min = FVector(0.0, 0.0, 0.0);
    Box.Max = FVector(10.0, 10.0, 10.0);
    if (!Box.Min.Equals(FVector(0.0, 0.0, 0.0), 0.0001) || !Box.Max.Equals(FVector(10.0, 10.0, 10.0), 0.0001))
    {
        return -1;
    }

    Box.IsValid = true;
    Box.Min = FVector(1.0, 2.0, 3.0);
    Box.Max = FVector(4.0, 5.0, 6.0);
    if (!UnrealAcceptsBox(Box))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxMinMaxProperties", Script, "int RunMinMaxProperties()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ComputeSquaredDistanceToPoint)
	{
		static const char Script[] = R"(
int RunComputeSquaredDistanceToPoint()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!FMath::IsNearlyEqual(Box.ComputeSquaredDistanceToPoint(FVector(3.0, 4.0, 1.0)), 5.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxComputeSquaredDistanceToPoint", Script, "int RunComputeSquaredDistanceToPoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ComputeSquaredDistanceToBox)
	{
		static const char Script[] = R"(
int RunComputeSquaredDistanceToBox()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!FMath::IsNearlyEqual(Box.ComputeSquaredDistanceToBox(FBox(FVector(4.0, 0.0, 0.0), FVector(6.0, 2.0, 2.0))), 4.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxComputeSquaredDistanceToBox", Script, "int RunComputeSquaredDistanceToBox()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ExpandBy)
	{
		static const char Script[] = R"(
int RunExpandBy()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));

    FBox ScalarExpanded = Box.ExpandBy(2.0);
    if (!ScalarExpanded.Min.Equals(FVector(-2.0, -2.0, -2.0), 0.0001) || !ScalarExpanded.Max.Equals(FVector(4.0, 4.0, 4.0), 0.0001))
    {
        return -1;
    }

    FBox VectorExpanded = Box.ExpandBy(FVector(1.0, 2.0, 3.0));
    if (!VectorExpanded.Min.Equals(FVector(-1.0, -2.0, -3.0), 0.0001) || !VectorExpanded.Max.Equals(FVector(3.0, 4.0, 5.0), 0.0001))
    {
        return -2;
    }

    FBox MinMaxExpanded = Box.ExpandBy(FVector(1.0, 0.5, 0.0), FVector(0.0, 1.5, 2.0));
    if (!MinMaxExpanded.Min.Equals(FVector(-1.0, -0.5, 0.0), 0.0001) || !MinMaxExpanded.Max.Equals(FVector(2.0, 3.5, 4.0), 0.0001))
    {
        return -3;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxExpandBy", Script, "int RunExpandBy()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ShiftBy)
	{
		static const char Script[] = R"(
int RunShiftBy()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.ShiftBy(FVector(3.0, 4.0, 5.0)).opEquals(FBox(FVector(3.0, 4.0, 5.0), FVector(5.0, 6.0, 7.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxShiftBy", Script, "int RunShiftBy()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(MoveTo)
	{
		static const char Script[] = R"(
int RunMoveTo()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.MoveTo(FVector(10.0, 20.0, 30.0)).opEquals(FBox(FVector(9.0, 19.0, 29.0), FVector(11.0, 21.0, 31.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxMoveTo", Script, "int RunMoveTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetCenter)
	{
		static const char Script[] = R"(
int RunGetCenter()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.GetCenter().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetCenter", Script, "int RunGetCenter()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetCenterAndExtents)
	{
		static const char Script[] = R"(
int RunGetCenterAndExtents()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FVector Center;
    FVector Extents;
    Box.GetCenterAndExtents(Center, Extents);
    if (!Center.Equals(FVector(1.0, 1.0, 1.0), 0.0001) || !Extents.Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetCenterAndExtents", Script, "int RunGetCenterAndExtents()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetClosestPointTo)
	{
		static const char Script[] = R"(
int RunGetClosestPointTo()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.GetClosestPointTo(FVector(3.0, -1.0, 1.0)).Equals(FVector(2.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetClosestPointTo", Script, "int RunGetClosestPointTo()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetVertices)
	{
		static const char Script[] = R"(
int RunGetVertices()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
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
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetVertices", Script, "int RunGetVertices()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetExtent)
	{
		static const char Script[] = R"(
int RunGetExtent()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.GetExtent().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetExtent", Script, "int RunGetExtent()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetSize)
	{
		static const char Script[] = R"(
int RunGetSize()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.GetSize().Equals(FVector(2.0, 2.0, 2.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetSize", Script, "int RunGetSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(GetVolume)
	{
		static const char Script[] = R"(
int RunGetVolume()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!FMath::IsNearlyEqual(Box.GetVolume(), 8.0, 0.0001) || !FMath::IsNearlyEqual(UnrealBoxVolume(Box), 8.0, 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxGetVolume", Script, "int RunGetVolume()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Init)
	{
		static const char Script[] = R"(
int RunInit()
{
    FBox Box(FVector(3.0, 4.0, 5.0), FVector(6.0, 7.0, 8.0));
    Box.Init();
    if (Box.IsValid || !Box.Min.Equals(FVector(0.0, 0.0, 0.0), 0.0001) || !Box.Max.Equals(FVector(0.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxInit", Script, "int RunInit()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Intersect)
	{
		static const char Script[] = R"(
int RunIntersect()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.Intersect(FBox(FVector(1.0, 1.0, 1.0), FVector(3.0, 3.0, 3.0))))
    {
        return -1;
    }
    if (Box.Intersect(FBox(FVector(3.1, 0.0, 0.0), FVector(4.0, 1.0, 1.0))))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIntersect", Script, "int RunIntersect()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IntersectXY)
	{
		static const char Script[] = R"(
int RunIntersectXY()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IntersectXY(FBox(FVector(1.0, 1.0, 100.0), FVector(3.0, 3.0, 200.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIntersectXY", Script, "int RunIntersectXY()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Overlap)
	{
		static const char Script[] = R"(
int RunOverlap()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FBox Result = Box.Overlap(FBox(FVector(1.0, -1.0, 1.0), FVector(3.0, 1.5, 4.0)));
    if (!Result.opEquals(FBox(FVector(1.0, 0.0, 1.0), FVector(2.0, 1.5, 2.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxOverlap", Script, "int RunOverlap()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(TransformBy)
	{
		static const char Script[] = R"(
int RunTransformBy()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FTransform Translation(FVector(5.0, 6.0, 7.0));
    FBox Transformed = Box.TransformBy(Translation);
    if (!Transformed.opEquals(FBox(FVector(5.0, 6.0, 7.0), FVector(7.0, 8.0, 9.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxTransformBy", Script, "int RunTransformBy()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InverseTransformBy)
	{
		static const char Script[] = R"(
int RunInverseTransformBy()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FTransform Translation(FVector(5.0, 6.0, 7.0));
    FBox Transformed = Box.TransformBy(Translation);
    if (!Transformed.InverseTransformBy(Translation).opEquals(Box))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxInverseTransformBy", Script, "int RunInverseTransformBy()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsidePoint)
	{
		static const char Script[] = R"(
int RunIsInsidePoint()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInside(FVector(1.0, 1.0, 1.0)) || Box.IsInside(FVector(0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsidePoint", Script, "int RunIsInsidePoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideOrOnPoint)
	{
		static const char Script[] = R"(
int RunIsInsideOrOnPoint()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInsideOrOn(FVector(0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsideOrOnPoint", Script, "int RunIsInsideOrOnPoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideBox)
	{
		static const char Script[] = R"(
int RunIsInsideBox()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInside(FBox(FVector(0.5, 0.5, 0.5), FVector(1.5, 1.5, 1.5))) || Box.IsInside(FBox(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsideBox", Script, "int RunIsInsideBox()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideOrOnBox)
	{
		static const char Script[] = R"(
int RunIsInsideOrOnBox()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInsideOrOn(FBox(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsideOrOnBox", Script, "int RunIsInsideOrOnBox()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideXYPoint)
	{
		static const char Script[] = R"(
int RunIsInsideXYPoint()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInsideXY(FVector(1.0, 1.0, 99.0)) || Box.IsInsideXY(FVector(0.0, 0.0, 0.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsideXYPoint", Script, "int RunIsInsideXYPoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideOrOnXYPoint)
	{
		static const char Script[] = R"(
int RunIsInsideOrOnXYPoint()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInsideOrOnXY(FVector(0.0, 2.0, -50.0)))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsideOrOnXYPoint", Script, "int RunIsInsideOrOnXYPoint()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IsInsideXYBox)
	{
		static const char Script[] = R"(
int RunIsInsideXYBox()
{
    FBox Box(FVector(0.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Box.IsInsideXY(FBox(FVector(0.5, 0.5, -10.0), FVector(1.5, 1.5, 10.0))) || Box.IsInsideXY(FBox(FVector(0.0, 0.0, -10.0), FVector(2.0, 2.0, 10.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxIsInsideXYBox", Script, "int RunIsInsideXYBox()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(BuildAABB)
	{
		static const char Script[] = R"(
int RunBuildAABB()
{
    FBox Box = FBox::BuildAABB(FVector(5.0, 6.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (!Box.opEquals(FBox(FVector(4.0, 4.0, 4.0), FVector(6.0, 8.0, 10.0))))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxBuildAABB", Script, "int RunBuildAABB()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FBox Box = FBox::BuildAABB(FVector(5.0, 6.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (Box.ToString() != UnrealBoxToString(Box))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ToCompactString)
	{
		static const char Script[] = R"(
int RunToCompactString()
{
    FBox Box = FBox::BuildAABB(FVector(5.0, 6.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (Box.ToCompactString() != UnrealBoxToCompactString(Box))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxToCompactString", Script, "int RunToCompactString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ContainsNaN)
	{
		static const char Script[] = R"(
int RunContainsNaN()
{
    FBox Box = FBox::BuildAABB(FVector(5.0, 6.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (Box.ContainsNaN())
    {
        return -1;
    }
    if (!UnrealBoxWithNaN().ContainsNaN())
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("FBoxContainsNaN", Script, "int RunContainsNaN()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif
#endif
