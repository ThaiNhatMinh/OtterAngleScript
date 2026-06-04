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
            "UObject", "UClass", "UPhysicalMaterial", "UInterface", "FLinearColor", "FColor",
            "FString", "FName", "FText",
            "FVector", "FVector2D", "FRotator", "FQuat",
            "FTransform", "FPlane", "FBox", "FBox2D",
            "FHitResult", "FTimerHandle",
            "FActorInstanceHandle", "FMatrix", "FKey", "FDateTime", "FInputChord", "FSoftObjectPath", "FSoftClassPath", "FPrimaryAssetId", "FInputEvent", "FKeyEvent", "FPointerEvent", "FPrimaryAssetType",
            "FPlatformUserId", "EPhysicalSurface", "FIntVector", "FInputDeviceId", "FGuid", "FIntPoint", "FIntVector", "FVector4", "FFrameRate", "FRandomStream", "EAxis", "FTimespan", "FVector3f", "FIntVector2", "FTopLevelAssetPath"
        };
        private static readonly HashSet<string> SkipStructs = new(StringComparer.Ordinal)
        {
            "FAudioBasedVibrationData", "FSoundClassProperties", "FSpecularProfileStruct", "FActorDesc", "FImportanceTexture", "FAnimationActiveTransitionEntry", "FFieldCookedMetaDataKey", "FEscalationState",
            "FInstancedSkinnedMeshComponentInstanceData", "FOverriddenPropertyNodeID", "FAttenuationSubmixSendSettings",
            // No copy constructor but there is no way to tell UHT that, so we skip it to avoid generation errors.
            "FTickFunction", "FNetworkPhysicsRewindDataProxy", "FStaticMeshSourceModel", "FSpatialHashStreamingGrid", "FSpatialHashStreamingGridLayerCell", "FSpatialHashStreamingGridLevel",
            // Cooked data structs
            "FAudioCookOutputs"
        };

        private static readonly HashSet<string> SkipClass = new(StringComparer.Ordinal)
        {
            "UKismetNodeHelperLibrary", "UAnimationDataModelNotifiesExtensions", "UBlueprintInstancedStructLibrary", "UDataTableFunctionLibrary", "UAnimationAttributeIdentifierExtensions", "UPhysicsThreadLibrary",
            "UCameraLensEffectInterfaceClassSupportLibrary", "UFieldNotificationLibrary", "UVisualLoggerKismetLibrary", "UWorldPartitionBlueprintLibrary", "UAudioParameterConversionStatics",
            "UMeshVertexPainterKismetLibrary", "UMaterialInstanceConstant", "UChaosBlueprintLibrary"
        };

        private static readonly HashSet<string> SkipHeaderPath = new(StringComparer.Ordinal)
        {
            "Tests", "Internal", "Private", "NoExportTypes.h", "EdGraph"
        };

        private static readonly HashSet<string> BuildModules = new(StringComparer.Ordinal)
        {
            "NetCore", "CoreUObject", "Engine", "AudioExtensions"
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
        private static readonly HashSet<string> SkipMethod = new HashSet<string>(StringComparer.Ordinal)
        {
            "UAnimMontage::IsValidAdditiveSlot",
            "UCameraLensEffectInterfaceClassSupportLibrary::IsInterfaceClassValid",
            "UKismetSystemLibrary::RaiseScriptError",
            "UKismetSystemLibrary::IsObjectOfSoftClass",
            "UMaterialInstanceConstant::K2_GetScalarParameterValue",
            "APlanarReflection::OnInterpToggle",
            "UPluginBlueprintLibrary::GetPluginDescriptorFilePath",
            "UPluginBlueprintLibrary::GetPluginBaseDir",
            "UPluginBlueprintLibrary::GetPluginMountedAssetPath",
            "UPluginBlueprintLibrary::GetPluginVersion",
            "UPluginBlueprintLibrary::GetPluginVersionName",
            "UPluginBlueprintLibrary::GetPluginDescription",
            "UPluginBlueprintLibrary::GetPluginEditorCustomVirtualPath",
            "UPluginBlueprintLibrary::IsPluginMounted",
            "UPoseAsset::RenamePose",
            "UPoseAsset::GetBasePoseName",
            "UPoseAsset::SetBasePoseName",
            "ASceneCapture2D::OnInterpToggle",
            "UTexture::SetVirtualTextureStreaming",
            "UTexture::Blueprint_GetMemorySize",
            "UTexture::Blueprint_GetTextureSourceDiskAndMemorySize",
            "UTexture::Blueprint_GetTextureSourceIdString",
            "UTexture2D::Blueprint_GetSizeX",
            "UTexture2D::Blueprint_GetSizeY",
            "UVisualLoggerKismetLibrary::EnableRecording",
            "UGameplayStatics::BlueprintSuggestProjectileVelocity",
            "UParticleSystem::ContainsEmitterType",
            "UPhysicsAsset::GetConstraintByName",
            "UPhysicsAsset::GetConstraintByBoneNames",
            "UPhysicsAsset::GetConstraints",
            "UPhysicsConstraintComponent::IsProjectionEnabled",
            "UPhysicsObjectBlueprintLibrary::GetPhysicsObjectWorldTransform",
            "UPluginBlueprintLibrary::GetAdditionalPluginSearchPaths",
            "UPluginBlueprintLibrary::GetAdditionalProjectPluginSearchPaths",
            "UPluginBlueprintLibrary::GetEnabledPluginNames",
            "UPoseAsset::GetPoseNames",
            "UTexture::ComputeTextureSourceChannelMinMax",
            "UTexture::Blueprint_GetBuiltTextureSize",
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
                .Where(m => m.Module.ModuleType == UHTModuleType.EngineRuntime)
                .Where(m => BuildModules.Contains(m.ShortName))
                .Select(m => m.ScriptPackage);
#else
            var packages = _factory.Session.Packages
                .Where(p => p.Module.ModuleType == UHTModuleType.EngineRuntime
                         || p.Module.ModuleType == UHTModuleType.GameRuntime);
#endif
            var GamePackages = _factory.Session.Modules
                .Where(m => m.Module.ModuleType == UHTModuleType.GameEditor
                            || m.Module.ModuleType == UHTModuleType.GameThirdParty
                            || m.Module.ModuleType == UHTModuleType.GameRuntime)
                .Select(m => m.ScriptPackage);

            var EditorPackages = _factory.Session.Modules
                .Where(m => m.Module.ModuleType == UHTModuleType.GameEditor)
                .Select(m => m.ScriptPackage);

            var packageList = packages.Concat(EditorPackages).Concat(GamePackages).Distinct().ToList();
            foreach ( var package in packageList )
            {

                _factory.Session.LogInfo($"OAS: found package {package.FullName}");
            }

            // Collect all UClasses excluding manually-bound types and interfaces.
            allClasses = packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtClass>()
                .Where(c => !c.ClassFlags.HasAnyFlags(EClassFlags.Interface))
                .Where(c => !ManuallyBoundTypes.Contains(c.SourceName))
                .Where(c => !SkipHeaderPath.Any(skip => c.HeaderFile.FilePath.Contains(skip) && c.Module.Module.ModuleType == UHTModuleType.EngineRuntime))
                .Where(c => !SkipClass.Contains(c.SourceName))
                .Where(c => !c.Deprecated)
                .ToList();

            //foreach (var c in allClasses)
            //{
            //    _factory.Session.LogInfo($"OAS: found class {c.FullName}");
            //}

            foreach (var ustruct in packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtScriptStruct>())
            {
                var result = ustruct.MetaData.ContainsKey("BlueprintType");
                var result1 = !ManuallyBoundTypes.Contains(ustruct.SourceName);
                var result2 = !SkipHeaderPath.Any(skip => ustruct.HeaderFile.FilePath.Contains(skip));
                var result6 = !ustruct.SourceName.Contains("Deprecated");
                var result7 = !SkipStructs.Contains(ustruct.SourceName);
                var result8 = !ustruct.Deprecated;
                var result9 = ustruct.Super != null ? ustruct.Super.SourceName != "FAnimNode_Base" : true;
                var result10 = !ustruct.ScriptStructFlags.HasFlag(EStructFlags.NoExport);
                //_factory.Session.LogInfo($"OAS: found struct {ustruct.SourceName} with {ustruct.ScriptStructExportFlags} {ustruct.FieldExportFlags}");

                //_factory.Session.LogInfo($"OAS: found struct {ustruct.SourceName} with {result} {result1}, {result2}, {result6}, {result7}, {result8}, {result9}, {result10} {HasScriptExposedStructContent(ustruct)}");
            }

            // Collect all BlueprintType USTRUCTs excluding manually-bound types.
            allStructs = packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtScriptStruct>()
                .Where(s => !s.ScriptStructFlags.HasFlag(EStructFlags.NoExport))
                .Where(s => s.MetaData.ContainsKey("BlueprintType"))
                .Where(s => !ManuallyBoundTypes.Contains(s.SourceName))
                .Where(s => !s.HeaderFile.FilePath.Contains("Tests"))
                .Where(s => !SkipHeaderPath.Any(skip => s.HeaderFile.FilePath.Contains(skip)))
                .Where(s => !s.SourceName.Contains("Deprecated"))
                .Where(s => !SkipStructs.Contains(s.SourceName))
                .Where(s => !s.Deprecated)
                .Where(s => s.Super != null ? s.Super.SourceName != "FAnimNode_Base" : true)
                .Where(s => !s.SourceName.StartsWith("FAnimNode_"))
                .ToList();

            // Collect all BlueprintType UENUMs excluding manually-bound types.
            allEnums = packageList
                .SelectMany(p => TraverseTree(p))
                .OfType<UhtEnum>()
                //.Where(e => e.MetaData.ContainsKey("BlueprintType"))
                .Where(e => e.UnderlyingType != UhtEnumUnderlyingType.Int64 && e.UnderlyingType != UhtEnumUnderlyingType.Uint64 && e.UnderlyingType != UhtEnumUnderlyingType.Uint32)
                .Where(e => !ManuallyBoundTypes.Contains(e.SourceName))
                .Where(e => !SkipHeaderPath.Any(skip => e.HeaderFile.FilePath.Contains(skip)))
                .Where(e => !e.SourceName.Contains("Deprecated"))
                .Where(e => !e.Deprecated)
                .ToList();

            if (allClasses.Count == 0 && allStructs.Count == 0 && allEnums.Count == 0)
                return;

            // Only structs that have at least one script-exposed property get registered.
            var allStructsWithContent = allStructs.Where(s => IsStructCanGenerate(s, allStructs)).ToList();
            //foreach (var s in allStructs)
            //{
            //    var result = HasScriptExposedStructContent(s);
            //    //_factory.Session.LogInfo($"OAS: struct {s.SourceName} has script-exposed content: {result}");
            //}
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
            allGroups = allClasses.Where(HasScriptExposedContent).Cast<UhtType>()
                .Concat(allStructsWithContent.Cast<UhtType>())
                .Concat(allEnums.Cast<UhtType>())
                .GroupBy(t => t.HeaderFile.FilePath)
                .Select(g => new HeaderFileGroup(
                    Modules: g.Select(t => t.Module).Distinct().ToList(),
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
                UhtModule? SelectModule = null;
                foreach (var module in group.Modules)
                {
                    if (SelectModule == null)
                        SelectModule = module;
                    else if (SelectModule != module)
                    {
                        _factory.Session.LogWarning($"OAS: multiple modules found for header {group.IncludePath} - selected {SelectModule.ShortName} arbitrarily for logging purposes.");
                        break;
                    }
                }
                if (SelectModule == null)
                {
                    _factory.Session.LogError($"OAS: no module found for header {group.IncludePath} - skipping generation for this header.");
                    continue;
                }
                string OutputDir;
                if (SelectModule.Module.ModuleType == UHTModuleType.EngineRuntime)
                {
                    OutputDir = _factory.PluginModule!.OutputDirectory;
                }
                else if (SelectModule.Module.ModuleType == UHTModuleType.GameRuntime || SelectModule.Module.ModuleType == UHTModuleType.GameEditor)
                {
                    OutputDir = SelectModule.Module.OutputDirectory;
                }
                else
                {
                    _factory.Session.LogError($"OAS: unsupported module type {SelectModule.Module.ModuleType} for header {group.IncludePath} - skipping generation for this header.");
                    continue;
                }

                var hdr = new StringBuilder();
                WriteAutoGenNotice(hdr);
                WritePerFileHeader(hdr, group);
                var GenPath = _factory.MakePath($"Bind_{group.FileStem}", ".oas.gen.h");
                //var GenPath = Path.Combine(OutputDir, $"Bind_{group.FileStem}.oas.gen.h");
                _factory.Session.LogInfo($"OAS: generating {GenPath}");
                _factory.CommitOutput(GenPath, hdr);

                var src = new StringBuilder();
                WriteAutoGenNotice(src);
                WritePerFileSource(src, group, registeredTypeNames);
                var SrcPath = _factory.MakePath($"Bind_{group.FileStem}", ".oas.gen.cpp");
                //var SrcPath = Path.Combine(OutputDir, $"Bind_{group.FileStem}.oas.gen.cpp");
                _factory.Session.LogInfo($"OAS: generating {SrcPath}");
                _factory.CommitOutput(SrcPath, src);
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
            {
                sb.AppendLine($"void OAS_Register_Declare{s.SourceName}(asIScriptEngine* Engine);");
                sb.AppendLine($"void OAS_Register_{s.SourceName}(asIScriptEngine* Engine);");
            }
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
            //sb.AppendLine($"#include \"OtterAngleScript.h\"");
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
            //sb.AppendLine("extern FSimpleMulticastDelegate OnRegisterGeneratedBindings;");

            foreach (var cls in group.Classes)
                WriteClassRegistrationFunction(sb, cls, registeredTypeNames, group);

            foreach (var s in group.Structs)
            {

                WriteStructHelpers(sb, s);
                WriteStructRegistrationFunction(sb, s, registeredTypeNames, group);
            }

            foreach (var e in group.Enums)
                WriteEnumRegistrationFunction(sb, e);
        }

        private void WriteClassRegistrationFunction(
            StringBuilder sb,
            UhtClass cls,
            HashSet<string> registeredTypeNames,
            HeaderFileGroup group)
        {
            sb.AppendLine($"void OAS_RegisterMethods_{cls.SourceName}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            //sb.AppendLine($"    if (auto TypeInfo = Engine->GetTypeInfoByName(\"{cls.SourceName}\")) {{ TypeInfo->SetUserData({cls.SourceName}::StaticClass()); }}");
            sb.AppendLine($"    {{auto TypeInfo = Engine->GetTypeInfoByName(\"{cls.SourceName}\"); TypeInfo->SetUserData({cls.SourceName}::StaticClass()); }}");

            sb.AppendLine("    int Result = 0;");

            var staticFuncs = ScriptFunctions(cls).Where(f => f.FunctionFlags.HasFlag(EFunctionFlags.Static)).ToList();
            {
                sb.AppendLine("    // --------------STATIC FUNCTIONS--------------");
                sb.AppendLine($"    Result = Engine->SetDefaultNamespace(\"{cls.SourceName}\"); check(Result >= 0);");
                sb.AppendLine();
                sb.AppendLine($"    Result = Engine->RegisterGlobalFunction(\"UClass StaticClass()\", asFUNCTION({cls.SourceName}::StaticClass), asCALL_CDECL); check(Result >= 0);");
                foreach (var func in staticFuncs)
                {
                    if (IsSkipMethod(cls, func))
                        continue;
                    if (!TryBuildAsSignature(func, registeredTypeNames, out string? asSig))
                    {
                        //_factory.Session.LogInfo($"OAS: skipping static function {cls.SourceName}::{func.SourceName} {func.FunctionFlags} since its signature contains unsupported types.");
                        continue;
                    }
                    if (!TryBuildAsMethodSignature(cls, func, registeredTypeNames, out string? asMethodSig))
                        continue;
                    sb.AppendLine($"    Result = Engine->RegisterGlobalFunction(\"{asSig}\",");
                    sb.AppendLine($"        asFUNCTIONPR({cls.SourceName}::{func.SourceName}, {asMethodSig}), asCALL_CDECL);");
                    sb.AppendLine("    check(Result >= 0);");
                    sb.AppendLine();
                }
                sb.AppendLine($"    Result = Engine->SetDefaultNamespace(\"\"); check(Result >= 0);");
                sb.AppendLine();
            }
            foreach (var func in cls.Functions)
            {
                //_factory.Session.LogInfo($"OAS: {func.SourceName} {func.FunctionExportFlags}");
            }
            var nonStaticFuncs = ScriptFunctions(cls).Where(f => !f.FunctionFlags.HasFlag(EFunctionFlags.Static)).ToList();
            if (nonStaticFuncs.Count > 0)
            {
                sb.AppendLine("    // --------------NON-STATIC FUNCTIONS--------------");
                const string callConv = "asCALL_THISCALL";
                foreach (var func in nonStaticFuncs)
                {
                    if (IsSkipMethod(cls, func))
                        continue;

                    if (!TryBuildAsSignature(func, registeredTypeNames, out string? asSig))
                    {
                        _factory.Session.LogInfo($"OAS: skipping function {cls.SourceName}::{func.SourceName} {func.FunctionFlags} TryBuildAsSignature");
                        continue;
                    }
                    if (func.FunctionFlags.HasAnyFlags(EFunctionFlags.Protected | EFunctionFlags.Private))
                    {
                        _factory.Session.LogInfo($"OAS: skipping non-public function {cls.SourceName}::{func.SourceName} {func.FunctionFlags} since non-public functions are not public.");
                        continue; // TODO: support non-public functions via wrapper functions.
                    }
                    if (!TryBuildAsMethodSignature(cls, func, registeredTypeNames, out string? asMethodSig))
                    {
                        _factory.Session.LogInfo($"OAS: skipping function {cls.SourceName}::{func.SourceName} {func.FunctionFlags} TryBuildAsMethodSignature");
                        continue;
                    }

                    sb.AppendLine($"    Result = Engine->RegisterObjectMethod(\"{cls.SourceName}\", \"{asSig}\",");
                    sb.AppendLine($"        asMETHODPR({cls.SourceName}, {func.SourceName}, {asMethodSig}), {callConv});");
                    sb.AppendLine("    check(Result >= 0);");
                    sb.AppendLine();
                }
            }

            foreach (var prop in ScriptProperties(cls))
            {
                if (!IsPropertySupport(prop))
                    continue;

                if (prop.IsBitfield)
                    continue; // TODO: support bitfield properties via wrapper functions.
                if (prop.PropertyFlags.HasAnyFlags(EPropertyFlags.NativeAccessSpecifierProtected | EPropertyFlags.NativeAccessSpecifierPrivate))
                    continue; // TODO: support non-public properties via wrapper functions.

                string? asType = GetAsPropertyDeclare(prop);
                if (asType == null)
                    continue;

                if (asType.ToLower().Contains("fdeprecate")) // FDeprecate
                    continue;

                sb.AppendLine($"    Result = Engine->RegisterObjectProperty(\"{cls.SourceName}\", \"{asType} {prop.SourceName}\",");
                sb.AppendLine($"        asOFFSET({cls.SourceName}, {prop.SourceName}));");
                sb.AppendLine("    check(Result >= 0);");
            }

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
            HashSet<string> registeredTypeNames,
            HeaderFileGroup group)
        {
            string name = s.SourceName;

            sb.AppendLine($"void OAS_Register_Declare{name}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine($"    int Result = Engine->RegisterObjectType(\"{name}\", sizeof({name}), asOBJ_VALUE | asGetTypeTraits<{name}>()); check(Result >= 0);");
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
            sb.AppendLine("}");
            sb.AppendLine();

            sb.AppendLine($"void OAS_Register_{name}(asIScriptEngine* Engine)");
            sb.AppendLine("{");
            sb.AppendLine("    int Result = 0;");
            sb.AppendLine($"    Result = Engine->RegisterObjectMethod(\"{name}\", \"{name}& opAssign(const {name} &in)\",");
            sb.AppendLine($"        asFUNCTION({name}_Assign), asCALL_CDECL_OBJFIRST);");
            sb.AppendLine("    check(Result >= 0);");
            sb.AppendLine();

            foreach (var prop in ScriptStructProperties(s))
            {
                if (!IsPropertySupport(prop))
                    continue;

                if (prop.IsBitfield)
                    // TODO: support bitfield properties via wrapper functions.
                    continue;

                if (prop.PropertyFlags.HasAnyFlags(EPropertyFlags.NativeAccessSpecifierProtected | EPropertyFlags.NativeAccessSpecifierPrivate))
                    continue;

                string? asType = GetAsPropertyDeclare(prop);
                if (asType == null)
                    continue;

                if (asType.ToLower().Contains("fdeprecate")) // FDeprecate
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
                sb.AppendLine($"    Result = Engine->RegisterEnumValue(\"{asTypeName}\", \"{shortName}\", ({(e.UnderlyingType == UhtEnumUnderlyingType.Unspecified ? "uint8" : e.UnderlyingType.ToString().ToLower())}){value.Name}); check(Result >= 0);");
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
            {
                //if (group.Modules.First() != null && group.Modules.First().Module.ModuleType != UHTModuleType.EngineRuntime)
                //    continue; 
                sb.AppendLine($"#include \"Bind_{group.FileStem}.oas.gen.h\"");
            }
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

            sb.AppendLine();
            foreach (var s in allStructsWithContent)
            {
                sb.AppendLine($"    OAS_Register_Declare{s.SourceName}(Engine);");
            }
            sb.AppendLine();

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
                {
                    //if (group.Modules.First() != null && group.Modules.First().Module.ModuleType != UHTModuleType.EngineRuntime)
                    //    continue;
                    sb.AppendLine($"    OAS_RegisterMethods_{cls.SourceName}(Engine);");
                }
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

        private bool IsPropertySupport(UhtProperty prop)
        {
            if (prop is UhtStructProperty structProp && !allStructs.Contains(structProp.ScriptStruct))
            {
                return false; // skip properties of struct types that aren't being registered (since we won't be able to map the type in the signature).
            }
            else if (prop is UhtEnumProperty enumProperty && !allEnums.Contains(enumProperty.Enum))
            {
                return false; // skip properties of enum types that aren't being registered (since we won't be able to map the type in the signature).
            }
            else if (prop is UhtByteProperty byteProperty && byteProperty.Enum != null && !allEnums.Contains(byteProperty.Enum))
            {
                return false; // skip enum properties where the enum type isn't being registered (since we won't be able to map the type in the signature).
            }
            else if (prop is UhtObjectProperty objectProperty && !allClasses.Contains(objectProperty.Class))
            {
                return false; // skip properties of class types that aren't being registered (since we won't be able to map the type in the signature).
            }
            else if (prop is UhtArrayProperty arrayProperty)
            {
                return IsPropertySupport(arrayProperty.ValueProperty);
            }
            else if (prop is UhtMapProperty mapProperty)
            {
                return IsPropertySupport(mapProperty.KeyProperty) && IsPropertySupport(mapProperty.ValueProperty);
            }
            return true;
        }
        private static bool HasScriptExposedContent(UhtClass cls)
        {
            return true;
        }

        private static IEnumerable<UhtFunction> ScriptFunctions(UhtClass cls)
        {
            return cls.Functions.Where(f =>
                f.FunctionFlags.HasAnyFlags(EFunctionFlags.BlueprintCallable | EFunctionFlags.BlueprintPure)
                && !f.Deprecated && !f.GetDisplayNameText().Contains("Deprecated"))
                //.Where(f => !f.FunctionFlags.HasFlag(EFunctionFlags.EditorOnly)); // TODO: consider allowing editor-only functions if the plugin is used in an editor build configuration.
                //.Where(f => f.FunctionExportFlags.HasFlag(UhtFunctionExportFlags.RequiredAPI)) // Can not bind function that are not exported.
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

        private bool IsStructCanGenerate(UhtScriptStruct s, List<UhtScriptStruct> allStruct)
        {
            foreach (var child in s.Children)
            {
                if (child is UhtScriptStruct childstruct)
                {
                    if (!allStruct.Contains(childstruct))
                    {
                        _factory.Session.LogInfo($"OAS: skipping struct {s.SourceName} since it contains field {childstruct.SourceName} which is not being registered.");
                        return false;
                    }
                }
            }
            return true;
            //return HasScriptExposedStructContent(s);
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
                         //&& p.PropertyFlags.HasAnyFlags(EPropertyFlags.BlueprintVisible | EPropertyFlags.BlueprintReadOnly)
                         && !p.Deprecated && !p.FullName.Contains("_DEPRECATED"))
                .Where(p => !p.IsEditorOnlyProperty)
                .Where(p => !p.PropertyFlags.HasAnyFlags(EPropertyFlags.Protected | EPropertyFlags.NativeAccessSpecifierPrivate));
        }

        private record HeaderFileGroup(
            List<UhtModule> Modules,
            string FileStem,
            string IncludePath,
            List<UhtClass> Classes,
            List<UhtScriptStruct> Structs,
            List<UhtEnum> Enums);

        List<HeaderFileGroup> allGroups = new List<HeaderFileGroup>();
        List<UhtClass> allClasses = new List<UhtClass>();
        List<UhtEnum> allEnums = new List<UhtEnum>();
        List<UhtScriptStruct> allStructs = new List<UhtScriptStruct>();

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
                ? MapPropertyAsType(returnProp) ?? ""
                : "void";

            if (returnProp != null && asRet == "")
                return false;

            var paramParts = new List<string>();
            foreach (var p in GetParamProperties(func))
            {
                string? asType = MapPropertyAsType(p);
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
            if (Prop.SourceName == "AssetBaseClass")
            {
                //_factory.Session.LogInfo($"OAS: skipping property {Prop.FullName} {Prop.GetType()}");
            }
            if (Prop is UhtClassProperty classProperty)
            {
                if (classProperty.CppForm == UhtObjectCppForm.TObjectPtrClass)
                {
                    return "UClass";
                }
            }
            else if (Prop is UhtObjectProperty PropObj)
            {
                if (PropObj.CppForm == UhtObjectCppForm.TObjectPtrObject)
                {
                    return PropObj.Class.SourceName;
                }
            }
            else if (Prop is UhtEnumProperty PropEnum)
            {
                if (PropEnum.Enum.CppForm == UhtEnumCppForm.Namespaced)
                {
                    return PropEnum.Enum.SourceName;
                }
            }
            else if (Prop is UhtByteProperty ByteProp)
            {
                if (ByteProp.Enum != null)
                {
                    return ByteProp.Enum.SourceName;
                }
            }
            else if (Prop is UhtArrayProperty ArrayProp)
            {
                if (ArrayProp.ValueProperty is UhtObjectProperty PropObjArr)
                {
                    _factory.Session.LogInfo($"OAS: OutOverlappingActors {Prop.FullName} {PropObjArr.CppForm}.");
                    if (PropObjArr.CppForm == UhtObjectCppForm.TObjectPtrObject)
                    {
                        return $"TArray<{PropObjArr.Class.SourceName}>";
                    }
                }
                else if (ArrayProp.ValueProperty is UhtByteProperty ArrayByteProp)
                {
                    if (ArrayByteProp.Enum != null)
                    {
                        return $"TArray<{ArrayByteProp.Enum.SourceName}>";
                    }
                }
            }
            else if (Prop is UhtSetProperty setProperty)
            {
                if (setProperty.ValueProperty is UhtObjectProperty PropObjSet)
                {
                    if (PropObjSet.CppForm == UhtObjectCppForm.TObjectPtrObject)
                    {
                        return $"TSet<{PropObjSet.Class.SourceName}>";
                    }
                }
                else if (setProperty.ValueProperty is UhtByteProperty SetByteProp)
                {
                    if (SetByteProp.Enum != null)
                    {
                        return $"TSet<{SetByteProp.Enum.SourceName}>";
                    }
                }
            }

            StringBuilder propertySB = new StringBuilder();
            Prop.AppendText(propertySB, UhtPropertyTextType.Sparse);
            return propertySB.ToString();
        }

        /// <summary>
        /// Maps a UHT property to an AngelScript type declaration string.
        /// Returns null if the type is not supported (the function will be skipped).
        /// </summary>
        private string? MapPropertyAsType(UhtProperty property)
        {
            bool isReturn = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm);
            bool isConst = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ConstParm);
            bool isOut = !isConst && !isReturn && property.PropertyFlags.HasAnyFlags(EPropertyFlags.OutParm);
            bool isRef = property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReferenceParm);
            bool isParam = property.PropertyFlags.HasAnyFlags(EPropertyFlags.Parm);
            if (isParam && !isOut && !isReturn)
            {
                isConst = true;
                isRef = true;
            }

            Func<UhtProperty, string> GetType = (prop) =>
            {
                String declare = prop.SourceName;
                switch (prop)
                {
                    case UhtBoolProperty:
                        declare = "bool";
                        break;
                    case UhtByteProperty { Enum: null }:
                        declare = "uint8";
                        break;
                    case UhtIntProperty:
                        declare = "int";
                        break;
                    case UhtInt64Property:
                        declare = "int64";
                        break;
                    case UhtDoubleProperty:
                        declare = "double";
                        break;
                    case UhtFloatProperty:
                        declare = "float";
                        break;
                    case UhtStrProperty:
                        declare = "FString";
                        break;
                    case UhtNameProperty:
                        declare = "FName";
                        break;
                    case UhtTextProperty:
                        declare = "FText";
                        break;
                    case UhtClassProperty uclass:
                        if (uclass.MetaClass == null)
                        {
                            _factory.Session.LogInfo($"skipping property {uclass.FullName} since it is a TSubclassOf without a specified base class.");
                            return ""; // unsupported: TSubclassOf without a specified base class
                        }
                        declare = $"TSubclassOf<{uclass.MetaClass.SourceName}>";
                        break;
                    case UhtSoftClassProperty softClassProp:
                        if (softClassProp.Class == null)
                        {
                            _factory.Session.LogInfo($"skipping property {softClassProp.FullName} since it is a TSoftClassPtr without a specified base class.");
                            return ""; // unsupported: TSoftClassPtr without a specified base class
                        }
                        declare = $"TSoftClassPtr<{softClassProp.Class.SourceName}>";
                        break;
                    case UhtSoftObjectProperty softObjProp:
                        if (softObjProp.Class == null)
                        {
                            _factory.Session.LogInfo($"skipping property {softObjProp.FullName} since it is a TSoftObjectPtr without a specified class.");
                            return ""; // unsupported: TSoftObjectPtr without a specified class
                        }
                        declare = $"TSoftObjectPtr<{softObjProp.Class.SourceName}>";
                        break;
                    case UhtObjectProperty objProp:
                        if (objProp.Class == null)
                        {
                            _factory.Session.LogInfo($"skipping property {objProp.FullName} since it is an object property without a specified class.");
                            return ""; // unsupported: object property without a specified class
                        }
                        declare = $"{objProp.Class.SourceName}";
                        break;
                    case UhtEnumProperty enumProp:
                        if (enumProp.Enum == null)
                        {
                            _factory.Session.LogInfo($"skipping property {enumProp.FullName} since it is an enum property of an unsupported enum type (only namespaced enums are supported).");
                            return ""; // unsupported: non-namespaced enum
                        }
                        declare = enumProp.Enum.SourceName;
                        break;
                    case UhtByteProperty byteProp:
                        if (byteProp.Enum != null)
                        {
                            declare = byteProp.Enum.SourceName;
                        }
                        else
                        {
                            declare = "uint8";
                        }
                        break;
                    case UhtStructProperty structProp:
                        declare = structProp.ScriptStruct.SourceName;
                        break;
                }

                return declare;
            };

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
                    return isOut ? "FString &out" : $"{(isConst ? "const" : "")} FString{(isRef ? "&in" : "")}";

                case UhtNameProperty:
                    if (isReturn) return "FName";
                    return isOut ? "FName &out" : $"{(isConst ? "const" : "")} FName{(isRef ? "&in" : "")}";
                case UhtTextProperty:
                    if (isReturn) return "FText";
                    return isOut ? "FText &out" : $"{(isConst ? "const" : "")} FText{(isRef ? "&in" : "")}";
                case UhtClassProperty uclass:
                    if (uclass.MetaClass == null)
                    {
                        _factory.Session.LogInfo($"skipping property {uclass.FullName} since it is a TSubclassOf without a specified base class.");
                        return null; // unsupported: TSubclassOf without a specified base class
                    }
                    if (!allClasses.Contains(uclass.MetaClass) && !ManuallyBoundTypes.Contains(uclass.MetaClass.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {uclass.FullName} since it references class {uclass.MetaClass.SourceName} which is not in the current group.");
                        return null; // unsupported: TSubclassOf with a base class not in the current group
                    }
                    return $"TSubclassOf<{uclass.MetaClass.SourceName}>";

                case UhtObjectProperty p when p.Class != null:
                    if (!allClasses.Contains(p.Class) && !ManuallyBoundTypes.Contains(p.Class.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {p.FullName} since it references class {p.Class.SourceName} which is not in the current group.");
                        return null; // unsupported: object with a class not in the current group
                    }
                    return $"{p.Class.SourceName}";
                case UhtSoftObjectProperty p when p.Class != null:
                    if (!allClasses.Contains(p.Class) && !ManuallyBoundTypes.Contains(p.Class.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {p.FullName} since it references class {p.Class.SourceName} which is not in the current group.");
                        return null; // unsupported: object with a class not in the current group
                    }
                    return $"TSoftObjectPtr<{p.Class.SourceName}>";
                case UhtSoftClassProperty p when p.Class != null:
                    if (!allClasses.Contains(p.Class) && !ManuallyBoundTypes.Contains(p.Class.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {p.FullName} since it references class {p.Class.SourceName} which is not in the current group.");
                        return null; // unsupported: object with a class not in the current group
                    }
                    return $"TSoftClassPtr<{p.Class.SourceName}>";

                case UhtStructProperty p:
                    if (!allStructs.Contains(p.ScriptStruct) && !ManuallyBoundTypes.Contains(p.ScriptStruct.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {p.FullName} since it references struct {p.ScriptStruct.SourceName} which is not in the current group.");
                        return null; // unsupported: struct type not in the current group
                    }
                    if (isReturn)
                        return p.ScriptStruct.SourceName;
                    return isOut
                        ? $"{p.ScriptStruct.SourceName} &out"
                        : $"const {p.ScriptStruct.SourceName} &in";

                case UhtEnumProperty p:
                    if (!allEnums.Contains(p.Enum) && !ManuallyBoundTypes.Contains(p.Enum.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {p.FullName} since it references enum {p.Enum.SourceName} which is not in the current group.");
                        return null; // unsupported: enum type not in the current group
                    }
                    return p.Enum.SourceName;
                case UhtByteProperty p when p.Enum != null:
                    if (!allEnums.Contains(p.Enum) && !ManuallyBoundTypes.Contains(p.Enum.SourceName))
                    {
                        _factory.Session.LogInfo($"skipping property {p.FullName} since it references enum {p.Enum.SourceName} which is not in the current group.");
                        return null; // unsupported: enum property where the enum type is not in the current group
                    }
                    return p.Enum.SourceName;
                case UhtArrayProperty arrayProperty:
                    if (!IsPropertyBinding(arrayProperty.ValueProperty))
                    {
                        _factory.Session.LogInfo($"skipping property {arrayProperty.FullName} since its value property {arrayProperty.ValueProperty.FullName} is not supported.");
                        return null; // unsupported: array property where the value property type is not supported
                    }

                    var declare = $"TArray<{GetType(arrayProperty.ValueProperty)}>";

                    return $"{(isConst ? "const" : "")} {declare} {(isRef ? "&" : "")}";
                case UhtSetProperty setProperty:
                    if (!IsPropertyBinding(setProperty.ValueProperty))
                    {
                        _factory.Session.LogInfo($"skipping property {setProperty.FullName} since its value property {setProperty.ValueProperty.FullName} is not supported.");
                        return null; // unsupported: set property where the value property type is not supported
                    }
                    var setDeclare = $"TSet<{GetType(setProperty.ValueProperty)}>";
                    return isOut ? $"{setDeclare} &out" : $"{(isConst ? "const" : "")} {setDeclare}{(isRef ? "&in" : "")}";
                case UhtMapProperty mapProperty:
                    if (!IsPropertyBinding(mapProperty.KeyProperty))
                    {
                        _factory.Session.LogInfo($"skipping property {mapProperty.FullName} since its key property {mapProperty.KeyProperty.FullName} is not supported.");
                        return null; // unsupported: map property where the key property type is not supported
                    }
                    if (!IsPropertyBinding(mapProperty.ValueProperty))
                    {
                        _factory.Session.LogInfo($"skipping property {mapProperty.FullName} since its value property {mapProperty.ValueProperty.FullName} is not supported.");
                        return null; // unsupported: map property where the value property type is not supported
                    }

                    var mapDeclare = $"TMap<{GetType(mapProperty.KeyProperty)}, {GetType(mapProperty.ValueProperty)}>";
                    return isOut ? $"{mapDeclare} &out" : $"{(isConst ? "const" : "")} {mapDeclare}{(isRef ? "&in" : "")}";
                default:
                    _factory.Session.LogInfo($"skipping property {property.FullName} since its type {property.GetType()} is not supported.");
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
            if (property is UhtByteProperty byteProperty && byteProperty.Enum != null && byteProperty.Enum.CppForm != UhtEnumCppForm.EnumClass)
            {
                StringBuilder test = new StringBuilder();
                UhtEnumProperty.AppendEnumText(test, property, byteProperty.Enum, UhtPropertyTextType.GenericFunctionArgOrRetVal, false);

                property.AppendText(builder, UhtPropertyTextType.Generic);
            }
            else
            {
                property.AppendText(builder, UhtPropertyTextType.GenericFunctionArgOrRetVal);

            }

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

        bool IsSkipMethod(UhtClass cls, UhtFunction func)
        {
            return SkipMethod.Contains($"{cls.SourceName}::{func.SourceName}");
        }

        bool IsPropertyBinding(UhtProperty prop)
        {
            if (!ManuallyBoundTypes.Contains(prop.SourceName))
                return false;

            if (prop is UhtObjectPropertyBase objProp)
            {
                return allClasses.Contains(objProp.Class);
            }
            else if (prop is UhtStructProperty structProp)
            {
                return allStructs.Contains(structProp.ScriptStruct);
            }
            else if (prop is UhtEnumProperty enumProp)
            {
                return allEnums.Contains(enumProp.Enum);
            }
            return false;
        }
    }
}
