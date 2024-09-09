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
#include "CUISwitch.h"
#include "Tween/CTweenManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUISwitch::CUISwitch(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_handle(NULL),
			m_on(NULL),
			m_toggleStatus(false),
			m_tween(NULL),
			m_onColor(255, 255, 255, 255),
			m_offColor(255, 255, 255, 255)
		{
			if (m_element)
			{
				CCanvas* canvas = getCanvas();
				m_background = canvas->getGUIByPath(element, "Background");
				m_handle = canvas->getGUIByPath(element, "HandleOff");
				m_on = canvas->getGUIByPath(element, "HandleOn");
				m_on->setVisible(false);

				if (m_handle)
					m_offPosition = m_handle->getPosition();

				if (m_on)
					m_onPosition = m_on->getPosition();
			}
		}

		CUISwitch::~CUISwitch()
		{
			stopTween();
		}

		void CUISwitch::stopTween()
		{
			if (m_tween)
			{
				m_tween->stop();
				m_tween = NULL;
			}
		}

		void CUISwitch::playTween()
		{
			stopTween();

			if (m_toggleStatus)
			{
				m_tween = new CTweenVector3df(m_offPosition, m_onPosition, 200.0f);
				m_handle->setColor(m_onColor);
			}
			else
			{
				m_tween = new CTweenVector3df(m_onPosition, m_offPosition, 200.0f);
				m_handle->setColor(m_offColor);
			}

			m_tween->setEase(EEasingFunctions::EaseOutCubic);
			m_tween->OnUpdate = [&](CTween* t)
				{
					CTweenVector3df* tween = (CTweenVector3df*)t;
					m_handle->setPosition(tween->getValue());
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;
				};
			CTweenManager::getInstance()->addTween(m_tween);
		}

		void CUISwitch::setToggle(bool b, bool invokeEvent, bool doAnimation)
		{
			m_toggleStatus = b;

			if (m_handle)
			{
				if (doAnimation)
				{
					playTween();
				}
				else
				{
					stopTween();
					if (m_toggleStatus)
					{
						m_handle->setPosition(m_onPosition);
						m_handle->setColor(m_onColor);
					}
					else
					{
						m_handle->setPosition(m_offPosition);
						m_handle->setColor(m_offColor);
					}
				}
			}

			if (invokeEvent)
			{
				if (OnChanged != nullptr)
					OnChanged(this, m_toggleStatus);
			}
		}

		void CUISwitch::onPressed()
		{
			m_toggleStatus = !m_toggleStatus;

			if (m_handle)
				playTween();

			if (OnChanged != nullptr)
				OnChanged(this, m_toggleStatus);
		}
	}
}