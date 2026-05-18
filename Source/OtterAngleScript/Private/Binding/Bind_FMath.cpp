// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

namespace
{
#define DEFINE_UNARY_FMATH_WRAPPER(Name, ReturnType, ArgType, FunctionName) \
	ReturnType Name(ArgType Value) \
	{ \
		return FMath::FunctionName(Value); \
	}

#define DEFINE_BINARY_FMATH_WRAPPER(Name, ReturnType, ArgTypeA, ArgTypeB, FunctionName) \
	ReturnType Name(ArgTypeA A, ArgTypeB B) \
	{ \
		return FMath::FunctionName(A, B); \
	}

	DEFINE_UNARY_FMATH_WRAPPER(FMath_AbsInt, int32, int32, Abs)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AbsInt64, int64, int64, Abs)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AbsFloat, float, float, Abs)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AbsDouble, double, double, Abs)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_SinFloat, float, float, Sin)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_SinDouble, double, double, Sin)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_CosFloat, float, float, Cos)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_CosDouble, double, double, Cos)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_TanFloat, float, float, Tan)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_TanDouble, double, double, Tan)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AsinFloat, float, float, Asin)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AsinDouble, double, double, Asin)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AcosFloat, float, float, Acos)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AcosDouble, double, double, Acos)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AtanFloat, float, float, Atan)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_AtanDouble, double, double, Atan)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_SqrtFloat, float, float, Sqrt)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_SqrtDouble, double, double, Sqrt)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_ExpFloat, float, float, Exp)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_ExpDouble, double, double, Exp)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_LogeFloat, float, float, Loge)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_LogeDouble, double, double, Loge)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_Log2Float, float, float, Log2)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_Log2Double, double, double, Log2)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_FastAsinFloat, float, float, FastAsin)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_FastAsinDouble, double, double, FastAsin)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_DegreesToRadiansFloat, float, float, DegreesToRadians)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_DegreesToRadiansDouble, double, double, DegreesToRadians)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_RadiansToDegreesFloat, float, float, RadiansToDegrees)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_RadiansToDegreesDouble, double, double, RadiansToDegrees)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_UnwindRadiansFloat, float, float, UnwindRadians)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_UnwindRadiansDouble, double, double, UnwindRadians)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_UnwindDegreesFloat, float, float, UnwindDegrees)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_UnwindDegreesDouble, double, double, UnwindDegrees)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_RandHelperInt, int32, int32, RandHelper)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_RandHelperInt64, int64, int64, RandHelper64)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_IsPowerOfTwoInt, bool, int32, IsPowerOfTwo)
	DEFINE_UNARY_FMATH_WRAPPER(FMath_IsPowerOfTwoInt64, bool, int64, IsPowerOfTwo)

	DEFINE_BINARY_FMATH_WRAPPER(FMath_Atan2Float, float, float, float, Atan2)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_Atan2Double, double, double, double, Atan2)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_PowFloat, float, float, float, Pow)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_PowDouble, double, double, double, Pow)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_FmodFloat, float, float, float, Fmod)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_FmodDouble, double, double, double, Fmod)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_LogXFloat, float, float, float, LogX)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_LogXDouble, double, double, double, LogX)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_RandRangeInt, int32, int32, int32, RandRange)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_RandRangeInt64, int64, int64, int64, RandRange)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_RandRangeFloat, float, float, float, RandRange)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_RandRangeDouble, double, double, double, RandRange)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_FRandRangeFloat, float, float, float, FRandRange)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_FRandRangeDouble, double, double, double, FRandRange)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_IsNearlyEqualFloatByUlp, bool, float, float, IsNearlyEqualByULP)
	DEFINE_BINARY_FMATH_WRAPPER(FMath_IsNearlyEqualDoubleByUlp, bool, double, double, IsNearlyEqualByULP)

	bool FMath_RandBoolWrapper()
	{
		return FMath::RandBool();
	}

	bool FMath_IsWithinInt(int32 Value, int32 MinValue, int32 MaxValue)
	{
		return FMath::IsWithin(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinInt64(int64 Value, int64 MinValue, int64 MaxValue)
	{
		return FMath::IsWithin(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinFloat(float Value, float MinValue, float MaxValue)
	{
		return FMath::IsWithin(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinDouble(double Value, double MinValue, double MaxValue)
	{
		return FMath::IsWithin(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinInclusiveInt(int32 Value, int32 MinValue, int32 MaxValue)
	{
		return FMath::IsWithinInclusive(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinInclusiveInt64(int64 Value, int64 MinValue, int64 MaxValue)
	{
		return FMath::IsWithinInclusive(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinInclusiveFloat(float Value, float MinValue, float MaxValue)
	{
		return FMath::IsWithinInclusive(Value, MinValue, MaxValue);
	}

	bool FMath_IsWithinInclusiveDouble(double Value, double MinValue, double MaxValue)
	{
		return FMath::IsWithinInclusive(Value, MinValue, MaxValue);
	}

	bool FMath_IsNearlyEqualFloat(float A, float B, float Tolerance)
	{
		return FMath::IsNearlyEqual(A, B, Tolerance);
	}

	bool FMath_IsNearlyEqualDouble(double A, double B, double Tolerance)
	{
		return FMath::IsNearlyEqual(A, B, Tolerance);
	}

	bool FMath_IsNearlyZeroFloat(float Value, float Tolerance)
	{
		return FMath::IsNearlyZero(Value, Tolerance);
	}

	bool FMath_IsNearlyZeroDouble(double Value, double Tolerance)
	{
		return FMath::IsNearlyZero(Value, Tolerance);
	}

	bool FMath_IsNearlyEqualByUlpFloat(float A, float B, int32 MaxUlps)
	{
		return FMath::IsNearlyEqualByULP(A, B, MaxUlps);
	}

	bool FMath_IsNearlyEqualByUlpDouble(double A, double B, int32 MaxUlps)
	{
		return FMath::IsNearlyEqualByULP(A, B, MaxUlps);
	}

	int32 FMath_Max3Int(int32 A, int32 B, int32 C)
	{
		return FMath::Max3(A, B, C);
	}

	double FMath_Max3Double(double A, double B, double C)
	{
		return FMath::Max3(A, B, C);
	}

	int32 FMath_Min3Int(int32 A, int32 B, int32 C)
	{
		return FMath::Min3(A, B, C);
	}

	double FMath_Min3Double(double A, double B, double C)
	{
		return FMath::Min3(A, B, C);
	}

	int32 FMath_Max3IndexInt(int32 A, int32 B, int32 C)
	{
		return FMath::Max3Index(A, B, C);
	}

	int32 FMath_Max3IndexDouble(double A, double B, double C)
	{
		return FMath::Max3Index(A, B, C);
	}

	int32 FMath_Min3IndexInt(int32 A, int32 B, int32 C)
	{
		return FMath::Min3Index(A, B, C);
	}

	int32 FMath_Min3IndexDouble(double A, double B, double C)
	{
		return FMath::Min3Index(A, B, C);
	}

	int32 FMath_SquareIntWrapper(int32 Value)
	{
		return FMath::Square(Value);
	}

	int64 FMath_SquareInt64Wrapper(int64 Value)
	{
		return FMath::Square(Value);
	}

	float FMath_SquareFloatWrapper(float Value)
	{
		return FMath::Square(Value);
	}

	double FMath_SquareDoubleWrapper(double Value)
	{
		return FMath::Square(Value);
	}

	int32 FMath_CubeIntWrapper(int32 Value)
	{
		return FMath::Cube(Value);
	}

	int64 FMath_CubeInt64Wrapper(int64 Value)
	{
		return FMath::Cube(Value);
	}

	float FMath_CubeFloatWrapper(float Value)
	{
		return FMath::Cube(Value);
	}

	double FMath_CubeDoubleWrapper(double Value)
	{
		return FMath::Cube(Value);
	}

	int32 FMath_ClampInt(int32 Value, int32 MinValue, int32 MaxValue)
	{
		return FMath::Clamp(Value, MinValue, MaxValue);
	}

	int64 FMath_ClampInt64(int64 Value, int64 MinValue, int64 MaxValue)
	{
		return FMath::Clamp(Value, MinValue, MaxValue);
	}

	float FMath_ClampFloat(float Value, float MinValue, float MaxValue)
	{
		return FMath::Clamp(Value, MinValue, MaxValue);
	}

	double FMath_ClampDouble(double Value, double MinValue, double MaxValue)
	{
		return FMath::Clamp(Value, MinValue, MaxValue);
	}

	float FMath_WrapFloat(float Value, float MinValue, float MaxValue)
	{
		return FMath::Wrap(Value, MinValue, MaxValue);
	}

	double FMath_WrapDouble(double Value, double MinValue, double MaxValue)
	{
		return FMath::Wrap(Value, MinValue, MaxValue);
	}

	int32 FMath_WrapExclusiveInt(int32 Value, int32 MinValue, int32 MaxValue)
	{
		return FMath::WrapExclusive(Value, MinValue, MaxValue);
	}

	int64 FMath_WrapExclusiveInt64(int64 Value, int64 MinValue, int64 MaxValue)
	{
		return FMath::WrapExclusive(Value, MinValue, MaxValue);
	}

	int32 FMath_GridSnapInt(int32 Value, int32 Grid)
	{
		return FMath::GridSnap(Value, Grid);
	}

	int64 FMath_GridSnapInt64(int64 Value, int64 Grid)
	{
		return FMath::GridSnap(Value, Grid);
	}

	float FMath_GridSnapFloat(float Value, float Grid)
	{
		return FMath::GridSnap(Value, Grid);
	}

	double FMath_GridSnapDouble(double Value, double Grid)
	{
		return FMath::GridSnap(Value, Grid);
	}

	int32 FMath_DivideAndRoundUpInt(int32 Dividend, int32 Divisor)
	{
		return FMath::DivideAndRoundUp(Dividend, Divisor);
	}

	int64 FMath_DivideAndRoundUpInt64(int64 Dividend, int64 Divisor)
	{
		return FMath::DivideAndRoundUp(Dividend, Divisor);
	}

	int32 FMath_DivideAndRoundDownInt(int32 Dividend, int32 Divisor)
	{
		return FMath::DivideAndRoundDown(Dividend, Divisor);
	}

	int64 FMath_DivideAndRoundDownInt64(int64 Dividend, int64 Divisor)
	{
		return FMath::DivideAndRoundDown(Dividend, Divisor);
	}

	int32 FMath_DivideAndRoundNearestInt(int32 Dividend, int32 Divisor)
	{
		return FMath::DivideAndRoundNearest(Dividend, Divisor);
	}

	int64 FMath_DivideAndRoundNearestInt64(int64 Dividend, int64 Divisor)
	{
		return FMath::DivideAndRoundNearest(Dividend, Divisor);
	}

	void FMath_SinCosFloat(float& OutSin, float& OutCos, float Value)
	{
		FMath::SinCos(&OutSin, &OutCos, Value);
	}

	void FMath_SinCosDouble(double& OutSin, double& OutCos, double Value)
	{
		FMath::SinCos(&OutSin, &OutCos, Value);
	}

	float FMath_ClampAngleFloat(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees)
	{
		return FMath::ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees);
	}

	double FMath_ClampAngleDouble(double AngleDegrees, double MinAngleDegrees, double MaxAngleDegrees)
	{
		return FMath::ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees);
	}

	float FMath_FindDeltaAngleDegreesFloat(float A1, float A2)
	{
		return FMath::FindDeltaAngleDegrees(A1, A2);
	}

	double FMath_FindDeltaAngleDegreesDouble(double A1, double A2)
	{
		return FMath::FindDeltaAngleDegrees(A1, A2);
	}

	float FMath_FindDeltaAngleRadiansFloat(float A1, float A2)
	{
		return FMath::FindDeltaAngleRadians(A1, A2);
	}

	double FMath_FindDeltaAngleRadiansDouble(double A1, double A2)
	{
		return FMath::FindDeltaAngleRadians(A1, A2);
	}

	void FMath_WindRelativeAnglesDegreesFloat(float InAngle0, float& InOutAngle1)
	{
		FMath::WindRelativeAnglesDegrees(InAngle0, InOutAngle1);
	}

	void FMath_WindRelativeAnglesDegreesDouble(double InAngle0, double& InOutAngle1)
	{
		FMath::WindRelativeAnglesDegrees(InAngle0, InOutAngle1);
	}

	void FMath_CartesianToPolarFloat(float X, float Y, float& OutRadius, float& OutAngle)
	{
		FMath::CartesianToPolar(X, Y, OutRadius, OutAngle);
	}

	void FMath_CartesianToPolarDouble(double X, double Y, double& OutRadius, double& OutAngle)
	{
		FMath::CartesianToPolar(X, Y, OutRadius, OutAngle);
	}

	void FMath_PolarToCartesianFloat(float Radius, float Angle, float& OutX, float& OutY)
	{
		FMath::PolarToCartesian(Radius, Angle, OutX, OutY);
	}

	void FMath_PolarToCartesianDouble(double Radius, double Angle, double& OutX, double& OutY)
	{
		FMath::PolarToCartesian(Radius, Angle, OutX, OutY);
	}

	float FMath_FixedTurn(float InCurrent, float InDesired, float InDeltaRate)
	{
		return FMath::FixedTurn(InCurrent, InDesired, InDeltaRate);
	}

	float FMath_LerpFloat(float A, float B, float Alpha)
	{
		return FMath::Lerp(A, B, Alpha);
	}

	double FMath_LerpDouble(double A, double B, double Alpha)
	{
		return FMath::Lerp(A, B, Alpha);
	}

	float FMath_LerpStableFloat(float A, float B, float Alpha)
	{
		return FMath::LerpStable(A, B, Alpha);
	}

	double FMath_LerpStableDouble(double A, double B, double Alpha)
	{
		return FMath::LerpStable(A, B, Alpha);
	}

	float FMath_BiLerpFloat(float P00, float P10, float P01, float P11, float FracX, float FracY)
	{
		return FMath::BiLerp(P00, P10, P01, P11, FracX, FracY);
	}

	double FMath_BiLerpDouble(double P00, double P10, double P01, double P11, double FracX, double FracY)
	{
		return FMath::BiLerp(P00, P10, P01, P11, FracX, FracY);
	}

	float FMath_CubicInterpFloat(float P0, float T0, float P1, float T1, float Alpha)
	{
		return FMath::CubicInterp(P0, T0, P1, T1, Alpha);
	}

	double FMath_CubicInterpDouble(double P0, double T0, double P1, double T1, double Alpha)
	{
		return FMath::CubicInterp(P0, T0, P1, T1, Alpha);
	}

	float FMath_CubicInterpDerivativeFloat(float P0, float T0, float P1, float T1, float Alpha)
	{
		return FMath::CubicInterpDerivative(P0, T0, P1, T1, Alpha);
	}

	double FMath_CubicInterpDerivativeDouble(double P0, double T0, double P1, double T1, double Alpha)
	{
		return FMath::CubicInterpDerivative(P0, T0, P1, T1, Alpha);
	}

	float FMath_CubicInterpSecondDerivativeFloat(float P0, float T0, float P1, float T1, float Alpha)
	{
		return FMath::CubicInterpSecondDerivative(P0, T0, P1, T1, Alpha);
	}

	double FMath_CubicInterpSecondDerivativeDouble(double P0, double T0, double P1, double T1, double Alpha)
	{
		return FMath::CubicInterpSecondDerivative(P0, T0, P1, T1, Alpha);
	}

	float FMath_InterpEaseInFloat(float A, float B, float Alpha, float Exp)
	{
		return FMath::InterpEaseIn(A, B, Alpha, Exp);
	}

	double FMath_InterpEaseInDouble(double A, double B, double Alpha, double Exp)
	{
		return FMath::InterpEaseIn(A, B, static_cast<float>(Alpha), static_cast<float>(Exp));
	}

	float FMath_InterpEaseOutFloat(float A, float B, float Alpha, float Exp)
	{
		return FMath::InterpEaseOut(A, B, Alpha, Exp);
	}

	double FMath_InterpEaseOutDouble(double A, double B, double Alpha, double Exp)
	{
		return FMath::InterpEaseOut(A, B, static_cast<float>(Alpha), static_cast<float>(Exp));
	}

	float FMath_InterpEaseInOutFloat(float A, float B, float Alpha, float Exp)
	{
		return FMath::InterpEaseInOut(A, B, Alpha, Exp);
	}

	double FMath_InterpEaseInOutDouble(double A, double B, double Alpha, double Exp)
	{
		return FMath::InterpEaseInOut(A, B, static_cast<float>(Alpha), static_cast<float>(Exp));
	}

	float FMath_InterpStepFloat(float A, float B, float Alpha, int32 Steps)
	{
		return FMath::InterpStep(A, B, Alpha, Steps);
	}

	double FMath_InterpStepDouble(double A, double B, double Alpha, int32 Steps)
	{
		return FMath::InterpStep(A, B, static_cast<float>(Alpha), Steps);
	}

	float FMath_SmoothStepFloat(float A, float B, float X)
	{
		return FMath::SmoothStep(A, B, X);
	}

	double FMath_SmoothStepDouble(double A, double B, double X)
	{
		return FMath::SmoothStep(A, B, X);
	}

#undef DEFINE_UNARY_FMATH_WRAPPER
#undef DEFINE_BINARY_FMATH_WRAPPER
}

void Bind_FMath(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

#define REGISTER_FMATH_FUNCTION(Declaration, Function) \
	Result = Engine->RegisterGlobalFunction(Declaration, asFUNCTION(Function), asCALL_CDECL); \
	check(Result >= 0)

	int Result = Engine->SetDefaultNamespace("FMath");
	check(Result >= 0);

	REGISTER_FMATH_FUNCTION("int Abs(int Value)", FMath_AbsInt);
	REGISTER_FMATH_FUNCTION("int64 Abs(int64 Value)", FMath_AbsInt64);
	REGISTER_FMATH_FUNCTION("float Abs(float Value)", FMath_AbsFloat);
	REGISTER_FMATH_FUNCTION("double Abs(double Value)", FMath_AbsDouble);
	REGISTER_FMATH_FUNCTION("float Sin(float Value)", FMath_SinFloat);
	REGISTER_FMATH_FUNCTION("double Sin(double Value)", FMath_SinDouble);
	REGISTER_FMATH_FUNCTION("float Cos(float Value)", FMath_CosFloat);
	REGISTER_FMATH_FUNCTION("double Cos(double Value)", FMath_CosDouble);
	REGISTER_FMATH_FUNCTION("float Tan(float Value)", FMath_TanFloat);
	REGISTER_FMATH_FUNCTION("double Tan(double Value)", FMath_TanDouble);
	REGISTER_FMATH_FUNCTION("float Asin(float Value)", FMath_AsinFloat);
	REGISTER_FMATH_FUNCTION("double Asin(double Value)", FMath_AsinDouble);
	REGISTER_FMATH_FUNCTION("float Acos(float Value)", FMath_AcosFloat);
	REGISTER_FMATH_FUNCTION("double Acos(double Value)", FMath_AcosDouble);
	REGISTER_FMATH_FUNCTION("float Atan(float Value)", FMath_AtanFloat);
	REGISTER_FMATH_FUNCTION("double Atan(double Value)", FMath_AtanDouble);
	REGISTER_FMATH_FUNCTION("float Atan2(float Y, float X)", FMath_Atan2Float);
	REGISTER_FMATH_FUNCTION("double Atan2(double Y, double X)", FMath_Atan2Double);
	REGISTER_FMATH_FUNCTION("float Sqrt(float Value)", FMath_SqrtFloat);
	REGISTER_FMATH_FUNCTION("double Sqrt(double Value)", FMath_SqrtDouble);
	REGISTER_FMATH_FUNCTION("float Pow(float Base, float Exponent)", FMath_PowFloat);
	REGISTER_FMATH_FUNCTION("double Pow(double Base, double Exponent)", FMath_PowDouble);
	REGISTER_FMATH_FUNCTION("float Fmod(float Dividend, float Divisor)", FMath_FmodFloat);
	REGISTER_FMATH_FUNCTION("double Fmod(double Dividend, double Divisor)", FMath_FmodDouble);
	REGISTER_FMATH_FUNCTION("float Exp(float Value)", FMath_ExpFloat);
	REGISTER_FMATH_FUNCTION("double Exp(double Value)", FMath_ExpDouble);
	REGISTER_FMATH_FUNCTION("float Loge(float Value)", FMath_LogeFloat);
	REGISTER_FMATH_FUNCTION("double Loge(double Value)", FMath_LogeDouble);
	REGISTER_FMATH_FUNCTION("float LogX(float Base, float Value)", FMath_LogXFloat);
	REGISTER_FMATH_FUNCTION("double LogX(double Base, double Value)", FMath_LogXDouble);
	REGISTER_FMATH_FUNCTION("int RandHelper(int A)", FMath_RandHelperInt);
	REGISTER_FMATH_FUNCTION("int64 RandHelper64(int64 A)", FMath_RandHelperInt64);
	REGISTER_FMATH_FUNCTION("int RandRange(int Min, int Max)", FMath_RandRangeInt);
	REGISTER_FMATH_FUNCTION("int64 RandRange(int64 Min, int64 Max)", FMath_RandRangeInt64);
	REGISTER_FMATH_FUNCTION("float RandRange(float Min, float Max)", FMath_RandRangeFloat);
	REGISTER_FMATH_FUNCTION("double RandRange(double Min, double Max)", FMath_RandRangeDouble);
	REGISTER_FMATH_FUNCTION("float FRandRange(float Min, float Max)", FMath_FRandRangeFloat);
	REGISTER_FMATH_FUNCTION("double FRandRange(double Min, double Max)", FMath_FRandRangeDouble);
	REGISTER_FMATH_FUNCTION("bool RandBool()", FMath_RandBoolWrapper);
	REGISTER_FMATH_FUNCTION("bool IsWithin(int Value, int MinValue, int MaxValue)", FMath_IsWithinInt);
	REGISTER_FMATH_FUNCTION("bool IsWithin(int64 Value, int64 MinValue, int64 MaxValue)", FMath_IsWithinInt64);
	REGISTER_FMATH_FUNCTION("bool IsWithin(float Value, float MinValue, float MaxValue)", FMath_IsWithinFloat);
	REGISTER_FMATH_FUNCTION("bool IsWithin(double Value, double MinValue, double MaxValue)", FMath_IsWithinDouble);
	REGISTER_FMATH_FUNCTION("bool IsWithinInclusive(int Value, int MinValue, int MaxValue)", FMath_IsWithinInclusiveInt);
	REGISTER_FMATH_FUNCTION("bool IsWithinInclusive(int64 Value, int64 MinValue, int64 MaxValue)", FMath_IsWithinInclusiveInt64);
	REGISTER_FMATH_FUNCTION("bool IsWithinInclusive(float Value, float MinValue, float MaxValue)", FMath_IsWithinInclusiveFloat);
	REGISTER_FMATH_FUNCTION("bool IsWithinInclusive(double Value, double MinValue, double MaxValue)", FMath_IsWithinInclusiveDouble);
	REGISTER_FMATH_FUNCTION("bool IsNearlyEqual(float A, float B, float Tolerance)", FMath_IsNearlyEqualFloat);
	REGISTER_FMATH_FUNCTION("bool IsNearlyEqual(double A, double B, double Tolerance)", FMath_IsNearlyEqualDouble);
	REGISTER_FMATH_FUNCTION("bool IsNearlyZero(float Value, float Tolerance)", FMath_IsNearlyZeroFloat);
	REGISTER_FMATH_FUNCTION("bool IsNearlyZero(double Value, double Tolerance)", FMath_IsNearlyZeroDouble);
	REGISTER_FMATH_FUNCTION("bool IsNearlyEqualByULP(float A, float B, int MaxUlps)", FMath_IsNearlyEqualByUlpFloat);
	REGISTER_FMATH_FUNCTION("bool IsNearlyEqualByULP(double A, double B, int MaxUlps)", FMath_IsNearlyEqualByUlpDouble);
	REGISTER_FMATH_FUNCTION("bool IsPowerOfTwo(int Value)", FMath_IsPowerOfTwoInt);
	REGISTER_FMATH_FUNCTION("bool IsPowerOfTwo(int64 Value)", FMath_IsPowerOfTwoInt64);
	REGISTER_FMATH_FUNCTION("int Max3(int A, int B, int C)", FMath_Max3Int);
	REGISTER_FMATH_FUNCTION("double Max3(double A, double B, double C)", FMath_Max3Double);
	REGISTER_FMATH_FUNCTION("int Min3(int A, int B, int C)", FMath_Min3Int);
	REGISTER_FMATH_FUNCTION("double Min3(double A, double B, double C)", FMath_Min3Double);
	REGISTER_FMATH_FUNCTION("int Max3Index(int A, int B, int C)", FMath_Max3IndexInt);
	REGISTER_FMATH_FUNCTION("int Max3Index(double A, double B, double C)", FMath_Max3IndexDouble);
	REGISTER_FMATH_FUNCTION("int Min3Index(int A, int B, int C)", FMath_Min3IndexInt);
	REGISTER_FMATH_FUNCTION("int Min3Index(double A, double B, double C)", FMath_Min3IndexDouble);
	REGISTER_FMATH_FUNCTION("int Square(int Value)", FMath_SquareIntWrapper);
	REGISTER_FMATH_FUNCTION("int64 Square(int64 Value)", FMath_SquareInt64Wrapper);
	REGISTER_FMATH_FUNCTION("float Square(float Value)", FMath_SquareFloatWrapper);
	REGISTER_FMATH_FUNCTION("double Square(double Value)", FMath_SquareDoubleWrapper);
	REGISTER_FMATH_FUNCTION("int Cube(int Value)", FMath_CubeIntWrapper);
	REGISTER_FMATH_FUNCTION("int64 Cube(int64 Value)", FMath_CubeInt64Wrapper);
	REGISTER_FMATH_FUNCTION("float Cube(float Value)", FMath_CubeFloatWrapper);
	REGISTER_FMATH_FUNCTION("double Cube(double Value)", FMath_CubeDoubleWrapper);
	REGISTER_FMATH_FUNCTION("int Clamp(int Value, int MinValue, int MaxValue)", FMath_ClampInt);
	REGISTER_FMATH_FUNCTION("int64 Clamp(int64 Value, int64 MinValue, int64 MaxValue)", FMath_ClampInt64);
	REGISTER_FMATH_FUNCTION("float Clamp(float Value, float MinValue, float MaxValue)", FMath_ClampFloat);
	REGISTER_FMATH_FUNCTION("double Clamp(double Value, double MinValue, double MaxValue)", FMath_ClampDouble);
	REGISTER_FMATH_FUNCTION("float Wrap(float Value, float MinValue, float MaxValue)", FMath_WrapFloat);
	REGISTER_FMATH_FUNCTION("double Wrap(double Value, double MinValue, double MaxValue)", FMath_WrapDouble);
	REGISTER_FMATH_FUNCTION("int WrapExclusive(int Value, int MinValue, int MaxValue)", FMath_WrapExclusiveInt);
	REGISTER_FMATH_FUNCTION("int64 WrapExclusive(int64 Value, int64 MinValue, int64 MaxValue)", FMath_WrapExclusiveInt64);
	REGISTER_FMATH_FUNCTION("int GridSnap(int Value, int Grid)", FMath_GridSnapInt);
	REGISTER_FMATH_FUNCTION("int64 GridSnap(int64 Value, int64 Grid)", FMath_GridSnapInt64);
	REGISTER_FMATH_FUNCTION("float GridSnap(float Value, float Grid)", FMath_GridSnapFloat);
	REGISTER_FMATH_FUNCTION("double GridSnap(double Value, double Grid)", FMath_GridSnapDouble);
	REGISTER_FMATH_FUNCTION("int DivideAndRoundUp(int Dividend, int Divisor)", FMath_DivideAndRoundUpInt);
	REGISTER_FMATH_FUNCTION("int64 DivideAndRoundUp(int64 Dividend, int64 Divisor)", FMath_DivideAndRoundUpInt64);
	REGISTER_FMATH_FUNCTION("int DivideAndRoundDown(int Dividend, int Divisor)", FMath_DivideAndRoundDownInt);
	REGISTER_FMATH_FUNCTION("int64 DivideAndRoundDown(int64 Dividend, int64 Divisor)", FMath_DivideAndRoundDownInt64);
	REGISTER_FMATH_FUNCTION("int DivideAndRoundNearest(int Dividend, int Divisor)", FMath_DivideAndRoundNearestInt);
	REGISTER_FMATH_FUNCTION("int64 DivideAndRoundNearest(int64 Dividend, int64 Divisor)", FMath_DivideAndRoundNearestInt64);
	REGISTER_FMATH_FUNCTION("float Log2(float Value)", FMath_Log2Float);
	REGISTER_FMATH_FUNCTION("double Log2(double Value)", FMath_Log2Double);
	REGISTER_FMATH_FUNCTION("void SinCos(float &out ScalarSin, float &out ScalarCos, float Value)", FMath_SinCosFloat);
	REGISTER_FMATH_FUNCTION("void SinCos(double &out ScalarSin, double &out ScalarCos, double Value)", FMath_SinCosDouble);
	REGISTER_FMATH_FUNCTION("float FastAsin(float Value)", FMath_FastAsinFloat);
	REGISTER_FMATH_FUNCTION("double FastAsin(double Value)", FMath_FastAsinDouble);
	REGISTER_FMATH_FUNCTION("float RadiansToDegrees(float Value)", FMath_RadiansToDegreesFloat);
	REGISTER_FMATH_FUNCTION("double RadiansToDegrees(double Value)", FMath_RadiansToDegreesDouble);
	REGISTER_FMATH_FUNCTION("float DegreesToRadians(float Value)", FMath_DegreesToRadiansFloat);
	REGISTER_FMATH_FUNCTION("double DegreesToRadians(double Value)", FMath_DegreesToRadiansDouble);
	REGISTER_FMATH_FUNCTION("float ClampAngle(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees)", FMath_ClampAngleFloat);
	REGISTER_FMATH_FUNCTION("double ClampAngle(double AngleDegrees, double MinAngleDegrees, double MaxAngleDegrees)", FMath_ClampAngleDouble);
	REGISTER_FMATH_FUNCTION("float FindDeltaAngleDegrees(float A1, float A2)", FMath_FindDeltaAngleDegreesFloat);
	REGISTER_FMATH_FUNCTION("double FindDeltaAngleDegrees(double A1, double A2)", FMath_FindDeltaAngleDegreesDouble);
	REGISTER_FMATH_FUNCTION("float FindDeltaAngleRadians(float A1, float A2)", FMath_FindDeltaAngleRadiansFloat);
	REGISTER_FMATH_FUNCTION("double FindDeltaAngleRadians(double A1, double A2)", FMath_FindDeltaAngleRadiansDouble);
	REGISTER_FMATH_FUNCTION("float UnwindRadians(float Value)", FMath_UnwindRadiansFloat);
	REGISTER_FMATH_FUNCTION("double UnwindRadians(double Value)", FMath_UnwindRadiansDouble);
	REGISTER_FMATH_FUNCTION("float UnwindDegrees(float Value)", FMath_UnwindDegreesFloat);
	REGISTER_FMATH_FUNCTION("double UnwindDegrees(double Value)", FMath_UnwindDegreesDouble);
	REGISTER_FMATH_FUNCTION("void WindRelativeAnglesDegrees(float InAngle0, float &inout InOutAngle1)", FMath_WindRelativeAnglesDegreesFloat);
	REGISTER_FMATH_FUNCTION("void WindRelativeAnglesDegrees(double InAngle0, double &inout InOutAngle1)", FMath_WindRelativeAnglesDegreesDouble);
	REGISTER_FMATH_FUNCTION("float FixedTurn(float InCurrent, float InDesired, float InDeltaRate)", FMath_FixedTurn);
	REGISTER_FMATH_FUNCTION("void CartesianToPolar(float X, float Y, float &out OutRadius, float &out OutAngle)", FMath_CartesianToPolarFloat);
	REGISTER_FMATH_FUNCTION("void CartesianToPolar(double X, double Y, double &out OutRadius, double &out OutAngle)", FMath_CartesianToPolarDouble);
	REGISTER_FMATH_FUNCTION("void PolarToCartesian(float Radius, float Angle, float &out OutX, float &out OutY)", FMath_PolarToCartesianFloat);
	REGISTER_FMATH_FUNCTION("void PolarToCartesian(double Radius, double Angle, double &out OutX, double &out OutY)", FMath_PolarToCartesianDouble);
	REGISTER_FMATH_FUNCTION("float Lerp(float A, float B, float Alpha)", FMath_LerpFloat);
	REGISTER_FMATH_FUNCTION("double Lerp(double A, double B, double Alpha)", FMath_LerpDouble);
	REGISTER_FMATH_FUNCTION("float LerpStable(float A, float B, float Alpha)", FMath_LerpStableFloat);
	REGISTER_FMATH_FUNCTION("double LerpStable(double A, double B, double Alpha)", FMath_LerpStableDouble);
	REGISTER_FMATH_FUNCTION("float BiLerp(float P00, float P10, float P01, float P11, float FracX, float FracY)", FMath_BiLerpFloat);
	REGISTER_FMATH_FUNCTION("double BiLerp(double P00, double P10, double P01, double P11, double FracX, double FracY)", FMath_BiLerpDouble);
	REGISTER_FMATH_FUNCTION("float CubicInterp(float P0, float T0, float P1, float T1, float Alpha)", FMath_CubicInterpFloat);
	REGISTER_FMATH_FUNCTION("double CubicInterp(double P0, double T0, double P1, double T1, double Alpha)", FMath_CubicInterpDouble);
	REGISTER_FMATH_FUNCTION("float CubicInterpDerivative(float P0, float T0, float P1, float T1, float Alpha)", FMath_CubicInterpDerivativeFloat);
	REGISTER_FMATH_FUNCTION("double CubicInterpDerivative(double P0, double T0, double P1, double T1, double Alpha)", FMath_CubicInterpDerivativeDouble);
	REGISTER_FMATH_FUNCTION("float CubicInterpSecondDerivative(float P0, float T0, float P1, float T1, float Alpha)", FMath_CubicInterpSecondDerivativeFloat);
	REGISTER_FMATH_FUNCTION("double CubicInterpSecondDerivative(double P0, double T0, double P1, double T1, double Alpha)", FMath_CubicInterpSecondDerivativeDouble);
	REGISTER_FMATH_FUNCTION("float InterpEaseIn(float A, float B, float Alpha, float Exp)", FMath_InterpEaseInFloat);
	REGISTER_FMATH_FUNCTION("double InterpEaseIn(double A, double B, double Alpha, double Exp)", FMath_InterpEaseInDouble);
	REGISTER_FMATH_FUNCTION("float InterpEaseOut(float A, float B, float Alpha, float Exp)", FMath_InterpEaseOutFloat);
	REGISTER_FMATH_FUNCTION("double InterpEaseOut(double A, double B, double Alpha, double Exp)", FMath_InterpEaseOutDouble);
	REGISTER_FMATH_FUNCTION("float InterpEaseInOut(float A, float B, float Alpha, float Exp)", FMath_InterpEaseInOutFloat);
	REGISTER_FMATH_FUNCTION("double InterpEaseInOut(double A, double B, double Alpha, double Exp)", FMath_InterpEaseInOutDouble);
	REGISTER_FMATH_FUNCTION("float InterpStep(float A, float B, float Alpha, int Steps)", FMath_InterpStepFloat);
	REGISTER_FMATH_FUNCTION("double InterpStep(double A, double B, double Alpha, int Steps)", FMath_InterpStepDouble);
	REGISTER_FMATH_FUNCTION("float SmoothStep(float A, float B, float X)", FMath_SmoothStepFloat);
	REGISTER_FMATH_FUNCTION("double SmoothStep(double A, double B, double X)", FMath_SmoothStepDouble);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);

#undef REGISTER_FMATH_FUNCTION
}
