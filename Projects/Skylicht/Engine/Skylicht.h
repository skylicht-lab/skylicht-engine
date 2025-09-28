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
* @defgroup Materials Materials, Shaders and Textures
* @brief Contains classes related to **materials, shaders, and textures**.
*/

/**
* @defgroup Graphics2D 2D Graphics and Drawing
* @brief A collection of object classes that handle **sprite, font**, and the core graphics objects for 2D drawing.
*/

/**
* @defgroup GUI Graphical User Interface (GUI)
* @brief Includes display classes for the **Canvas** and other GUI elements.
*/

/**
* @defgroup GameObject Scene and Game Object System
* @brief Object classes used for **scene construction** and 3D rendering.
*/

/**
* @defgroup ECS Entity Component System (ECS)
* @brief Objects designed to support the **Entity Component System (ECS) architecture**.
*/

/**
* @defgroup Mesh Mesh Loading and Management
* @brief Classes used to **load models** (such as .fbx, .dae, and .obj) into the engine, as well as to export them to other formats.
*/

/**
* @defgroup RP Render Pipeline
* @brief Object classes that **execute the 3D scene rendering** process to the screen or a texture.
*/

/**
* @defgroup Components Engine Components
* @brief The core collection of **component objects** within the engine.
*/

/**
* @defgroup Camera Camera System
* @ingroup Components
* @brief Includes classes for **managing and controlling the camera**.
*/

/**
* @defgroup Transform Transformations
* @ingroup Components
* @brief Objects used to set the **Transform** (position, rotation, scale) for a GameObject.
*/

/**
* @defgroup RenderMesh 3D Model Rendering
* @ingroup Components
* @brief Components for **displaying and rendering models** on the screen.
*/

/**
* @defgroup Lighting Lighting System
* @ingroup Components
* @brief Gathers all objects related to the **lighting** mechanism.
*/

/**
* @defgroup IndirectLighting Indirect Lighting
* @ingroup Components
* @brief Gathers all objects and components related to **simulating indirect light**.
*/

/**
* @defgroup Debug Debug
* @brief Helper objects used to **draw overlays** on the scene for debugging purposes.
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
	/**
	 * @brief Initialize the Skylicht Engine core systems and all required managers.
	 *
	 * This function must be called once at the start of your application, after creating the Irrlicht device.
	 * It sets up input, graphics, animation, shader, mesh, material, and debug managers, and resets random timers.
	 *
	 * @param device Pointer to the Irrlicht device to use.
	 * @param server Set to true if running in server mode (default: false).
	 */
	SKYLICHT_API void initSkylicht(IrrlichtDevice* device, bool server = false);

	/**
	 * @brief Release and clean up all Skylicht Engine core resources and managers.
	 *
	 * Call this function before exiting your application to ensure proper resource management and avoid memory leaks.
	 */
	SKYLICHT_API void releaseSkylicht();

	/**
	 * @brief Update input managers and tween system, clear debug scenes.
	 *
	 * This function should be called every frame in your application's main loop.
	 * It updates touch, accelerometer, joystick, and tween managers, and clears debug visualizations.
	 */
	SKYLICHT_API void updateSkylicht();

	/**
	 * @brief Get the currently active Irrlicht device.
	 * @return Pointer to the IrrlichtDevice instance used by Skylicht Engine.
	 */
	SKYLICHT_API IrrlichtDevice* getIrrlichtDevice();

	/**
	 * @brief Get the video driver used for rendering.
	 * @return Pointer to the IVideoDriver instance.
	 */
	SKYLICHT_API IVideoDriver* getVideoDriver();

	/**
	 * @brief Get the current time step in milliseconds, scaled by the time scale.
	 *
	 * If fixed time step is enabled, returns the fixed time step instead.
	 * @return The time step value in milliseconds.
	 */
	SKYLICHT_API float getTimeStep();

	/**
	 * @brief Set the time step for the current frame.
	 *
	 * @param timestep The time step in milliseconds.
	 */
	SKYLICHT_API void setTimeStep(float timestep);

	/**
	 * @brief Set the total elapsed application time in milliseconds.
	 *
	 * @param t The total time value to set.
	 */
	SKYLICHT_API void setTotalTime(float t);

	/**
	 * @brief Get the current time step in milliseconds, without time scale applied.
	 * @return The raw (unscaled) time step value in milliseconds.
	 */
	SKYLICHT_API float getNonScaledTimestep();

	/**
	 * @brief Get the total elapsed application time in milliseconds.
	 * @return The total time since application start.
	 */
	SKYLICHT_API float getTotalTime();

	/**
	 * @brief Enable or disable the use of fixed time step for updates.
	 *
	 * When enabled, the engine will use a fixed time step for all updates instead of the dynamically calculated time step.
	 * @param b True to enable fixed time step, false to disable.
	 */
	SKYLICHT_API void enableFixedTimeStep(bool b);

	/**
	 * @brief Set the value of the fixed time step in milliseconds.
	 *
	 * @param s The fixed time step to use.
	 */
	SKYLICHT_API void setFixedTimeStep(float s);

	/**
	 * @brief Set the time scale multiplier for time-based updates.
	 *
	 * This allows for slow-motion or speed-up effects by scaling the time step value.
	 * @param scale The time scale multiplier.
	 */
	SKYLICHT_API void setTimeScale(float scale);

	/**
	 * @brief Get the current time scale multiplier.
	 *
	 * @return The time scale value.
	 */
	SKYLICHT_API float getTimeScale();
}