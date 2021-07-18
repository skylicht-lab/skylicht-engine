# Skylicht Engine
## About
Skylicht Engine, which is an evolution of the [Irrlicht Project](http://irrlicht.sourceforge.net). We have upgraded more features: Sound Engine, Physics Engine, Particle Engine...

-   Core functionality: Direct3D11, OpenGL 4+, OpenGLES 3+, Google Angle GLES video driver backends.

-   Cross-Platform Framework: Android, IOS, Win32/Win64, UWP, Linux, MacOS, HTML5.

-   Render pipeline: Forwarder & Deferred.

Skylicht Engine is a super lightweight Game Engine that targets mobile platforms (Android, IOS). And, it's completely free.

## How To Build
| Platform                                                                                 | Build Status                                                                                                                                                                      |
| -----------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <img src="Documents/Media/Platforms/windows.png" width="25" valign="middle"> Windows     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/cygwin.png" width="25" valign="middle"> Cygwin64     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/uwp.png" width="25" valign="middle"> Windows Store   | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/apple.png" width="25" valign="middle"> MacOS         | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/ubuntu.png" width="25" valign="middle"> Linux        | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/html5.png" width="25" valign="middle"> Emscripten    | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/android.png" width="25" valign="middle"> Android     | [![Build status](https://circleci.com/gh/skylicht-lab/skylicht-engine.svg?style=shield)](https://circleci.com/gh/skylicht-lab/skylicht-engine/tree/master)                        |

### Prerequisites
-   [CMake](https://cmake.org/download/) 3.12 or higher
-   [Python](https://www.python.org/downloads/) 3.x
-   [Visual Studio](https://visualstudio.microsoft.com/downloads/)  (2017 or higher if you want to build on Windows OS)

### Build Command
-   Clone this repo

-   Build assets bundle
    ```console
    C:\skylicht-engine>cd Assets
    C:\skylicht-engine\Assets>python BuildAssetBundles.py
    C:\skylicht-engine\Assets>cd ..
    ```
    
-   Run CMake from the current directory to generate visual studio project or xcode project
    ```console
    # Visual Studio 2017
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
  
    # Visual Studio 2019
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 16 2019" -A x64
    
    # Xcode
    /User/Skylicht/Documents/skylicht-engine$cmake -S . -B ./PrjMac -G Xcode 
    ```
-   Open the generated solution `PrjVisualStudio/SKYLICHT_ENGINE.sln`

-   Once the solution opens, right click the **SampleXXX** project, click **"Set as StartUp Project"** and click the play button at the top to run the Demo App.

-   More details: please preview command in **BuildCommand** folder.

### Build Compress Texture, Shader

-   This python tool will convert texture .TGA to compressed texture (.DDS, .ETC2, .PVRTC) to optimize gpu memory on runtime. (**Optional**, just rebuild when you modify/add texture to asset)
    ```console
    # Install Tinydb
    # https://pypi.org/project/tinydb
    C:\skylicht-engine>C:\Python37\Scripts\pip install tinydb

    # Install Pillow (Image processing)
    # https://pillow.readthedocs.io/en/4.1.x/index.html
    C:\skylicht-engine>C:\Python37\Scripts\pip install Pillow

    # Compress TGA to DDS, ETC2, PVR texture
    C:\skylicht-engine>cd Assets    
    C:\skylicht-engine\Assets>python BuildTextureCompressDDS.py
    C:\skylicht-engine\Assets>python BuildTextureCompressETC.py
    C:\skylicht-engine\Assets>python BuildTextureCompressPVR.py
    C:\skylicht-engine\Assets>cd ..
    ```
    
-   This python tool use C Preprocessor to inline shader script (HLSL, HLSL) (**Optional**, just rebuild when you modify built-in shader)
    ```console
    # Install pcpp (C Preprocessor tool)
    # https://pypi.org/project/pcpp
    C:\skylicht-engine>C:\Python37\Scripts\pip install pcpp
    
    # Build shader script
    C:\skylicht-engine>cd Assets
    C:\skylicht-engine\Assets>python BuildShader.py
    C:\skylicht-engine\Assets>cd ..
    ```
    
### Add your code to  Project

#### Add source code
- Add new source files or subfolders on `Projects/{ProjectName}/Source` and regenerate project
    ```console
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
    ```

#### Create application project
- Run **Scripts/create_project.py**. Example: Create application *NewApplication* at folder *Samples\NewApplication*
```console
    C:\skylicht-engine>python Scripts\create_project.py NewApplication Samples\NewApplication
```
- Edit **CMakeProjects.cmake**, add new line:"*subdirs (Samples/NewApplication)*" and regenerate projects
- Open Visual Studio Solution and click **NewApplication** - **"Set as StartUp Project"**.

### Android
##### Prerequisites
- Install [Android Studio and SDK](https://developer.android.com/studio)
- Install [mingw-w64](http://mingw-w64.org/doku.php/download/mingw-builds) for Windows
- Install [Android NDK](https://developer.android.com/ndk/downloads)

##### How to build
###### **Step 1: Build native library**
- Open **BuildCommand/BuildAndroidNDK.cmd** by text editor and update your path **MINGW** and **NDK**

```code
# BuildAndroidNDK.cmd
# Set your pc folder, example
set MINGW=C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin
set NDK=C:\SDK\android-ndk-r21e
```

- Run **BuildCommand/BuildAndroidNDK.cmd** to build Android Native Library
- More details about **cmake android ndk**, [please read here](https://developer.android.com/ndk/guides/cmake)

###### **Step 2: Copy native library to Gradle Project**
```console
# Make folder jniLibs on Android Project
C:\skylicht-engine>mkdir Projects\Android\SkylichtEngine\app\src\main\jniLibs\armeabi-v7a
C:\skylicht-engine>mkdir Projects\Android\SkylichtEngine\app\src\main\jniLibs\arm64-v8a

# Copy result native ndk (from step 1) to jniLibs
# Example test project SampleSkinnedMesh
C:\skylicht-engine>copy Bin\Android\Libs\arm64-v8a\libSampleSkinnedMesh.so Projects\Android\SkylichtEngine\app\src\main\jniLibs\arm64-v8a
C:\skylicht-engine>copy Bin\Android\Libs\armeabi-v7a\libSampleSkinnedMesh.so Projects\Android\SkylichtEngine\app\src\main\jniLibs\armeabi-v7a
```
- More details about **Link Gradle to your native library**, [please read here](https://developer.android.com/studio/projects/gradle-external-native-builds)

###### **Step 3: Copy Resource to Gradle (Android) Project**
```console
C:\skylicht-engine\Assets>python BuildTextureCompressETC.py
C:\skylicht-engine\Assets>python BuildAssetBundles.py
C:\skylicht-engine\Assets>cd..

# Create Assets Gradle
C:\skylicht-engine>mkdir Projects\Android\SkylichtEngine\app\src\main\assets

# Copy built-in asset
C:\skylicht-engine>copy Bin\BuiltIn.zip Projects\Android\SkylichtEngine\app\src\main\assets

# Copy project asset
# Example test project SampleSkinnedMesh
C:\skylicht-engine>copy Bin\Common.zip Projects\Android\SkylichtEngine\app\src\main\assets
C:\skylicht-engine>copy Bin\SampleModelsResource.zip Projects\Android\SkylichtEngine\app\src\main\assets

# Dont forget copy ETC texture
C:\skylicht-engine>copy Bin\SampleModelsETC.zip Projects\Android\SkylichtEngine\app\src\main\assets
```

###### **Step 4: Build APK by Android Studio or Gradle**
Open Android Studio and import project **C:\skylicht-engine\Projects\Android\SkylichtEngine**

<img src="Documents/Media/Samples/android/android-studio.jpg"/>

Run Build from Android Studio:

<img src="Documents/Media/Samples/android/build-apk.jpg"/>

### Roadmap
- [Skylight Engine Core](https://github.com/skylicht-lab/skylicht-engine/issues/5) (In progress)
- [Engine Editor](https://github.com/skylicht-lab/skylicht-engine/issues/6) (In progress)

### Samples\HelloWorld
Engine Components are used:
- **FreeType Font**
- **Glyph Font**
- **Canvas Component**
- **Graphics2D**
- **GUI Text**
<img src="Documents/Media/Samples/sample_hello_world.jpg"/>

### Samples\DrawPrimitives
This demo is to draw Cube 3D, set up, transform and use components.
<img src="Documents/Media/Samples/sample_draw_cube.png"/>

### Samples\Materials
This demo code, that renders Sphere 3D, loads textures (Diffuse, Normal, Specular), and binds SpecGloss shader to Material.
<img src="Documents/Media/Samples/sample_material.jpg"/>

### Samples\SampleShader
This demo code, How to bind shader on Material.
<img src="Documents/Media/Samples/sample_shader.jpg"/>

### Samples\LuckyDraw
Engine Components are used:
- **FreeType Font**
- **Glyph Font**
- **Graphics2D**
- **GUI Text**
- **GUI Image**
- **GUI Mask**
- **EventManager**
<img src="Documents/Media/Samples/sample_lucky_draw.gif" width="50%"/>

### Samples\SkinnedMesh
This is example code. How to render character model with animation, models are downloaded from [mixamo](https://www.mixamo.com)
- **DAE Collada** loader model/animation
- **Forwarder** rendering
- **Skydome**
- **AnimationController**
<img src="Documents/Media/Samples/sample_skinned_mesh.jpg"/>

### Samples\LightmapUV
This is a demo how to use UnwrapUV tool to create Lightmap UV. 
[Library xatlas](https://github.com/jpcy/xatlas) is integrated to Skylicht Engine.

<img src="Documents/Media/Samples/lightmap-uv/lightmap-uv.png"/>

<img src="Documents/Media/Samples/lightmap-uv/mesh-charts00.png" width="50%"/>

### Samples\Lightmapping
This is a demo code to use lightmapper to bake indirect lighting color on lightmap texture.
[Project references: Lightmapper](https://github.com/ands/lightmapper)

#### Gazebo

<img src="Documents/Media/Samples/lightmap/lightmapping_a.jpg"/>

Result lightmap

<img src="Documents/Media/Samples/lightmap/rasterize_1.png"/>

### Sponza lightmap
Sponza indirect lighting baked by Skylicht Lightmapper

<img src="Documents/Media/Samples/sponza/Sponza_indirect_01.jpg"/>

<img src="Documents/Media/Samples/sponza/Sponza_indirect_02.jpg"/>

### Samples\Sponza
This example is how to set up a simple lighting & lightmap. 
This scene uses basic diffuse bump specular shader.

<img src="Documents/Media/Samples/sponza/Sponza_01.jpg"/>

<img src="Documents/Media/Samples/sponza/Sponza_02.jpg"/>

<img src="Documents/Media/Samples/sponza/Sponza_03.jpg"/>

<img src="Documents/Media/Samples/sponza/Sponza_04.jpg"/>

Run on Ubuntu 20.04
<img src="Documents/Media/Samples/sponza/sponza_ubuntu.jpg"/>

### Samples\LightmappingVertex
This is a demo to use lightmapper to set indirect lighting color on vertex color.

Engine Components are used:
- **OBJ Wavefront** loader
- **Deferred** and **Forwarder** rendering
- **Dynamic Direction Lighting**
- **Shadow Mapping**
- **Lightmapper**
- **Skydome**

#### Baked Global Illumination (3 bounces)
Irradiance baked on Vertex Color:

<img src="Documents/Media/Samples/tank-scene/indirect-bake-3-bounce.jpg"/>

#### Indirect + Direction light - No post processing (3 bounces light)

<img src="Documents/Media/Samples/tank-scene/tank-scene-3-bounce-a.jpg"/>

### Samples\Particles
Skylicht Particle Component is a cleanup version of [SPARK](https://github.com/Synxis/SPARK). We have optimized by GPU Billboard Instancing.

This is demo code how to create particle system and use [imgui](https://github.com/ocornut/imgui)

#### Basic particle
<img src="Documents/Media/Samples/sample_particles.jpg"/>

#### Explosion demo
<img src="Documents/Media/Samples/sample_particles_explosion.jpg"/>

#### Magic particle
This is a demo code on how to use a simple projectile, impact, trail particle, sub emitter system.
<img src="Documents/Media/Samples/sample_magic.jpg"/>

### Samples\Noise2D, Noise3D
This is demo code how to use noise library shader to make vfx effect, particle turbulence.

<img src="Documents/Media/Samples/sample_noise_2d.jpg"/>

<img src="Documents/Media/Samples/sample_noise_3d.jpg"/>

## Contributing
Welcome if you want to contribute your code to **Skylicht Engine** (Ex: add feature or fix bug). Wellcome to join the group. Open an issue and submit a pull request to this repository. Please read [CONTRIBUTING.md](Documents/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

## Thanks
- Julien Fryer ([SPARK](https://github.com/Synxis/SPARK)) about references and source code demo of particle system.
- [Inigo Quilez](https://www.shadertoy.com/view/4sfGzS) about perlin noise reference.

## License
Skylicht Engine is licensed under [MIT License](LICENSE.md)

Skylicht Engine is based in part on the work of:
-   [Irrlicht Engine](http://irrlicht.sourceforge.net)
-   [Bullet Physics](https://github.com/bulletphysics/bullet3)
-   [Google Angle](https://github.com/google/angle)
-   [Ocornut Imgui](https://github.com/ocornut/imgui)
-   [Xatlas](https://github.com/jpcy/xatlas)
-   [Spark Particle](https://github.com/Synxis/SPARK)
-   [Freetype2](http://git.savannah.gnu.org/cgit/freetype/freetype2.git)
-   Independent JPEG Group, libPng, zlib, curl...

This means that if you've used the Skylicht Engine in your product, you must acknowledge somewhere in your documentation that you've used. It would also be nice to mention that you use the 3rd parties library... Please see the README files from 3rd parties for further information.

Thanks,
Project Maintainer: Pham Hong Duc