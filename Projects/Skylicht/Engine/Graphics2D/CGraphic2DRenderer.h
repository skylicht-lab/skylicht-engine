/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#include "Entity/IRenderSystem.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	/// @brief This is a Render System object class that supports 2D drawing.
	/// @ingroup Graphics2D
	/// 
	/// CGraphic2DRenderer is used to draw 2D graphics by setting up an orthogonal projection.
	/// It saves and restores the projection and view matrices so that 2D rendering does not interfere with 3D rendering.
	/// 
	/// You can find more information in the CTextBilboardRenderer class, which inherits from it.
	class SKYLICHT_API CGraphic2DRenderer : public IRenderSystem
	{
	private:
		/// Saved projection matrix before 2D rendering.
		core::matrix4 m_saveProjection;

		/// Saved view matrix before 2D rendering.
		core::matrix4 m_saveView;

	public:
		/**
		 * @brief Constructor.
		 */
		CGraphic2DRenderer();

		/**
		 * @brief Destructor.
		 */
		virtual ~CGraphic2DRenderer();

		/**
		* @brief Begin 2D rendering by setting an orthogonal projection matrix.
		*        Saves the previous projection and view matrices.
		* @param width The width of the 2D rendering area.
		* @param height The height of the 2D rendering area.
		*/
		void beginRender2D(float width, float height);


		/**
		 * @brief End 2D rendering and restore the previous projection and view matrices.
		 */
		void endRender2D();
	};
}
