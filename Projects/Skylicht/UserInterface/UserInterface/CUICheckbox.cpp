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
#include "CUICheckbox.h"
#include "Tween/CTweenManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUICheckbox::CUICheckbox(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_checked(NULL),
			m_toggleStatus(false),
			m_tween(NULL)
		{
			if (m_element)
			{
				CCanvas* canvas = getCanvas();
				m_background = canvas->getGUIByPath(element, "Background");
				m_checked = canvas->getGUIByPath(element, "Checked");
				m_checked->setVisible(false);
			}
		}

		CUICheckbox::~CUICheckbox()
		{
			stopTween();
		}

		void CUICheckbox::stopTween()
		{
			if (m_tween)
			{
				m_tween->stop();
				m_tween = NULL;
			}
		}

		void CUICheckbox::playTween()
		{
			stopTween();

			if (m_toggleStatus)
			{
				m_tween = new CTweenFloat(0.0f, 1.0f, 200.0f);
				m_checked->setVisible(true);
			}
			else
			{
				m_tween = new CTweenFloat(1.0f, 0.0f, 200.0f);
			}

			m_tween->setEase(EEasingFunctions::EaseOutCubic);
			m_tween->OnUpdate = [&](CTween* t)
				{
					float alpha = ((CTweenFloat*)t)->getValue();
					SColor c = m_checked->getColor();
					c.setAlpha((u32)(alpha * 255.0f));
					m_checked->setColor(c);

					core::vector3df scale(1.0f, 1.0f, 1.0f);
					float s = (0.5f + alpha * 0.5f);
					scale.X = scale.X * s;
					scale.Y = scale.Y * s;
					m_checked->setScale(scale);
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;

					if (!m_toggleStatus)
						m_checked->setVisible(false);
				};
			CTweenManager::getInstance()->addTween(m_tween);
		}

		void CUICheckbox::setToggle(bool b, bool invokeEvent, bool doAnimation)
		{
			m_toggleStatus = b;

			if (m_checked)
			{
				if (doAnimation)
				{
					playTween();
				}
				else
				{
					stopTween();
					if (m_toggleStatus)
						m_checked->setVisible(true);
					else
						m_checked->setVisible(false);
				}
			}

			if (invokeEvent)
			{
				if (OnChanged != nullptr)
					OnChanged(this, m_toggleStatus);
			}
		}

		void CUICheckbox::onPressed()
		{
			m_toggleStatus = !m_toggleStatus;

			if (m_checked)
				playTween();

			if (OnChanged != nullptr)
				OnChanged(this, m_toggleStatus);
		}
	}
}