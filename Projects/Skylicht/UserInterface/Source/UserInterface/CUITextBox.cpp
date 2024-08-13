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
#include "CUIContainer.h"
#include "CUITextBox.h"
#include "CUIEventManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUITextBox::CUITextBox(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_text(NULL)
		{
			CCanvas* canvas = getCanvas();
			if (canvas)
			{
				m_background = canvas->getGUIByPath(element, "Background");
				m_text = dynamic_cast<CGUIText*>(canvas->getGUIByPath(element, "Text"));
			}
		}

		CUITextBox::~CUITextBox()
		{

		}

		void CUITextBox::onPointerHover(float pointerX, float pointerY)
		{
			if (!m_isPointerHover)
			{
				gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
				cursorControl->setActiveIcon(gui::ECI_IBEAM);
			}

			CUIBase::onPointerHover(pointerX, pointerY);
		}

		void CUITextBox::onPointerOut(float pointerX, float pointerY)
		{
			CUIBase::onPointerOut(pointerX, pointerY);

			gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
			cursorControl->setActiveIcon(gui::ECI_NORMAL);
		}

		void CUITextBox::onPointerDown(float pointerX, float pointerY)
		{
			CUIBase::onPointerDown(pointerX, pointerY);

			if (m_text)
			{
				int l, c;
				m_text->getClosestCharacter(pointerX, pointerY, l, c);

				// enable carret
				m_text->showCaret(true);
				m_text->setCaret(l, c);

				CUIEventManager::getInstance()->setFocus(this);
			}
		}

		void CUITextBox::onLostFocus()
		{
			m_text->showCaret(false);
		}
	}
}


