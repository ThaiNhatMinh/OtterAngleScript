// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class AngleScriptSDK : ModuleRules
{
	public AngleScriptSDK(ReadOnlyTargetRules Target) : base(Target)
	{
        bEnforceIWYU = false;

		PublicDefinitions.Add("ANGELSCRIPT_EXPORT");

    }
}