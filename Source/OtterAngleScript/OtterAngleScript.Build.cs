// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using Microsoft.Extensions.Logging;

public class OtterAngleScript : ModuleRules
{
	public OtterAngleScript(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
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

        // Add the plugin's Intermediate directory so that the .inl file produced by the
        // OtterAngleScriptUbtPlugin script generator can be found via
        //   #include "GeneratedAngelScriptBindings.inl"
        // The file is committed by UHT before the C++ compiler runs, so it is always
        // present when this module is compiled (after the first UHT pass).
        string GeneratedIncludePath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Intermediate"));
        PrivateIncludePaths.Add(GeneratedIncludePath);
    }
}
