# Skylicht Engine
## About
Skylicht Engine is a fork from [Irrlicht 3D Engine](http://irrlicht.sourceforge.net), that we have upgraded more feature: Sound Engine, Physics Engine, Particle Engine...
-   Core functionality: Direct3D11, OpenGL 4+, OpenGLES 3+ video driver backends.
-   Cross-Platform Framework: Android, IOS, Win32/Win64, UWP, Linux, MacOS, HTML5.
-   Render pipeline: Forwarder & Deferred.
Skylicht Engine is a super lightweight Game Engine, that target to mobile platform (Android, IOS). And, it's completely free.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6d6fbf50a10a4cf38426b9fabfc1fabc)](https://www.codacy.com/manual/ducphamhong/skylicht-engine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=skylicht-lab/skylicht-engine&amp;utm_campaign=Badge_Grade)
## How To Build
| Platform                                                                                 | Build Status                                                                                                                                                                      |
| -----------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <img src="Documents/Media/Platforms/windows.png" width="30" valign="middle"> Win32/Win64 | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/cygwin.png" width="30" valign="middle"> Cygwin64     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/uwp.png" width="30" valign="middle"> Windows Store   | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/ubuntu.png" width="30" valign="middle"> Linux        | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/html5.png" width="30" valign="middle"> Emscripten    | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
### Prerequisites
-   [CMake](https://cmake.org/download/) 3.12 or higher
-   [Python](https://www.python.org/downloads/) 3.x
-   [Visual Studio](https://visualstudio.microsoft.com/downloads/)  (2017 or higher if you want to build on Windows OS)
### Build Command
-   Clone this repo

-   Build Data Assets
  ```
  C:\skylicht-engine>cd Assets
  C:\skylicht-engine\Assets>python BuildAssetBundles.py
  C:\skylicht-engine\Assets>cd ..
  ```  
  
-   Run CMake from the current directory
  ```
  # Visual Studio 2017
  C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
  
  # Visual Studio 2019
  C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 16 2019" -A x64
  ```
  
-   Open the generated solution `PrjVisualStudio/SKYLICHT_ENGINE.sln`

-   Once the solution opens, right click the MainApp project, click "Set as StartUp Project" and click the play button at the top to run the Demo App.
### Add Source To Project
Add new source files or folder on `Projects/{ProjectName}/Source` and regenerate project.
  ```
  C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
  ```
## Contributing
If you want to contribute your code to `Skylicht Engine` (Ex: add feature or fixbug). Open an issues and submit a pull request to this repository. Please read [CONTRIBUTING.md](Documents/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.
## License
Skylicht Engine is under [MIT License](LICENSE.md)
Skylicht Engine is based in part on the work of:
-   Irrlicht Engine
-   Bullet Physics
-   Independent JPEG Group
-   zlib
-   curl 
-   libPng

This means that if you've used the Skylicht Engine in your product, you must acknowledge somewhere in your documentation that you've used. It would also be nice to mention that you use the Irrlicht Engine, Bullet Physics ,the zlib, curl and libPng... Please see the README files from 3rd parties for further informations.
