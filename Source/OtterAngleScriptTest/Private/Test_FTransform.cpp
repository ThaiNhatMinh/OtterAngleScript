// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Transform.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FTransform UnrealBuildTransform(double X, double Y, double Z)
	{
		return FTransform(FVector(X, Y, Z));
	}

	bool UnrealAcceptsTransform(const FTransform& Value)
	{
		return Value.Equals(FTransform(FVector(11.0, 12.0, 13.0)), 0.0001);
	}

	double UnrealSumTransformTranslation(const FTransform& Value)
	{
		const FVector Translation = Value.GetTranslation();
		return Translation.X + Translation.Y + Translation.Z;
	}

	FTransform UnrealTransformWithNaN()
	{
		FTransform Value;
		Value.SetTranslation(FVector(std::numeric_limits<double>::quiet_NaN(), 1.0, 2.0));
		return Value;
	}

	bool RegisterFTransformInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FTransform UnrealBuildTransform(double X, double Y, double Z)",
			asFUNCTION(UnrealBuildTransform),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsTransform(const FTransform &in Value)",
			asFUNCTION(UnrealAcceptsTransform),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealSumTransformTranslation(const FTransform &in Value)",
			asFUNCTION(UnrealSumTransformTranslation),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"FTransform UnrealTransformWithNaN()",
			asFUNCTION(UnrealTransformWithNaN),
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
	FOtterAngleScriptFTransformTests,
	"OtterAngleScript.FTransform",
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
		ASSERT_THAT(IsTrue(RegisterFTransformInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptTransformTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeInfo)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTransform")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FQuat")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EAxisType")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FTransform Identity;
    if (!Identity.Equals(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_DefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TranslationOnlyConstructor)
	{
		static const char Script[] = R"(
int RunTranslationOnlyConstructor()
{
    FTransform TranslationOnly(FVector(1.0, 2.0, 3.0));
    if (!TranslationOnly.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !TranslationOnly.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TranslationOnlyConstructor", Script, "int RunTranslationOnlyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RotationOnlyConstructor)
	{
		static const char Script[] = R"(
int RunRotationOnlyConstructor()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform RotationOnly(QuarterTurn);
    if (!RotationOnly.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_RotationOnlyConstructor", Script, "int RunRotationOnlyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RotatorConstructor)
	{
		static const char Script[] = R"(
int RunRotatorConstructor()
{
    FTransform RotatorOnly(FRotator(0.0, 90.0, 0.0));
    if (!RotatorOnly.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_RotatorConstructor", Script, "int RunRotatorConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FullQuatConstructor)
	{
		static const char Script[] = R"(
int RunFullQuatConstructor()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform FullQuat(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!FullQuat.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !FullQuat.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001) || !FullQuat.GetRotation().Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_FullQuatConstructor", Script, "int RunFullQuatConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FullRotatorConstructor)
	{
		static const char Script[] = R"(
int RunFullRotatorConstructor()
{
    FTransform FullRotator(FRotator(0.0, 90.0, 0.0), FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!FullRotator.GetLocation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !FullRotator.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_FullRotatorConstructor", Script, "int RunFullRotatorConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform FullQuat(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform Copy(FullQuat);
    if (!Copy.Equals(FullQuat, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AssignmentOperator)
	{
		static const char Script[] = R"(
int RunAssignmentOperator()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform FullQuat(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform Assigned;
    Assigned = FullQuat;
    if (!Assigned.Equals(FullQuat, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AssignmentOperator", Script, "int RunAssignmentOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AdditionOperator)
	{
		static const char Script[] = R"(
int RunAdditionOperator()
{
    FTransform TranslationOnly(FVector(1.0, 2.0, 3.0));
    FTransform Added = TranslationOnly + TranslationOnly;
    if (!Added.GetTranslation().Equals(FVector(2.0, 4.0, 6.0), 0.0001) || !Added.GetScale3D().Equals(FVector(2.0, 2.0, 2.0), 0.0001) || !Added.GetRotation().opEquals(FQuat(0.0, 0.0, 0.0, 2.0)))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AdditionOperator", Script, "int RunAdditionOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompoundAddAssign)
	{
		static const char Script[] = R"(
int RunCompoundAddAssign()
{
    FTransform TranslationOnly(FVector(1.0, 2.0, 3.0));
    FTransform AddAssign;
    AddAssign += TranslationOnly;
    if (!AddAssign.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CompoundAddAssign", Script, "int RunCompoundAddAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MultiplyByScalar)
	{
		static const char Script[] = R"(
int RunMultiplyByScalar()
{
    FTransform TranslationOnly(FVector(1.0, 2.0, 3.0));
    FTransform ScalarScaled = TranslationOnly * 2.0;
    if (!ScalarScaled.GetTranslation().Equals(FVector(2.0, 4.0, 6.0), 0.0001) || !ScalarScaled.GetScale3D().Equals(FVector(2.0, 2.0, 2.0), 0.0001) || !ScalarScaled.GetRotation().opEquals(FQuat(0.0, 0.0, 0.0, 2.0)))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_MultiplyByScalar", Script, "int RunMultiplyByScalar()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CompoundScaleAssign)
	{
		static const char Script[] = R"(
int RunCompoundScaleAssign()
{
    FTransform ScalarAssign(FVector(2.0, 4.0, 6.0));
    ScalarAssign *= 0.5;
    if (!ScalarAssign.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !ScalarAssign.GetScale3D().Equals(FVector(0.5, 0.5, 0.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CompoundScaleAssign", Script, "int RunCompoundScaleAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformComposition)
	{
		static const char Script[] = R"(
int RunTransformComposition()
{
    FTransform Composed = FTransform(FVector(1.0, 2.0, 3.0)) * FTransform(FVector(4.0, 5.0, 6.0));
    if (!Composed.Equals(FTransform(FVector(5.0, 7.0, 9.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformComposition", Script, "int RunTransformComposition()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformCompoundAssign)
	{
		static const char Script[] = R"(
int RunTransformCompoundAssign()
{
    FTransform ComposeAssign(FVector(1.0, 2.0, 3.0));
    ComposeAssign *= FTransform(FVector(4.0, 5.0, 6.0));
    if (!ComposeAssign.Equals(FTransform(FVector(5.0, 7.0, 9.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformCompoundAssign", Script, "int RunTransformCompoundAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformMultiplyByQuat)
	{
		static const char Script[] = R"(
int RunTransformMultiplyByQuat()
{
    const double HalfPi = 1.5707963267948966;
    const double InvSqrtTwo = 0.7071067811865476;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Rotated = FTransform() * QuarterTurn;
    if (!Rotated.GetRotation().Equals(FQuat(0.0, 0.0, InvSqrtTwo, InvSqrtTwo), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformMultiplyByQuat", Script, "int RunTransformMultiplyByQuat()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformCompoundQuatAssign)
	{
		static const char Script[] = R"(
int RunTransformCompoundQuatAssign()
{
    const double HalfPi = 1.5707963267948966;
    const double InvSqrtTwo = 0.7071067811865476;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform RotatedAssign;
    RotatedAssign *= QuarterTurn;
    if (!RotatedAssign.GetRotation().Equals(FQuat(0.0, 0.0, InvSqrtTwo, InvSqrtTwo), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformCompoundQuatAssign", Script, "int RunTransformCompoundQuatAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformPosition)
	{
		static const char Script[] = R"(
int RunTransformPosition()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FVector WorldPos = Base.TransformPosition(FVector(1.0, 0.0, 0.0));
    if (!WorldPos.Equals(FVector(10.0, 2.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformPosition", Script, "int RunTransformPosition()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformPositionNoScale)
	{
		static const char Script[] = R"(
int RunTransformPositionNoScale()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FVector WorldPosNoScale = Base.TransformPositionNoScale(FVector(1.0, 0.0, 0.0));
    if (!WorldPosNoScale.Equals(FVector(10.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformPositionNoScale", Script, "int RunTransformPositionNoScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InverseTransformPosition)
	{
		static const char Script[] = R"(
int RunInverseTransformPosition()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FVector WorldPos = Base.TransformPosition(FVector(1.0, 0.0, 0.0));
    if (!Base.InverseTransformPosition(WorldPos).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_InverseTransformPosition", Script, "int RunInverseTransformPosition()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InverseTransformPositionNoScale)
	{
		static const char Script[] = R"(
int RunInverseTransformPositionNoScale()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FVector WorldPosNoScale = Base.TransformPositionNoScale(FVector(1.0, 0.0, 0.0));
    if (!Base.InverseTransformPositionNoScale(WorldPosNoScale).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_InverseTransformPositionNoScale", Script, "int RunInverseTransformPositionNoScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformVector)
	{
		static const char Script[] = R"(
int RunTransformVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.TransformVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 2.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformVector", Script, "int RunTransformVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformVectorNoScale)
	{
		static const char Script[] = R"(
int RunTransformVectorNoScale()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformVectorNoScale", Script, "int RunTransformVectorNoScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InverseTransformVector)
	{
		static const char Script[] = R"(
int RunInverseTransformVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.InverseTransformVector(FVector(0.0, 2.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_InverseTransformVector", Script, "int RunInverseTransformVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InverseTransformVectorNoScale)
	{
		static const char Script[] = R"(
int RunInverseTransformVectorNoScale()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.InverseTransformVectorNoScale(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_InverseTransformVectorNoScale", Script, "int RunInverseTransformVectorNoScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TransformRotation)
	{
		static const char Script[] = R"(
int RunTransformRotation()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.TransformRotation(FQuat(0.0, 0.0, 0.0, 1.0)).Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TransformRotation", Script, "int RunTransformRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InverseTransformRotation)
	{
		static const char Script[] = R"(
int RunInverseTransformRotation()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.InverseTransformRotation(QuarterTurn).Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_InverseTransformRotation", Script, "int RunInverseTransformRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Inverse)
	{
		static const char Script[] = R"(
int RunInverse()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    FVector WorldPos = Base.TransformPosition(FVector(1.0, 0.0, 0.0));
    if (!Base.Inverse().TransformPosition(WorldPos).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_Inverse", Script, "int RunInverse()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Blend)
	{
		static const char Script[] = R"(
int RunBlend()
{
    FTransform Blended;
    Blended.Blend(FTransform(), FTransform(FVector(10.0, 20.0, 30.0)), 0.25);
    if (!Blended.GetTranslation().Equals(FVector(2.5, 5.0, 7.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_Blend", Script, "int RunBlend()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(BlendWith)
	{
		static const char Script[] = R"(
int RunBlendWith()
{
    FTransform BlendedWith;
    BlendedWith.BlendWith(FTransform(FVector(10.0, 20.0, 30.0)), 0.25);
    if (!BlendedWith.GetTranslation().Equals(FVector(2.5, 5.0, 7.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_BlendWith", Script, "int RunBlendWith()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScaleTranslation)
	{
		static const char Script[] = R"(
int RunScaleTranslation()
{
    FTransform TranslationScaler(FVector(1.0, 2.0, 3.0));
    TranslationScaler.ScaleTranslation(FVector(2.0, 3.0, 4.0));
    TranslationScaler.ScaleTranslation(0.5);
    if (!TranslationScaler.GetTranslation().Equals(FVector(1.0, 3.0, 6.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_ScaleTranslation", Script, "int RunScaleTranslation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RemoveScaling)
	{
		static const char Script[] = R"(
int RunRemoveScaling()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform ScaleRemoval(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    ScaleRemoval.RemoveScaling();
    if (!ScaleRemoval.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001) || !ScaleRemoval.IsRotationNormalized())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_RemoveScaling", Script, "int RunRemoveScaling()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetMaximumAxisScale)
	{
		static const char Script[] = R"(
int RunGetMaximumAxisScale()
{
    FTransform AxisScale(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(2.0, -3.0, 4.0));
    if (!FMath::IsNearlyEqual(AxisScale.GetMaximumAxisScale(), 4.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetMaximumAxisScale", Script, "int RunGetMaximumAxisScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetMinimumAxisScale)
	{
		static const char Script[] = R"(
int RunGetMinimumAxisScale()
{
    FTransform AxisScale(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(2.0, -3.0, 4.0));
    if (!FMath::IsNearlyEqual(AxisScale.GetMinimumAxisScale(), 2.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetMinimumAxisScale", Script, "int RunGetMinimumAxisScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetRelativeTransform)
	{
		static const char Script[] = R"(
int RunGetRelativeTransform()
{
    FTransform Child(FVector(10.0, 0.0, 0.0));
    FTransform Parent(FVector(3.0, 0.0, 0.0));
    if (!Child.GetRelativeTransform(Parent).Equals(FTransform(FVector(7.0, 0.0, 0.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetRelativeTransform", Script, "int RunGetRelativeTransform()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetRelativeTransformReverse)
	{
		static const char Script[] = R"(
int RunGetRelativeTransformReverse()
{
    FTransform Child(FVector(10.0, 0.0, 0.0));
    FTransform Parent(FVector(3.0, 0.0, 0.0));
    if (!Child.GetRelativeTransformReverse(Parent).Equals(FTransform(FVector(-7.0, 0.0, 0.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetRelativeTransformReverse", Script, "int RunGetRelativeTransformReverse()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetToRelativeTransform)
	{
		static const char Script[] = R"(
int RunSetToRelativeTransform()
{
    FTransform Parent(FVector(3.0, 0.0, 0.0));
    FTransform RelativeSet(FVector(10.0, 0.0, 0.0));
    RelativeSet.SetToRelativeTransform(Parent);
    if (!RelativeSet.Equals(FTransform(FVector(7.0, 0.0, 0.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetToRelativeTransform", Script, "int RunSetToRelativeTransform()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetScaledWithDouble)
	{
		static const char Script[] = R"(
int RunGetScaledWithDouble()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Scaled = FTransform(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!Scaled.GetScaled(2.0).GetScale3D().Equals(FVector(4.0, 6.0, 8.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetScaledWithDouble", Script, "int RunGetScaledWithDouble()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetScaledWithFVector)
	{
		static const char Script[] = R"(
int RunGetScaledWithFVector()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Scaled = FTransform(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!Scaled.GetScaled(FVector(2.0, 1.0, 0.5)).GetScale3D().Equals(FVector(4.0, 3.0, 2.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetScaledWithFVector", Script, "int RunGetScaledWithFVector()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetScaledAxis)
	{
		static const char Script[] = R"(
int RunGetScaledAxis()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.GetScaledAxis(X).Equals(FVector(0.0, 2.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetScaledAxis", Script, "int RunGetScaledAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetUnitAxis)
	{
		static const char Script[] = R"(
int RunGetUnitAxis()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.GetUnitAxis(X).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetUnitAxis", Script, "int RunGetUnitAxis()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Mirror)
	{
		static const char Script[] = R"(
int RunMirror()
{
    FTransform Mirrored(FVector(1.0, 2.0, 3.0));
    Mirrored.Mirror(X, Y);
    if (!Mirrored.IsValid())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_Mirror", Script, "int RunMirror()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetLocation)
	{
		static const char Script[] = R"(
int RunGetLocation()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.GetLocation().Equals(FVector(10.0, 0.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetLocation", Script, "int RunGetLocation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetLocation)
	{
		static const char Script[] = R"(
int RunSetLocation()
{
    FTransform SetLocationValue;
    SetLocationValue.SetLocation(FVector(1.0, 2.0, 3.0));
    if (!SetLocationValue.GetLocation().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetLocation", Script, "int RunSetLocation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Rotator)
	{
		static const char Script[] = R"(
int RunRotator()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.Rotator().Equals(FRotator(0.0, 90.0, 0.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_Rotator", Script, "int RunRotator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetDeterminant)
	{
		static const char Script[] = R"(
int RunGetDeterminant()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!FMath::IsNearlyEqual(Base.GetDeterminant(), 8.0, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetDeterminant", Script, "int RunGetDeterminant()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ContainsNaN)
	{
		static const char Script[] = R"(
int RunContainsNaN()
{
    if (!UnrealTransformWithNaN().ContainsNaN())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_ContainsNaN", Script, "int RunContainsNaN()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsValidPositive)
	{
		static const char Script[] = R"(
int RunIsValidPositive()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    if (!Base.IsValid())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_IsValidPositive", Script, "int RunIsValidPositive()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsValidNegative)
	{
		static const char Script[] = R"(
int RunIsValidNegative()
{
    if (FTransform(FQuat(0.0, 0.0, 0.0, 2.0), FVector(0.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)).IsValid())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_IsValidNegative", Script, "int RunIsValidNegative()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(RotationEquals)
	{
		static const char Script[] = R"(
int RunRotationEquals()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!A.RotationEquals(B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_RotationEquals", Script, "int RunRotationEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TranslationEquals)
	{
		static const char Script[] = R"(
int RunTranslationEquals()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!A.TranslationEquals(B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_TranslationEquals", Script, "int RunTranslationEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Scale3DEquals)
	{
		static const char Script[] = R"(
int RunScale3DEquals()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!A.Scale3DEquals(B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_Scale3DEquals", Script, "int RunScale3DEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Equals)
	{
		static const char Script[] = R"(
int RunEquals()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!A.Equals(B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_Equals", Script, "int RunEquals()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(EqualsNoScale)
	{
		static const char Script[] = R"(
int RunEqualsNoScale()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform DifferentScale(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(5.0, 6.0, 7.0));
    if (!A.EqualsNoScale(DifferentScale) || A.Equals(DifferentScale))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_EqualsNoScale", Script, "int RunEqualsNoScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetComponents)
	{
		static const char Script[] = R"(
int RunSetComponents()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Value;
    Value.SetComponents(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!Value.GetRotation().Equals(QuarterTurn, 0.0001) || !Value.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !Value.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetComponents", Script, "int RunSetComponents()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetIdentity)
	{
		static const char Script[] = R"(
int RunSetIdentity()
{
    FTransform Value;
    Value.SetIdentity();
    if (!Value.Equals(FTransform(), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetIdentity", Script, "int RunSetIdentity()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MultiplyScale3D)
	{
		static const char Script[] = R"(
int RunMultiplyScale3D()
{
    FTransform Value;
    Value.MultiplyScale3D(FVector(2.0, 3.0, 4.0));
    if (!Value.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_MultiplyScale3D", Script, "int RunMultiplyScale3D()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetTranslation)
	{
		static const char Script[] = R"(
int RunSetTranslation()
{
    FTransform Value;
    Value.SetTranslation(FVector(5.0, 6.0, 7.0));
    if (!Value.GetTranslation().Equals(FVector(5.0, 6.0, 7.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetTranslation", Script, "int RunSetTranslation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyTranslation)
	{
		static const char Script[] = R"(
int RunCopyTranslation()
{
    FTransform Value;
    Value.SetTranslation(FVector(5.0, 6.0, 7.0));
    FTransform CopyTarget;
    CopyTarget.CopyTranslation(Value);
    if (!CopyTarget.GetTranslation().Equals(FVector(5.0, 6.0, 7.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CopyTranslation", Script, "int RunCopyTranslation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ConcatenateRotation)
	{
		static const char Script[] = R"(
int RunConcatenateRotation()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform CopyTarget;
    CopyTarget.ConcatenateRotation(QuarterTurn);
    if (!CopyTarget.GetRotation().Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_ConcatenateRotation", Script, "int RunConcatenateRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AddToTranslation)
	{
		static const char Script[] = R"(
int RunAddToTranslation()
{
    FTransform CopyTarget;
    CopyTarget.SetTranslation(FVector(5.0, 6.0, 7.0));
    CopyTarget.AddToTranslation(FVector(1.0, 1.0, 1.0));
    if (!CopyTarget.GetTranslation().Equals(FVector(6.0, 7.0, 8.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AddToTranslation", Script, "int RunAddToTranslation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetRotation)
	{
		static const char Script[] = R"(
int RunSetRotation()
{
    FTransform CopyTarget;
    CopyTarget.SetRotation(FQuat(0.0, 0.0, 0.0, 1.0));
    if (!CopyTarget.GetRotation().Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetRotation", Script, "int RunSetRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyRotation)
	{
		static const char Script[] = R"(
int RunCopyRotation()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform CopyTarget;
    CopyTarget.SetRotation(FQuat(0.0, 0.0, 0.0, 1.0));
    CopyTarget.CopyRotation(FTransform(QuarterTurn));
    if (!CopyTarget.GetRotation().Equals(QuarterTurn, 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CopyRotation", Script, "int RunCopyRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetScale3D)
	{
		static const char Script[] = R"(
int RunSetScale3D()
{
    FTransform CopyTarget;
    CopyTarget.SetScale3D(FVector(3.0, 4.0, 5.0));
    if (!CopyTarget.GetScale3D().Equals(FVector(3.0, 4.0, 5.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetScale3D", Script, "int RunSetScale3D()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyScale3D)
	{
		static const char Script[] = R"(
int RunCopyScale3D()
{
    FTransform CopyTarget;
    CopyTarget.SetScale3D(FVector(3.0, 4.0, 5.0));
    CopyTarget.CopyScale3D(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(6.0, 7.0, 8.0)));
    if (!CopyTarget.GetScale3D().Equals(FVector(6.0, 7.0, 8.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CopyScale3D", Script, "int RunCopyScale3D()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SetTranslationAndScale3D)
	{
		static const char Script[] = R"(
int RunSetTranslationAndScale3D()
{
    FTransform CopyTarget;
    CopyTarget.SetTranslationAndScale3D(FVector(9.0, 8.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (!CopyTarget.GetTranslation().Equals(FVector(9.0, 8.0, 7.0), 0.0001) || !CopyTarget.GetScale3D().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SetTranslationAndScale3D", Script, "int RunSetTranslationAndScale3D()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AccumulateNoWeight)
	{
		static const char Script[] = R"(
int RunAccumulateNoWeight()
{
    FTransform Accumulated;
    Accumulated.Accumulate(FTransform(FVector(2.0, 0.0, 0.0)));
    if (!Accumulated.GetTranslation().Equals(FVector(2.0, 0.0, 0.0), 0.0001) || !Accumulated.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AccumulateNoWeight", Script, "int RunAccumulateNoWeight()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AccumulateWithWeight)
	{
		static const char Script[] = R"(
int RunAccumulateWithWeight()
{
    FTransform Weighted;
    Weighted.Accumulate(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)), 0.5);
    if (!Weighted.GetTranslation().Equals(FVector(1.0, 0.0, 0.0), 0.0001) || !Weighted.GetScale3D().Equals(FVector(0.5, 0.5, 0.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AccumulateWithWeight", Script, "int RunAccumulateWithWeight()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AccumulateWithShortestRotation)
	{
		static const char Script[] = R"(
int RunAccumulateWithShortestRotation()
{
    FTransform Shortest;
    Shortest.AccumulateWithShortestRotation(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 0.0, 0.0), FVector(3.0, 3.0, 3.0)), 0.5);
    if (!Shortest.GetTranslation().Equals(FVector(1.0, 0.0, 0.0), 0.0001) || !Shortest.GetScale3D().Equals(FVector(2.5, 2.5, 2.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AccumulateWithShortestRotation", Script, "int RunAccumulateWithShortestRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AccumulateWithAdditiveScale)
	{
		static const char Script[] = R"(
int RunAccumulateWithAdditiveScale()
{
    FTransform Additive;
    Additive.AccumulateWithAdditiveScale(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)), 0.5);
    if (!Additive.GetTranslation().Equals(FVector(1.0, 0.0, 0.0), 0.0001) || !Additive.GetScale3D().Equals(FVector(1.5, 1.5, 1.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AccumulateWithAdditiveScale", Script, "int RunAccumulateWithAdditiveScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(LerpTranslationScale3D)
	{
		static const char Script[] = R"(
int RunLerpTranslationScale3D()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform LerpValue;
    LerpValue.LerpTranslationScale3D(FTransform(), FTransform(QuarterTurn, FVector(10.0, 20.0, 30.0), FVector(3.0, 5.0, 7.0)), 0.25);
    if (!LerpValue.GetTranslation().Equals(FVector(2.5, 5.0, 7.5), 0.0001) || !LerpValue.GetScale3D().Equals(FVector(1.5, 2.0, 2.5), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_LerpTranslationScale3D", Script, "int RunLerpTranslationScale3D()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsRotationNormalizedFalse)
	{
		static const char Script[] = R"(
int RunIsRotationNormalizedFalse()
{
    FTransform NeedsNormalize(FQuat(0.0, 0.0, 0.0, 2.0));
    if (NeedsNormalize.IsRotationNormalized())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_IsRotationNormalizedFalse", Script, "int RunIsRotationNormalizedFalse()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NormalizeRotation)
	{
		static const char Script[] = R"(
int RunNormalizeRotation()
{
    FTransform NeedsNormalize(FQuat(0.0, 0.0, 0.0, 2.0));
    NeedsNormalize.NormalizeRotation();
    if (!NeedsNormalize.IsRotationNormalized())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_NormalizeRotation", Script, "int RunNormalizeRotation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyRotationPart)
	{
		static const char Script[] = R"(
int RunCopyRotationPart()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform RotationPart(FVector(9.0, 9.0, 9.0));
    RotationPart.CopyRotationPart(A);
    if (!RotationPart.GetRotation().Equals(QuarterTurn, 0.0001) || !RotationPart.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001) || !RotationPart.GetTranslation().Equals(FVector(9.0, 9.0, 9.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CopyRotationPart", Script, "int RunCopyRotationPart()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyTranslationAndScale3D)
	{
		static const char Script[] = R"(
int RunCopyTranslationAndScale3D()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform CopyTarget;
    CopyTarget.SetTranslationAndScale3D(FVector(9.0, 8.0, 7.0), FVector(1.0, 2.0, 3.0));
    FTransform TranslationScalePart(QuarterTurn, FVector(1.0, 1.0, 1.0), FVector(1.0, 1.0, 1.0));
    TranslationScalePart.CopyTranslationAndScale3D(CopyTarget);
    if (!TranslationScalePart.GetRotation().Equals(QuarterTurn, 0.0001) || !TranslationScalePart.GetTranslation().Equals(FVector(9.0, 8.0, 7.0), 0.0001) || !TranslationScalePart.GetScale3D().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_CopyTranslationAndScale3D", Script, "int RunCopyTranslationAndScale3D()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToHumanReadableString)
	{
		static const char Script[] = R"(
int RunToHumanReadableString()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FString HumanReadable = A.ToHumanReadableString();
    if (!HumanReadable.StartsWith("Rotation:"))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_ToHumanReadableString", Script, "int RunToHumanReadableString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FString Serialized = A.ToString();
    if (Serialized.IsEmpty())
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_ToString", Script, "int RunToString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(InitFromString)
	{
		static const char Script[] = R"(
int RunInitFromString()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FString Serialized = A.ToString();
    FTransform Parsed;
    if (!Parsed.InitFromString(Serialized))
    {
        return -1;
    }
    if (!Parsed.Equals(A, 0.01))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_InitFromString", Script, "int RunInitFromString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AnyHasNegativeScale)
	{
		static const char Script[] = R"(
int RunAnyHasNegativeScale()
{
    if (!FTransform::AnyHasNegativeScale(FVector(1.0, -1.0, 1.0), FVector(1.0, 1.0, 1.0)))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AnyHasNegativeScale", Script, "int RunAnyHasNegativeScale()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetSafeScaleReciprocal)
	{
		static const char Script[] = R"(
int RunGetSafeScaleReciprocal()
{
    FVector SafeReciprocal = FTransform::GetSafeScaleReciprocal(FVector(2.0, 0.0, -4.0));
    if (!SafeReciprocal.Equals(FVector(0.5, 0.0, -0.25), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_GetSafeScaleReciprocal", Script, "int RunGetSafeScaleReciprocal()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AreRotationsEqual)
	{
		static const char Script[] = R"(
int RunAreRotationsEqual()
{
    FTransform A(FQuat(0.0, 0.0, 0.0, 1.0), FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!FTransform::AreRotationsEqual(A, B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AreRotationsEqual", Script, "int RunAreRotationsEqual()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AreTranslationsEqual)
	{
		static const char Script[] = R"(
int RunAreTranslationsEqual()
{
    FTransform A(FQuat(0.0, 0.0, 0.0, 1.0), FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!FTransform::AreTranslationsEqual(A, B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AreTranslationsEqual", Script, "int RunAreTranslationsEqual()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AreScale3DsEqual)
	{
		static const char Script[] = R"(
int RunAreScale3DsEqual()
{
    FTransform A(FQuat(0.0, 0.0, 0.0, 1.0), FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!FTransform::AreScale3DsEqual(A, B))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AreScale3DsEqual", Script, "int RunAreScale3DsEqual()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(StaticMultiply)
	{
		static const char Script[] = R"(
int RunStaticMultiply()
{
    if (!FTransform::Multiply(FTransform(FVector(1.0, 2.0, 3.0)), FTransform(FVector(4.0, 5.0, 6.0))).Equals(FTransform(FVector(5.0, 7.0, 9.0)), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_StaticMultiply", Script, "int RunStaticMultiply()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(AddTranslations)
	{
		static const char Script[] = R"(
int RunAddTranslations()
{
    if (!FTransform::AddTranslations(FTransform(FVector(1.0, 2.0, 3.0)), FTransform(FVector(4.0, 5.0, 6.0))).Equals(FVector(5.0, 7.0, 9.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_AddTranslations", Script, "int RunAddTranslations()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(SubtractTranslations)
	{
		static const char Script[] = R"(
int RunSubtractTranslations()
{
    if (!FTransform::SubtractTranslations(FTransform(FVector(4.0, 5.0, 6.0)), FTransform(FVector(1.0, 2.0, 3.0))).Equals(FVector(3.0, 3.0, 3.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_SubtractTranslations", Script, "int RunSubtractTranslations()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(BlendFromIdentityAndAccumulate)
	{
		static const char Script[] = R"(
int RunBlendFromIdentityAndAccumulate()
{
    FTransform FinalAtom;
    FTransform SourceAtom(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 4.0, 6.0), FVector(0.0, 0.0, 0.0));
    FTransform::BlendFromIdentityAndAccumulate(FinalAtom, SourceAtom, 0.5);
    if (!FinalAtom.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !FinalAtom.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("FTransform_BlendFromIdentityAndAccumulate", Script, "int RunBlendFromIdentityAndAccumulate()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ScriptCallsUnrealWithTransform)
	{
		static const char Script[] = R"(
int RunTransformInteropTest()
{
    FTransform Value = UnrealBuildTransform(11.0, 12.0, 13.0);
    if (!UnrealAcceptsTransform(Value))
    {
        return -1;
    }

    if (!FMath::IsNearlyEqual(UnrealSumTransformTranslation(Value), 36.0, 0.0001))
    {
        return -2;
    }

    return int(Value.GetTranslation().X + Value.GetTranslation().Y + Value.GetTranslation().Z);
}
)";

		asIScriptFunction* Function = BuildFunction("FTransformInterop", Script, "int RunTransformInteropTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 36));
	}

	TEST_METHOD(UnrealCallsScriptWithTransform)
	{
		static const char Script[] = R"(
FTransform BuildTransformForUnreal(const FTransform &in Value)
{
    FTransform Result(Value);
    Result.AddToTranslation(FVector(1.0, 2.0, 3.0));
    Result.SetScale3D(Result.GetScale3D() + FVector(1.0, 1.0, 1.0));
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FTransformUnrealCall", Script, "FTransform BuildTransformForUnreal(const FTransform &in Value)");
		ASSERT_THAT(IsNotNull(Function));

		FTransform Input(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 4.0, 6.0), FVector(1.0, 2.0, 3.0));

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));
		ASSERT_THAT(IsTrue(Context->SetArgObject(0, &Input) >= 0));

		const int ExecuteResult = Context->Execute();
		if (ExecuteResult == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		ASSERT_THAT(IsTrue(ExecuteResult == asEXECUTION_FINISHED));

		const FTransform* ReturnValue = static_cast<const FTransform*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(ReturnValue->Equals(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(3.0, 6.0, 9.0), FVector(2.0, 3.0, 4.0)), 0.0001)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
