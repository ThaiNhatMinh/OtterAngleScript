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

	TEST_METHOD(ConstructorsOperatorsAndComposition)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTransform")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FQuat")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRotator")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EAxisType")));

		static const char Script[] = R"(
int RunConstructorsOperatorsAndComposition()
{
    const double HalfPi = 1.5707963267948966;
    const double InvSqrtTwo = 0.7071067811865476;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);

    FTransform Identity;
    if (!Identity.Equals(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)), 0.0001))
    {
        return -1;
    }

    FTransform TranslationOnly(FVector(1.0, 2.0, 3.0));
    if (!TranslationOnly.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !TranslationOnly.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -2;
    }

    FTransform RotationOnly(QuarterTurn);
    if (!RotationOnly.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -3;
    }

    FTransform RotatorOnly(FRotator(0.0, 90.0, 0.0));
    if (!RotatorOnly.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -4;
    }

    FTransform FullQuat(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!FullQuat.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !FullQuat.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001) || !FullQuat.GetRotation().Equals(QuarterTurn, 0.0001))
    {
        return -5;
    }

    FTransform FullRotator(FRotator(0.0, 90.0, 0.0), FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!FullRotator.GetLocation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !FullRotator.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001))
    {
        return -6;
    }

    FTransform Copy(FullQuat);
    if (!Copy.Equals(FullQuat, 0.0001))
    {
        return -7;
    }

    FTransform Assigned;
    Assigned = FullQuat;
    if (!Assigned.Equals(FullQuat, 0.0001))
    {
        return -8;
    }

    FTransform Added = TranslationOnly + TranslationOnly;
    if (!Added.GetTranslation().Equals(FVector(2.0, 4.0, 6.0), 0.0001) || !Added.GetScale3D().Equals(FVector(2.0, 2.0, 2.0), 0.0001) || !Added.GetRotation().opEquals(FQuat(0.0, 0.0, 0.0, 2.0)))
    {
        return -9;
    }

    FTransform AddAssign;
    AddAssign += TranslationOnly;
    if (!AddAssign.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -10;
    }

    FTransform ScalarScaled = TranslationOnly * 2.0;
    if (!ScalarScaled.GetTranslation().Equals(FVector(2.0, 4.0, 6.0), 0.0001) || !ScalarScaled.GetScale3D().Equals(FVector(2.0, 2.0, 2.0), 0.0001) || !ScalarScaled.GetRotation().opEquals(FQuat(0.0, 0.0, 0.0, 2.0)))
    {
        return -11;
    }

    FTransform ScalarAssign(FVector(2.0, 4.0, 6.0));
    ScalarAssign *= 0.5;
    if (!ScalarAssign.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !ScalarAssign.GetScale3D().Equals(FVector(0.5, 0.5, 0.5), 0.0001))
    {
        return -12;
    }

    FTransform Composed = FTransform(FVector(1.0, 2.0, 3.0)) * FTransform(FVector(4.0, 5.0, 6.0));
    if (!Composed.Equals(FTransform(FVector(5.0, 7.0, 9.0)), 0.0001))
    {
        return -13;
    }

    FTransform ComposeAssign(FVector(1.0, 2.0, 3.0));
    ComposeAssign *= FTransform(FVector(4.0, 5.0, 6.0));
    if (!ComposeAssign.Equals(FTransform(FVector(5.0, 7.0, 9.0)), 0.0001))
    {
        return -14;
    }

    FTransform Rotated = FTransform() * QuarterTurn;
    if (!Rotated.GetRotation().Equals(FQuat(0.0, 0.0, InvSqrtTwo, InvSqrtTwo), 0.0001))
    {
        return -15;
    }

    FTransform RotatedAssign;
    RotatedAssign *= QuarterTurn;
    if (!RotatedAssign.GetRotation().Equals(FQuat(0.0, 0.0, InvSqrtTwo, InvSqrtTwo), 0.0001))
    {
        return -16;
    }

    return 16;
}
)";

		asIScriptFunction* Function = BuildFunction("FTransformConstructors", Script, "int RunConstructorsOperatorsAndComposition()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 16));
	}

	TEST_METHOD(InstanceMathMethods)
	{
		static const char Script[] = R"(
int RunInstanceMathMethods()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform Base(QuarterTurn, FVector(10.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));

    FVector WorldPos = Base.TransformPosition(FVector(1.0, 0.0, 0.0));
    if (!WorldPos.Equals(FVector(10.0, 2.0, 0.0), 0.0001))
    {
        return -1;
    }

    FVector WorldPosNoScale = Base.TransformPositionNoScale(FVector(1.0, 0.0, 0.0));
    if (!WorldPosNoScale.Equals(FVector(10.0, 1.0, 0.0), 0.0001))
    {
        return -2;
    }

    if (!Base.InverseTransformPosition(WorldPos).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -3;
    }

    if (!Base.InverseTransformPositionNoScale(WorldPosNoScale).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -4;
    }

    if (!Base.TransformVector(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 2.0, 0.0), 0.0001))
    {
        return -5;
    }

    if (!Base.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -6;
    }

    if (!Base.InverseTransformVector(FVector(0.0, 2.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -7;
    }

    if (!Base.InverseTransformVectorNoScale(FVector(0.0, 1.0, 0.0)).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -8;
    }

    if (!Base.TransformRotation(FQuat(0.0, 0.0, 0.0, 1.0)).Equals(QuarterTurn, 0.0001))
    {
        return -9;
    }

    if (!Base.InverseTransformRotation(QuarterTurn).Equals(FQuat(0.0, 0.0, 0.0, 1.0), 0.0001))
    {
        return -10;
    }

    if (!Base.Inverse().TransformPosition(WorldPos).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -11;
    }

    if (!Base.InverseFast().TransformPosition(WorldPos).Equals(FVector(1.0, 0.0, 0.0), 0.0001))
    {
        return -12;
    }

    FTransform Blended;
    Blended.Blend(FTransform(), FTransform(FVector(10.0, 20.0, 30.0)), 0.25);
    if (!Blended.GetTranslation().Equals(FVector(2.5, 5.0, 7.5), 0.0001))
    {
        return -13;
    }

    FTransform BlendedWith;
    BlendedWith.BlendWith(FTransform(FVector(10.0, 20.0, 30.0)), 0.25);
    if (!BlendedWith.GetTranslation().Equals(FVector(2.5, 5.0, 7.5), 0.0001))
    {
        return -14;
    }

    FTransform TranslationScaler(FVector(1.0, 2.0, 3.0));
    TranslationScaler.ScaleTranslation(FVector(2.0, 3.0, 4.0));
    TranslationScaler.ScaleTranslation(0.5);
    if (!TranslationScaler.GetTranslation().Equals(FVector(1.0, 3.0, 6.0), 0.0001))
    {
        return -15;
    }

    FTransform ScaleRemoval(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    ScaleRemoval.RemoveScaling();
    if (!ScaleRemoval.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001) || !ScaleRemoval.IsRotationNormalized())
    {
        return -16;
    }

    FTransform AxisScale(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(2.0, -3.0, 4.0));
    if (!FMath::IsNearlyEqual(AxisScale.GetMaximumAxisScale(), 4.0, 0.0001) || !FMath::IsNearlyEqual(AxisScale.GetMinimumAxisScale(), 2.0, 0.0001))
    {
        return -17;
    }

    FTransform Child(FVector(10.0, 0.0, 0.0));
    FTransform Parent(FVector(3.0, 0.0, 0.0));
    if (!Child.GetRelativeTransform(Parent).Equals(FTransform(FVector(7.0, 0.0, 0.0)), 0.0001))
    {
        return -18;
    }

    if (!Child.GetRelativeTransformReverse(Parent).Equals(FTransform(FVector(-7.0, 0.0, 0.0)), 0.0001))
    {
        return -19;
    }

    FTransform RelativeSet(FVector(10.0, 0.0, 0.0));
    RelativeSet.SetToRelativeTransform(Parent);
    if (!RelativeSet.Equals(FTransform(FVector(7.0, 0.0, 0.0)), 0.0001))
    {
        return -20;
    }

    FTransform Scaled = FTransform(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!Scaled.GetScaled(2.0).GetScale3D().Equals(FVector(4.0, 6.0, 8.0), 0.0001))
    {
        return -21;
    }

    if (!Scaled.GetScaled(FVector(2.0, 1.0, 0.5)).GetScale3D().Equals(FVector(4.0, 3.0, 2.0), 0.0001))
    {
        return -22;
    }

    if (!Base.GetScaledAxis(X).Equals(FVector(0.0, 2.0, 0.0), 0.0001))
    {
        return -23;
    }

    if (!Base.GetUnitAxis(X).Equals(FVector(0.0, 1.0, 0.0), 0.0001))
    {
        return -24;
    }

    FTransform Mirrored(FVector(1.0, 2.0, 3.0));
    Mirrored.Mirror(X, Y);
    if (!Mirrored.IsValid())
    {
        return -25;
    }

    if (!Base.GetLocation().Equals(FVector(10.0, 0.0, 0.0), 0.0001))
    {
        return -26;
    }

    if (!Base.Rotator().Equals(FRotator(0.0, 90.0, 0.0), 0.0001))
    {
        return -27;
    }

    if (!FMath::IsNearlyEqual(Base.GetDeterminant(), 8.0, 0.0001))
    {
        return -28;
    }

    FTransform SetLocationValue;
    SetLocationValue.SetLocation(FVector(1.0, 2.0, 3.0));
    if (!SetLocationValue.GetLocation().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -29;
    }

    if (!UnrealTransformWithNaN().ContainsNaN())
    {
        return -30;
    }

    if (!Base.IsValid())
    {
        return -31;
    }

    if (FTransform(FQuat(0.0, 0.0, 0.0, 2.0), FVector(0.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)).IsValid())
    {
        return -32;
    }

    return 32;
}
)";

		asIScriptFunction* Function = BuildFunction("FTransformMathMethods", Script, "int RunInstanceMathMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 32));
	}

	TEST_METHOD(MutationEqualityAndSerializationMethods)
	{
		static const char Script[] = R"(
int RunMutationEqualityAndSerializationMethods()
{
    const double HalfPi = 1.5707963267948966;
    FQuat QuarterTurn(FVector(0.0, 0.0, 1.0), HalfPi);
    FTransform A(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);

    if (!A.RotationEquals(B) || !A.TranslationEquals(B) || !A.Scale3DEquals(B) || !A.Equals(B))
    {
        return -1;
    }

    FTransform DifferentScale(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(5.0, 6.0, 7.0));
    if (!A.EqualsNoScale(DifferentScale) || A.Equals(DifferentScale))
    {
        return -2;
    }

    FTransform Value;
    Value.SetComponents(QuarterTurn, FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    if (!Value.GetRotation().Equals(QuarterTurn, 0.0001) || !Value.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !Value.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001))
    {
        return -3;
    }

    Value.SetIdentity();
    if (!Value.Equals(FTransform(), 0.0001))
    {
        return -4;
    }

    Value.MultiplyScale3D(FVector(2.0, 3.0, 4.0));
    if (!Value.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001))
    {
        return -5;
    }

    Value.SetTranslation(FVector(5.0, 6.0, 7.0));
    if (!Value.GetTranslation().Equals(FVector(5.0, 6.0, 7.0), 0.0001))
    {
        return -6;
    }

    FTransform CopyTarget;
    CopyTarget.CopyTranslation(Value);
    if (!CopyTarget.GetTranslation().Equals(FVector(5.0, 6.0, 7.0), 0.0001))
    {
        return -7;
    }

    CopyTarget.ConcatenateRotation(QuarterTurn);
    if (!CopyTarget.GetRotation().Equals(QuarterTurn, 0.0001))
    {
        return -8;
    }

    CopyTarget.AddToTranslation(FVector(1.0, 1.0, 1.0));
    if (!CopyTarget.GetTranslation().Equals(FVector(6.0, 7.0, 8.0), 0.0001))
    {
        return -9;
    }

    CopyTarget.SetRotation(FQuat(0.0, 0.0, 0.0, 1.0));
    CopyTarget.CopyRotation(FTransform(QuarterTurn));
    if (!CopyTarget.GetRotation().Equals(QuarterTurn, 0.0001))
    {
        return -10;
    }

    CopyTarget.SetScale3D(FVector(3.0, 4.0, 5.0));
    CopyTarget.CopyScale3D(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(0.0, 0.0, 0.0), FVector(6.0, 7.0, 8.0)));
    if (!CopyTarget.GetScale3D().Equals(FVector(6.0, 7.0, 8.0), 0.0001))
    {
        return -11;
    }

    CopyTarget.SetTranslationAndScale3D(FVector(9.0, 8.0, 7.0), FVector(1.0, 2.0, 3.0));
    if (!CopyTarget.GetTranslation().Equals(FVector(9.0, 8.0, 7.0), 0.0001) || !CopyTarget.GetScale3D().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -12;
    }

    FTransform Accumulated;
    Accumulated.Accumulate(FTransform(FVector(2.0, 0.0, 0.0)));
    if (!Accumulated.GetTranslation().Equals(FVector(2.0, 0.0, 0.0), 0.0001) || !Accumulated.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -13;
    }

    FTransform Weighted;
    Weighted.Accumulate(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)), 0.5);
    if (!Weighted.GetTranslation().Equals(FVector(1.0, 0.0, 0.0), 0.0001) || !Weighted.GetScale3D().Equals(FVector(0.5, 0.5, 0.5), 0.0001))
    {
        return -14;
    }

    FTransform Shortest;
    Shortest.AccumulateWithShortestRotation(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 0.0, 0.0), FVector(3.0, 3.0, 3.0)), 0.5);
    if (!Shortest.GetTranslation().Equals(FVector(1.0, 0.0, 0.0), 0.0001) || !Shortest.GetScale3D().Equals(FVector(2.5, 2.5, 2.5), 0.0001))
    {
        return -15;
    }

    FTransform Additive;
    Additive.AccumulateWithAdditiveScale(FTransform(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0)), 0.5);
    if (!Additive.GetTranslation().Equals(FVector(1.0, 0.0, 0.0), 0.0001) || !Additive.GetScale3D().Equals(FVector(1.5, 1.5, 1.5), 0.0001))
    {
        return -16;
    }

    FTransform LerpValue;
    LerpValue.LerpTranslationScale3D(FTransform(), FTransform(QuarterTurn, FVector(10.0, 20.0, 30.0), FVector(3.0, 5.0, 7.0)), 0.25);
    if (!LerpValue.GetTranslation().Equals(FVector(2.5, 5.0, 7.5), 0.0001) || !LerpValue.GetScale3D().Equals(FVector(1.5, 2.0, 2.5), 0.0001))
    {
        return -17;
    }

    FTransform NeedsNormalize(FQuat(0.0, 0.0, 0.0, 2.0));
    if (NeedsNormalize.IsRotationNormalized())
    {
        return -18;
    }

    NeedsNormalize.NormalizeRotation();
    if (!NeedsNormalize.IsRotationNormalized())
    {
        return -19;
    }

    if (!CopyTarget.GetRotation().Equals(QuarterTurn, 0.0001) || !CopyTarget.GetTranslation().Equals(FVector(9.0, 8.0, 7.0), 0.0001) || !CopyTarget.GetScale3D().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -20;
    }

    FTransform RotationPart(FVector(9.0, 9.0, 9.0));
    RotationPart.CopyRotationPart(A);
    if (!RotationPart.GetRotation().Equals(QuarterTurn, 0.0001) || !RotationPart.GetScale3D().Equals(FVector(2.0, 3.0, 4.0), 0.0001) || !RotationPart.GetTranslation().Equals(FVector(9.0, 9.0, 9.0), 0.0001))
    {
        return -21;
    }

    FTransform TranslationScalePart(QuarterTurn, FVector(1.0, 1.0, 1.0), FVector(1.0, 1.0, 1.0));
    TranslationScalePart.CopyTranslationAndScale3D(CopyTarget);
    if (!TranslationScalePart.GetRotation().Equals(QuarterTurn, 0.0001) || !TranslationScalePart.GetTranslation().Equals(FVector(9.0, 8.0, 7.0), 0.0001) || !TranslationScalePart.GetScale3D().Equals(FVector(1.0, 2.0, 3.0), 0.0001))
    {
        return -22;
    }

    FString HumanReadable = A.ToHumanReadableString();
    if (!HumanReadable.StartsWith("Rotation:"))
    {
        return -23;
    }

    FString Serialized = A.ToString();
    if (Serialized.IsEmpty())
    {
        return -24;
    }

    FTransform Parsed;
    if (!Parsed.InitFromString(Serialized))
    {
        return -25;
    }

    if (!Parsed.Equals(A, 0.01))
    {
        return -26;
    }

    return 26;
}
)";

		asIScriptFunction* Function = BuildFunction("FTransformMutationMethods", Script, "int RunMutationEqualityAndSerializationMethods()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 26));
	}

	TEST_METHOD(StaticNamespaceFunctions)
	{
		static const char Script[] = R"(
int RunStaticNamespaceFunctions()
{
    if (!FTransform::AnyHasNegativeScale(FVector(1.0, -1.0, 1.0), FVector(1.0, 1.0, 1.0)))
    {
        return -1;
    }

    FVector SafeReciprocal = FTransform::GetSafeScaleReciprocal(FVector(2.0, 0.0, -4.0));
    if (!SafeReciprocal.Equals(FVector(0.5, 0.0, -0.25), 0.0001))
    {
        return -2;
    }

    FTransform A(FQuat(0.0, 0.0, 0.0, 1.0), FVector(1.0, 2.0, 3.0), FVector(2.0, 3.0, 4.0));
    FTransform B(A);
    if (!FTransform::AreRotationsEqual(A, B) || !FTransform::AreTranslationsEqual(A, B) || !FTransform::AreScale3DsEqual(A, B))
    {
        return -3;
    }

    if (!FTransform::Multiply(FTransform(FVector(1.0, 2.0, 3.0)), FTransform(FVector(4.0, 5.0, 6.0))).Equals(FTransform(FVector(5.0, 7.0, 9.0)), 0.0001))
    {
        return -4;
    }

    if (!FTransform::AddTranslations(FTransform(FVector(1.0, 2.0, 3.0)), FTransform(FVector(4.0, 5.0, 6.0))).Equals(FVector(5.0, 7.0, 9.0), 0.0001))
    {
        return -5;
    }

    if (!FTransform::SubtractTranslations(FTransform(FVector(4.0, 5.0, 6.0)), FTransform(FVector(1.0, 2.0, 3.0))).Equals(FVector(3.0, 3.0, 3.0), 0.0001))
    {
        return -6;
    }

    FTransform FinalAtom;
    FTransform SourceAtom(FQuat(0.0, 0.0, 0.0, 1.0), FVector(2.0, 4.0, 6.0), FVector(0.0, 0.0, 0.0));
    FTransform::BlendFromIdentityAndAccumulate(FinalAtom, SourceAtom, 0.5);
    if (!FinalAtom.GetTranslation().Equals(FVector(1.0, 2.0, 3.0), 0.0001) || !FinalAtom.GetScale3D().Equals(FVector(1.0, 1.0, 1.0), 0.0001))
    {
        return -7;
    }

    return 7;
}
)";

		asIScriptFunction* Function = BuildFunction("FTransformStaticFunctions", Script, "int RunStaticNamespaceFunctions()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 7));
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
