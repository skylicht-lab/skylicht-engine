# USE_CRASHHANDLER: crash logs from a standalone Windows EXE

## Purpose

Enable `USE_CRASHHANDLER` to record a call stack when the application encounters an unhandled exception, including when a tester runs the `.exe` directly without Visual Studio. For example, when running a sample application from `SampleApp/Runtime`, a crash can produce `SampleApp-2026-1116-1215.txt`. This file helps trace function names, source files, and line numbers when matching symbols load successfully.

This guide describes the current implementation in [CApplication.cpp](../Projects/Skylicht/Client/CApplication.cpp) and [CCrashHandler.cpp](../Projects/Skylicht/Client/CrashHandler/CCrashHandler.cpp). `SampleApp.exe` is an illustrative application executable, not a required engine target. Replace it with your own sample executable. The DLL names below are actual Skylicht engine modules; include the modules required by your application.

## 1. Enable the engine crash handler

The engine's CMake configuration declares `BUILD_DEBUG_CRASHHANDLER`, which defaults to `OFF`. When this option is `ON` and the compiler is MSVC, the engine defines `USE_CRASHHANDLER`.

Example configuration from a workspace containing the `skylicht-engine` checkout:

```cmd
cmake -S skylicht-engine -B skylicht-engine/PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=ON -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF -DUSE_OPENMP=OFF -DBUILD_DEBUG_CRASHHANDLER=ON -DUSE_PRECISE_FLOATINGPOINT=ON
cmake --build skylicht-engine/PrjVisualStudio --target install --config Debug
```

These are reference commands for developers to run when a build is needed. Keep the other options consistent with the application consuming the SDK. Replace `Debug` with `Release` if both the engine and application use that configuration. With the Visual Studio generator, select the configuration through `--config`.

When staging a shared-engine build, copy the required DLLs and their matching linker PDBs together. A deployment step that copies only `*.dll` leaves out the symbols needed to trace engine frames.

Defining the macro only in the application cannot enable the handler inside a previously built `Client.dll`. Rebuild the engine with this option and use the resulting DLL.

The `CApplication` constructor calls:

```cpp
#ifdef USE_CRASHHANDLER
CCrashHandler::createGetInstance()->init();
#endif
```

`init()` registers `SetUnhandledExceptionFilter(dump)`, initializes DbgHelp through `SymInitialize`, and enables loading source line information. The `CApplication` destructor releases the handler, which calls `SymCleanup`. Applications using this lifecycle do not need to register the handler again.

## 2. Prepare EXE, DLL, and PDB files from the same build

**Always keep the PDB generated during the same build as each EXE or DLL being tested. Correct crash log symbol resolution requires that exact matching PDB for each module you need to trace.** Matching filenames, source revisions, architectures, or configurations alone are insufficient: a PDB from a later rebuild cannot replace the original, even if the source code has not changed. Preserve the complete EXE/DLL/PDB set together for every test build before rebuilding or distributing it.

Example runtime directory for a sample application using shared engine libraries, abbreviated:

```text
SampleApp/Runtime/
    SampleApp.exe
    SampleApp.pdb
    Client.dll
    Client.pdb
    Engine.dll
    Engine.pdb
    Irrlicht.dll
    Irrlicht.pdb
    System.dll
    System.pdb
    Components.dll
    Components.pdb
    UserInterface.dll
    UserInterface.pdb
    Audio.dll
    Audio.pdb
    ... other required engine DLLs, matching PDBs, and runtime assets
```

The exact module set depends on the enabled engine features. Placing each linker PDB beside its corresponding module is a straightforward setup for standalone testing. `SampleApp.pdb` resolves application frames, while `Client.pdb`, `Engine.pdb`, `Irrlicht.pdb`, and the other engine PDBs resolve frames in their respective DLLs. Compiler PDBs such as `vc*.pdb` do not replace EXE/DLL linker PDBs.

For example, after staging `Client.dll` from an engine Debug build, copy its matching PDB from that same build output. Run this from the workspace containing both directories:

```powershell
Copy-Item .\skylicht-engine\Bin\x64\Debug\Client.pdb .\SampleApp\Runtime\Client.pdb
```

Locate and copy linker PDBs for the remaining engine DLLs from the same build output. Do not pair Release PDBs with Debug DLLs or newly generated PDBs with older DLLs.

The current engine CMake configuration adds `/Zi` and `/DEBUG:FULL` for MSVC Release builds. Release builds can therefore produce symbols too; collect and copy their PDBs alongside the matching binaries. Optimization can inline functions and make call stacks or line information less complete than in Debug builds.

## 3. Run the EXE directly and collect crash logs

After staging the sample executable, matching symbols, required engine DLLs, and runtime assets, run in PowerShell from the workspace root:

