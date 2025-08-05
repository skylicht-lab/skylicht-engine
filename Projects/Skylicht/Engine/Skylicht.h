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
  * @defgroup GameObject Scene and Game Objects
  * @brief This group includes scene object classes for 3D rendering.
  */

/**
  * @defgroup ECS Entity Component System
  * @brief This group contains objects used to support ECS architecture.
  */

/**
  * @defgroup Mesh Mesh Manager
  * @brief This group includes object classes used to load models such as .fbx, .dae, and .obj into the engine, as well as to export them to other formats.
  */

/**
  * @defgroup RP Render pipeline
  * @brief This group includes includes object classes that execute 3D scene rendering to the screen or to a texture.
  */

/*! \mainpage Skylicht Engine API documentation
 *
 * \section intro Introduction
 *
 * Welcome to the Skylicht Engine API documentation.
 * Here you'll find any information you'll need to develop applications with
 * the Skylicht Engine. 
 * 
 * For more information on how to install and build this library, you can refer to the Readme.md file.
 *
 * To update to the latest version, please visit <A HREF="https://github.com/skylicht-lab/skylicht-engine" >github: skylicht-engine</A>
 *
 * You can start by looking at the <A HREF="topics.html">topics</A>
 * 
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

	SKYLICHT_API float getTotalTime();

	SKYLICHT_API void setTotalTime(float t);
}