// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/DateTime.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FDateTime_DefaultConstruct(FDateTime* Memory)
	{
		new (Memory) FDateTime();
	}

	static void FDateTime_CopyConstruct(const FDateTime& Other, FDateTime* Memory)
	{
		new (Memory) FDateTime(Other);
	}

	static void FDateTime_ConstructTicks(int64 InTicks, FDateTime* Memory)
	{
		new (Memory) FDateTime(InTicks);
	}

	static void FDateTime_ConstructYMDHMS(int32 Year, int32 Month, int32 Day,
		int32 Hour, int32 Minute, int32 Second, int32 Millisecond, FDateTime* Memory)
	{
		new (Memory) FDateTime(Year, Month, Day, Hour, Minute, Second, Millisecond);
	}

	static void FDateTime_Destruct(FDateTime* Memory)
	{
		Memory->~FDateTime();
	}

	// --- Operators ---

	static FDateTime& FDateTime_Assign(FDateTime& Value, const FDateTime& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FDateTime_OpEquals(const FDateTime& Value, const FDateTime& Other)
	{
		return Value == Other;
	}

	static int FDateTime_OpCmp(const FDateTime& Value, const FDateTime& Other)
	{
		if (Value < Other) return -1;
		if (Other < Value) return 1;
		return 0;
	}

	// --- Accessors ---

	static void FDateTime_GetDateComponents(const FDateTime& Value,
		int32& OutYear, int32& OutMonth, int32& OutDay)
	{
		Value.GetDate(OutYear, OutMonth, OutDay);
	}

	static int32 FDateTime_GetDay(const FDateTime& Value)
	{
		return Value.GetDay();
	}

	static EDayOfWeek FDateTime_GetDayOfWeek(const FDateTime& Value)
	{
		return Value.GetDayOfWeek();
	}

	static int32 FDateTime_GetDayOfYear(const FDateTime& Value)
	{
		return Value.GetDayOfYear();
	}

	static int32 FDateTime_GetHour(const FDateTime& Value)
	{
		return Value.GetHour();
	}

	static int32 FDateTime_GetHour12(const FDateTime& Value)
	{
		return Value.GetHour12();
	}

	static double FDateTime_GetJulianDay(const FDateTime& Value)
	{
		return Value.GetJulianDay();
	}

	static double FDateTime_GetModifiedJulianDay(const FDateTime& Value)
	{
		return Value.GetModifiedJulianDay();
	}

	static int32 FDateTime_GetMillisecond(const FDateTime& Value)
	{
		return Value.GetMillisecond();
	}

	static int32 FDateTime_GetMinute(const FDateTime& Value)
	{
		return Value.GetMinute();
	}

	static int32 FDateTime_GetMonth(const FDateTime& Value)
	{
		return Value.GetMonth();
	}

	static EMonthOfYear FDateTime_GetMonthOfYear(const FDateTime& Value)
	{
		return Value.GetMonthOfYear();
	}

	static int32 FDateTime_GetSecond(const FDateTime& Value)
	{
		return Value.GetSecond();
	}

	static int64 FDateTime_GetTicks(const FDateTime& Value)
	{
		return Value.GetTicks();
	}

	static int32 FDateTime_GetYear(const FDateTime& Value)
	{
		return Value.GetYear();
	}

	static bool FDateTime_IsAfternoon(const FDateTime& Value)
	{
		return Value.IsAfternoon();
	}

	static bool FDateTime_IsMorning(const FDateTime& Value)
	{
		return Value.IsMorning();
	}

	// --- Conversion ---

	static FString FDateTime_ToHttpDate(const FDateTime& Value)
	{
		return Value.ToHttpDate();
	}

	static FString FDateTime_ToIso8601(const FDateTime& Value)
	{
		return Value.ToIso8601();
	}

	static FString FDateTime_ToStringDefault(const FDateTime& Value)
	{
		return Value.ToString();
	}

	static FString FDateTime_ToStringFormat(const FDateTime& Value, const FString& Format)
	{
		return Value.ToString(*Format);
	}

	static FString FDateTime_ToFormattedString(const FDateTime& Value, const FString& Format)
	{
		return Value.ToFormattedString(*Format);
	}

	static int64 FDateTime_ToUnixTimestamp(const FDateTime& Value)
	{
		return Value.ToUnixTimestamp();
	}

	static double FDateTime_ToUnixTimestampDecimal(const FDateTime& Value)
	{
		return Value.ToUnixTimestampDecimal();
	}

	// --- Static methods ---

	static int32 FDateTime_DaysInMonth(int32 Year, int32 Month)
	{
		return FDateTime::DaysInMonth(Year, Month);
	}

	static int32 FDateTime_DaysInYear(int32 Year)
	{
		return FDateTime::DaysInYear(Year);
	}

	static FDateTime FDateTime_FromJulianDay(double JulianDay)
	{
		return FDateTime::FromJulianDay(JulianDay);
	}

	static FDateTime FDateTime_FromUnixTimestamp(int64 UnixTime)
	{
		return FDateTime::FromUnixTimestamp(UnixTime);
	}

	static FDateTime FDateTime_FromUnixTimestampDecimal(double UnixTime)
	{
		return FDateTime::FromUnixTimestampDecimal(UnixTime);
	}

	static bool FDateTime_IsLeapYear(int32 Year)
	{
		return FDateTime::IsLeapYear(Year);
	}

	static FDateTime FDateTime_MaxValue()
	{
		return FDateTime::MaxValue();
	}

	static FDateTime FDateTime_MinValue()
	{
		return FDateTime::MinValue();
	}

	static FDateTime FDateTime_Now()
	{
		return FDateTime::Now();
	}

	static bool FDateTime_Parse(const FString& DateTimeString, FDateTime& OutDateTime)
	{
		return FDateTime::Parse(DateTimeString, OutDateTime);
	}

	static bool FDateTime_ParseHttpDate(const FString& HttpDate, FDateTime& OutDateTime)
	{
		return FDateTime::ParseHttpDate(HttpDate, OutDateTime);
	}

	static bool FDateTime_ParseIso8601(const FString& DateTimeString, FDateTime& OutDateTime)
	{
		return FDateTime::ParseIso8601(*DateTimeString, OutDateTime);
	}

	static FDateTime FDateTime_Today()
	{
		return FDateTime::Today();
	}

	static FDateTime FDateTime_UtcNow()
	{
		return FDateTime::UtcNow();
	}

	static bool FDateTime_Validate(int32 Year, int32 Month, int32 Day,
		int32 Hour, int32 Minute, int32 Second, int32 Millisecond)
	{
		return FDateTime::Validate(Year, Month, Day, Hour, Minute, Second, Millisecond);
	}
}

