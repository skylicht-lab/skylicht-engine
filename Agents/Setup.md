# Set up a standalone Skylicht application

This guide supplements [README.md](../README.md). It describes how to create an independent `NewGame` application using an installed Skylicht SDK, with separate application modules, platform entry points, assets, and optional editor integration.

The architecture is documented here so that creating a new application does not require access to any private game repository. All `NewGame` paths below describe the project to create, not an existing template directory to copy. Use public engine sources and the public integration example for reusable implementation details.

## Public references and future updates

- Engine repository: <https://github.com/skylicht-lab/skylicht-engine>
- Standalone integration example: <https://github.com/skylicht-lab/hello-skylicht>
- Example README: <https://github.com/skylicht-lab/hello-skylicht/blob/master/README.md>

A local checkout may be available at `C:\Projects\hello-skylicht`; this machine-specific path is optional and must not become a dependency of a generated project.

Before implementing a new project, inspect the current checkout or retrieve the public repository. Check its default branch rather than assuming the README link's branch always remains current. Review the following files when checking for updates:

| File in `hello-skylicht` | What to recheck |
| --- | --- |
| `README.md` | SDK install sequence and built-in asset requirements |
| `CMakeLists.txt` | SDK paths, runtime options, application registration |
| `SkylichtLibrary.cmake` | Export paths, target imports, static/shared selection |
| `SkylichtConfig.cmake`, `PlatformConfig.cmake` | Features and platform definitions |
| `Source/HelloWorld/CMakeLists.txt` | Native targets, platform sources, PCH, linking |
| `Source/HelloWorld/CMakeProject.cmake` | Additional source/include hooks |
| `Source/HelloWorld/Source/CHelloWorld.h`, `CHelloWorld.cpp` | Application interface, registration, initialization, rendering |
| `Source/Main/` | Shared headers and platform entry points |
| `.github/workflows/build.yml` | Engine-first build sequence and platform configurations |

Record the engine and example commit IDs used when generating a project. Compare updated examples with the pinned SDK revision before adopting changes. Do not automatically pull or update an existing engine checkout during normal application setup. Workflows show intended build configurations; they do not prove a new application has been tested.

The public example is a starting point, not a complete production game framework. Reimplement application-specific modules in the new repository. Do not reference, copy, or assume availability of private game code, assets, service configuration, or build scripts.

## 1. Choose the project layout

Examples use folder `new-game`, CMake project `NEW_GAME`, class `CNewGame`, and native target `NewGame`.

```text
workspace/
  skylicht-engine/                     Pinned engine checkout
    InstallLibs/                      Generated engine SDK
  new-game/                           Independent application CMake root
    CMakeLists.txt
    CMake/
      CMakeVisualStudioSourceGroup.cmake
      CMakeVisualStudioPCH.cmake
    SkylichtLibrary.cmake
    SkylichtConfig.cmake
    PlatformConfig.cmake
    Version.txt
    Build/                            New project's setup/staging scripts
    Assets/                           Editable assets when needed
      BuiltIn/
      Game/
    Source/
      Main/
        pch.h
        pch.cpp
        SkylichtEngine.h
        Platforms/
      NewGame/
        CMakeLists.txt
        CMakeProject.cmake            Optional additional-source hook
        Source/
          CNewGame.h
          CNewGame.cpp
      DTO/                            Optional application modules
      Configs/
      DAL/
      Business/
      CoreGameplay/
      View/
      Editor/                         Optional embedded editor
    Bin/
      BuiltIn.zip                     Required by the public Hello example
```

Start with the public example's common CMake files, required `Source/Main` files, and `Source/HelloWorld` target. Rename and adapt them as described below. Add modules and editable asset tooling as the new application needs them.

The engine's `Scripts/create_project.py` creates a sample inside the engine build. It does not generate the complete standalone SDK consumer described here. `Scripts/Template.h`, `Scripts/Template.cpp`, and `Samples/HelloWorld` provide additional lifecycle examples.

Preserve licenses when reusing public code. Do not copy build caches, generated projects, installed SDK binaries, signing keys, or unrelated sample assets into the application's tracked source tree. Required runtime archives may be staged or tracked according to the new repository's asset policy.

## 2. Install a compatible engine SDK

Build and install the engine before configuring the application. Headers, libraries, and built-in resources must match the selected engine revision. Match architecture, configuration, CRT, OpenMP, and optional engine features between the SDK and application.

For a minimal Windows x64 static-engine setup, run from `skylicht-engine/` after build authorization:

```cmd
cmake -S . -B PrjNewGameSDK -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON -DUSE_OPENMP=OFF
cmake --build PrjNewGameSDK --target install --config Debug
```

For a shared-engine setup, use a separate engine build directory and set `BUILD_SHARED_LIBS=ON`. Select shared imports in the application with `USE_SKYLICHT_SHARED_LIBS=ON`. Build/install Release separately when required.

On MSVC, `USE_MSVC_RUNTIME_LIBRARY_DLL=ON` selects `/MD` or `/MDd`; `OFF` selects `/MT` or `/MTd`. Keep both sides consistent. If choosing `/MT` with DLL modules, retain the engine's shared-heap allocation support and corresponding definitions; do not mix allocation conventions across DLL boundaries.

Expected installed target layouts include:

```text
InstallLibs/
  include/
  cmake/Windows/Static/x64/
  cmake/Windows/Shared/x64/
  cmake/Android/<ABI>/
  cmake/iOS/<PLATFORM>/
  bin/                                Platform-specific runtime libraries
```

Only layouts actually installed will exist. Inspect generated target files for exact library locations. Use separate build directories for incompatible platforms, ABIs, or static/shared settings.

## 3. Rename the public Hello application

