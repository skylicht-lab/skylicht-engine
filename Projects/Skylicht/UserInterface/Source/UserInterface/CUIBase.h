/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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

#include "Graphics2D/GUI/CGUIElement.h"
#include "Graphics2D/CCanvas.h"

#include "Motion/CMotion.h"
#include "Motion/CColorMotion.h"
#include "Motion/CAlphaMotion.h"
#include "Motion/CPositionMotion.h"
#include "Motion/CScaleMotion.h"
#include "Motion/CRotationMotion.h"

namespace Skylicht
{
	namespace UI
	{
		class CUIContainer;

		class CUIBase
		{
		protected:
			CUIContainer* m_container;

			CGUIElement* m_element;

			core::vector3df m_rectTransform[4];

			std::vector<CMotion*> m_motions[(int)EMotionEvent::NumEvent];

		public:
			std::function<void(float, float)> OnPointerHover;
			std::function<void(float, float)> OnPointerOut;
			std::function<void(float, float)> OnPointerDown;
			std::function<void(float, float)> OnPointerUp;

			std::function<void()> OnPressed;

		public:
			CUIBase(CUIContainer* container, CGUIElement* element);

			virtual ~CUIBase();

			inline CGUIElement* getElement()
			{
				return m_element;
			}

			core::vector3df* getRectTransform();

			virtual void onPointerHover(float pointerX, float pointerY);

			virtual void onPointerOut(float pointerX, float pointerY);

			virtual void onPointerDown(float pointerX, float pointerY);

			virtual void onPointerUp(float pointerX, float pointerY);

			virtual void onPressed();

			void startMotion(EMotionEvent event);

			CMotion* addMotion(EMotionEvent event, CMotion* motion);

			bool removeMotion(EMotionEvent event, CMotion* motion);

			void removeMotions(EMotionEvent event);
		};
	}
}
