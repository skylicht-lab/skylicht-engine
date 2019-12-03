# Skylicht Engine

## About

Skylicht Engine is a fork from [Irrlicht 3D Engine](http://irrlicht.sourceforge.net), that we have upgraded more feature: Sound Engine, Physics Engine, Particle Engine...

- Core functionality: Direct3D11, OpenGL 4+, OpenGLES 3+ video driver backends.
- Cross-Platform Framework: Android, IOS, Win32/Win64, UWP, Linux, MacOS, HTML5.
- Render pipeline: Forwarder & Deferred.

Skylicht Engine is a super lightweight Game Engine, that target to mobile platform (Android, IOS). And, it's completely free.

| Platform    | Build Status                                                                                                                                                                      |
| ------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Win32/Win64 | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| Cygwin64    | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| Android     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| Linux       | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6d6fbf50a10a4cf38426b9fabfc1fabc)](https://www.codacy.com/manual/ducphamhong/skylicht-engine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=skylicht-lab/skylicht-engine&amp;utm_campaign=Badge_Grade)

## How To Build

#### Prerequisites
- [CMake](https://cmake.org/download/) 3.12 or higher
- [Python](https://www.python.org/downloads/) 3.x
- [Visual Studio](https://visualstudio.microsoft.com/downloads/)  (2017 or higher if you want to build on Windows OS)

#### Build Command

- Clone this repo.
- Update the submodules.
  ```
  C:\skylicht-engine>git submodule update --init --recursive
  ```
- Create a new directory for the build files, e.g. `PrjVisualStudio` at the root of the repo.
  ```
  C:\skylicht-engine>mkdir PrjVisualStudio
  ```
- Run CMake from the current directory
  ```
  C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
  ```
- Open the generated solution `PrjVisualStudio/SKYLICHT_ENGINE.sln`.

- Once the solution opens, right click the MainApp project, click "Set as StartUp Project" and click the play button at the top to run the Demo App.

#### Add Source To Project

- You create new source files or folder on `Projects/{ProjectName}/Source/` and regenerate project.
  ```
  C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
  ```

## Features [WIP]

## Editor [WIP]

## Contributing

If you want to contribute to `Skylicht Engine`. Open an issues and submit a pull request to this repository. Please read CONTRIBUTING.md for details on our code of conduct, and the process for submitting pull requests.

## License

Skylicht Engine is under [MIT License](LICENSE.md).

Skylicht Engine is based in part on the work of:
- Irrlicht Engine
- Bullet Physics
- Independent JPEG Group
- zlib
- curl 
- libPng  

This means that if you've used the Skylicht Engine in your product, you must acknowledge somewhere in your documentation that you've used. It would also be nice to mention that you use the Irrlicht Engine, Bullet Physics ,the zlib, curl and libPng... Please see the README files from 3rd parties for further informations.