Apply the following mapping to copied source/CMake files and relevant resource settings:

| Public example | New application |
| --- | --- |
| `project(HELLO_SKYLICHT)` | `project(NEW_GAME)` |
| `HELLO_SKYLICHT_SOURCE_DIR` | `NEW_GAME_SOURCE_DIR` |
| `HELLO_SKYLICHT_VERSION` | `NEW_GAME_VERSION`, if retaining that variable |
| `Source/HelloWorld` | `Source/NewGame` |
| `SampleHelloWorld` | `NewGame` |
| `CHelloWorld` | `CNewGame` |
| `CHelloWorld.h`, `CHelloWorld.cpp` | `CNewGame.h`, `CNewGame.cpp` |

Update application registration strings, includes, output HTML names, bundle/display names, platform resources, and identifiers as appropriate. Inspect matches rather than replacing unrelated sample names globally.

The public example stores application files under `Source/HelloWorld/Source/`, and its target globs `./Source/**.cpp`, `./Source/**.c`, and `./Source/**.h`. Keep the nested `Source/NewGame/Source/` layout or explicitly change both the source collection and include paths if flattening it.

The inspected public root registers both HelloWorld and Physics, plus desktop UnitTest targets. For a minimal `NewGame`, register only `Source/NewGame`; remove the Physics and UnitTest registrations unless intentionally retaining them. Inspect `CMakeProject.cmake`: it adds a `Source/SampleFW` include/source hook. Remove this hook if unused or supply the public helpers actually required.

If retaining the example's version reader, create a two-line `Version.txt`:

```text
0.0.1
1
```

The first line is the application version; the second is its numeric version code.

## 4. Configure application CMake

Use the public example's full root and target CMake files as the implementation base. Modernize configuration for the selected engine revision rather than copying old minimum versions unchanged. The engine inspected for this guide requires CMake 3.15; use a compatible minimum and C++17.

Set policy `CMP0091` to `NEW` before `project()` to use `CMAKE_MSVC_RUNTIME_LIBRARY` reliably. Keep source-group/PCH helper includes, platform definitions, Unicode/compiler settings, and optional feature imports required by retained code.

The following is a path/import fragment, not a complete root CMake file:

```cmake
# After project(NEW_GAME), before application targets.
set(SKYLICHT_ENGINE_INSTALL_DIR
	"${NEW_GAME_SOURCE_DIR}/../skylicht-engine/InstallLibs")
set(SKYLICHT_ENGINE_PROJECT_DIR "${SKYLICHT_ENGINE_INSTALL_DIR}/include")
set(SKYLICHT_ENGINE_BIN_DIR "${NEW_GAME_SOURCE_DIR}/Bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${NEW_GAME_SOURCE_DIR}/Bin")
set(SKYLICHT_EDITOR_SOURCE_DIR "${NEW_GAME_SOURCE_DIR}/Source")
set(SKYLICHT_EDITOR_ROOT_DIR "${NEW_GAME_SOURCE_DIR}")

set(USE_SKYLICHT_SHARED_LIBS OFF CACHE BOOL "Use shared engine libraries")
set(USE_OPENMP OFF CACHE BOOL "Use OpenMP")
set(BUILD_EDITOR_TOOL OFF)
include(SkylichtLibrary.cmake)

add_subdirectory(Source/NewGame)
```

Make the SDK path configurable if the new repository uses another layout. A relative sibling path is convenient, but not an engine requirement.

`BUILD_SHARED_LIBS` controls application module defaults. `USE_SKYLICHT_SHARED_LIBS` chooses installed engine exports in `SkylichtLibrary.cmake`. Setting only `BUILD_SHARED_LIBS=ON` does not select a shared engine. The public importer selects desktop exports by system/library type/architecture, Android exports by ABI, and iOS exports by platform.

Set `SkylichtConfig.cmake` switches to match installed targets. The public example enables many optional features; disabling an unused feature requires removing any corresponding application includes, code, and links. Do not disable a feature merely to hide a missing SDK export without checking usage.

In `Source/NewGame/CMakeLists.txt`, retain the appropriate platform branch from the public Hello target. Include common PCH, application sources, platform entry points and resources. Link `Skylicht::Client`, and optional `Skylicht::Imgui` or `Skylicht::SpineRuntimes` only when enabled and needed. Android's application target remains `SHARED` even when the engine and internal modules are static; retain its `log` linkage.

For application DLL modules, configure exported symbols (for example, the existing `WINDOWS_EXPORT_ALL_SYMBOLS` pattern), matching CRT/configuration, and runtime DLL staging. For multi-config generators, inspect the actual Debug/Release output directory rather than assuming `Bin` is the final executable directory.

## 5. Implement the initial lifecycle

The public `CHelloWorld.cpp` demonstrates a minimal 2D startup:

1. Provide one `installApplication(const std::vector<std::string>& argv)` per application target and register `CNewGame` through `getApplication()->registerAppEvent(...)`.
2. Mount `BuiltIn.zip` using `app->getBuiltInPath(...)` and `addFileArchive(...)`.
3. Initialize the bundled Segoe UI font and basic shaders.
4. Create `CScene`, a zone, an `OrthoUI` camera, a canvas, and a text element.
5. Call scene update in `onUpdate()` and `CGraphics2D::render(...)` in `onRender()`.
6. Release owned scene/font resources through the application lifecycle.

Keep event signatures from the selected public example/engine revision. Use `pch.h` first where the existing pattern requires it. Do not add account, ads, purchases, or persistence systems to the first startup milestone.

For 3D rendering, use the engine template or a small public sample to create a camera and render pipeline, then handle pipeline resize and release explicitly. The Hello application's empty resize/pause callbacks are placeholders, not a full production lifecycle implementation.

