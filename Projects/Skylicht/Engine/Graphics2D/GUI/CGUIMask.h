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
	/// @brief This is the object class that represents a masked rectangle, and other GUIs will only display within that rectangle. Any drawing outside of this mask rectangle will be clipped.
	/// @ingroup GUI
	/// 
	/// @code
	/// CCanvas *canvas = gameobject->addComponent<CCanvas>();
	/// 
	/// core::rectf maskRect(50.0f, 50.0f, 100.0f, 100.0f);
	/// CGUIMask* mask = canvas->createMask(maskRect);
	/// 
	/// core::rectf r(0.0f, 0.0f, 200.0f, 200.0f);
	/// CGUIImage* gui = canvas->createImage(r);
	/// gui->setImage(CTextureManager::getInstance()->getTexture("BuiltIn/Textures/Skylicht.png"));
	/// gui->setMask(mask);
	/// @endcode
	class SKYLICHT_API CGUIMask : public CGUIElement
	{
		friend class CCanvas;

		CGUIMask(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

		bool m_drawMask;
		core::vector3df m_topLeft;
		core::vector3df m_bottomRight;

	public:
		virtual ~CGUIMask();

		virtual void update(CCamera* camera);

		virtual void render(CCamera* camera);

		void applyParentClip(CGUIMask* parent);

		void beginMaskTest(CCamera* camera);

		void drawMask(CCamera* camera);

		void endMaskTest();

		inline void clearMask()
		{
			m_drawMask = false;
		}

	protected:

		void updateClipRect(CCamera* camera);

		DECLARE_GETTYPENAME(CGUIMask)
	};
}