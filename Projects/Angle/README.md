# How to build Angle on MACOS

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

#### DEBUG
```console
  gn args out/Debug
```
After vi editor display, press Enter and edit **args.gn**:
```console
is_debug = true
```

Press ESC and type :wq to quit vi

```console
  autoninja -C out/Debug libEGL libGLESv2
```
#### RELEASE

```console
  gn gen out/Release
  autoninja -C out/Release libEGL libGLESv2
```
### RESULT

Copy **libEGL.dylib** and **libGLESv2.dylib** to **Projects/Angle/out/MacOS/Release** use in Skylicht Engine Project

## More infomation

- Please read [angle/DevSetup.md](https://github.com/google/angle/blob/master/doc/DevSetup.md) for more details

- [Google Angle Project](https://github.com/google/angle)