Audit common platform entry points for dependencies on optional helpers, SDKs, platform headers, and application identity. Compile exactly the required entry point for each target.

## 6. Grow into a modular game project

Create the following modules only as responsibilities appear:

| Module | Responsibility |
| --- | --- |
| `DTO` | Serializable application data |
| `Configs` | Config models and loaders |
| `DAL` | Local or remote persistence |
| `Business` | Services, authentication, background tasks |
| `CoreGameplay` | Gameplay systems, session state, scene coordination |
| `View` | Screens, popups, UI helpers |
| `NewGame` | Application registration and lifecycle |
| `NewGameTest` | Optional separate debug application |

Each module needs its own source/include setup and CMake target. Establish real `target_link_libraries` dependencies; directory ordering alone does not link modules. A useful registration order is DTO, Configs, DAL, Business, CoreGameplay, View, then application targets. Keep dependencies acyclic and adjust this order to actual dependencies.

Use C++17, tabs, existing engine singleton conventions, dependency-ordered service initialization, and reverse shutdown. Register service creation and release explicitly in the owning module lifecycle.

When using the engine's DLL-based JSON library, keep allocations and destruction inside `json_lib` through its factories/destructors. Use `asCString()` and copy into application-owned strings rather than passing DLL-owned strings across the boundary.

These module names describe an architecture to implement. They are not supplied by the public Hello application, and no private source tree is required to create them.

## 7. Stage resources and choose an asset mode

The public Hello application runs from its configured runtime output directory and loads `Bin/BuiltIn.zip`. Its README explicitly says to update this archive from the engine. Stage a compatible archive from the selected engine revision before launch; a successful native link does not verify shaders or fonts.

For a minimal Hello-derived project, keep archive loading and a runtime working directory containing the archive. Setting an arbitrary `LOAD_BUNDLES` option does not change the public Hello implementation: implement that switch in both CMake and runtime code if adding it.

For a larger project, the following modes are useful:

| Mode | Working directory | Required implementation |
| --- | --- | --- |
| Loose assets | `Assets/` | Direct logical resource paths, with compatible built-in loading |
| Bundles | Runtime output directory | Staged archives mounted before first use |

Set `VS_DEBUGGER_WORKING_DIRECTORY` to the selected location. Distinguish executable output directory from runtime working directory. For packaged applications, configure resource lookup using the platform's bundle/assets conventions.

If adopting custom bundle tooling, use the engine's public asset scripts and their helper dependencies. Define the new project's package list, compression formats, output folders, and runtime mount paths. Use `.zip` archives consistently in generation, staging, and runtime mounting. Asset rebuilds require authorization.

For a shared-engine setup, stage the required SDK DLLs beside the actual executable or use a deliberate loader search path. Inspect platform/architecture subdirectories: `InstallLibs/bin/*.dll` may not match nested runtime files.

If audio loads asynchronously from packaged files, review archive-reader concurrency. Separate logical archives inside one APK can still share underlying file handles. Where needed, copy the audio archive to application storage and mount it through a separate handle before initializing sound. Keep this optional behavior documented in the new project's own implementation.

## 8. Optional embedded editor

The public root CMake describes copying engine `Projects/Editor` into application `Source/Editor`, then replacing the copied `CMakeLists.txt` with `CShared.txt`. Inspect the selected revision's editor integration before adopting it.

Configure editor source/root paths, imported `EditorGUI` targets, `Assets/BuiltIn`, `Assets/SampleGUI`, and runtime libraries together. Verify the new project's editor can resolve its own assets and application components.

Keep `BUILD_EDITOR_TOOL=OFF` while no editor source is present. When staging editor files, preserve existing destination changes and avoid destructive replacement scripts.

## 9. Add other platforms explicitly

Start with one desktop target. The public Hello CMake and workflow contain platform examples; audit each branch for standalone paths and current SDK requirements before using it.

### Android: copy the public host into Source/Android

Copy the public engine host from `skylicht-engine/Projects/Android` into `new-game/Source/Android`. Keep the Gradle wrapper (`gradlew`, `gradlew.bat`, `gradle/wrapper`), root Gradle files, `settings.gradle`, `app`, and `appdata` if using asset delivery. Do not copy `.idea`, `.gradle`, `local.properties`, generated `build` directories, existing `jniLibs` binaries, or signing credentials. Copy into a new destination; merge deliberately if it already exists.

For a clean engine checkout and an empty destination, this PowerShell example runs from `new-game/`:

```powershell
$androidTemplate = '../skylicht-engine/Projects/Android'
$androidDestination = 'Source/Android'
if (Test-Path -LiteralPath $androidDestination) {
    throw 'Source/Android already exists; inspect and merge it explicitly.'
}
New-Item -ItemType Directory -Path $androidDestination -Force | Out-Null
$androidItems = @('gradlew', 'gradlew.bat', 'gradle', 'build.gradle',
    'gradle.properties', 'settings.gradle', '.gitignore', 'app', 'appdata')
foreach ($androidItem in $androidItems) {
    Copy-Item -LiteralPath (Join-Path $androidTemplate $androidItem) -Destination $androidDestination -Recurse
}
```

The directory entries above must be clean source directories. If the engine host has already been built locally, copy only their source/configuration files and Gradle wrapper rather than carrying generated output across.

Customize the copied host as follows:

