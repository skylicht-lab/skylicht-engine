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

#include "CGUIElement.h"

namespace Skylicht
{
	/**
	 * @brief This is the object class that represents a masked rectangle, and other GUIs will only display within that rectangle. Any drawing outside of this mask rectangle will be clipped.
	 * @ingroup GUI
	 *
	 * @code
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 *
	 * core::rectf maskRect(50.0f, 50.0f, 100.0f, 100.0f);
	 * CGUIMask* mask = canvas->createMask(maskRect);
	 *
	 * core::rectf r(0.0f, 0.0f, 200.0f, 200.0f);
	 * CGUIImage* gui = canvas->createImage(r);
	 * gui->setImage(CTextureManager::getInstance()->getTexture("BuiltIn/Textures/Skylicht.png"));
	 * gui->setMask(mask);
	 * @endcode
	 */
	class SKYLICHT_API CGUIMask : public CGUIElement
	{
		friend class CCanvas;

		/**
		 * @brief Construct a mask element with an initial clip rectangle.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 */
		CGUIMask(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

		bool m_drawMask;
		core::vector3df m_topLeft;
		core::vector3df m_bottomRight;

		int m_vertexColorShader;

		bool m_circleMask;
		float m_a;
		float m_b;

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUIMask();

		/**
		 * @brief Update the mask clip rectangle before rendering.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void update(CCamera* camera);

		/**
		 * @brief Render the mask element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Apply a parent mask so this mask is clipped by the parent clip region.
		 * @param parent Parent mask to clip against.
		 */
		void applyParentClip(CGUIMask* parent);

		/**
		 * @brief Begin stencil or clip testing for this mask.
		 * @param camera Camera used for GUI rendering.
		 */
		void beginMaskTest(CCamera* camera);

		/**
		 * @brief Draw the geometry that defines the mask region.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void drawMask(CCamera* camera);

		/**
		 * @brief End stencil or clip testing for this mask.
		 */
		void endMaskTest();

		/**
		 * @brief Clear the pending draw-mask flag.
		 */
		inline void clearMask()
		{
			m_drawMask = false;
		}

		/**
		 * @brief Enable or disable circular mask rendering.
		 * @param b True to render the mask as a circle or ellipse.
		 */
		inline void enableCircleMask(bool b)
		{
			m_circleMask = b;
		}

		/**
		 * @brief Set the angular range used to fill the circular mask.
		 * @param a Start angle in degrees.
		 * @param b End angle in degrees.
		 */
		inline void setFillAngle(float a, float b)
		{
			m_a = a;
			m_b = b;
		}

		/**
		 * @brief Create a serializable object that stores this mask state.
		 * @return Serializable object.
		 */
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load mask state from a serializable object.
		 * @param object Serializable object to read from.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

	protected:

		/**
		 * @brief Recalculate the world-space clip rectangle for this mask.
		 * @param camera Camera used for GUI rendering.
		 */
		void updateClipRect(CCamera* camera);

		DECLARE_GETTYPENAME(CGUIMask)
	};
}
