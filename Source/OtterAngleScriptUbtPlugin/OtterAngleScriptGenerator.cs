// Copyright Epic Games, Inc. All Rights Reserved.

using EpicGames.Core;
using EpicGames.UHT.Exporters.CodeGen;
using EpicGames.UHT.Tables;
using EpicGames.UHT.Types;
using EpicGames.UHT.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace OtterAngleScriptUbtPlugin
{
    /// <summary>
    /// UHT exporter that generates AngelScript binding registration code for
    /// all BlueprintCallable / BlueprintPure UFunctions and BlueprintVisible UProperties
    /// found in Engine and Game runtime modules.
    ///
    /// Output (per class):
    ///   <PluginIntermediate>/Bind_<ClassName>.oas.gen.h   – registration function declaration
    ///   <PluginIntermediate>/Bind_<ClassName>.oas.gen.cpp – wrappers + registration function definition
    ///
    /// Output (master):
    ///   <PluginIntermediate>/OtterAngelScriptBindings.gen.h   – includes all per-class headers
    ///   <PluginIntermediate>/OtterAngelScriptBindings.gen.cpp – OAS_RegisterGeneratedTypes + Bind_Generated
    ///
    /// Include GeneratedAngelScriptBindings.h from OtterAngleScript.cpp and call Bind_Generated(Engine).
    /// </summary>
    [UnrealHeaderTool]
    class OtterAngleScriptGeneratorEntry
    {
        [UhtExporter(
            Name = "OtterAngleScript",
            Description = "AngelScript Binding Generator for OtterAngleScript",
            Options = UhtExporterOptions.Default,
            ModuleName = "OtterAngleScript",
			CppFilters = new string[] { "Bind*.oas.gen.cpp", "OtterAngelScriptBindings.gen.cpp"},
			HeaderFilters = new string[] { "Bind*.oas.gen.h" , "OtterAngelScriptBindings.gen.h"})]
        private static void GenerateAngelScriptBindings(IUhtExportFactory factory)
        {
            factory.Session.LogInfo("Starting OtterAngleScript binding generation...");
            new OtterAngleScriptGenerator(factory).Generate();
        }
    }

    class OtterAngleScriptGenerator
    {
        private readonly IUhtExportFactory _factory;

        /// <summary>
        /// Types already manually registered in the C++ plugin.  These are skipped to
        /// avoid double-registration errors.
        /// </summary>
        private static readonly HashSet<string> ManuallyBoundTypes = new(StringComparer.Ordinal)
        {
            "UObject", "UClass", "FLinearColor", "FColor",
            "FString", "FName", "FText",
            "FVector", "FVector2D", "FRotator", "FQuat",
            "FTransform", "FPlane", "FBox",
            "FHitResult", "FTimerHandle", "FLatentActionInfo",
            "FActorInstanceHandle", "FMatrix"
        };
        private static readonly HashSet<string> SkipStructs = new(StringComparer.Ordinal)
        {
            "FAudioBasedVibrationData", "FSoundClassProperties", "FSpecularProfileStruct", "FActorDesc"
        };
        private static readonly HashSet<string> BuildModules = new(StringComparer.Ordinal)
        {
            "NetCore", "CoreUObject", "Engine"
        };

        Dictionary<string, Dictionary<string, string>> CustomCppSignature = new Dictionary<string, Dictionary<string, string>>(StringComparer.Ordinal)
        {
            { "UGameplayStatics" , new Dictionary<string, string>(StringComparer.Ordinal)
            {
                { "DeprojectScreenToWorld", "const APlayerController* , const FVector2D& , FVector& , FVector&" },
                { "DeprojectSceneCaptureToWorld", "const ASceneCapture2D* , const FVector2D& , FVector& , FVector&" },
                { "ProjectWorldToScreen", "const APlayerController* , const FVector& , FVector2D& , bool" }
            }}
        };

        public OtterAngleScriptGenerator(IUhtExportFactory factory)
        {
            _factory = factory;
        }

        // -------------------------------------------------------------------------
        // Entry point
        // -------------------------------------------------------------------------

        public void Generate()
        {
#if TRUE
            var packages = _factory.Session.Modules
                .Where(m => m.Module.ModuleType == UHTModuleType.EngineRuntime
                         || m.Module.ModuleType == UHTModuleType.GameRuntime)
                .Where(m => BuildModules.Contains(m.ShortName))
                .Select(m => m.ScriptPackage);
#else
            var packages = _factory.Session.Packages
                .Where(p => p.Module.ModuleType == UHTModuleType.EngineRuntime
                         || p.Module.ModuleType == UHTModuleType.GameRuntime);
#endif
            var packageList = packages.ToList();

            // Collect all UClasses excluding manually-bound types and interfaces.
            var allClasses = packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtClass>()
                .Where(c => !c.ClassFlags.HasAnyFlags(EClassFlags.Interface))
                .Where(c => !ManuallyBoundTypes.Contains(c.SourceName))
                .Where(c => !c.HeaderFile.FilePath.Contains("Tests"))
                .Where(c => !c.HeaderFile.FilePath.Contains("Internal"))
                .Where(c => !c.HeaderFile.FilePath.Contains("Private"))
                .Where(c => !c.Deprecated)
                .ToList();

            // Collect all BlueprintType USTRUCTs excluding manually-bound types.
            var allStructs = packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtScriptStruct>()
                .Where(s => s.MetaData.ContainsKey("BlueprintType"))
                .Where(s => !ManuallyBoundTypes.Contains(s.SourceName))
                .Where(s => !s.HeaderFile.FilePath.Contains("Tests"))
                .Where(s => !s.HeaderFile.FilePath.Contains("Internal"))
                .Where(s => !s.HeaderFile.FilePath.Contains("Private"))
                .Where(s => !s.HeaderFile.FilePath.Contains("NoExportTypes.h"))
                .Where(s => !s.SourceName.Contains("Deprecated"))
                .Where(s => !SkipStructs.Contains(s.SourceName))
                .Where(s => !s.Deprecated)
                .ToList();

            // Collect all BlueprintType UENUMs excluding manually-bound types.
            var allEnums = packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtEnum>()
                .Where(e => e.MetaData.ContainsKey("BlueprintType"))
                .Where(e => !ManuallyBoundTypes.Contains(e.SourceName))
                .Where(e => !e.HeaderFile.FilePath.Contains("Tests"))
                .Where(e => !e.HeaderFile.FilePath.Contains("Internal"))
                .Where(e => !e.HeaderFile.FilePath.Contains("Private"))
                .Where(e => !e.HeaderFile.FilePath.Contains("NoExportTypes.h"))
                .Where(e => !e.SourceName.Contains("Deprecated"))
                .Where(e => !e.Deprecated)
                .ToList();

            if (allClasses.Count == 0 && allStructs.Count == 0 && allEnums.Count == 0)
                return;

            // Only structs that have at least one script-exposed property get registered.
            var allStructsWithContent = allStructs.Where(HasScriptExposedStructContent).ToList();

            // Build the set of all type names that will be registered (used to validate
            // parameter types later).
            var registeredTypeNames = new HashSet<string>(ManuallyBoundTypes, StringComparer.Ordinal);
            foreach (var cls in allClasses)
                registeredTypeNames.Add(cls.SourceName);
            foreach (var s in allStructsWithContent)
                registeredTypeNames.Add(s.SourceName);
            foreach (var e in allEnums)
                registeredTypeNames.Add(e.SourceName);

            // Group classes (with content), structs (with content), and enums by their source header
            // file so that everything declared in the same header shares one generated file pair.
            var allGroups = allClasses.Where(HasScriptExposedContent).Cast<UhtType>()
                .Concat(allStructsWithContent.Cast<UhtType>())
                .Concat(allEnums.Cast<UhtType>())
                .GroupBy(t => t.HeaderFile.FilePath)
                .Select(g => new HeaderFileGroup(
                    FileStem: Path.GetFileNameWithoutExtension(g.Key),
                    IncludePath: g.First().HeaderFile.IncludeFilePath,
                    Classes: g.OfType<UhtClass>().OrderBy(c => c.SourceName).ToList(),
                    Structs: g.OfType<UhtScriptStruct>().OrderBy(s => s.SourceName).ToList(),
                    Enums: g.OfType<UhtEnum>().OrderBy(e => e.SourceName).ToList()))
                .OrderBy(g => g.FileStem)
                .ToList();

            // Generate one .h / .cpp pair per header group.
            foreach (var group in allGroups)
            {
                var hdr = new StringBuilder();
                WriteAutoGenNotice(hdr);
                WritePerFileHeader(hdr, group);
                _factory.CommitOutput(_factory.MakePath($"Bind_{group.FileStem}", ".oas.gen.h"), hdr);

                var src = new StringBuilder();
                WriteAutoGenNotice(src);
                WritePerFileSource(src, group, registeredTypeNames);
                _factory.CommitOutput(_factory.MakePath($"Bind_{group.FileStem}", ".oas.gen.cpp"), src);
            }

            // Generate the master header and master source.
            var masterHdr = new StringBuilder();
            WriteAutoGenNotice(masterHdr);
            WriteMasterHeader(masterHdr, allGroups);
            _factory.CommitOutput(_factory.MakePath("OtterAngelScriptBindings", ".gen.h"), masterHdr);

            var masterSrc = new StringBuilder();
            WriteAutoGenNotice(masterSrc);
            WriteMasterSource(masterSrc, allClasses, allStructsWithContent, allEnums, allGroups);
            _factory.CommitOutput(_factory.MakePath("OtterAngelScriptBindings", ".gen.cpp"), masterSrc);

            _factory.Session.LogInfo(
                $"OtterAngleScript: generated {allGroups.Count} file pairs ({allStructsWithContent.Count} structs, {allEnums.Count} enums, {allGroups.Sum(g => g.Classes.Count)} classes) + master header/source.");
        }

        // -------------------------------------------------------------------------
        // File-level helpers
        // -------------------------------------------------------------------------

        private static void WriteAutoGenNotice(StringBuilder sb)
        {
            sb.AppendLine("// Auto-generated by OtterAngleScript UHT Plugin.");
            sb.AppendLine("// DO NOT EDIT MANUALLY - regenerated on every build.");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Per-file header: declares registration functions for all classes and
        // structs declared in one source header.
        // -------------------------------------------------------------------------

        private static void WritePerFileHeader(StringBuilder sb, HeaderFileGroup group)
        {
            sb.AppendLine("#pragma once");
            sb.AppendLine();
            sb.AppendLine("class asIScriptEngine;");
            sb.AppendLine();
            foreach (var cls in group.Classes)
                sb.AppendLine($"void OAS_RegisterMethods_{cls.SourceName}(asIScriptEngine* Engine);");
            foreach (var s in group.Structs)
                sb.AppendLine($"void OAS_Register_{s.SourceName}(asIScriptEngine* Engine);");
            foreach (var e in group.Enums)
                sb.AppendLine($"void OAS_Register_{e.SourceName}(asIScriptEngine* Engine);");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Per-file source: registration function definitions for all classes and
        // structs declared in one source header.
        // -------------------------------------------------------------------------

        private void WritePerFileSource(
            StringBuilder sb,
            HeaderFileGroup group,
            HashSet<string> registeredTypeNames)
        {
            sb.AppendLine($"#include \"Bind_{group.FileStem}.oas.gen.h\"");
            sb.AppendLine($"#include \"{group.IncludePath}\"");
            if (group.Structs.Count > 0)
                sb.AppendLine("#include <new>");
            sb.AppendLine();
            sb.AppendLine("#ifdef _MSC_VER");
            sb.AppendLine("#pragma warning(disable:4191 4996)");
            sb.AppendLine("#endif");
            sb.AppendLine("#include \"angelscript.h\"");
            sb.AppendLine();

            foreach (var cls in group.Classes)
                WriteClassRegistrationFunction(sb, cls, registeredTypeNames);

            foreach (var s in group.Structs)
            {
                WriteStructHelpers(sb, s);
                WriteStructRegistrationFunction(sb, s, registeredTypeNames);
            }

            foreach (var e in group.Enums)
                WriteEnumRegistrationFunction(sb, e);
        }

        private void WriteClassRegistrationFunction(
            StringBuilder sb,
            UhtClass cls,
            HashSet<string> registeredTypeNames)
        {
            sb.AppendLine($"void OAS_RegisterMethods_{cls.SourceName}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");
            bool anyContent = false;

            var staticFuncs = ScriptFunctions(cls).Where(f => f.FunctionFlags.HasFlag(EFunctionFlags.Static)).ToList();
            if (staticFuncs.Count > 0)
            {
                sb.AppendLine("    // --------------STATIC FUNCTIONS--------------");
                sb.AppendLine($"    Result = Engine->SetDefaultNamespace(\"{cls.SourceName}\"); check(Result >= 0);");
                foreach (var func in staticFuncs)
                {
                    if (!TryBuildAsSignature(func, registeredTypeNames, out string? asSig))
                    {
                        _factory.Session.LogInfo($"OAS: skipping static function {cls.SourceName}::{func.SourceName} {func.FunctionFlags} since its signature contains unsupported types.");
                        continue;
                    }
                    if (!TryBuildAsMethodSignature(cls, func, registeredTypeNames, out string? asMethodSig))
                        continue;
                    sb.AppendLine($"    Result = Engine->RegisterGlobalFunction(\"{asSig}\",");
                    sb.AppendLine($"        asFUNCTIONPR({cls.SourceName}::{func.SourceName}, {asMethodSig}), asCALL_CDECL);");
                    sb.AppendLine("    check(Result >= 0);");
                    sb.AppendLine();
                    anyContent = true;
                }
                sb.AppendLine($"    Result = Engine->SetDefaultNamespace(\"\"); check(Result >= 0);");
                sb.AppendLine();
            }

            var nonStaticFuncs = ScriptFunctions(cls).Where(f => !f.FunctionFlags.HasFlag(EFunctionFlags.Static)).ToList();
            if (nonStaticFuncs.Count > 0)
            {
                sb.AppendLine("    // --------------NON-STATIC FUNCTIONS--------------");
                const string callConv = "asCALL_THISCALL";
                foreach (var func in nonStaticFuncs)
                {
                    if (!TryBuildAsSignature(func, registeredTypeNames, out string? asSig))
                        continue;
                    if (func.FunctionFlags.HasAnyFlags(EFunctionFlags.Protected | EFunctionFlags.Private))
                        continue; // TODO: support non-public functions via wrapper functions.
                    if (!TryBuildAsMethodSignature(cls, func, registeredTypeNames, out string? asMethodSig))
                        continue;

                    sb.AppendLine($"    Result = Engine->RegisterObjectMethod(\"{cls.SourceName}\", \"{asSig}\",");
                    sb.AppendLine($"        asMETHODPR({cls.SourceName}, {func.SourceName}, {asMethodSig}), {callConv});");
                    sb.AppendLine("    check(Result >= 0);");
                    sb.AppendLine();
                    anyContent = true;
                }
            }

            foreach (var prop in ScriptProperties(cls))
            {
                if (prop.IsBitfield)
                    continue; // TODO: support bitfield properties via wrapper functions.
                if (prop.PropertyFlags.HasAnyFlags(EPropertyFlags.NativeAccessSpecifierProtected | EPropertyFlags.NativeAccessSpecifierPrivate))
                    continue; // TODO: support non-public properties via wrapper functions.

                string? asType = MapPropertyAsType(prop, registeredTypeNames);
                if (asType == null)
                    continue;

                sb.AppendLine($"    Result = Engine->RegisterObjectProperty(\"{cls.SourceName}\", \"{asType} {prop.SourceName}\",");
                sb.AppendLine($"        asOFFSET({cls.SourceName}, {prop.SourceName}));");
                sb.AppendLine("    check(Result >= 0);");
                anyContent = true;
            }

            if (!anyContent)
                sb.AppendLine("    (void)Engine; // nothing to register");

            sb.AppendLine("}");
            sb.AppendLine();
        }

        /// <summary>
        /// Emits file-scoped ctor/dtor/assign helper functions used by the struct registration.
        /// </summary>
        private static void WriteStructHelpers(StringBuilder sb, UhtScriptStruct s)
        {
            string name = s.SourceName;
            sb.AppendLine($"static void {name}_DefaultConstruct({name}* InMemory) {{ new (InMemory) {name}(); }}");
            sb.AppendLine($"static void {name}_CopyConstruct(const {name}& Other, {name}* InMemory) {{ new (InMemory) {name}(Other); }}");
            sb.AppendLine($"static void {name}_Destruct({name}* InMemory) {{ InMemory->~{name}(); }}");
            sb.AppendLine($"static {name}& {name}_Assign({name}& Value, const {name}& Other) {{ Value = Other; return Value; }}");
            sb.AppendLine();
        }

        private void WriteStructRegistrationFunction(
            StringBuilder sb,
            UhtScriptStruct s,
            HashSet<string> registeredTypeNames)
        {
            string name = s.SourceName;
            sb.AppendLine($"void OAS_Register_{name}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");
            sb.AppendLine($"    Result = Engine->RegisterObjectType(\"{name}\", sizeof({name}),");
            sb.AppendLine($"        asOBJ_VALUE | asGetTypeTraits<{name}>());");
            sb.AppendLine("    check(Result >= 0);");
            sb.AppendLine();
            sb.AppendLine($"    Result = Engine->RegisterObjectBehaviour(\"{name}\", asBEHAVE_CONSTRUCT,");
            sb.AppendLine($"        \"void f()\", asFUNCTION({name}_DefaultConstruct), asCALL_CDECL_OBJLAST);");
            sb.AppendLine("    check(Result >= 0);");
            sb.AppendLine($"    Result = Engine->RegisterObjectBehaviour(\"{name}\", asBEHAVE_CONSTRUCT,");
            sb.AppendLine($"        \"void f(const {name} &in)\", asFUNCTION({name}_CopyConstruct), asCALL_CDECL_OBJLAST);");
            sb.AppendLine("    check(Result >= 0);");
            sb.AppendLine($"    Result = Engine->RegisterObjectBehaviour(\"{name}\", asBEHAVE_DESTRUCT,");
            sb.AppendLine($"        \"void f()\", asFUNCTION({name}_Destruct), asCALL_CDECL_OBJLAST);");
            sb.AppendLine("    check(Result >= 0);");
            sb.AppendLine($"    Result = Engine->RegisterObjectMethod(\"{name}\", \"{name}& opAssign(const {name} &in)\",");
            sb.AppendLine($"        asFUNCTION({name}_Assign), asCALL_CDECL_OBJFIRST);");
            sb.AppendLine("    check(Result >= 0);");
            sb.AppendLine();

            foreach (var prop in ScriptStructProperties(s))
            {
                if (prop.IsBitfield)
                    // TODO: support bitfield properties via wrapper functions.
                    continue;
                if (prop.SourceName == "Category")
                {
                    _factory.Session.LogInfo($"OAS: skipping struct property {name}::{prop.SourceName} {prop.PropertyFlags}");
                }
                if (prop.PropertyFlags.HasAnyFlags(EPropertyFlags.NativeAccessSpecifierProtected | EPropertyFlags.NativeAccessSpecifierPrivate))
                    continue;

                string? asType = GetAsPropertyDeclare(prop);
                if (asType == null)
                    continue;

                sb.AppendLine($"    Result = Engine->RegisterObjectProperty(\"{name}\", \"{asType} {prop.SourceName}\",");
                sb.AppendLine($"        asOFFSET({name}, {prop.SourceName}));");
                sb.AppendLine("    check(Result >= 0);");
            }

            sb.AppendLine("}");
            sb.AppendLine();
        }

        private static void WriteEnumRegistrationFunction(StringBuilder sb, UhtEnum e)
        {
            string name = e.SourceName;
            bool isNamespaced = e.CppForm == UhtEnumCppForm.Namespaced;
            string asTypeName = name;

            sb.AppendLine($"void OAS_Register_{name}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");
            sb.AppendLine($"    Result = Engine->RegisterEnum(\"{asTypeName}\"); check(Result >= 0);");
            foreach (var value in e.EnumValues)
            {
                string shortName = value.Name.Contains("::")
                    ? value.Name.Substring(value.Name.LastIndexOf("::") + 2)
                    : value.Name;
                if (shortName.EndsWith("_MAX") || shortName == "MAX" || shortName == "COUNT")
                    continue;
                sb.AppendLine($"    Result = Engine->RegisterEnumValue(\"{asTypeName}\", \"{shortName}\", (int32){value.Name}); check(Result >= 0);");
            }
            sb.AppendLine("}");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Master header: includes all per-file headers + declares master functions.
        // -------------------------------------------------------------------------

        private static void WriteMasterHeader(StringBuilder sb, List<HeaderFileGroup> groups)
        {
            sb.AppendLine("#pragma once");
            sb.AppendLine();

            sb.AppendLine("class asIScriptEngine;");

            sb.AppendLine();
            sb.AppendLine("// Registers all auto-generated UClass reference types and USTRUCT value types.");
            sb.AppendLine("void OAS_RegisterGeneratedTypes(asIScriptEngine* Engine);");
            sb.AppendLine();
            sb.AppendLine("// Call from StartupModule() AFTER all manual bindings have been registered.");
            sb.AppendLine("void Bind_Generated(asIScriptEngine* Engine);");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Master source: OAS_RegisterGeneratedTypes + Bind_Generated.
        // -------------------------------------------------------------------------

        private static void WriteMasterSource(
            StringBuilder sb,
            List<UhtClass> allClasses,
            List<UhtScriptStruct> allStructsWithContent,
            List<UhtEnum> allEnums,
            List<HeaderFileGroup> groups)
        {
            sb.AppendLine("#include \"OtterAngelScriptBindings.gen.h\"");
            sb.AppendLine();

            foreach (var group in groups)
                sb.AppendLine($"#include \"Bind_{group.FileStem}.oas.gen.h\"");
            sb.AppendLine();
            sb.AppendLine("#ifdef _MSC_VER");
            sb.AppendLine("#pragma warning(disable:4191 4996)");
            sb.AppendLine("#endif");
            sb.AppendLine("#include \"angelscript.h\"");

            sb.AppendLine("void OAS_RegisterGeneratedTypes(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");

            // Register all UENUM types first (no dependencies on other types).
            foreach (var e in allEnums)
            {
                sb.AppendLine($"    OAS_Register_{e.SourceName}(Engine);");
            }
            sb.AppendLine();

            // Stub-register every UClass so that parameter/return-type references resolve
            // regardless of registration order.
            foreach (var cls in allClasses)
            {
                sb.AppendLine($"    Result = Engine->RegisterObjectType(\"{cls.SourceName}\", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE); check(Result >= 0);");
            }

            // Register all USTRUCT value types (full registration, must come after UClass stubs
            // so struct property types that reference UClasses are already known).
            foreach (var s in allStructsWithContent)
            {
                sb.AppendLine($"    OAS_Register_{s.SourceName}(Engine);");
            }

            sb.AppendLine("}");
            sb.AppendLine();

            sb.AppendLine("void Bind_Generated(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    check(Engine != nullptr);");
            foreach (var group in groups)
                foreach (var cls in group.Classes)
                    sb.AppendLine($"    OAS_RegisterMethods_{cls.SourceName}(Engine);");
            sb.AppendLine("}");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Helpers – type traversal & filtering
        // -------------------------------------------------------------------------

        private static IEnumerable<UhtType> TraverseTree(UhtType parent)
        {
            yield return parent;
            if (parent.Children != null)
                foreach (var child in parent.Children.SelectMany(TraverseTree))
                    yield return child;
        }

        private static bool HasScriptExposedContent(UhtClass cls)
        {
            return ScriptFunctions(cls).Any() || ScriptProperties(cls).Any();
        }

        private static IEnumerable<UhtFunction> ScriptFunctions(UhtClass cls)
        {
            return cls.Functions.Where(f =>
                f.FunctionFlags.HasAnyFlags(EFunctionFlags.BlueprintCallable | EFunctionFlags.BlueprintPure)
                && !f.Deprecated && !f.GetDisplayNameText().Contains("Deprecated"))
                //.Where(f => !f.FunctionFlags.HasFlag(EFunctionFlags.EditorOnly)); // TODO: consider allowing editor-only functions if the plugin is used in an editor build configuration.
                .Where(f => f.FunctionExportFlags.HasFlag(UhtFunctionExportFlags.RequiredAPI)) // Can not bind function that are not exported.
                .Where(f => !f.FunctionExportFlags.HasFlag(UhtFunctionExportFlags.CustomThunk))
                ;
        }

        private static IEnumerable<UhtProperty> ScriptProperties(UhtClass cls)
        {
            return cls.Children
                .OfType<UhtProperty>()
                .Where(p => !p.PropertyFlags.HasAnyFlags(EPropertyFlags.Parm | EPropertyFlags.Deprecated)
                         && p.PropertyFlags.HasAnyFlags(EPropertyFlags.BlueprintVisible | EPropertyFlags.BlueprintReadOnly)
                         && !p.Deprecated && !p.FullName.Contains("_DEPRECATED"))
                .Where(p => !p.IsEditorOnlyProperty)
                .Where(p => !(p.Getter != null && p.Setter != null))
                .Where(p => !(p.MetaData.ContainsKey(UhtNames.BlueprintGetter) && p.MetaData.ContainsKey(UhtNames.BlueprintSetter)));
        }

        private static bool HasScriptExposedStructContent(UhtScriptStruct s)
        {
            return ScriptStructProperties(s).Any();
        }

        private static IEnumerable<UhtProperty> ScriptStructProperties(UhtScriptStruct s)
        {
            return s.Children
                .OfType<UhtProperty>()
                .Where(p => !p.PropertyFlags.HasAnyFlags(EPropertyFlags.Parm | EPropertyFlags.Deprecated)
                         && p.PropertyFlags.HasAnyFlags(EPropertyFlags.BlueprintVisible | EPropertyFlags.BlueprintReadOnly)
                         && !p.Deprecated && !p.FullName.Contains("_DEPRECATED"))
                .Where(p => !p.IsEditorOnlyProperty)
                .Where(p => !p.PropertyFlags.HasAnyFlags(EPropertyFlags.Protected | EPropertyFlags.NativeAccessSpecifierPrivate));
        }

        private record HeaderFileGroup(
            string FileStem,
            string IncludePath,
            List<UhtClass> Classes,
            List<UhtScriptStruct> Structs,
            List<UhtEnum> Enums);

        // -------------------------------------------------------------------------
        // Helpers – code generation
        // -------------------------------------------------------------------------

        /// <summary>
        /// Tries to build a C++ static wrapper function line for the given UFunction.
        /// Returns false if any parameter/return type cannot be mapped.
        /// </summary>
        private bool TryBuildWrapper(
            UhtClass cls,
            UhtFunction func,
            HashSet<string> registeredTypeNames,
            out string? wrapperLine)
        {
            wrapperLine = null;

            // Return type
            var returnProp = GetReturnProperty(func);
            string cppRet = returnProp != null
                ? MapPropertyCppType(returnProp, isParam: false, registeredTypeNames) ?? ""
                : "void";
            if (returnProp != null && cppRet == "") return false;

            // Parameters (excludes return parm)
            var paramProps = GetParamProperties(func).ToList();
            var cppParamParts = new List<string>();
            var argNames = new List<string>();

            bool isConst = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Const);
            bool isStatic = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Static);

            if (!isStatic)
                cppParamParts.Add($"{cls.SourceName}*{(isConst ? " const" : "")} Self");

            foreach (var p in paramProps)
            {
                string? cppType = MapPropertyCppType(p, isParam: true, registeredTypeNames);
                if (cppType == null) return false;
                cppParamParts.Add($"{cppType} {p.SourceName}");
                argNames.Add(p.SourceName);
            }

            string paramStr = string.Join(", ", cppParamParts);
            string argStr = string.Join(", ", argNames);
            string retKeyword = returnProp == null ? "" : "return ";
            string call = isStatic
                ? $"{cls.SourceName}::{func.SourceName}({argStr})"
                : $"Self->{func.SourceName}({argStr})";

            wrapperLine = $"static {cppRet} {func.SourceName}({paramStr}) {{ {retKeyword}{call}; }}";
            return true;
        }

        /// <summary>
        /// Tries to build an AngelScript method signature string for RegisterObjectMethod.
        /// Returns false if any parameter/return type cannot be mapped.
        /// </summary>
        private bool TryBuildAsSignature(UhtFunction func, HashSet<string> registeredTypeNames, out string? asSignature)
        {
            asSignature = null;

            var returnProp = GetReturnProperty(func);
            string asRet = returnProp != null
                ? MapPropertyAsType(returnProp, registeredTypeNames) ?? ""
                : "void";
            if (returnProp != null && asRet == "")
                return false;

            var paramParts = new List<string>();
            foreach (var p in GetParamProperties(func))
            {
                string? asType = MapPropertyAsType(p, registeredTypeNames);
                if (asType == null)
                {
                    _factory.Session.LogInfo($"skipping function {func.SourceName} since parameter {p.SourceName} has unsupported type.");
                    return false;
                }
                paramParts.Add($"{asType} {p.SourceName}");
            }

            bool isConst = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Const);
            string constSuffix = isConst ? " const" : "";
            asSignature = $"{asRet} {func.SourceName}({string.Join(", ", paramParts)}){constSuffix}";
            return true;
        }

        private bool TryBuildAsMethodSignature(UhtClass uclass, UhtFunction func, HashSet<string> registeredTypeNames, out string? asSignature)
        {
            asSignature = null;
            bool IsConstFunction = func.FunctionFlags.HasFlag(EFunctionFlags.Const);

            var returnProp = GetReturnProperty(func);
            string asRet = returnProp != null
                ? MapPropertyCppType(returnProp, false, registeredTypeNames) ?? ""
                : "void";
            if (returnProp != null && asRet == "")
                return false;
            string cppMethodSignature;
            if (GetCustomCppSignature(uclass, func) is string customSig)
            {
                cppMethodSignature = customSig;
            }
            else
            {
                var paramParts = new List<string>();
                foreach (var p in GetParamProperties(func))
                {
                    string? asType = MapPropertyCppType(p, true, registeredTypeNames);
                    if (asType == null)
                        return false;
                    paramParts.Add($"{asType}");
                }
                cppMethodSignature = string.Join(", ", paramParts);
            }
            bool isConst = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Const);
            asSignature = $"({cppMethodSignature}){(IsConstFunction ? " const" : "")}, {asRet}";
            return true;
        }

        // -------------------------------------------------------------------------
        // Helpers – function parameter extraction
        // -------------------------------------------------------------------------

        private static UhtProperty? GetReturnProperty(UhtFunction func)
        {
            return func.Children
                .OfType<UhtProperty>()
                .FirstOrDefault(p => p.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm));
        }

        private static IEnumerable<UhtProperty> GetParamProperties(UhtFunction func)
        {
            return func.Children
                .OfType<UhtProperty>()
                .Where(p => p.PropertyFlags.HasAnyFlags(EPropertyFlags.Parm)
                         && !p.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm));
        }

        // -------------------------------------------------------------------------
        // Type mapping – AngelScript (for method signatures)
        // -------------------------------------------------------------------------

        private string? GetAsPropertyDeclare(UhtProperty Prop)
        {
            if (Prop is UhtObjectProperty PropObj)
            {
                if (PropObj.CppForm == UhtObjectCppForm.TObjectPtrObject)
                {
                    return Prop.SourceName;
                }
            }
            else if (Prop is UhtEnumProperty PropEnum)
            {
                if (PropEnum.Enum.CppForm == UhtEnumCppForm.Namespaced)
                {
                    return $"{PropEnum.Enum.SourceName}::Type";
                }
            }
            StringBuilder propertySB = new StringBuilder();
            Prop.AppendText(propertySB, UhtPropertyTextType.Sparse);
            _factory.Session.LogInfo($"UhtPropertyTextType.Sparse {Prop.SourceName} {propertySB}");
            return propertySB.ToString();
        }

        /// <summary>
        /// Maps a UHT property to an AngelScript type declaration string.
        /// Returns null if the type is not supported (the function will be skipped).
        /// </summary>
        private static string? MapPropertyAsType(UhtProperty property, HashSet<string> registeredTypeNames)
        {
            bool isReturn = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm);
            bool isOut = !isReturn && property.PropertyFlags.HasAnyFlags(EPropertyFlags.OutParm);
            bool isRef = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReferenceParm);
            bool isConst = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ConstParm);
            switch (property)
            {
                case UhtBoolProperty:
                    return isOut ? "bool &out" : "bool";

                case UhtByteProperty { Enum: null }:
                    return isOut ? "uint8 &out" : "uint8";

                case UhtIntProperty:
                    return isOut ? "int &out" : "int";

                case UhtInt64Property:
                    return isOut ? "int64 &out" : "int64";

                case UhtUInt32Property:
                    return isOut ? "uint &out" : "uint";

                case UhtUInt64Property:
                    return isOut ? "uint64 &out" : "uint64";

                case UhtFloatProperty:
                    return isOut ? "float &out" : "float";

                case UhtDoubleProperty:
                    return isOut ? "double &out" : "double";

                case UhtStrProperty:
                    if (isReturn) return "FString";
                    return isOut ? "FString&" : $"{(isConst ? "const" : "")} FString{(isRef ? "&in" : "")}";

                case UhtNameProperty:
                    if (isReturn) return "FName";
                    return isOut ? "FName&" : $"{(isConst ? "const" : "")} FName{(isRef ? "&in" : "")}";

                case UhtTextProperty:
                    if (isReturn) return "FText";
                    return isOut ? "FText&" : $"{(isConst ? "const" : "")} FText{(isRef ? "&in" : "")}";
                case UhtClassProperty uclass:
                    if (uclass.MetaClass == null)
                        return null; // unsupported: TSubclassOf without a specified base class
                    return $"TSubclassOf<class {uclass.MetaClass.SourceName}>";

                case UhtObjectProperty p when p.Class != null:
                    return $"{p.Class.SourceName}";

                case UhtStructProperty p
                        when registeredTypeNames.Contains(p.ScriptStruct.SourceName):
                    if (isReturn) return p.ScriptStruct.SourceName;
                    return isOut
                        ? $"{p.ScriptStruct.SourceName} &out"
                        : $"const {p.ScriptStruct.SourceName} &in";

                case UhtEnumProperty p when registeredTypeNames.Contains(p.Enum.SourceName):
                    if (p.Enum.CppForm == UhtEnumCppForm.Namespaced)
                        return $"{p.Enum.SourceName}::Type";
                    else
                        return p.Enum.SourceName;

                default:
                    return null;
            }
        }

        // -------------------------------------------------------------------------
        // Type mapping – C++ (for wrapper function signatures)
        // -------------------------------------------------------------------------

        /// <summary>
        /// Maps a UHT property to a C++ type declaration string.
        /// Returns null if the type is not supported.
        /// </summary>
        private string? MapPropertyCppType(
            UhtProperty property,
            bool isParam,
            HashSet<string> registeredTypeNames)
        {
            StringBuilder stringBuilder = new StringBuilder();
            AppendFullDecl(property, stringBuilder, true);
            property.Resolve(UhtResolvePhase.Final);

            var Result = stringBuilder.ToString();
            return Result;

            //bool isReturn = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm);
            //bool isConst = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ConstParm);
            //bool isRef = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReferenceParm);

            //switch (property)
            //{
            //    case UhtBoolProperty:
            //        return isRef ? "bool&" : "bool";

            //    case UhtByteProperty { Enum: null }:
            //        return isRef ? "uint8&" : "uint8";

            //    case UhtIntProperty:
            //        return isRef ? "int32&" : "int32";

            //    case UhtInt64Property:
            //        return isRef ? "int64&" : "int64";

            //    case UhtUInt32Property:
            //        return isRef ? "uint32&" : "uint32";

            //    case UhtUInt64Property:
            //        return isRef ? "uint64&" : "uint64";

            //    case UhtFloatProperty:
            //        return isRef ? "float&" : "float";

            //    case UhtDoubleProperty:
            //        return isRef ? "double&" : "double";

            //    case UhtStrProperty:
            //        if (!isParam) return "FString";
            //        return isRef ? "FString&" : $"{(isConst ? "const" : "")} FString{(isRef ? "&" : "")}";

            //    case UhtNameProperty:
            //        if (!isParam) return "FName";
            //        return isRef ? "FName&" : $"{(isConst ? "const" : "")} FName{(isRef ? "&" : "")}";
            //    case UhtTextProperty:
            //        if (!isParam) return "FText";
            //        return isRef ? "FText&" : $"{(isConst ? "const" : "")} FText{(isRef ? "&" : "")}";

            //    case UhtClassProperty uclass:
            //        if (uclass.MetaClass == null)
            //            return null; // unsupported: TSubclassOf without a specified base class
            //        return $"TSubclassOf<class {uclass.MetaClass.SourceName}>";

            //    case UhtObjectProperty p when p.Class != null:
            //        return $"{p.Class.SourceName}*";

            //    case UhtStructProperty p
            //            when registeredTypeNames.Contains(p.ScriptStruct.SourceName):
            //        if (!isParam) return p.ScriptStruct.SourceName;
            //        return isRef
            //            ? $"{p.ScriptStruct.SourceName}&"
            //            //: $"const {p.ScriptStruct.SourceName}&";
            //              : $"{(isConst ? "const " : "")}{p.ScriptStruct.SourceName}{(isRef ? "&" : "")}";
            //    case UhtEnumProperty p:
            //        if (p.Enum.CppForm == UhtEnumCppForm.Namespaced)
            //            return $"{p.Enum.SourceName}::Type";
            //        else
            //            return p.Enum.SourceName;

            //    default:
            //        return null;
            //}
        }
        public StringBuilder AppendFullDecl(UhtProperty property, StringBuilder builder, bool skipParameterName = false)
        {
            UhtPropertyCaps caps = property.PropertyCaps;

            bool isInterfaceProp = property is UhtInterfaceProperty;
                
            bool passCppArgsByRef = caps.HasAnyFlags(UhtPropertyCaps.PassCppArgsByRef);
            bool isConstParam = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ConstParm) || property.RefQualifier.HasFlag(UhtPropertyRefQualifier.ConstRef);
            bool isReturnParm = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm);

            bool shouldHaveRef = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReferenceParm | EPropertyFlags.OutParm) && !isReturnParm || property.RefQualifier.HasFlag(UhtPropertyRefQualifier.ConstRef);

            if (isConstParam)
            {
                builder.Append("const ");
            }

            property.AppendText(builder, UhtPropertyTextType.GenericFunctionArgOrRetVal);

            //bool fromConstClass = false;
            //bool constAtTheEnd = fromConstClass || (isConstParam && shouldHaveRef);
            //if (constAtTheEnd)
            //{
            //    builder.Append(" const");
            //}

            if (shouldHaveRef)
            {
                builder.Append('&');
            }

            builder.Append(' ');
            if (!skipParameterName)
            {
                builder.Append(property.SourceName);
            }

            if (property.ArrayDimensions != null)
            {
                builder.Append('[').Append(property.ArrayDimensions).Append(']');
            }
            return builder;
        }

        string? GetCustomCppSignature(UhtClass cls, UhtFunction func)
        {
            if (CustomCppSignature.TryGetValue(cls.SourceName, out var funcSigs) &&
                funcSigs.TryGetValue(func.SourceName, out var sig))
            {
                return sig;
            }
            return null;
        }
    }
}