| File/setting | NewGame customization |
| --- | --- |
| `settings.gradle` | `rootProject.name = "NewGame"`; keep `:app`; retain `:appdata` only when used |
| `app/build.gradle` | New `applicationId`, version name/code, installed SDK/NDK/CMake versions, ABI list, native CMake wiring |
| `app/src/main/res/values/strings.xml` | Set `app_name` to `NewGame`; remove or replace optional service IDs |
| `app/src/main/AndroidManifest.xml` | App label, icons, orientation, activity, required permissions and optional service metadata |
| `app/src/main/java/com/skylicht/engine3d/NativeInterface.java` | Replace `System.loadLibrary("SampleGUI")` with `System.loadLibrary("NewGame")` |
| `app/src/main/res/mipmap*` | New application's launcher icons |
| `app/proguard-rules.pro` | Keep Java classes/methods accessed through JNI when shrinking |
| `local.properties` | Local Android SDK path; keep it untracked |

The library name is case-sensitive and omits `lib` and `.so`: `NewGame` loads `libNewGame.so`. It must agree with the Android `add_library(NewGame SHARED ...)` target.

Initially retain Java package/Gradle namespace `com.skylicht.engine3d` while changing only the application ID to an example such as `com.example.newgame`. These are different settings. If renaming the Java namespace, update package declarations, directory layout, generated `R`/binding imports, manifest activity names, and JNI mappings together. The engine's `Projects/Skylicht/Client/JavaClassDefined.h` defines `JNI_FUNCTION` and `JNI_CLASSNAME` for the original package. Changing only application bridge files may leave the installed Client library with old JNI class paths; rebuild/adapt the SDK consistently if changing that contract.

The copied engine host uses prebuilt native library staging by default. To build the application's native target from Gradle instead, add these fragments inside the copied `app/build.gradle`'s existing `android` block; merge with its existing `defaultConfig`, do not create duplicate blocks:

```groovy
android {
    defaultConfig {
        applicationId "com.example.newgame"
        ndk {
            abiFilters 'arm64-v8a'
        }
        externalNativeBuild {
            cmake {
                arguments '-DBUILD_SHARED_LIBS=OFF',
                          '-DUSE_SKYLICHT_SHARED_LIBS=OFF',
                          '-DUSE_OPENMP=OFF',
                          '-DBUILD_EDITOR_TOOL=OFF'
                targets 'NewGame'
            }
        }
    }
    externalNativeBuild {
        cmake {
            path file('../../../CMakeLists.txt')
            // Set version to the CMake installation selected for this project.
        }
    }
}
```

The CMake path is relative to `Source/Android/app`, so it reaches `new-game/CMakeLists.txt`. Set `ndkVersion` to the installed NDK used for the engine ABI builds. Match Gradle's min SDK to the SDK's native API assumptions. Add more ABIs only after installing matching engine exports. Do not carry machine-local NDK paths or release-only optimization flags into every build type.

An alternative is to build `NewGame` outside Gradle and stage `libNewGame.so` into `app/src/main/jniLibs/<ABI>/`, together with required shared dependencies. Choose one owner for native library packaging; avoid packaging the same library through both externalNativeBuild and prebuilt jniLibs.

Build/install the engine once per ABI before Gradle configures the app. Example from the engine root with `ANDROID_NDK_HOME` already pointing to the chosen NDK:

```cmd
cmake -S . -B PrjNewGameAndroid-arm64-v8a -G Ninja -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_HOME%/build/cmake/android.toolchain.cmake" -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-26 -DCMAKE_BUILD_TYPE=Release -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DUSE_OPENMP=OFF
cmake --build PrjNewGameAndroid-arm64-v8a --target install
```

`android-26` is an example matching the inspected host's minimum; choose the project's supported level explicitly. Ninja is single-config, so select configuration when configuring. Install a matching Debug SDK as well if the application's imported exports require it. Gradle's externalNativeBuild builds the application, not the sibling engine SDK.

For assets, choose one supported delivery flow:

- Asset pack: retain `include ':appdata'`, `assetPacks = [":appdata"]`, the `appdata` install-time pack, and the Java host's asset-pack lookup. Stage archives into `Source/Android/appdata/src/main/assets`.
- APK assets: stage archives into `Source/Android/app/src/main/assets`. Remove the asset-pack Gradle registration and adapt `FullscreenActivity` to initialize from APK assets without waiting for `appdata` delivery.

The inspected host requests an asset pack named `appdata`. Removing only the Gradle module leaves Java startup inconsistent. Stage `BuiltIn.zip` for the Hello-derived runtime, plus the new application's own archives and platform texture variants. Do not copy unrelated game package lists.

Review optional Play Games, billing, review, notification, and asset-delivery code/dependencies in the host. Remove unused integrations together with their Java calls, native hooks, manifest metadata, resources, and Gradle dependencies. Configure new service/signing values only when needed.

After authorization, run from `new-game/Source/Android`:

```cmd
gradlew.bat assembleDebug
```

On macOS/Linux use `./gradlew assembleDebug` with executable permission on the wrapper. Inspect the packaged ABI libraries and asset locations, then verify native startup and asset loading on a device/emulator. Signing and release packaging are separate steps.

### iOS: generate the Xcode project and CocoaPods workspace

Use public engine `Projects/Main/Platforms/IOS` or the public Hello project's equivalent as the source for `Source/Main/Platforms/IOS`. Keep the required Objective-C/Objective-C++ entry points, renderer, storyboards, `Assets.xcassets`, and `plist.in`. Add optional SDK bridge files only when implementing those services.

A clear setup sequence for new scripts under `Build/` is:

1. `iOS-Step01-BuildSkylicht.sh`: configure/build/install the pinned iOS engine SDK.
2. `iOS-Step02-Install.sh`: stage compatible built-in assets and the new project's generated runtime archives; copy SampleGUI only if using it.
3. `iOS-Step03-GenerateProject.sh`: generate the application `.xcodeproj`, then run CocoaPods if the application uses Pods.

