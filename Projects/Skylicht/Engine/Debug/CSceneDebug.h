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

#include "Utils/CSingleton.h"
#include "TextBillboard/CTextBillboardManager.h"

namespace Skylicht
{
	/**
	 * @brief Structure to hold debug line information.
	 */
	struct SLineDebug
	{
		core::line3df line;
		SColor color;
	};

	/**
	 * @brief Structure to hold debug linestrip information.
	 */
	struct SLineStripDebug
	{
		core::array<core::vector3df> point;
		SColor color;
	};

	/**
	 * @brief Structure to hold debug box information.
	 */
	struct SBoxDebug
	{
		core::aabbox3df box;
		SColor color;
	};

	/**
	 * @brief Structure to hold debug triangle information.
	 */
	struct STriDebug
	{
		core::triangle3df tri;
		SColor color;
	};

	/// @brief A helper class that allows you to draw lines, circles, text... etc., for debugging.
	/// @ingroup Debug
	/// 
	/// You can draw debug visualizations by calling addLine, addText, addCircle, etc., in each loop.
	/// The debug data added will be automatically cleared every frame.
	/// 
	/// @code
	/// CSceneDebug* debug = CSceneDebug::getInstance();
	/// 
	/// // Use debugNoZ for drawing that ignores depth and appears in front of all other 3D objects.
	/// CSceneDebug* debugNoZ = debug->getNoZDebug();
	/// debugNoZ->addText(core::vector3df(0.0f, 1.0f, 0.0f), "Debug string 3d", SColor(255,255,255,255));
	/// debugNoZ->addText2D(core::vector2df(10.0f, 10.0f), "Debug string 2d", SColor(255,255,255,255));
	/// @endcode
	class SKYLICHT_API CSceneDebug
	{
	public:
		DECLARE_SINGLETON(CSceneDebug)

	protected:
		/// Array of debug lines
		core::array<SLineDebug*> m_lines;

		/// Array of debug linestrips
		core::array<SLineStripDebug*> m_linestrip;

		/// Array of debug boxes
		core::array<SBoxDebug*> m_boxs;

		/// Array of debug triangles
		core::array<STriDebug*> m_tri;

		/// Array of debug text billboards
		core::array<CRenderTextData*> m_texts;

		///< Dedicated CSceneDebug instance for "No Z Debug" visualization (renders without depth testing)
		CSceneDebug* m_noZDebug;

	public:
		/**
		 * @brief Constructor. Initializes internal arrays.
		 */
		CSceneDebug();

		/**
		 * @brief Destructor. Cleans up all debug primitives and the noZDebug instance.
		 */
		virtual ~CSceneDebug();

		/**
		 * @brief Returns the singleton instance dedicated for "No Z Debug" visualization.
		 *
		 * "No Z Debug" means drawing debug primitives that ignore 3D object depth, allowing them to be rendered through all geometry (no depth testing).
		 * This is useful for visualizing guides or markers regardless of occlusion by scene objects.
		 *
		 * Creates a new instance if it doesn't exist.
		 * @return Pointer to the noZDebug instance.
		 */
		CSceneDebug* getNoZDebug();

		/**
		 * @brief Adds a debug circle to the scene.
		 * @param pos Center position of the circle.
		 * @param radius Radius of the circle.
		 * @param normal Normal vector defining the circle's orientation.
		 * @param color Color of the circle.
		 */
		void addCircle(const core::vector3df& pos, float radius, const core::vector3df& normal, const SColor& color);

		/**
		 * @brief Adds a debug ellipse to the scene.
		 * @param pos Center position of the ellipse.
		 * @param radiusZ Radius along the Z axis.
		 * @param radiusX Radius along the X axis.
		 * @param normal Normal vector defining the ellipse's orientation.
		 * @param color Color of the ellipse.
		 */
		void addEclipse(const core::vector3df& pos, float radiusZ, float radiusX, const core::vector3df& normal, const SColor& color);

		/**
		 * @brief Adds a debug triangle to the scene.
		 * @param tri The triangle geometry.
		 * @param color Color of the triangle.
		 */
		void addTri(const core::triangle3df& tri, const SColor& color);

		/**
		 * @brief Adds a debug line to the scene.
		 * @param line Line geometry.
		 * @param color Color of the line.
		 */
		void addLine(const core::line3df& line, const SColor& color);

		/**
		 * @brief Adds a debug line to the scene.
		 * @param v1 First endpoint of the line.
		 * @param v2 Second endpoint of the line.
		 * @param color Color of the line.
		 */
		void addLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color);

		/**
		 * @brief Adds a debug linestrip (connected line segments) to the scene.
		 * @param point Array of points forming the linestrip.
		 * @param color Color of the linestrip.
		 */
		void addLinestrip(const core::array<core::vector3df>& point, const SColor& color);

		/**
		 * @brief Adds a debug linestrip to the scene.
		 * @param point Irrlicht array of points forming the linestrip.
		 * @param color Color of the linestrip.
		 */
		void addLinestrip(const std::vector<core::vector3df>& point, const SColor& color);