void Bind_FDateTime(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Register enums
	Result = Engine->RegisterEnum("EDayOfWeek");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Monday", static_cast<int>(EDayOfWeek::Monday));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Tuesday", static_cast<int>(EDayOfWeek::Tuesday));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Wednesday", static_cast<int>(EDayOfWeek::Wednesday));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Thursday", static_cast<int>(EDayOfWeek::Thursday));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Friday", static_cast<int>(EDayOfWeek::Friday));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Saturday", static_cast<int>(EDayOfWeek::Saturday));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EDayOfWeek", "Sunday", static_cast<int>(EDayOfWeek::Sunday));
	check(Result >= 0);

	Result = Engine->RegisterEnum("EMonthOfYear");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "January", static_cast<int>(EMonthOfYear::January));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "February", static_cast<int>(EMonthOfYear::February));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "March", static_cast<int>(EMonthOfYear::March));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "April", static_cast<int>(EMonthOfYear::April));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "May", static_cast<int>(EMonthOfYear::May));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "June", static_cast<int>(EMonthOfYear::June));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "July", static_cast<int>(EMonthOfYear::July));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "August", static_cast<int>(EMonthOfYear::August));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "September", static_cast<int>(EMonthOfYear::September));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "October", static_cast<int>(EMonthOfYear::October));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "November", static_cast<int>(EMonthOfYear::November));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EMonthOfYear", "December", static_cast<int>(EMonthOfYear::December));
	check(Result >= 0);

	// Register the type
	Result = Engine->RegisterObjectType("FDateTime", sizeof(FDateTime),
		asOBJ_VALUE | asGetTypeTraits<FDateTime>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FDateTime, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FDateTime_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FDateTime, asBEHAVE_CONSTRUCT, "void f(const FDateTime &in Other)", asFUNCTION(FDateTime_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FDateTime, asBEHAVE_CONSTRUCT, "void f(int64 InTicks)", asFUNCTION(FDateTime_ConstructTicks), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FDateTime, asBEHAVE_CONSTRUCT, "void f(int Year, int Month, int Day, int Hour = 0, int Minute = 0, int Second = 0, int Millisecond = 0)", asFUNCTION(FDateTime_ConstructYMDHMS), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FDateTime, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FDateTime_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FDateTime, "FDateTime &opAssign(const FDateTime &in Other)", asFUNCTION(FDateTime_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "bool opEquals(const FDateTime &in Other) const", asFUNCTION(FDateTime_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int opCmp(const FDateTime &in Other) const", asFUNCTION(FDateTime_OpCmp), asCALL_CDECL_OBJFIRST);

	// Accessors
	REGISTER_METHOD(FDateTime, "void GetDate(int &out OutYear, int &out OutMonth, int &out OutDay) const", asFUNCTION(FDateTime_GetDateComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetDay() const", asFUNCTION(FDateTime_GetDay), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "EDayOfWeek GetDayOfWeek() const", asFUNCTION(FDateTime_GetDayOfWeek), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetDayOfYear() const", asFUNCTION(FDateTime_GetDayOfYear), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetHour() const", asFUNCTION(FDateTime_GetHour), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetHour12() const", asFUNCTION(FDateTime_GetHour12), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "double GetJulianDay() const", asFUNCTION(FDateTime_GetJulianDay), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "double GetModifiedJulianDay() const", asFUNCTION(FDateTime_GetModifiedJulianDay), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetMillisecond() const", asFUNCTION(FDateTime_GetMillisecond), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetMinute() const", asFUNCTION(FDateTime_GetMinute), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetMonth() const", asFUNCTION(FDateTime_GetMonth), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "EMonthOfYear GetMonthOfYear() const", asFUNCTION(FDateTime_GetMonthOfYear), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetSecond() const", asFUNCTION(FDateTime_GetSecond), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int64 GetTicks() const", asFUNCTION(FDateTime_GetTicks), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int GetYear() const", asFUNCTION(FDateTime_GetYear), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "bool IsAfternoon() const", asFUNCTION(FDateTime_IsAfternoon), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "bool IsMorning() const", asFUNCTION(FDateTime_IsMorning), asCALL_CDECL_OBJFIRST);

	// Conversion
	REGISTER_METHOD(FDateTime, "FString ToHttpDate() const", asFUNCTION(FDateTime_ToHttpDate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "FString ToIso8601() const", asFUNCTION(FDateTime_ToIso8601), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "FString ToString() const", asFUNCTION(FDateTime_ToStringDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "FString ToString(const FString &in Format) const", asFUNCTION(FDateTime_ToStringFormat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "FString ToFormattedString(const FString &in Format) const", asFUNCTION(FDateTime_ToFormattedString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "int64 ToUnixTimestamp() const", asFUNCTION(FDateTime_ToUnixTimestamp), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FDateTime, "double ToUnixTimestampDecimal() const", asFUNCTION(FDateTime_ToUnixTimestampDecimal), asCALL_CDECL_OBJFIRST);

	// Register static methods under the FDateTime namespace
	Result = Engine->SetDefaultNamespace("FDateTime");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("int DaysInMonth(int Year, int Month)", asFUNCTION(FDateTime_DaysInMonth), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("int DaysInYear(int Year)", asFUNCTION(FDateTime_DaysInYear), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime FromJulianDay(double JulianDay)", asFUNCTION(FDateTime_FromJulianDay), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime FromUnixTimestamp(int64 UnixTime)", asFUNCTION(FDateTime_FromUnixTimestamp), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime FromUnixTimestampDecimal(double UnixTime)", asFUNCTION(FDateTime_FromUnixTimestampDecimal), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool IsLeapYear(int Year)", asFUNCTION(FDateTime_IsLeapYear), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime MaxValue()", asFUNCTION(FDateTime_MaxValue), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime MinValue()", asFUNCTION(FDateTime_MinValue), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime Now()", asFUNCTION(FDateTime_Now), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Parse(const FString &in DateTimeString, FDateTime &out OutDateTime)", asFUNCTION(FDateTime_Parse), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool ParseHttpDate(const FString &in HttpDate, FDateTime &out OutDateTime)", asFUNCTION(FDateTime_ParseHttpDate), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool ParseIso8601(const FString &in DateTimeString, FDateTime &out OutDateTime)", asFUNCTION(FDateTime_ParseIso8601), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime Today()", asFUNCTION(FDateTime_Today), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FDateTime UtcNow()", asFUNCTION(FDateTime_UtcNow), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Validate(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond)", asFUNCTION(FDateTime_Validate), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
