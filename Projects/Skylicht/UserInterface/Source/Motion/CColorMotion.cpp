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
#include "CColorMotion.h"

#include "Tween/CTweenColor.h"

namespace Skylicht
{
	namespace UI
	{
		CColorMotion::CColorMotion(const SColor& color) :
			m_color(color)
		{

		}

		CColorMotion::CColorMotion()
		{
			m_toDefault = true;
		}

		CColorMotion::~CColorMotion()
		{

		}

		void CColorMotion::init(CGUIElement* gui)
		{
			CMotion::init(gui);
			m_defaultColor = gui->getColor();
		}

		void CColorMotion::start()
		{
			if (m_toDefault)
				m_tween = new CTweenColor(m_gui->getColor(), m_defaultColor, m_duration);
			else
			{
				if (m_inverseMotion)
					m_tween = new CTweenColor(m_color, m_defaultColor, m_duration);
				else
					m_tween = new CTweenColor(m_defaultColor, m_color, m_duration);
			}

			m_tween->setDelay(m_delay);
			m_tween->setEase(m_ease);
			m_tween->OnUpdate = [&](CTween* t)
				{
					m_gui->setColor(((CTweenColor*)t)->getValue());
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;
				};

			CTweenManager::getInstance()->addTween(m_tween);
		}
	}
}