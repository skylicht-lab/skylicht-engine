# How to build Angle on MacOS

Currently, Skylicht Engine use [Angle](https://github.com/google/angle) to simulation OpenGLES on MacOS and IOS.

## Step 1: GET DEPOT TOOLS

```console
  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
```

And export ENV PATH
Example: **'depot_tools'** at /Users/Skylicht/Documents/projects/depot_tools

```console
  export PATH=/Users/Skylicht/Documents/projects/depot_tools:$PATH
```

## Step 2: GET ANGLE PROJECT

```console
  git clone https://chromium.googlesource.com/angle/angle
```

## Step 3: BUILD ANGLE PROJECT

### Setup project
```console
  cd angle
  python scripts/bootstrap.py
  gclient sync
```

### Build project

#### RELEASE

```console
gn gen out/Release
```
Edit generated file **args.gn**
```console
is_debug = false
target_cpu = "arm64"
angle_enable_vulkan = false
angle_enable_wgpu = false
angle_enable_swiftshader = false
```
For intel: target_cpu = "x64"

Use autoninja (depot_tools) tool to build Angle project

```console
autoninja -C out/Release libEGL libGLESv2
```

### IOS
Edit out/Release/**args.gn**
```console
is_debug = false
target_os = "ios"
target_cpu = "arm64"
target_environment = "device"
ios_enable_code_signing = false
angle_enable_vulkan = false
angle_enable_wgpu = false
angle_enable_swiftshader = false
```

arm64 simulator:
```console
target_environment = "simulator"
target_cpu = "arm64"
```
x64 simulator
```console
target_environment = "simulator"
target_cpu = "x64"
```

#### DEBUG (Optional)
```console
gn args out/Debug
```
After vi editor display, press Enter and edit **args.gn**:
```console
is_debug = true
```

Press ESC and type :wq to quit vi

```console
autoninja -C out/Release libEGL libGLESv2
```
### RESULT

Copy **libEGL.dylib** and **libGLESv2.dylib** (in folder out/Release) to **Projects/Angle/out/MacOS/Release** and update include folder (.h).

## More infomation

- Please read [angle/DevSetup.md](https://github.com/google/angle/blob/master/doc/DevSetup.md) for more details

- [Google Angle Project](https://github.com/google/angle)