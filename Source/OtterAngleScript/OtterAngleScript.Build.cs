// Copyright Epic Games, Inc. All Rights Reserved.

using EpicGames.Core;
using Microsoft.Extensions.Logging;
using System.IO;
using UnrealBuildTool;

public class OtterAngleScript : ModuleRules
{
	public OtterAngleScript(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		//DirectoryReference configDirectory = DirectoryReference.Combine(Unreal.EngineDirectory, "Programs/UnrealBuildTool");
		ConfigHierarchy ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, DirectoryReference.FromFile(Target.ProjectFile), Target.Platform);
		Logger.LogInformation("Configuring OtterAngleScript module for target {}", ini.FindSection("OtterAngelScript").KeyNames);
        //ini.GetArray("Plugins", "ScriptSupportedModules", out List<string>? supportedScriptModules);
        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "AngleScriptSDK",
                "AIModule",
                "InputCore",
                "Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"PhysicsCore",
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        string AngleScriptSdkPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/sdk/angelscript"));

        /* Link to Angelscript */
		Logger.LogInformation("Linking to AngelScript SDK at " + AngleScriptSdkPath);
        PublicIncludePaths.Add(Path.Combine(AngleScriptSdkPath, "include"));
        PublicIncludePaths.Add(Path.Combine(AngleScriptSdkPath, "source"));

        string AsbindSdkPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/asbind20/include"));
		Logger.LogInformation("Linking to asbind20 at " + AsbindSdkPath);
        PublicIncludePaths.Add(AsbindSdkPath);

        // Add the plugin's Intermediate directory so that the per-class headers and master
        // header produced by the OtterAngleScriptUbtPlugin script generator can be found via
        //   #include "GeneratedAngelScriptBindings.h"
        //   #include "Bind_<ClassName>.h"
        // The files are committed by UHT before the C++ compiler runs, so they are always
        // present when this module is compiled (after the first UHT pass).
        string GeneratedIncludePath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Intermediate"));
        PrivateIncludePaths.Add(GeneratedIncludePath);
    }
}
