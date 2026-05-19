// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/Object.h"
#include "UObject/UObjectGlobals.h"
#include "angelscript.h"

namespace
{
	UObject* GetFixtureOuterObject()
	{
		return GetTransientPackage();
	}

	UObject* GetFixtureObject()
	{
		static UObject* Object = nullptr;
		if (Object == nullptr)
		{
			Object = NewObject<UObject>(GetTransientPackage(), NAME_None, RF_Transient);
			Object->AddToRoot();
		}

		return Object;
	}

	UClass* GetFixtureClass()
	{
		return UObject::StaticClass();
	}

	UClass* Get_UClass_UClass()
	{
		return UClass::StaticClass();
	}

	FString GetFixtureAbsoluteFilename()
	{
		return UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content\\OtterAngleScript\\Fixture.txt");
	}

	FString GetFixtureRelativeFilename()
	{
		return FString(TEXT("Content\\OtterAngleScript\\Fixture.txt"));
	}

	FString GetFixtureUnnormalizedFilename()
	{
		return FString(TEXT("C:\\Temp\\OtterAngleScript\\..\\Fixture.txt"));
	}

	FName MakeFixtureDuplicateName()
	{
		return MakeUniqueObjectName(GetTransientPackage(), UObject::StaticClass(), TEXT("OtterAngleScriptDuplicate"));
	}