```powershell
Set-Location .\SampleApp\Runtime
.\SampleApp.exe
```

Perform the steps that reproduce the crash. Run without an attached debugger to test the unhandled exception filter path. The working directory must allow file creation.

The handler uses relative paths, so output goes into the **current working directory**, which is not necessarily the EXE directory. The commands above place both runtime assets and crash logs in `SampleApp/Runtime`. For a shortcut, set `Start in` to this directory.

For example, a crash at 12:15 on November 16, 2026, with the application name `SampleApp`, produces filenames such as:

```text
SampleApp-2026-1116-1215.dmp
SampleApp-2026-1116-1215.txt
```

The format is `<AppName>-YYYY-MMDD-HHmm`, using the test machine's local time. The prefix comes from `getApplication()->getAppName()` rather than automatically using the EXE filename. Crashes from the same application within the same minute can overwrite each other because the handler uses `CREATE_ALWAYS`; copy the logs elsewhere after each reproduction.

- `.dmp`: a text file containing `CRASH HANDLER` and up to 64 call stack addresses written as decimal numbers.
- `.txt`: created during the crash when resolution is enabled, which defaults to `true`. Addresses are replaced with function names, with `File` and `Line` added when DbgHelp returns that information.

**This `.dmp` file is not a Windows minidump.** The implementation does not call `MiniDumpWriteDump`; do not treat it as a memory dump to open in Visual Studio or WinDbg.

## 4. Read the TXT file to trace the crash

Illustrative output structure, not an actual crash:

```text
CRASH HANDLER

140700000000000: Skylicht::SomeClass::someMethod(...)
    - File: C:\build\skylicht-engine\Projects\Skylicht\SomeClass.cpp
    - Line: 123
140700000000128: SampleApp::onUpdate(...)
    - File: C:\build\SampleApp\Source\SampleApp.cpp
    - Line: 456
```

Locate relevant application or engine frames near the failure, open the source revision matching the build, and inspect the call flow around the reported lines. Source paths may refer to the build machine; testers do not need the source code to collect logs.

The handler calls `CaptureStackBackTrace` inside the exception filter instead of unwinding from `exp->ContextRecord`. The first frames may therefore belong to the handler or Windows, and the stack is not guaranteed to identify the exact faulting instruction. The current log does not record the exception code, registers, or memory. Use the call stack as an investigative lead together with reproduction steps and the matching build.

When reporting a crash, collect the `.txt`, `.dmp`, timestamp, reproduction steps, build/revision, configuration, and architecture. Preserve the corresponding EXE/DLL/PDB set for developer investigation.

## 5. Additional APIs and resolution on a later run

`CCrashHandler::enableResolve(false)` disables creation of the `.txt` file during a crash; the `.dmp` file is still written. Resolution is enabled by default, so normal testing does not require this API call.

`CCrashHandler::resolveDumpFiles()` scans `./*.dmp` and writes files with the same base names and a `.log` extension. An application can call this method after the crash handler has been initialized. `CApplication` does not call it automatically.

A `.log` generated on a later run is not always reliable: the `.dmp` stores absolute addresses without module bases or context. ASLR can change module addresses between processes even with identical binaries and PDBs. Prefer the `.txt` resolved inside the crashing process; copying only the `.dmp` and PDBs to another machine does not guarantee successful resolution.

`OnCrashHandler` is an optional callback invoked after writing and resolving the log. It is not required for default crash log creation.

## 6. Troubleshoot missing logs or unresolved addresses

| Symptom | Check |
| --- | --- |
| Neither `.dmp` nor `.txt` appears | Confirm the engine was built with `BUILD_DEBUG_CRASHHANDLER=ON` using MSVC, the correct `Client.dll` is loaded, and the current working directory is writable. |
| Logs cannot be found in `SampleApp/Runtime` | Check the terminal or shortcut working directory and the application name used as the filename prefix. |
| A `.dmp` appears without a `.txt` | Check whether resolution was disabled, whether the handler completed writing/resolution or encountered another failure, and whether the directory is writable. |
| The `.txt` still contains many numeric addresses | Check for missing or mismatched PDBs, symbols that cannot be found, or frames without available symbols. |
| Function names appear without `File`/`Line` | Line information may be missing, symbols may be restricted, optimization may affect the result, or line resolution may have failed. The handler currently uses `SYMOPT_PUBLICS_ONLY`. |
| Running under a debugger does not produce the expected logs | Reproduce with the standalone EXE; the debugger may handle the exception before the filter. |

The handler works only after initialization and for exceptions that reach the unhandled filter. Killing the process, losing power, failing before the `CApplication` constructor, or crashing in a way that prevents the handler from continuing does not guarantee log creation. Missing logs do not prove that the application did not crash.
