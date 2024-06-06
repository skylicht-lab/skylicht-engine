# Skylicht Engine
Skylicht Engine, which is an evolution of the [Irrlicht Project](http://irrlicht.sourceforge.net). We have upgraded more features: Sound Engine, Physics Engine, Particle Engine...

-   Core functionality: Direct3D11, OpenGL 4+, OpenGLES 3+, Google Angle GLES video driver backends.

-   Cross-Platform Framework: Android, IOS, Win32/Win64, UWP, Linux, MacOS, HTML5.

-   Render pipeline: Forwarder & Deferred.

Skylicht Engine is a super lightweight Game Engine that targets mobile platforms (Android, IOS). And, it's completely free.

<img src="Documents/Media/logo-landscape.png"/>

# How to build the project
| Platform                                                                                  | Build Status                                                                                                                                                                      |
| ------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <img src="Documents/Media/Platforms/cygwin.png" width="25" valign="middle"> Cygwin64      | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/windows.png" width="25" valign="middle"> Windows      | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/apple.png" width="25" valign="middle"> MacOS, IOS     | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/ubuntu.png" width="25" valign="middle"> Linux         | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/emscripten.png" width="25" valign="middle"> Emscripten| [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/android.png" width="25" valign="middle"> Android      | [![Build status](https://circleci.com/gh/skylicht-lab/skylicht-engine.svg?style=shield)](https://circleci.com/gh/skylicht-lab/skylicht-engine/tree/master)                        |

## Prerequisites
-   [CMake](https://cmake.org/download/) 3.12 or higher
-   [Python](https://www.python.org/downloads/) 3.x
-   [Visual Studio](https://visualstudio.microsoft.com/downloads/)  (2017 or higher if you want to build on Windows OS)

## Build source code
-   Clone this repo

-   Build assets bundle
```Shell
C:\skylicht-engine>cd Assets
C:\skylicht-engine\Assets>python BuildAssetBundles.py
C:\skylicht-engine\Assets>cd ..
```

-   Run CMake from the current directory to generate visual studio project or xcode project
```Shell
# Visual Studio 2017
C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64

# Visual Studio 2019
C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 16 2019" -A x64

# Xcode
/User/Skylicht/Documents/skylicht-engine$cmake -S . -B ./PrjMac -G Xcode 

# Linux
$ skylicht-engine$ cmake -S . -B ./PrjLinux -G "Unix Makefiles"
```
-   Open the generated solution `PrjVisualStudio/SKYLICHT_ENGINE.sln`

-   Once the solution opens, right click the **SampleXXX** project, click **"Set as StartUp Project"** and click the play button at the top to run the Demo App.

-   More details: please preview command in **BuildCommand** folder.

## Build data assets
 
### Compress Texture
-   **Optional**, You can skip this step if you don't modify any resource texture.
-   This python tool will convert texture .TGA to compressed texture (.DDS, .ETC2, .PVRTC) to optimize gpu memory on runtime.
```Shell
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
### Build Shader
-   **Optional**, You can skip this step if you don't modify any shader.
-   This python tool use C Preprocessor to inline shader script (HLSL, HLSL)
```Shell
# Install pcpp (C Preprocessor tool)
# https://pypi.org/project/pcpp
C:\skylicht-engine>C:\Python37\Scripts\pip install pcpp

# Build shader script
C:\skylicht-engine>cd Assets
C:\skylicht-engine\Assets>python BuildShader.py
C:\skylicht-engine\Assets>cd ..
```

- You can optimize shader for rendering on IOS, Android by recompiling the shader with HARD_SHADOW.

```C++
// Assets\BuiltIn\Shader\Shadow\GLSL\LibShadow.glsl

#define HARD_SHADOW
// #define PCF_NOISE
```

And re-build the shaders
```Shell
# Build shader script
C:\skylicht-engine>cd Assets
C:\skylicht-engine\Assets>python BuildShader.py
C:\skylicht-engine\Assets>cd ..
```

# Add your code to Project

## Add source code
- Add new source files or subfolders on `Projects/{ProjectName}/Source` and regenerate project
```Shell
C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
```

## Create application project
- Run **Scripts/create_project.py**. Example: Create application *NewApplication* at folder *Samples\NewApplication*
```Shell
    C:\skylicht-engine>python Scripts\create_project.py NewApplication Samples\NewApplication
```
- Edit **CMakeProjects.cmake**, add new line:"*subdirs (Samples/NewApplication)*" and regenerate projects
- Open Visual Studio Solution and click **NewApplication** - **"Set as StartUp Project"**.

# Android
## Prerequisites
- Install [Android Studio and SDK](https://developer.android.com/studio)
- Install [mingw-w64](https://www.mingw-w64.org) for Windows
- Install [Android NDK](https://developer.android.com/ndk/downloads)

## How to build
### **Step 1: Build native library**
- Edit the file BuildCommand/BuildAndroidNDK.cmd by text editor and update your path **MINGW** and **NDK**

```Shell
# BuildAndroidNDK.cmd
# Set your pc folder, example
set MINGW=C:\MinGW\bin
set NDK=C:\Android\android-ndk-r26d
...
```

- Run **BuildCommand/BuildAndroidNDK.cmd** to build android native library

### **Step 2: Copy native library to Gradle Project**
```Shell
# Make folder jniLibs on Android Project
C:\skylicht-engine>mkdir Projects\Android\app\src\main\jniLibs
C:\skylicht-engine>mkdir Projects\Android\app\src\main\jniLibs\armeabi-v7a
C:\skylicht-engine>mkdir Projects\Android\app\src\main\jniLibs\arm64-v8a

# Copy result native ndk (from step 1) to jniLibs
# Project SampleSkinnedMesh
C:\skylicht-engine>copy Bin\Android\Libs\arm64-v8a\libSampleSkinnedMesh.so Projects\Android\app\src\main\jniLibs\arm64-v8a
C:\skylicht-engine>copy Bin\Android\Libs\armeabi-v7a\libSampleSkinnedMesh.so Projects\Android\app\src\main\jniLibs\armeabi-v7a
```

### **Step 3: Copy asset resource to Gradle Project**
```Shell
C:\skylicht-engine\Assets>python BuildTextureCompressETC.py
C:\skylicht-engine\Assets>python BuildAssetBundles.py
C:\skylicht-engine\Assets>cd..
```
**Android APK:**
```
C:\skylicht-engine>SET ASSET_PATH=Projects\Android\app\src\main\assets
```
**Android AAB:** Assets will be copied to assetPack: **appdata**
```
C:\skylicht-engine>SET ASSET_PATH=Projects\Android\appdata\src\main\assets
```
And copy command
```
# Copy built-in asset
C:\skylicht-engine>copy Bin\BuiltIn.zip %ASSET_PATH%

# Copy project asset
# Project SampleSkinnedMesh
C:\skylicht-engine>copy Bin\Common.zip %ASSET_PATH%
C:\skylicht-engine>copy Bin\SampleModelsResource.zip %ASSET_PATH%

# Dont forget copy ETC texture
C:\skylicht-engine>copy Bin\SampleModelsETC.zip %ASSET_PATH%
```

### **Step 4: Build APK by Android Studio or Gradle**
Open Android Studio and import project **Projects\Android**

<img src="Documents/Media/Samples/android/android-studio.jpg"/>

Run command build from Android Studio:

<img src="Documents/Media/Samples/android/build-apk.jpg"/>


### Android development
- You can rename your package name at **build.gradle** (Projects\Android\app)
- You can rename your application name at **string.xml** (Projects\Android\app\src\main\res\values)

# iOS
## Prerequisites
- Apple arm64 device
- Install XCode

## How to build
### **Step 1: Build compress texture ETC and Build Asset Bundles**
```Shell
# Grant permission executive for PVRTexTool
skylicht-engine/Tools/PVRTexTool/darwin$ chmod +x PVRTexToolCLI

# Build compress texture ETC
skylicht-engine/Assets$ python3 BuildTextureCompressETC.py

# Build asset bundles
skylicht-engine/Assets$ python3 BuildAssetBundles.py
```

### **Step 2: Generate iOS xCode Project**

If you want build samples for iOS device, you should replace the **bundleIdentifier** in Samples/{ProjectName}/**CMakeLists.txt**.

```CMake
elseif (BUILD_IOS)
    # replace your application name
    set(MACOSX_BUNDLE_EXECUTABLE_NAME SampleHelloWorld)
    set(APP_NAME "SampleHelloWorld")
    
    # replace your bundleIdentifier
    set(APP_BUNDLE_IDENTIFIER "com.skylicht.helloword")
    set(CODE_SIGN_IDENTITY "iPhone Developer")
    
else()
```

And generate iOS xCode Project by this script.

```Shell
# generate project for iOS device
skylicht-engine/BuildCommand$ ./GenerateXCodeIOS.sh

# or generate for iOS simulator
skylicht-engine/BuildCommand$ ./GenerateXCodeIOSSimulator.sh
```
Open iOS XCode Project on folder **skylicht-engine/PrjIOS/SKYLICHT_ENGINE.xcodeproj**

<img src="Documents/Media/Samples/ios/iosSimulator.png"/>

*Note: iOS Simulator is not yet support compressed texture and hardware skinning.*

# Build static or shared library
Skylicht Engine can be compiled into a standalone library to simplify integration into other projects.

## Shared library
```Shell
C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=ON
C:\skylicht-engine>cmake --build ./PrjVisualStudio --target install --config Debug
```
*Note: Use the parameters "-DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=ON" with cmake command*

<img src="Documents/Media/dll-library.jpg"/>

## Static library
```Shell
C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON
C:\skylicht-engine>cmake --build ./PrjVisualStudio --target install --config Debug
```
## How to integrate
And then copy the resulting InstallLibs folder to another project.
See the project example: https://github.com/skylicht-lab/hello-skylicht

# Roadmap
- [Skylight Engine Core](https://github.com/skylicht-lab/skylicht-engine/issues/5) (In progress)
- [Engine Editor](https://github.com/skylicht-lab/skylicht-engine/issues/6) (In progress)
<img src="Documents/Media/Editor/skylicht-engine-editor.jpg"/>

# Code example

```C++
// INIT SCENE FUNCTION
// init scene/zone
CScene* scene = new CScene();
CZone* zone = scene->createZone();

// camera
CGameObject *camObj = zone->createEmptyObject();
CCamera* camera = camObj->addComponent<CCamera>();
camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
camera->lookAt(
   core::vector3df(0.0f, 0.0f, 0.0f),  // look at target position
   core::vector3df(0.0f, 1.0f, 0.0f)   // head up vector
);

// load model
CMeshManager* meshManager = CMeshManager::getInstance();
CEntityPrefab* meshPrefab = meshManager->loadModel("SampleModels/BlendShape/Cat.fbx", NULL, true);

// load material
CMaterialManager* materialMgr = CMaterialManager::getInstance();

// search textures folder
std::vector<std::string> textureFolders;

// use Skylicht Editor to edit .mat file
ArrayMaterial& catMaterials = materialMgr->loadMaterial("SampleModels/BlendShape/Cat.mat", true, textureFolders);

// create 3d object
CGameObject* cat = zone->createEmptyObject();
cat->setName("Cat");
cat->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

// add render mesh component & init material
CRenderMesh* meshRenderer = cat->addComponent<CRenderMesh>();
meshRenderer->initFromPrefab(meshPrefab);
meshRenderer->initMaterial(catMaterials);

// update scene (1 frame)
scene->update();

// render scene to screen
CForwardRP* renderPipeline = new CForwardRP();

CBaseApp* app = getApplication();
u32 w = app->getWidth();
u32 h = app->getHeight();
renderPipeline->initRender(w, h);

// RENDER TO SCREEN FUNCTION
renderPipeline->render(
    NULL, // render target is screen
    camera, // the camera
    scene->getEntityManager(), // all entities in scene
    core::recti() // the viewport is fullscreen
);
```

# Sample Projects

### Samples\HelloWorld
Engine Components are used:
- FreeType Font
- Glyph Font
- Canvas Component
- Graphics2D
- GUI Text
<img src="Documents/Media/Samples/sample_hello_world.jpg"/>

### Samples\DrawPrimitives
This demo is to draw Cube 3D, set up, transform and use components.
<img src="Documents/Media/Samples/sample_draw_cube.png"/>

### Samples\Materials
This demo code, that renders Sphere 3D, loads textures (Diffuse, Normal, Specular), and binds SpecGloss shader to Material.
<img src="Documents/Media/Samples/sample_material.jpg"/>

### Samples\PBR
This demo code, that renders Sci-fi Helmet, loads textures (Albedo, Roughness, Metal, AO, Emissive), and binds Physically Based Rendering shader (PBR) to Material.
<img src="Documents/Media/Samples/sample_pbr.jpg"/>

### Samples\SampleShader
This demo code, How to binds shader to Material.
<img src="Documents/Media/Samples/sample_shader.jpg"/>

### Samples\LuckyDraw
Engine Components are used:
- FreeType Font
- Glyph Font
- Graphics2D
- GUI Text
- GUI Image
- GUI Mask
- EventManager
<img src="Documents/Media/Samples/sample_lucky_draw.gif" width="50%"/>

### Samples\SkinnedMesh
This is example code. How to render character model with animation, models are downloaded from [mixamo](https://www.mixamo.com)
- **DAE Collada** loader model/animation
- Forwarder rendering
- Skydome
- AnimationController
<img src="Documents/Media/Samples/sample_skinned_mesh.jpg"/>

Live demo: [Emscripten](http://34.149.122.230/SampleSkinnedMesh.html)

Blendshape live demo: [Emscripten](http://34.149.122.230/SampleBlendShape.html)

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

Live demo: (Web PC Only) [Emscripten](http://34.149.122.230/SampleSponza.html)

### Samples\LightmappingVertex
This is a demo to use lightmapper to set indirect lighting color on vertex color.

Engine Components are used:
- OBJ Wavefront loader
- **Deferred** and **Forwarder** rendering
- Dynamic Direction Lighting
- Shadow Mapping
- Lightmapper
- Skydome

#### Baked Global Illumination (3 bounces)
Irradiance baked on Vertex Color:

<img src="Documents/Media/Samples/tank-scene/indirect-bake-3-bounce.jpg"/>

#### Indirect + Direction light - No post processing (3 bounces light)

<img src="Documents/Media/Samples/tank-scene/tank-scene-3-bounce-a.jpg"/>

### Samples\Collision
This is a demo to use **CollisionManager** to get collision point from the ray.
<img src="Documents/Media/Samples/sample_collision.jpg"/>

And how to query all triangles inside a Box
<img src="Documents/Media/Samples/sample_collision_box.jpg"/>

And how to use the decal projector feature
<img src="Documents/Media/Samples/sample_collision_decal.jpg"/>

Live demo: (Web PC Only) [Emscripten](http://34.149.122.230/SampleCollision.html)

### Samples\Instancing
This is demo setup render many mesh instancing to optimize draw call and render performance.

Live demo: [Emscripten](http://34.149.122.230/SampleInstancing.html)

### Samples\SkinnedMeshInstancing
This is a demo of drawing many characters and animations, that use GPU Instancing technique.
<img src="Documents/Media/Samples/sample_skinned_instancing.jpg"/>

### Samples\BoidSystem
<img src="Documents/Media/Samples/sample_bolds.jpg"/>

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

# Contributing
Welcome if you want to contribute your code to **Skylicht Engine** (Ex: add feature or fix bug). Wellcome to join the group. Open an issue and submit a pull request to this repository. Please read [CONTRIBUTING.md](Documents/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

# Thanks
- Julien Fryer ([SPARK](https://github.com/Synxis/SPARK)) about references and source code demo of particle system.
- [Inigo Quilez](https://www.shadertoy.com/view/4sfGzS) about perlin noise reference.
- Irrlicht Community, who have shared [some demo](https://github.com/netpipe/IrrlichtDemos/tree/master/SceneNodes)

# License
Skylicht Engine is licensed under [MIT License](LICENSE.md)

Skylicht Engine is based in part on the work of:
-   [Irrlicht Engine](http://irrlicht.sourceforge.net)
-   [Bullet Physics](https://github.com/bulletphysics/bullet3)
-   [Google Angle](https://github.com/google/angle)
-   [Ocornut Imgui](https://github.com/ocornut/imgui)
-   [Xatlas](https://github.com/jpcy/xatlas)
-   [ufbx](https://github.com/bqqbarbhg/ufbx)
-   [Spark Particle](https://github.com/Synxis/SPARK)
-   [Freetype2](http://git.savannah.gnu.org/cgit/freetype/freetype2.git)
-   Independent JPEG Group, libPng, zlib, curl...

This means that if you've used the Skylicht Engine in your product, you must acknowledge somewhere in your documentation that you've used. It would also be nice to mention that you use the 3rd parties library... Please see the README files from 3rd parties for further information.

Thanks,
Project Maintainer: Pham Hong Duc
