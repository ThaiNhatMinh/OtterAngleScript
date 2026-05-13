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
                "AngleScriptSDK",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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
    }
}