	bool RegisterKismetFixtureBindings(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction("UObject@ GetFixtureOuterObject()", asFUNCTION(GetFixtureOuterObject), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("UObject@ GetFixtureObject()", asFUNCTION(GetFixtureObject), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("UClass@ GetFixtureClass()", asFUNCTION(GetFixtureClass), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("UClass@ Get_UClass_UClass()", asFUNCTION(Get_UClass_UClass), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("FString GetFixtureAbsoluteFilename()", asFUNCTION(GetFixtureAbsoluteFilename), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("FString GetFixtureRelativeFilename()", asFUNCTION(GetFixtureRelativeFilename), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("FString GetFixtureUnnormalizedFilename()", asFUNCTION(GetFixtureUnnormalizedFilename), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction("FName MakeFixtureDuplicateName()", asFUNCTION(MakeFixtureDuplicateName), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptUObjectAndKismetSystemLibraryTests,
	"OtterAngleScript.UObjectAndKismetSystemLibrary",
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
		ASSERT_THAT(IsTrue(RegisterKismetFixtureBindings(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptUObjectAndKismetSystemLibraryTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(UObjectTypeRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UObject")));
	}

	TEST_METHOD(UClassTypeRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UClass")));
	}

	TEST_METHOD(UObjectGetClassBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetClassBinding()
{
    if (GetFixtureObject().GetClass() !is GetFixtureClass())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetClassBinding", Script, "int RunUObjectGetClassBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectGetOuterBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetOuterBinding()
{
    if (GetFixtureObject().GetOuter() !is GetFixtureOuterObject())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetOuterBinding", Script, "int RunUObjectGetOuterBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectGetFNameBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetFNameBinding()
{
    if (GetFixtureObject().GetFName().ToString() != GetFixtureObject().GetName())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetFNameBinding", Script, "int RunUObjectGetFNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectGetNameBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetNameBinding()
{
    if (GetFixtureObject().GetName().IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetNameBinding", Script, "int RunUObjectGetNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectGetPathNameBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetPathNameBinding()
{
    if (!GetFixtureObject().GetPathName().Contains(GetFixtureObject().GetName()))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetPathNameBinding", Script, "int RunUObjectGetPathNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectGetFullNameBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetFullNameBinding()
{
    if (!GetFixtureObject().GetFullName().Contains(GetFixtureObject().GetName()))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetFullNameBinding", Script, "int RunUObjectGetFullNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectIsABinding)
	{
		static const char Script[] = R"(
int RunUObjectIsABinding()
{
    if (!GetFixtureObject().IsA(GetFixtureClass()))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectIsABinding", Script, "int RunUObjectIsABinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectGetUniqueIDBinding)
	{
		static const char Script[] = R"(
int RunUObjectGetUniqueIDBinding()
{
    if (GetFixtureObject().GetUniqueID() <= 0)
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectGetUniqueIDBinding", Script, "int RunUObjectGetUniqueIDBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectIsValidLowLevelBinding)
	{
		static const char Script[] = R"(
int RunUObjectIsValidLowLevelBinding()
{
    if (!GetFixtureObject().IsValidLowLevel())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectIsValidLowLevelBinding", Script, "int RunUObjectIsValidLowLevelBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectIsValidLowLevelFastBinding)
	{
		static const char Script[] = R"(
int RunUObjectIsValidLowLevelFastBinding()
{
    if (!GetFixtureObject().IsValidLowLevelFast())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectIsValidLowLevelFastBinding", Script, "int RunUObjectIsValidLowLevelFastBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UObjectIsTemplateBinding)
	{
		static const char Script[] = R"(
int RunUObjectIsTemplateBinding()
{
    if (GetFixtureObject().IsTemplate())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectIsTemplateBinding", Script, "int RunUObjectIsTemplateBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetFNameBinding)
	{
		static const char Script[] = R"(
int RunUClassGetFNameBinding()
{
    if (GetFixtureClass().GetFName().ToString() != GetFixtureClass().GetName())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetFNameBinding", Script, "int RunUClassGetFNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetNameBinding)
	{
		static const char Script[] = R"(
int RunUClassGetNameBinding()
{
    if (GetFixtureClass().GetName().IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetNameBinding", Script, "int RunUClassGetNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetPathNameBinding)
	{
		static const char Script[] = R"(
int RunUClassGetPathNameBinding()
{
    if (!GetFixtureClass().GetPathName().Contains(GetFixtureClass().GetName()))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetPathNameBinding", Script, "int RunUClassGetPathNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetFullNameBinding)
	{
		static const char Script[] = R"(
int RunUClassGetFullNameBinding()
{
    if (!GetFixtureClass().GetFullName().Contains(GetFixtureClass().GetName()))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetFullNameBinding", Script, "int RunUClassGetFullNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetOuterBinding)
	{
		static const char Script[] = R"(
int RunUClassGetOuterBinding()
{
    if (GetFixtureClass().GetOuter() is null)
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetOuterBinding", Script, "int RunUClassGetOuterBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetSuperClassBinding)
	{
		static const char Script[] = R"(
int RunUClassGetSuperClassBinding()
{
    if (Get_UClass_UClass().GetSuperClass() is null)
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetSuperClassBinding", Script, "int RunUClassGetSuperClassBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassIsChildOfBinding)
	{
		static const char Script[] = R"(
int RunUClassIsChildOfBinding()
{
    if (!Get_UClass_UClass().IsChildOf(GetFixtureClass()))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassIsChildOfBinding", Script, "int RunUClassIsChildOfBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetDefaultObjectBinding)
	{
		static const char Script[] = R"(
int RunUClassGetDefaultObjectBinding()
{
    if (GetFixtureClass().GetDefaultObject() is null)
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetDefaultObjectBinding", Script, "int RunUClassGetDefaultObjectBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassGetPropertiesSizeBinding)
	{
		static const char Script[] = R"(
int RunUClassGetPropertiesSizeBinding()
{
    if (GetFixtureClass().GetPropertiesSize() < 0)
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassGetPropertiesSizeBinding", Script, "int RunUClassGetPropertiesSizeBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UClassIsNativeBinding)
	{
		static const char Script[] = R"(
int RunUClassIsNativeBinding()
{
    if (!GetFixtureClass().IsNative())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("UClassIsNativeBinding", Script, "int RunUClassIsNativeBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValidBinding)
	{
		static const char Script[] = R"(
int RunIsValidBinding()
{
    if (!UKismetSystemLibrary::IsValid(GetFixtureObject()))
    {
        return -1;
    }

    if (UKismetSystemLibrary::IsValid(null))
    {
        return -2;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsValidBinding", Script, "int RunIsValidBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValidClassBinding)
	{
		static const char Script[] = R"(
int RunIsValidClassBinding()
{
    if (!UKismetSystemLibrary::IsValidClass(GetFixtureClass()))
    {
        return -1;
    }

    if (UKismetSystemLibrary::IsValidClass(null))
    {
        return -2;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("IsValidClassBinding", Script, "int RunIsValidClassBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetObjectNameBinding)
	{
		static const char Script[] = R"(
int RunGetObjectNameBinding()
{
	    FString Name = UKismetSystemLibrary::GetObjectName(GetFixtureObject());
	    if (Name.IsEmpty())
	    {
	        return -1;
	    }

	    if (!UKismetSystemLibrary::GetPathName(GetFixtureObject()).Contains(Name))
	    {
	        return -2;
	    }

	    return 0;
	}
	)";

		asIScriptFunction* Function = BuildFunction("GetObjectNameBinding", Script, "int RunGetObjectNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetPathNameBinding)
	{
		static const char Script[] = R"(
int RunGetPathNameBinding()
{
    FString Path = UKismetSystemLibrary::GetPathName(GetFixtureObject());
    if (Path.IsEmpty() || !Path.Contains("Transient"))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetPathNameBinding", Script, "int RunGetPathNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetDisplayNameBinding)
	{
		static const char Script[] = R"(
int RunGetDisplayNameBinding()
{
    FString Name = UKismetSystemLibrary::GetDisplayName(GetFixtureObject());
    if (Name.IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetDisplayNameBinding", Script, "int RunGetDisplayNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetClassDisplayNameBinding)
	{
		static const char Script[] = R"(
int RunGetClassDisplayNameBinding()
{
    FString Name = UKismetSystemLibrary::GetClassDisplayName(GetFixtureClass());
    if (Name.IsEmpty() || !Name.Contains("Object"))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetClassDisplayNameBinding", Script, "int RunGetClassDisplayNameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetOuterObjectBinding)
	{
		static const char Script[] = R"(
int RunGetOuterObjectBinding()
{
    if (UKismetSystemLibrary::GetOuterObject(GetFixtureObject()) !is GetFixtureOuterObject())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetOuterObjectBinding", Script, "int RunGetOuterObjectBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DuplicateObjectBinding)
	{
		static const char Script[] = R"(
int RunDuplicateObjectBinding()
{
    FName DuplicateName = MakeFixtureDuplicateName();
    UObject@ Duplicate = UKismetSystemLibrary::DuplicateObject(GetFixtureObject(), GetFixtureOuterObject(), DuplicateName);
    if (Duplicate is null)
    {
        return -1;
    }

    if (Duplicate is GetFixtureObject())
    {
        return -2;
    }

    if (UKismetSystemLibrary::GetObjectName(Duplicate) != DuplicateName.ToString())
    {
        return -3;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("DuplicateObjectBinding", Script, "int RunDuplicateObjectBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetEngineVersionBinding)
	{
		static const char Script[] = R"(
int RunGetEngineVersionBinding()
{
    if (UKismetSystemLibrary::GetEngineVersion().IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetEngineVersionBinding", Script, "int RunGetEngineVersionBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetProjectDirectoryBinding)
	{
		static const char Script[] = R"(
int RunGetProjectDirectoryBinding()
{
    FString Value = UKismetSystemLibrary::GetProjectDirectory();
    if (Value.IsEmpty() || !Value.EndsWith("\\"))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetProjectDirectoryBinding", Script, "int RunGetProjectDirectoryBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConvertToRelativePathBinding)
	{
		static const char Script[] = R"(
int RunConvertToRelativePathBinding()
{
    FString Filename = GetFixtureAbsoluteFilename();
    FString Value = UKismetSystemLibrary::ConvertToRelativePath(Filename);
    if (Value.IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("ConvertToRelativePathBinding", Script, "int RunConvertToRelativePathBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConvertToAbsolutePathBinding)
	{
		static const char Script[] = R"(
int RunConvertToAbsolutePathBinding()
{
    FString Filename = GetFixtureRelativeFilename();
    FString Value = UKismetSystemLibrary::ConvertToAbsolutePath(Filename);
    if (Value.IsEmpty() || !Value.Contains(":"))
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("ConvertToAbsolutePathBinding", Script, "int RunConvertToAbsolutePathBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(NormalizeFilenameBinding)
	{
		static const char Script[] = R"(
int RunNormalizeFilenameBinding()
{
    FString Filename = GetFixtureUnnormalizedFilename();
    FString Value = UKismetSystemLibrary::NormalizeFilename(Filename);
    if (Value.Contains("..") || Value.IsEmpty())
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NormalizeFilenameBinding", Script, "int RunNormalizeFilenameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetFrameCountBinding)
	{
		static const char Script[] = R"(
int RunGetFrameCountBinding()
{
    if (UKismetSystemLibrary::GetFrameCount() <= 0)
    {
        return -1;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("GetFrameCountBinding", Script, "int RunGetFrameCountBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