Run commands below on macOS with the intended Xcode selected. From the engine root, an example device SDK build is:

```sh
cmake -S . -B PrjNewGameIOS -G Xcode \
  -DCMAKE_TOOLCHAIN_FILE=./ios.cmake -DPLATFORM=OS64 \
  -DDEPLOYMENT_TARGET=15.0 -DINSTALL_LIBS=ON \
  -DBUILD_SHARED_LIBS=OFF -DUSE_OPENMP=OFF
cmake --build PrjNewGameIOS --target install --config Release
```

From `new-game/`, generate the application:

```sh
cmake -S . -B PrjIOS -G Xcode \
  -DCMAKE_TOOLCHAIN_FILE=./ios.cmake -DPLATFORM=OS64 \
  -DDEPLOYMENT_TARGET=15.0 -DBUILD_SHARED_LIBS=OFF \
  -DUSE_SKYLICHT_SHARED_LIBS=OFF -DUSE_OPENMP=OFF \
  -DBUILD_EDITOR_TOOL=OFF
```

These commands assume the copied public toolchain and matching imports are configured. `15.0` is an example deployment target, not a universal requirement. Align the toolchain, app target, and Podfile deployment targets. For a simulator, choose a supported simulator `PLATFORM` from `ios.cmake` and install the matching engine/ANGLE slices into separate build directories; device libraries cannot substitute for simulator libraries.

#### Configure Source/Main/Platforms/IOS/plist.in

Keep this file as a tracked application template. CMake generates the final app Info.plist from it; edit the template and target properties instead of patching generated plist files in the build tree.

Wire these settings into the iOS application target after `add_executable(NewGame MACOSX_BUNDLE ...)`. The version variables below come from the new project's `Version.txt` reader:

```cmake
set_target_properties(NewGame PROPERTIES
	MACOSX_BUNDLE_INFO_PLIST "${NEW_GAME_SOURCE_DIR}/Source/Main/Platforms/IOS/plist.in"
	MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.newgame"
	MACOSX_BUNDLE_BUNDLE_NAME "NewGame"
	MACOSX_BUNDLE_EXECUTABLE_NAME "NewGame"
	MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION_CODE}"
	MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.example.newgame"
	XCODE_ATTRIBUTE_PRODUCT_NAME "NewGame"
	XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "15.0"
	XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
	XCODE_ATTRIBUTE_SKIP_INSTALL "NO"
	XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
)
```

CMake supplies the `MACOSX_BUNDLE_*` substitutions for its Info.plist template. If retaining custom `${PRODUCT_NAME}` and `${EXECUTABLE_NAME}` placeholders, explicitly define those CMake variables to `NewGame` before target generation; they must describe the actual bundle executable. A minimal template is:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleDisplayName</key><string>${MACOSX_BUNDLE_BUNDLE_NAME}</string>
  <key>CFBundleName</key><string>${MACOSX_BUNDLE_BUNDLE_NAME}</string>
  <key>CFBundleExecutable</key><string>${MACOSX_BUNDLE_EXECUTABLE_NAME}</string>
  <key>CFBundleIdentifier</key><string>${MACOSX_BUNDLE_GUI_IDENTIFIER}</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleVersion</key><string>${MACOSX_BUNDLE_BUNDLE_VERSION}</string>
  <key>CFBundleShortVersionString</key><string>${MACOSX_BUNDLE_SHORT_VERSION_STRING}</string>
  <key>UILaunchStoryboardName</key><string>LaunchScreen</string>
  <key>UIMainStoryboardFile</key><string>Main</string>
  <key>UISupportedInterfaceOrientations</key>
  <array>
    <string>UIInterfaceOrientationLandscapeLeft</string>
    <string>UIInterfaceOrientationLandscapeRight</string>
  </array>
</dict>
</plist>
```

Select supported orientations for the application. Add iPad-specific settings when needed. Add `Main.storyboard`, `LaunchScreen.storyboard`, `Assets.xcassets`, and the staged archives to the application resources using the public target's `RESOURCE`/`target_sources` pattern. A storyboard name in the plist does not copy that storyboard into the app. Match `.zip` resource globs to runtime archive names and stage assets before CMake collects them; regenerate after changing a globbed resource list.

Only add background modes, network transport exceptions, tracking descriptions, ad-network lists, URL schemes, Firebase configuration, or service IDs when required by implemented features. Use new application values; these are not generic engine configuration. Configure encryption declarations according to the application's actual use.

Set development team and signing in the new project's chosen Xcode/CMake configuration without copying another application's identity. Keep generated plist and signing changes reproducible through tracked templates/configuration.

#### Create the Podfile and open the workspace

CMake's Xcode generator creates `NEW_GAME.xcodeproj` from `project(NEW_GAME)`; it does not create the CocoaPods workspace. If the application requires Pods, create a template at `Source/Main/Platforms/IOS/podfile.in`:

```ruby
platform :ios, '15.0'
project 'NEW_GAME.xcodeproj'
workspace 'NewGame'
use_modular_headers!

target '@POD_TARGET_NAME@' do
  # Add only the Pods required by NewGame's implemented SDK integrations.
