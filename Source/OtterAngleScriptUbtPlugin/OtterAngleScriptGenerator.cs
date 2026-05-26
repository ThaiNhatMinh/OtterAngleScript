// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using EpicGames.Core;
using EpicGames.UHT.Tables;
using EpicGames.UHT.Types;
using EpicGames.UHT.Utils;

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
            "UObject", "UClass",
            "FString", "FName", "FText",
            "FVector", "FVector2D", "FRotator", "FQuat",
            "FTransform", "FPlane", "FBox",
            "FHitResult", "FTimerHandle", "FLatentActionInfo",
            "FActorInstanceHandle",
        };
        private static readonly HashSet<string> BuildModules = new(StringComparer.Ordinal)
        {
            "NetCore", "CoreUObject", "Engine"
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
            // Collect all UClasses excluding manually-bound types and interfaces.
            var allClasses = packages
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtClass>()
                .Where(c => !c.ClassFlags.HasAnyFlags(EClassFlags.Interface))
                .Where(c => !ManuallyBoundTypes.Contains(c.SourceName))
                .Where(c => !c.HeaderFile.FilePath.Contains("Tests"))
                .Where(c => !c.HeaderFile.FilePath.Contains("Internal"))
                .Where(c => !c.HeaderFile.FilePath.Contains("Private"))
                .Where(c => !c.Deprecated)
                .OrderBy(c => c.SourceName)
                .ToList();
            foreach (var package in packages)
            {
                _factory.Session.LogInfo("Package {0}", package.FullName);
            }
            if (allClasses.Count == 0)
                return;

            // Build the set of all type names that will be registered (used to validate
            // struct parameter types later).
            var registeredTypeNames = new HashSet<string>(ManuallyBoundTypes, StringComparer.Ordinal);
            foreach (var cls in allClasses)
                registeredTypeNames.Add(cls.SourceName);

            // Classes that have at least one script-exposed method or property.
            var classesWithContent = allClasses
                .Where(c => HasScriptExposedContent(c))
                .ToList();

            // Generate one .h / .cpp pair per class.
            foreach (var cls in classesWithContent)
            {
                var hdr = new StringBuilder();
                WriteAutoGenNotice(hdr);
                WritePerClassHeader(hdr, cls);
                _factory.CommitOutput(_factory.MakePath($"Bind_{cls.SourceName}", ".oas.gen.h"), hdr);

                var src = new StringBuilder();
                WriteAutoGenNotice(src);
                WritePerClassSource(src, cls, registeredTypeNames);
                _factory.CommitOutput(_factory.MakePath($"Bind_{cls.SourceName}", ".oas.gen.cpp"), src);
            }

            // Generate the master header and master source.
            var masterHdr = new StringBuilder();
            WriteAutoGenNotice(masterHdr);
            WriteMasterHeader(masterHdr, classesWithContent);
            _factory.CommitOutput(_factory.MakePath("OtterAngelScriptBindings", ".gen.h"), masterHdr);

            var masterSrc = new StringBuilder();
            WriteAutoGenNotice(masterSrc);
            WriteMasterSource(masterSrc, allClasses, classesWithContent);
            _factory.CommitOutput(_factory.MakePath("OtterAngelScriptBindings", ".gen.cpp"), masterSrc);

            _factory.Session.LogInfo(
                "OtterAngleScript: generated {0} per-class file pairs + master header/source.",
                classesWithContent.Count);
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
        // Per-class header: declares the registration function.
        // -------------------------------------------------------------------------

        private static void WritePerClassHeader(StringBuilder sb, UhtClass cls)
        {
            sb.AppendLine("#pragma once");
            sb.AppendLine();
            sb.AppendLine("#include \"angelscript.h\"");
            sb.AppendLine();
            sb.AppendLine($"void OAS_RegisterMethods_{cls.SourceName}(asIScriptEngine* Engine);");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Per-class source: wrapper namespace + registration function definition.
        // -------------------------------------------------------------------------

        private void WritePerClassSource(
            StringBuilder sb,
            UhtClass cls,
            HashSet<string> registeredTypeNames)
        {
            sb.AppendLine($"#include \"Bind_{cls.SourceName}.oas.gen.h\"");
            sb.AppendLine($"#include \"{cls.HeaderFile.IncludeFilePath}\"");
            sb.AppendLine();

            // Wrapper namespace – only emitted when at least one function is mappable.
            /*
            var wrapperLines = new List<string>();
            foreach (var func in ScriptFunctions(cls))
            {
                if (func.FunctionFlags.HasAnyFlags(EFunctionFlags.Protected | EFunctionFlags.Private))
                {
                    continue; // skip non-public functions since the generated wrapper won't be able to call them.
                }
                if (TryBuildWrapper(cls, func, registeredTypeNames, out string? line))
                    wrapperLines.Add(line!);
            }

            if (wrapperLines.Count > 0)
            {
                sb.AppendLine($"namespace OAS_Gen_{cls.SourceName}");
                sb.AppendLine("{");
                foreach (var line in wrapperLines)
                    sb.AppendLine($"    {line}");
                sb.AppendLine("}");
                sb.AppendLine();
            }
            */
            // Registration function definition.
            sb.AppendLine($"void OAS_RegisterMethods_{cls.SourceName}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");
            bool anyContent = false;

            foreach (var func in ScriptFunctions(cls))
            {
                if (!TryBuildAsSignature(func, registeredTypeNames, out string? asSig))
                    continue;

                bool isStatic = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Static);
                if (isStatic)
                {
                    sb.AppendLine($"    Result = Engine->SetDefaultNamespace(\"{cls.SourceName}\"); check(Result >= 0);");
                    sb.AppendLine($"    Result = Engine->RegisterGlobalFunction(\"{asSig}\",");
                    sb.AppendLine($"        asFUNCTION({cls.SourceName}::{func.SourceName}), asCALL_CDECL);");
                    sb.AppendLine("    check(Result >= 0);");
                    anyContent = true;
                }
                else
                {
                    if (!TryBuildAsMethodSignature(func, registeredTypeNames, out string? asMethodSig))
                        continue;
                    string callConv = "asCALL_THISCALL";

                    sb.AppendLine($"    Result = Engine->RegisterObjectMethod(\"{cls.SourceName}\", \"{asSig}\",");
                    //sb.AppendLine($"        asFUNCTION(OAS_Gen_{cls.SourceName}::{func.SourceName}), {callConv});");
                    sb.AppendLine($"        asMETHODPR({cls.SourceName}, {func.SourceName}, {asMethodSig}), {callConv});");
                    sb.AppendLine("    check(Result >= 0);");
                    anyContent = true;
                }
            }

            foreach (var prop in ScriptProperties(cls))
            {
                if (prop.SourceName == "NetCullDistanceSquared")
                {
                    _factory.Session.LogWarning($"Property {prop.Getter}::{prop.Setter} {prop.PropertyExportFlags.HasAnyFlags(UhtPropertyExportFlags.GetterFound | UhtPropertyExportFlags.GetterSpecified)} is skipped due to special handling.");
                }
                if (prop.IsBitfield)
                {
                    // TODO: support bitfield properties by generating appropriate wrapper functions.
                }
                else if (prop.PropertyFlags.HasAnyFlags(EPropertyFlags.NativeAccessSpecifierProtected | EPropertyFlags.NativeAccessSpecifierPrivate))
                {
                    // Skip protected properties since they won't be accessible from the generated wrapper functions.
                }
                else
                {
                    string? asType = MapPropertyAsType(prop, registeredTypeNames);
                    if (asType == null)
                        continue;

                    sb.AppendLine($"    Result = Engine->RegisterObjectProperty(\"{cls.SourceName}\", \"{asType} {prop.SourceName}\",");
                    sb.AppendLine($"        asOFFSET({cls.SourceName}, {prop.SourceName}));");
                    sb.AppendLine("    check(Result >= 0);");
                    anyContent = true;
                }
            }

            if (!anyContent)
                sb.AppendLine("    (void)Engine; // nothing to register");

            sb.AppendLine("}");
            sb.AppendLine();
        }

        // -------------------------------------------------------------------------
        // Master header: includes all per-class headers + declares master functions.
        // -------------------------------------------------------------------------

        private static void WriteMasterHeader(StringBuilder sb, List<UhtClass> classesWithContent)
        {
            sb.AppendLine("#pragma once");
            sb.AppendLine();
            sb.AppendLine("#include \"angelscript.h\"");
            sb.AppendLine();

            foreach (var cls in classesWithContent)
                sb.AppendLine($"#include \"Bind_{cls.SourceName}.oas.gen.h\"");

            sb.AppendLine();
            sb.AppendLine("// Registers all auto-generated UClass reference types with the AngelScript engine.");
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
            List<UhtClass> classesWithContent)
        {
            sb.AppendLine("#include \"OtterAngelScriptBindings.gen.h\"");
            sb.AppendLine();

            // Forward-declare every collected UClass so that parameter / return-type
            // references work regardless of registration order.
            sb.AppendLine("void OAS_RegisterGeneratedTypes(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");
            foreach (var cls in allClasses)
            {
                sb.AppendLine($"    if (Engine->GetTypeInfoByName(\"{cls.SourceName}\") == nullptr)");
                sb.AppendLine("    {");
                sb.AppendLine($"        Result = Engine->RegisterObjectType(\"{cls.SourceName}\", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);");
                sb.AppendLine("        check(Result >= 0);");
                sb.AppendLine("    }");
            }
            sb.AppendLine("}");
            sb.AppendLine();

            sb.AppendLine("void Bind_Generated(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    check(Engine != nullptr);");
            sb.AppendLine("    OAS_RegisterGeneratedTypes(Engine);");
            foreach (var cls in classesWithContent)
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
                && !f.Deprecated && !f.GetDisplayNameText().Contains("Deprecated"));
                //.Where(f => !f.FunctionFlags.HasFlag(EFunctionFlags.EditorOnly)); // TODO: consider allowing editor-only functions if the plugin is used in an editor build configuration.
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
            if (returnProp != null && asRet == "") return false;

            var paramParts = new List<string>();
            foreach (var p in GetParamProperties(func))
            {
                string? asType = MapPropertyAsType(p, registeredTypeNames);
                if (asType == null) return false;
                paramParts.Add($"{asType} {p.SourceName}");
            }

            bool isConst = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Const);
            string constSuffix = isConst ? " const" : "";
            asSignature = $"{asRet} {func.SourceName}({string.Join(", ", paramParts)}){constSuffix}";
            return true;
        }

        private bool TryBuildAsMethodSignature(UhtFunction func, HashSet<string> registeredTypeNames, out string? asSignature)
        {
            asSignature = null;
            bool IsConstFunction = func.FunctionFlags.HasFlag(EFunctionFlags.Const);

            var returnProp = GetReturnProperty(func);
            string asRet = returnProp != null
                ? MapPropertyCppType(returnProp, false, registeredTypeNames) ?? ""
                : "void";
            if (returnProp != null)
                _factory.Session.LogInfo($"Function {func.GetDisplayNameText()} has an unmappable parameter {returnProp.SourceName} of type {returnProp.PropertyFlags.ToString()}. Skipping.");

            if (returnProp != null && asRet == "")
                return false;
            var paramParts = new List<string>();
            foreach (var p in GetParamProperties(func))
            {
                string? asType = MapPropertyCppType(p, true, registeredTypeNames);
                if (asType == null)
                    return false;
                paramParts.Add($"{asType}");
            }
            bool isConst = func.FunctionFlags.HasAnyFlags(EFunctionFlags.Const);
            asSignature = $"({string.Join(", ", paramParts)}){(IsConstFunction ? " const" : "")}, {asRet}";
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

                case UhtEnumProperty p:
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
        private static string? MapPropertyCppType(
            UhtProperty property,
            bool isParam,
            HashSet<string> registeredTypeNames)
        {
            bool isReturn = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm);
            bool isConst = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ConstParm);
            bool isRef = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReferenceParm);

            switch (property)
            {
                case UhtBoolProperty:
                    return isRef ? "bool&" : "bool";

                case UhtByteProperty { Enum: null }:
                    return isRef ? "uint8&" : "uint8";

                case UhtIntProperty:
                    return isRef ? "int32&" : "int32";

                case UhtInt64Property:
                    return isRef ? "int64&" : "int64";

                case UhtUInt32Property:
                    return isRef ? "uint32&" : "uint32";

                case UhtUInt64Property:
                    return isRef ? "uint64&" : "uint64";

                case UhtFloatProperty:
                    return isRef ? "float&" : "float";

                case UhtDoubleProperty:
                    return isRef ? "double&" : "double";

                case UhtStrProperty:
                    if (!isParam) return "FString";
                    return isRef ? "FString&" : $"{(isConst ? "const" : "")} FString{(isRef ? "&" : "")}";

                case UhtNameProperty:
                    if (!isParam) return "FName";
                    return isRef ? "FName&" : $"{(isConst ? "const" : "")} FName{(isRef ? "&" : "")}";
                case UhtTextProperty:
                    if (!isParam) return "FText";
                    return isRef ? "FText&" : $"{(isConst ? "const" : "")} FText{(isRef ? "&" : "")}";

                case UhtClassProperty uclass:
                    if (uclass.MetaClass == null)
                        return null; // unsupported: TSubclassOf without a specified base class
                    return $"TSubclassOf<class {uclass.MetaClass.SourceName}>";

                case UhtObjectProperty p when p.Class != null:
                    return $"{p.Class.SourceName}*";

                case UhtStructProperty p
                        when registeredTypeNames.Contains(p.ScriptStruct.SourceName):
                    if (!isParam) return p.ScriptStruct.SourceName;
                    return isRef
                        ? $"{p.ScriptStruct.SourceName}&"
                        //: $"const {p.ScriptStruct.SourceName}&";
                          : $"{(isConst ? "const " : "")}{p.ScriptStruct.SourceName}{(isRef ? "&" : "")}";
                case UhtEnumProperty p:
                    if (p.Enum.CppForm == UhtEnumCppForm.Namespaced)
                        return $"{p.Enum.SourceName}::Type";
                    else
                        return p.Enum.SourceName;

                default:
                    return null;
            }
        }
    }
}
