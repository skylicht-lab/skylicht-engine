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

#include "pch.h"
#include "CUIButton.h"

namespace Skylicht
{
	namespace UI
	{
		CUIButton::CUIButton(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_text(NULL),
			m_isToggle(false),
			m_toggleStatus(false)
		{
			CCanvas* canvas = getCanvas();
			if (canvas)
			{
				m_background = canvas->getGUIByPath(element, "Background");
				m_text = dynamic_cast<CGUIText*>(canvas->getGUIByPath(element, "Text"));
			}
		}

		CUIButton::~CUIButton()
		{

		}

		void CUIButton::setLabel(const char* string)
		{
			if (m_text)
				m_text->setText(string);
		}

		void CUIButton::setLabel(const wchar_t* string)
		{
			if (m_text)
				m_text->setText(string);
		}

		void CUIButton::setLabelAndChangeWidth(const char* string, float addition)
		{
			if (m_text)
			{
				int stringWidth = m_text->getStringWidth(string);
				m_text->setText(string);
				m_element->setWidth((float)stringWidth + addition);
			}
		}

		void CUIButton::setLabelAndChangeWidth(const wchar_t* string, float addition)
		{
			if (m_text)
			{
				int stringWidth = m_text->getStringWidth(string);
				m_text->setText(string);
				m_element->setWidth((float)stringWidth + addition);
			}
		}

		void CUIButton::setToggle(bool b, bool invokeEvent)
		{
			m_toggleStatus = b;
			if (invokeEvent && OnToggle != nullptr)
				OnToggle(this, b);
		}

		void CUIButton::onPressed()
		{
			CUIBase::onPressed();
			if (m_isToggle)
				setToggle(!m_toggleStatus);
		}

		void CUIButton::onPointerHover(float pointerX, float pointerY)
		{
			if (m_isToggle && m_toggleStatus)
			{
				m_isPointerHover = true;
				if (OnPointerHover != nullptr)
					OnPointerHover(pointerX, pointerY);
			}
			else
			{
				CUIBase::onPointerHover(pointerX, pointerY);
			}
		}

		void CUIButton::onPointerOut(float pointerX, float pointerY)
		{
			if (m_isToggle && m_toggleStatus)
			{
				m_isPointerHover = false;
				if (OnPointerOut != nullptr)
					OnPointerOut(pointerX, pointerY);
			}
			else
			{
				CUIBase::onPointerOut(pointerX, pointerY);
			}
		}
	}
}