end
```

The Pod target name must match the native CMake target `NewGame`, not a display name. Retain `use_modular_headers!` only if compatible with the selected Pods. Specify dependencies and versions according to the new project's integrations; a Podfile with no dependencies is not needed for a basic engine-only application.

Generate the Podfile during the iOS CMake branch, independently of a Firebase flag:

```cmake
set(POD_TARGET_NAME "NewGame")
configure_file(
	"${NEW_GAME_SOURCE_DIR}/Source/Main/Platforms/IOS/podfile.in"
	"${CMAKE_BINARY_DIR}/Podfile"
	@ONLY
)
```

Using `CMAKE_BINARY_DIR` keeps the Podfile next to the generated `.xcodeproj` for any chosen build directory. After successful CMake generation, and with CocoaPods available and dependency installation authorized:

```sh
cd PrjIOS
pod install
open NewGame.xcworkspace
```

The explicit `workspace 'NewGame'` determines that workspace name. Without it, inspect CocoaPods' generated workspace name instead of assuming `NewGame.xcworkspace` exists. If there are no Pods, open `NEW_GAME.xcodeproj` directly; a workspace is not required by the engine itself.

For a Pods-enabled application, build/archive through the `.xcworkspace` so the app and Pods participate in the same build. Select the `NewGame` scheme, intended configuration, team, and device destination in Xcode. A command-line example after checking schemes with `xcodebuild -list -workspace NewGame.xcworkspace` is:

```sh
xcodebuild -workspace NewGame.xcworkspace -scheme NewGame \
  -configuration Release -destination 'generic/platform=iOS' \
  -archivePath NewGame.xcarchive archive
```

Signing/archive execution requires the owner's configured identity and authorization. Keep `Podfile.lock` according to the app repository's dependency policy. After CMake regeneration or Podfile changes, rerun `pod install` as needed, preserving the lockfile for reproducible dependency selection. Preserve inherited Xcode search/link settings when adding application overrides so CocoaPods xcconfig settings remain effective; retain `-ObjC` only where required by the linked SDKs.

For release archives, keep app bundles and matching dSYM output together in Xcode's configuration build location. Avoid forcing the iOS runtime output into the desktop `Bin` tree; remove such overrides in the iOS branch and verify the archive contains the app and matching symbols.

### macOS

Use the public platform sources, Hello target branch, and workflow as references. Match installed architecture, preserve required Cocoa/framework links and ANGLE dylib staging, set bundle resource locations, and configure runtime search paths for the selected renderer. Stage built-in resources before generating targets that collect resources through globs.

### Linux, SDL, Emscripten, and UWP

Retain only supported platform sources and required host dependencies. Inspect Emscripten preload paths and shell assets. Some sample UWP branches refer to engine source-root/generated sample paths; rewrite them to the standalone application's directories rather than assuming `SKYLICHT_ENGINE_SOURCE_DIR` exists in an SDK consumer.

Treat platform branches as implementation references, not proof the generated application builds or runs on those platforms.

## 10. Build scripts and verification

### Complete setup script templates

Create the following files in `new-game/Build/`. These are complete starter scripts documented here, not references to scripts in another repository. They use the sibling engine layout above, stop on failure, and never update Git or delete existing source. They assume the application CMake, public platform sources, and iOS templates have already been prepared as described in this guide.

Windows examples use PowerShell `.ps1` instead of requiring GNU `cp`/`rm` from `.cmd` files. iOS examples use Bash. Save shell scripts with LF and executable permission on macOS; preserve CRLF for Windows scripts. All archives below are `.zip`.

Build scripts perform builds only when explicitly invoked with authorization. Staging scripts copy already generated archives; asset rebuilding is a separate step. Prerequisites such as CMake, Ninja, Xcode, the NDK, and CocoaPods must already be available as needed.

#### Build/Win-Step01-BuildSkylicht.ps1

```powershell
param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateSet('Static', 'Shared')][string]$Linkage = 'Static'
)
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$engineRoot = (Resolve-Path (Join-Path $projectRoot '../skylicht-engine')).Path
$sdkBuild = Join-Path $engineRoot "PrjNewGameWindows-$Linkage"
$sharedFlag = if ($Linkage -eq 'Shared') { 'ON' } else { 'OFF' }
& cmake -S $engineRoot -B $sdkBuild -G 'Visual Studio 17 2022' -A x64 `
    -DINSTALL_LIBS=ON "-DBUILD_SHARED_LIBS=$sharedFlag" `
    -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON -DUSE_OPENMP=OFF
if ($LASTEXITCODE -ne 0) { throw 'Engine configuration failed.' }
& cmake --build $sdkBuild --target install --config $Configuration
if ($LASTEXITCODE -ne 0) { throw 'Engine installation failed.' }
```

Example from `new-game/`: `./Build/Win-Step01-BuildSkylicht.ps1 -Configuration Debug -Linkage Static`. Match the application's `USE_SKYLICHT_SHARED_LIBS` setting to the selected SDK.

#### Build/Win-Step02-Install.ps1

```powershell
param(
    [string]$BuiltInArchive = '',
    [string]$RuntimeDirectory = ''
)
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$engineRoot = (Resolve-Path (Join-Path $projectRoot '../skylicht-engine')).Path
if (-not $BuiltInArchive) { $BuiltInArchive = Join-Path $engineRoot 'Bin/BuiltIn.zip' }
if (-not $RuntimeDirectory) { $RuntimeDirectory = Join-Path $projectRoot 'Bin' }
if (-not (Test-Path -LiteralPath $BuiltInArchive -PathType Leaf)) {
    throw 'Build or obtain matching engine Bin/BuiltIn.zip before staging.'
}
New-Item -ItemType Directory -Path $RuntimeDirectory -Force | Out-Null
Copy-Item -LiteralPath $BuiltInArchive -Destination (Join-Path $RuntimeDirectory 'BuiltIn.zip') -Force
```

For a multi-config build, pass the actual working directory, for example `-RuntimeDirectory ./Bin/Debug`, if that is where the app resolves resources. Shared SDK users must also stage the matching configuration's runtime DLLs from paths recorded in installed target files; do not mix Debug and Release DLLs or flatten every platform directory into one folder.

This minimal script stages the built-in archive only. Editable built-in assets and embedded editor source are optional; when adding them, copy public `Assets/BuiltIn`, `Assets/SampleGUI`, and `Projects/Editor` into explicit application destinations and follow section 8. Do not overwrite existing edited source without reviewing it.

#### Build/Android-Step01-BuildSkylicht.ps1

```powershell
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('arm64-v8a', 'armeabi-v7a', 'x86', 'x86_64')][string]$Abi,
    [Parameter(Mandatory = $true)][string]$NdkRoot,
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
    [int]$ApiLevel = 26
)
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$engineRoot = (Resolve-Path (Join-Path $projectRoot '../skylicht-engine')).Path
$toolchain = Join-Path $NdkRoot 'build/cmake/android.toolchain.cmake'
if (-not (Test-Path -LiteralPath $toolchain -PathType Leaf)) {
    throw 'NdkRoot must point to an installed Android NDK.'
}
$sdkBuild = Join-Path $engineRoot "PrjNewGameAndroid-$Abi-$Configuration"
& cmake -S $engineRoot -B $sdkBuild -G Ninja `
    "-DCMAKE_TOOLCHAIN_FILE=$toolchain" "-DANDROID_ABI=$Abi" `
    "-DANDROID_PLATFORM=android-$ApiLevel" "-DCMAKE_BUILD_TYPE=$Configuration" `
    -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DUSE_OPENMP=OFF
if ($LASTEXITCODE -ne 0) { throw 'Android SDK configuration failed.' }
& cmake --build $sdkBuild --target install
if ($LASTEXITCODE -ne 0) { throw 'Android SDK installation failed.' }
```