		/**
		 * @brief Adds a debug axis-aligned bounding box to the scene.
		 * @param box Bounding box geometry.
		 * @param color Color of the box.
		 */
		void addBoudingBox(const core::aabbox3df& box, const SColor& color);

		/**
		 * @brief Adds a debug bounding box with a transformation applied.
		 *        The box is drawn with its edges transformed by the given matrix.
		 * @param box Bounding box geometry.
		 * @param color Color of the box.
		 * @param mat Transformation matrix.
		 */
		void addTransformBBox(const core::aabbox3df& box, const SColor& color, const core::matrix4& mat);

		/**
		 * @brief Adds a debug sphere (as two circles) to the scene.
		 * @param pos Center position of the sphere.
		 * @param radius Radius of the sphere.
		 * @param color Color of the sphere.
		 */
		void addSphere(const core::vector3df& pos, float radius, const SColor& color);

		/**
		 * @brief Adds debug axis lines to visualize a position and orientation.
		 * @param pos Center position.
		 * @param length Length of each axis line.
		 * @param color Color of the axes.
		 */
		void addPosition(const core::vector3df& pos, float length, const SColor& color);

		/**
		 * @brief Adds debug axis lines at a transformed position (visualizes transform matrix axes).
		 *        X (red), Y (green), Z (blue).
		 * @param mat Transformation matrix.
		 * @param vectorLength Length of each axis line.
		 */
		void addTransform(const core::matrix4& mat, float vectorLength);

		/**
		 * @brief Adds a 3D debug text billboard at the given scene position.
		 * @param pos 3D position for the text.
		 * @param string Text to display.
		 * @param color Text color.
		 */
		void addText(const core::vector3df& pos, const char* string, const SColor& color);

		/**
		 * @brief Adds a 2D debug text billboard at the given screen position.
		 * @param pos 2D screen position for the text.
		 * @param string Text to display.
		 * @param color Text color.
		 */
		void addText2D(const core::vector2df& pos, const char* string, const SColor& color);

		/**
		 * @brief Clears all debug text billboards.
		 */
		void clearText();

		void clear()
		{
			clearLines();
			clearLineStrip();
			clearBoxs();
			clearTri();
			clearText();
		}

		/**
		 * @brief Clears all debug lines.
		 */
		void clearLines();

		/**
		 * @brief Clears all debug linestrips.
		 */
		void clearLineStrip();

		/**
		 * @brief Clears all debug bounding boxes.
		 */
		void clearBoxs();

		/**
		 * @brief Clears all debug triangles.
		 */
		void clearTri();

		/**
		 * @brief Get the number of debug lines currently stored.
		 * @return Number of debug lines.
		 */
		inline u32 getLinesCount()
		{
			return m_lines.size();
		}

		/**
		 * @brief Get the number of debug linestrips currently stored.
		 * @return Number of debug linestrips.
		 */
		inline u32 getLineStripCount()
		{
			return m_linestrip.size();
		}

		/**
		 * @brief Get the number of debug boxes currently stored.
		 * @return Number of debug boxes.
		 */
		inline u32 getBoxCount()
		{
			return m_boxs.size();
		}

		/**
		 * @brief Get the number of debug triangles currently stored.
		 * @return Number of debug triangles.
		 */
		inline u32 getTriCount()
		{
			return m_tri.size();
		}

		/**
		 * @brief Get a reference to the debug line at the specified index.
		 *
		 * This function allows direct inspection or rendering of a specific debug line primitive currently stored.
		 * @param i Index of the debug line in the internal array.
		 * @return Constant reference to the SLineDebug structure.
		 */
		inline const SLineDebug& getLine(int i)
		{
			return *m_lines[i];
		}

		/**
		 * @brief Get a reference to the debug linestrip at the specified index.
		 *
		 * This function allows direct access to a specific debug linestrip (sequence of connected points) for custom processing or visualization.
		 * @param i Index of the linestrip in the internal array.
		 * @return Constant reference to the SLineStripDebug structure.
		 */
		inline const SLineStripDebug& getLineStrip(int i)
		{
			return *m_linestrip[i];
		}

		/**
		 * @brief Get a reference to the debug box at the specified index.
		 *
		 * Allows direct access to a specific debug bounding box for inspection or custom rendering.
		 * @param i Index of the box in the internal array.
		 * @return Constant reference to the SBoxDebug structure.
		 */
		inline const SBoxDebug& getBox(int i)
		{
			return *m_boxs[i];
		}

		/**
		 * @brief Get a reference to the debug triangle at the specified index.
		 *
		 * Allows direct access to a specific debug triangle for inspection or custom rendering.
		 * @param i Index of the triangle in the internal array.
		 * @return Constant reference to the STriDebug structure.
		 */
		inline const STriDebug& getTri(int i)
		{
			return *m_tri[i];
		}
	};
}