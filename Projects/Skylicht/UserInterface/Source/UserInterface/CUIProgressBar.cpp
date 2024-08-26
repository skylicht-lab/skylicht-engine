/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "CUIProgressBar.h"

namespace Skylicht
{
	namespace UI
	{
		CUIProgressBar::CUIProgressBar(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_loading(NULL),
			m_mask(NULL)
		{
			CCanvas* canvas = getCanvas();

			if (m_element && canvas)
			{
				m_background = canvas->getGUIByPath(element, "Background");
				m_loading = canvas->getGUIByPath(element, "Loading");

				m_mask = m_element->getCanvas()->createMask(m_element, m_element->getRect());
				element->bringToNext(m_mask, m_background, true);

				m_loading->setMask(m_mask);
			}
		}

		CUIProgressBar::~CUIProgressBar()
		{

		}

		void CUIProgressBar::setPercent(float f)
		{
			core::rectf maskRect = m_element->getRect();
			maskRect.LowerRightCorner.X = maskRect.getWidth() * f;
			m_mask->setRect(maskRect);
		}
	}
}