Example: `./Build/Android-Step01-BuildSkylicht.ps1 -Abi arm64-v8a -NdkRoot $env:ANDROID_NDK_HOME`. Repeat for each Gradle ABI and required native configuration before invoking Gradle. Match the installed NDK and API assumptions to `app/build.gradle`.

#### Build/Android-Step02-Install.ps1

```powershell
param(
    [ValidateSet('AssetPack', 'Apk')][string]$Delivery = 'AssetPack',
    [string[]]$Packages = @('BuiltIn.zip')
)
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$runtimeRoot = Join-Path $projectRoot 'Bin'
$moduleName = if ($Delivery -eq 'AssetPack') { 'appdata' } else { 'app' }
$assetRoot = Join-Path $projectRoot "Source/Android/$moduleName/src/main/assets"
foreach ($package in $Packages) {
    if ([System.IO.Path]::GetFileName($package) -ne $package -or
        [System.IO.Path]::GetExtension($package) -ne '.zip') {
        throw 'Packages must contain plain .zip filenames.'
    }
    if (-not (Test-Path -LiteralPath (Join-Path $runtimeRoot $package) -PathType Leaf)) {
        throw "Missing runtime archive: $package"
    }
}
New-Item -ItemType Directory -Path $assetRoot -Force | Out-Null
foreach ($package in $Packages) {
    Copy-Item -LiteralPath (Join-Path $runtimeRoot $package) -Destination $assetRoot -Force
}
```

Stage `BuiltIn.zip` into application `Bin` first using the previous staging script, or provide it from the matching engine revision. Add only archives produced by the new project's public asset tooling, for example `-Packages BuiltIn.zip,Game.zip,GameETC.zip` if all three actually exist. The script intentionally leaves unlisted files untouched; review stale archives separately. Delivery must agree with the Gradle/Java startup flow in section 9.

#### Build/ConfigMac.sh

```bash
#!/usr/bin/env bash
# Shared configuration; sourced by the three iOS scripts below.
BUILD_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$BUILD_DIR/.." && pwd)"
ENGINE_ROOT="${ENGINE_ROOT:-$PROJECT_ROOT/../skylicht-engine}"
IOS_PLATFORM="${IOS_PLATFORM:-OS64}"
IOS_DEPLOYMENT_TARGET="${IOS_DEPLOYMENT_TARGET:-15.0}"
BUILD_CONFIGURATION="${BUILD_CONFIGURATION:-Release}"
ENGINE_BUILD_DIR="${ENGINE_BUILD_DIR:-$ENGINE_ROOT/PrjNewGameIOS-$IOS_PLATFORM}"
APP_BUILD_DIR="${APP_BUILD_DIR:-$PROJECT_ROOT/PrjIOS-$IOS_PLATFORM}"
USE_PODS="${USE_PODS:-OFF}"
# Set overridden paths to absolute paths; no signing secrets belong here.
```

These scripts use `PrjIOS-OS64` by default so device/simulator projects stay separate. Set `APP_BUILD_DIR` explicitly if choosing another location. CMake generates the Podfile into `CMAKE_BINARY_DIR`, so it follows the chosen directory automatically.

#### Build/iOS-Step01-BuildSkylicht.sh

```bash
#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/ConfigMac.sh"
cmake -S "$ENGINE_ROOT" -B "$ENGINE_BUILD_DIR" -G Xcode \
  -DCMAKE_TOOLCHAIN_FILE="$ENGINE_ROOT/ios.cmake" \
  -DPLATFORM="$IOS_PLATFORM" -DDEPLOYMENT_TARGET="$IOS_DEPLOYMENT_TARGET" \
  -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DUSE_OPENMP=OFF
cmake --build "$ENGINE_BUILD_DIR" --target install --config "$BUILD_CONFIGURATION"
```

For a simulator, set a supported toolchain platform such as `IOS_PLATFORM=SIMULATORARM64` consistently for engine and application invocations. Confirm that the selected revision has compatible simulator ANGLE dependencies before building.

#### Build/iOS-Step02-Install.sh

