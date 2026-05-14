using UnrealBuildTool;

public class OtterAngleScriptTest : ModuleRules
{
	public OtterAngleScriptTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CQTest",
				"Core",
				"CoreUObject",
				"Engine",
				"OtterAngleScript"
			}
		);
	}
}
