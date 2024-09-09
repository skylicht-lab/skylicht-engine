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
#include "CPositionMotion.h"

namespace Skylicht
{
	namespace UI
	{
		CPositionMotion::CPositionMotion(const core::vector3df& position) :
			m_position(position)
		{

		}

		CPositionMotion::CPositionMotion(float x, float y, float z) :
			m_position(x, y, z)
		{
		}

		CPositionMotion::CPositionMotion()
		{
			m_toDefault = true;
		}

		CPositionMotion::~CPositionMotion()
		{

		}

		void CPositionMotion::init(CGUIElement* gui)
		{
			CMotion::init(gui);
			m_defaultPosition = gui->getPosition();
		}

		void CPositionMotion::start()
		{
			if (m_toDefault)
				m_tween = new CTweenVector3df(m_gui->getPosition(), m_defaultPosition, m_duration);
			else
			{
				if (m_inverseMotion)
					m_tween = new CTweenVector3df(m_defaultPosition + m_position, m_defaultPosition, m_duration);
				else
					m_tween = new CTweenVector3df(m_defaultPosition, m_defaultPosition + m_position, m_duration);
			}

			m_tween->setDelay(m_delay);
			m_tween->setEase(m_ease);
			m_tween->OnUpdate = [&](CTween* t)
				{
					m_gui->setPosition(((CTweenVector3df*)t)->getValue());
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;
				};

			CTweenManager::getInstance()->addTween(m_tween);
		}
	}
}