```bash
#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/ConfigMac.sh"
archive="${BUILTIN_ARCHIVE:-$ENGINE_ROOT/Bin/BuiltIn.zip}"
if [[ ! -f "$archive" ]]; then
  echo 'Missing compatible BuiltIn.zip; build or obtain it before staging.' >&2
  exit 1
fi
mkdir -p "$PROJECT_ROOT/Bin"
# Skip an exact-path copy when sourcing the already staged application archive.
if [[ "$archive" != "$PROJECT_ROOT/Bin/BuiltIn.zip" ]]; then
  cp "$archive" "$PROJECT_ROOT/Bin/BuiltIn.zip"
fi
# Additional application .zip archives must already exist in PROJECT_ROOT/Bin.
# The iOS target must include these files in its bundle resources (section 9).
```

The Hello-derived runtime mounts `BuiltIn.zip` from the app resources. Copying the archive to desktop `Bin` alone is insufficient unless the iOS CMake target registers it as a bundle resource. Stage before application generation, and regenerate after adding resource files collected by CMake globs.

#### Build/iOS-Step03-GenerateProject.sh

```bash
#!/usr/bin/env bash
set -euo pipefail
source "$(dirname -- "${BASH_SOURCE[0]}")/ConfigMac.sh"
case "$USE_PODS" in
  ON|OFF) ;;
  *) echo 'USE_PODS must be ON or OFF.' >&2; exit 1 ;;
esac
cmake -S "$PROJECT_ROOT" -B "$APP_BUILD_DIR" -G Xcode \
  -DCMAKE_TOOLCHAIN_FILE="$PROJECT_ROOT/ios.cmake" \
  -DPLATFORM="$IOS_PLATFORM" -DDEPLOYMENT_TARGET="$IOS_DEPLOYMENT_TARGET" \
  -DBUILD_SHARED_LIBS=OFF -DUSE_SKYLICHT_SHARED_LIBS=OFF \
  -DUSE_OPENMP=OFF -DBUILD_EDITOR_TOOL=OFF -DNEW_GAME_USE_PODS="$USE_PODS"
if [[ "$USE_PODS" == 'ON' ]]; then
  command -v pod >/dev/null || { echo 'CocoaPods is not installed.' >&2; exit 1; }
  [[ -f "$APP_BUILD_DIR/Podfile" ]] || { echo 'CMake did not generate Podfile.' >&2; exit 1; }
  (cd "$APP_BUILD_DIR" && pod install)
  [[ -d "$APP_BUILD_DIR/NewGame.xcworkspace" ]] || {
    echo 'Expected NewGame.xcworkspace; check the Podfile workspace setting.' >&2
    exit 1
  }
  echo "Open in Xcode: $APP_BUILD_DIR/NewGame.xcworkspace"
else
  echo "Open in Xcode: $APP_BUILD_DIR/NEW_GAME.xcodeproj"
fi
```

Add this application option to the root CMake, then guard the Podfile generation fragment from section 9:

```cmake
option(NEW_GAME_USE_PODS "Integrate NewGame iOS CocoaPods dependencies" OFF)
if (BUILD_IOS AND NEW_GAME_USE_PODS)
	set(POD_TARGET_NAME "NewGame")
	configure_file(
		"${NEW_GAME_SOURCE_DIR}/Source/Main/Platforms/IOS/podfile.in"
		"${CMAKE_BINARY_DIR}/Podfile"
		@ONLY
	)
endif()
```

Place the guard after platform detection has set `BUILD_IOS`. It replaces unconditional Podfile generation. The Podfile template in section 9 must declare `workspace 'NewGame'` and actual required dependencies when enabling Pods.

Example invocation sequence on macOS, from `new-game/`:

```bash
bash Build/iOS-Step01-BuildSkylicht.sh
bash Build/iOS-Step02-Install.sh
# Engine-only application:
bash Build/iOS-Step03-GenerateProject.sh
# Or, for a configured Pods-enabled application, after dependency authorization:
USE_PODS=ON bash Build/iOS-Step03-GenerateProject.sh
```

These scripts print the project/workspace to open instead of launching a GUI automatically. Run archive/signing steps separately using the workspace instructions in section 9. For Windows and Android, run the appropriate SDK build and staging scripts above before the application configure/build or Gradle command.

Shared numerical behavior such as `USE_PRECISE_FLOATINGPOINT` must use matching settings in engine builds, desktop generation, Android Gradle arguments, and iOS generation if enabled. It is optional for a basic Hello application.

### Verification

After build authorization, a Windows static-engine application build from `new-game/` is:

```cmd
cmake -S . -B PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=OFF -DUSE_SKYLICHT_SHARED_LIBS=OFF -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON -DUSE_OPENMP=OFF -DBUILD_EDITOR_TOOL=OFF
cmake --build PrjVisualStudio --config Debug --target NewGame
```

These commands assume the renamed application CMake and matching SDK are ready. For a shared engine, build/install that SDK variant first and set `USE_SKYLICHT_SHARED_LIBS=ON` for the application. Keep application module linkage as a separate decision.

Static verification must check retained includes, source files, imported targets, platform resources, and asset paths. Search copied files for stale `HELLO_SKYLICHT`, `SampleHelloWorld`, `CHelloWorld`, old package IDs, and machine-local paths.

Runtime verification covers resource/font/shader loading, initial rendering, resize, clean shutdown, and pause/resume on supported platforms. Test each implemented asset mode separately. Do not claim unrun builds or untested platforms succeeded. Follow the active repository's build permissions; in the authoring workspace, MSVC builds are owner-run unless explicitly requested.

Deliver a self-contained application source tree with a documented engine revision, build settings, resource staging, supported platforms, and reproducible commands. Include the public reference commit used and clearly report any remaining owner validation.
