# OtterAngleScript Copilot Instructions

## Build, test, and lint commands

- This repository does **not** define repo-local lint commands. Build and test work goes through Unreal tooling.
- The plugin is built through Unreal tooling, not through scripts checked into this repo.
- Typical local packaging/build command for a standalone plugin checkout:

```powershell
"<UE_ROOT>\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="<repo>\OtterAngleScript.uplugin" -Package="<output-dir>"
```

- If you are working from a host Unreal project, build that project target in Visual Studio or through Unreal Build Tool with this plugin enabled. This repo does not include a `.uproject`, so future sessions should expect to supply the engine path and host project path from the local environment.
- The editor-only `OtterAngleScriptTest` module contains CQTest coverage for the `FString` AngelScript binding.
- Run the full test group from a host project with:

```powershell
"<UE_ROOT>\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "<HostProject>.uproject" -ExecCmds="Automation RunTests OtterAngleScript.FString; Quit" -unattended -nop4 -nosplash -NullRHI -log
```

- **Single test command:**

```powershell
"<UE_ROOT>\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "<HostProject>.uproject" -ExecCmds="Automation RunTests OtterAngleScript.FString.BasicOperations; Quit" -unattended -nop4 -nosplash -NullRHI -log
```

## High-level architecture

- `OtterAngleScript.uplugin` declares the runtime module `OtterAngleScript` plus the editor-only test module `OtterAngleScriptTest`.
- `Source\OtterAngleScript\Private\OtterAngleScript.cpp` owns the plugin lifecycle. `StartupModule()` creates one AngelScript engine with `asCreateScriptEngine()`, installs the message callback that forwards AngelScript diagnostics into `LogOtterAngleScript`, registers Unreal bindings, and creates a single script module named `"OtterAngleScript"`.
- `Source\OtterAngleScript\Public\OtterAngleScript.h` exposes module-level access to the shared `asIScriptEngine*` and `asIScriptModule*`. The free `GetScriptEngine()` helper resolves the module through `FModuleManager`, so bindings and future runtime code are expected to work through the plugin module singleton rather than creating their own engine instances.
- `Source\OtterAngleScript\Private\Binding\Bind_FString.cpp` shows the intended extension point for exposing Unreal types to scripts. The plugin currently binds `FString` into AngelScript, including constructors/behaviors, operators, utility methods, a custom `FStringFactory` for script literals, and a couple of global helper functions.
- `Source\OtterAngleScript\OtterAngleScript.Build.cs` wires the runtime module to the vendored AngelScript and asbind20 code by adding `AngleScriptSDK` and `Projects` as public dependencies and by adding include paths under `Source\ThirdParty\sdk\angelscript` and `Source\ThirdParty\asbind20`.
- `Source\ThirdParty\sdk` and `Source\ThirdParty\asbind20` are vendored upstream code. Most feature work for this plugin should stay in `Source\OtterAngleScript` unless the task is explicitly about upgrading or patching those dependencies.

## Key conventions

- New script bindings should follow the existing `Bind_<Type>` pattern: declare the binder in `Source\OtterAngleScript\Private\Binding.h`, implement it in `Source\OtterAngleScript\Private\Binding\Bind_<Type>.cpp`, and call it from `StartupModule()` after the AngelScript engine is created.
- Bindings use the fluent `asbind20` registration style (`value_class`, `global`, `fp`, lambdas for overload adaptation) instead of raw AngelScript registration calls. Match that style when exposing additional Unreal APIs.
- Script-visible strings are represented as `FString`, not the standard AngelScript `string` add-on. `Bind_FString()` registers a custom string factory so script string literals produce `FString` instances; preserve that assumption when adding script APIs.
- The plugin currently assumes a single shared AngelScript engine and a single shared script module for the whole runtime module. Avoid introducing per-call or per-object engine creation unless the module architecture is intentionally being redesigned.
- AngelScript compiler/runtime messages are expected to surface through `LogOtterAngleScript`. Keep that logging path intact when changing initialization or adding compile/build steps around scripts.
