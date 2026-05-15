using System;
using System.Diagnostics;
using System.IO;
using UnrealBuildTool;
using Microsoft.Extensions.Logging;

public class AngleScriptSDK : ModuleRules
{
    private const string LibraryTargetName = "angelscript";

    public AngleScriptSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        IWYUSupport = IWYUSupport.None;

        string SdkRoot = Path.GetFullPath(Path.Combine(ModuleDirectory, ".."));
        string CmakeListsPath = Path.Combine(SdkRoot, "CMakeLists.txt");
        string BuildDirectory = Path.Combine(PluginDirectory, "Intermediate", "AngleScriptSDK", Target.Platform.ToString());
        string LibraryPath = Path.Combine(ModuleDirectory, "asm", GetLibraryFileName(Target));

        ExternalDependencies.Add(CmakeListsPath);

        EnsureAngelScriptLibraryBuilt(Target, SdkRoot, CmakeListsPath, BuildDirectory, LibraryPath);

        PublicDefinitions.Add("ANGELSCRIPT_EXPORT");
        PublicAdditionalLibraries.Add(LibraryPath);
    }

    private static string GetBuildConfiguration(ReadOnlyTargetRules Target)
    {
        return UsesDebugLibrary(Target) ? "Debug" : "Release";
    }

    private static string GetLibraryFileName(ReadOnlyTargetRules Target)
    {
        return UsesDebugLibrary(Target) ? "angelscriptd.lib" : "angelscript.lib";
    }

    private static bool UsesDebugLibrary(ReadOnlyTargetRules Target)
    {
        if (Target.Configuration == UnrealTargetConfiguration.DebugGame)
            return true;
        else if (Target.Configuration == UnrealTargetConfiguration.Debug)
            return true;
        else if (Target.Configuration == UnrealTargetConfiguration.Development)
            return true;
        else
            return false;
    }

    private void EnsureAngelScriptLibraryBuilt(
        ReadOnlyTargetRules Target,
        string SdkRoot,
        string cmakeListsPath,
        string BuildDirectory,
        string LibraryPath)
    {
        Logger.LogInformation("LibraryPath " + LibraryPath);
        if (!ShouldBuildLibrary(cmakeListsPath, LibraryPath))
        {
            Logger.LogInformation("AngelScript library is up to date, skipping build.");
            return;
        }

        Directory.CreateDirectory(BuildDirectory);

        string ConfigureArguments =
            $"-S {Quote(SdkRoot)} -B {Quote(BuildDirectory)} -DAS_DISABLE_INSTALL=ON -DBUILD_SHARED_LIBS=OFF";
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            ConfigureArguments += " -A x64";
        }
        // TODO - Consider adding support for other platforms as needed

        RunProcess("cmake", ConfigureArguments, SdkRoot);
        RunProcess(
            "cmake",
            $"--build {Quote(BuildDirectory)} --config {GetBuildConfiguration(Target)} --target {LibraryTargetName}",
            SdkRoot);
        if (!File.Exists(LibraryPath))
        {
            throw new BuildException($"AngelScript CMake build did not produce the expected library: {LibraryPath}");
        }
    }

    private bool ShouldBuildLibrary(string CmakeListsPath, string LibraryPath)
    {
        if (!File.Exists(LibraryPath))
        {
            return true;
        }
        return false;
    }

    private void RunProcess(string FileName, string Arguments, string WorkingDirectory)
    {
        Logger.LogInformation("Running " + FileName + " " + Arguments);

        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            FileName = FileName,
            Arguments = Arguments,
            WorkingDirectory = WorkingDirectory,
            CreateNoWindow = true,
            UseShellExecute = false,
            RedirectStandardOutput = true,
            RedirectStandardError = true
        };

        using (Process process = Process.Start(startInfo))
        {
            if (process == null)
            {
                throw new BuildException($"Failed to start process: {FileName}");
            }

            string standardOutput = process.StandardOutput.ReadToEnd();
            string standardError = process.StandardError.ReadToEnd();

            process.WaitForExit();

            if (!string.IsNullOrWhiteSpace(standardOutput))
            {
                Logger.LogInformation(standardOutput.TrimEnd());
            }

            if (!string.IsNullOrWhiteSpace(standardError))
            {
                Logger.LogInformation(standardError.TrimEnd());
            }

            if (process.ExitCode != 0)
            {
                throw new BuildException($"{FileName} exited with code {process.ExitCode}.");
            }
        }
    }

    private static string Quote(string value)
    {
        return "\"" + value + "\"";
    }
}
