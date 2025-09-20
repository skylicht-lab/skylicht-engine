/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

namespace irr
{
	class IrrlichtDevice;
}

/**
 * @defgroup Materials
 * @brief This group includes object classes related to materials, shaders, and textures
 */

 /**
  * @defgroup Graphics2D Graphics 2D
  * @brief This group includes object classes related to sprite, font... and a graphics object for 2D drawing.
  */

/**
  * @defgroup GUI GUI
  * @brief This group includes GUI display object classes for Canvas.
  */

/**
  * @defgroup GameObject Scene and game objects
  * @brief This group includes scene object classes for 3D rendering.
  */

/**
  * @defgroup ECS Entity component system
  * @brief This group contains objects used to support ECS architecture.
  */

/**
  * @defgroup Mesh Mesh manager
  * @brief This group includes object classes used to load models such as .fbx, .dae, and .obj into the engine, as well as to export them to other formats.
  */

/**
  * @defgroup RP Render pipeline
  * @brief This group includes includes object classes that execute 3D scene rendering to the screen or to a texture.
  */

/**
  * @defgroup Components Main components
  * @brief It is a group that includes the component objects within the engine.
  */

/**
  * @defgroup Camera Camera
  * @ingroup Components
  * @brief This is a group that includes the object classes related to the camera.
  */

/**
  * @defgroup Transform Transform
  * @ingroup Components
  * @brief This is a group of objects used to set the Transform (position, rotation, scale) for a GameObject.
  */

/**
  * @defgroup RenderMesh Render mesh
  * @ingroup Components
  * @brief This group includes components for rendering models on the screen.
  */

 /**
 * @defgroup Lighting Lighting
 * @ingroup Components
 * @brief This group contains all the objects related to lighting.
 */

/**
  * @defgroup Debug Debug
  * @brief A group of helper object that draw on the scene for debugging purposes.
  */

/*! \mainpage Skylicht-Engine API documentation
 *
 * \section intro Introduction
 *
 * Welcome to the Skylicht-Engine API documentation.
 * Here you'll find any information you'll need to develop applications with
 * the Skylicht Engine. 
 * 
 * For more information on how to install and build this library, you can refer to the Readme.md file.
 *
 * To update to the latest version, please visit <A HREF="https://github.com/skylicht-lab/skylicht-engine" >github: skylicht-engine</A>
 *
 * You can start by looking at the <A HREF="topics.html">**topics**</A>
 * 
 * \section build How to build
 * 
 * ## Prerequisites
 * - <A HREF="https://cmake.org/download/">CMake</A> 3.12 or higher
 * - <A HREF="https://visualstudio.microsoft.com/downloads/">Visual Studio</A>
 * - <A HREF="https://www.python.org/downloads">Python</A> 3.x
 * 
 * ## Build library
 * ### Static
 * You can call the build command `InstallLibVCPrj2022.cmd` in the BuildCommand directory after you have fully installed CMake and Visual Studio 2022.
 * @code
 * C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON
 * C:\skylicht-engine>cmake --build ./PrjVisualStudio --target install --config Debug
 * @endcode
 * 
 * ### Dynamic dll
 * You can call the build command `InstallSharedLibVCPrj2022.cmd` in the BuildCommand directory.
 * @code
 * C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON -DBUILD_SHARED_LIBS=ON
 * C:\skylicht-engine>cmake --build ./PrjVisualStudio --target install --config Debug
 * @endcode
 * 
 * ## How to integrate
 * After building the Skylicht-Engine into a library, you can include the .h files and link the .lib files into your project.
 * @image html library.jpg width=600px
 * 
 * This is a quick reminder: An application running on the Skylicht-Engine platform needs sufficient shader data to start. 
 * Therefore, you must copy all files from the `Asset/BuildIn` folder into your project.
 * 
 * You can check out the <A HREF="https://github.com/skylicht-lab/hello-skylicht">sample project</a>.
 */

/// @brief Everything in the Skylicht Engine. You can start by looking at the <A HREF="topics.html">topics</A>.
namespace Skylicht
{
	SKYLICHT_API void initSkylicht(IrrlichtDevice* device, bool server = false);

	SKYLICHT_API void releaseSkylicht();

	SKYLICHT_API void updateSkylicht();

	SKYLICHT_API IrrlichtDevice* getIrrlichtDevice();

	SKYLICHT_API IVideoDriver* getVideoDriver();

	SKYLICHT_API float getTimeStep();

	SKYLICHT_API void setTimeStep(float timestep);

	SKYLICHT_API void setTotalTime(float t);

	SKYLICHT_API float getNonScaledTimestep();

	SKYLICHT_API float getTotalTime();

	SKYLICHT_API void enableFixedTimeStep(bool b);

	SKYLICHT_API void setFixedTimeStep(float s);

	SKYLICHT_API void setTimeScale(float scale);

	SKYLICHT_API float getTimeScale();
}