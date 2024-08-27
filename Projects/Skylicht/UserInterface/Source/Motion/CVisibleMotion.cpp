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
#include "CVisibleMotion.h"

namespace Skylicht
{
	namespace UI
	{
		CVisibleMotion::CVisibleMotion(bool visible) :
			m_visible(visible),
			m_defaultVisible(true)
		{

		}

		CVisibleMotion::CVisibleMotion() :
			m_visible(true),
			m_defaultVisible(true)
		{
			m_toDefault = true;
		}

		CVisibleMotion::~CVisibleMotion()
		{

		}

		void CVisibleMotion::init(CGUIElement* gui)
		{
			CMotion::init(gui);
			m_defaultVisible = gui->isVisible();
		}

		void CVisibleMotion::start()
		{
			m_tween = new CTweenFloat(0.0f, 1.0f, m_duration);
			m_tween->setDelay(m_delay);
			m_tween->setEase(m_ease);
			m_tween->OnUpdate = [&](CTween* t)
				{
					// wait delay and update
					if (m_toDefault)
					{
						m_gui->setVisible(m_defaultVisible);
					}
					else
					{
						if (m_inverseMotion)
							m_gui->setVisible(m_defaultVisible);
						else
							m_gui->setVisible(m_visible);
					}
					m_tween->stop();
					m_tween = NULL;
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;
				};

			CTweenManager::getInstance()->addTween(m_tween);
		}
	}
}