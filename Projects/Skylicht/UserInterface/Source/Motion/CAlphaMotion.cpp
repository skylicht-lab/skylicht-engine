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
#include "CAlphaMotion.h"

namespace Skylicht
{
	namespace UI
	{
		CAlphaMotion::CAlphaMotion(float alpha) :
			m_alpha(alpha),
			m_defaultAlpha(1.0f),
			m_toDefault(false)
		{

		}

		CAlphaMotion::CAlphaMotion() :
			m_alpha(1.0f),
			m_defaultAlpha(1.0f),
			m_toDefault(true)
		{

		}

		CAlphaMotion::~CAlphaMotion()
		{

		}

		void CAlphaMotion::init(CGUIElement* gui)
		{
			CMotion::init(gui);

			const SColor& c = gui->getColor();
			m_defaultAlpha = c.getAlpha() / 255.0f;
		}

		void CAlphaMotion::start()
		{
			if (m_toDefault)
			{
				const SColor& c = m_gui->getColor();
				float alpha = c.getAlpha() / 255.0f;
				m_tween = new CTweenFloat(alpha, m_defaultAlpha, m_duration);
			}
			else
			{
				m_tween = new CTweenFloat(m_defaultAlpha, m_alpha, m_duration);
			}

			m_tween->setDelay(m_delay);
			m_tween->OnUpdate = [&](CTween* t)
				{
					float alpha = ((CTweenFloat*)t)->getValue();
					SColor c = m_gui->getColor();
					c.setAlpha((u32)(alpha * 255.0f));
					m_gui->setColor(c);
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;
				};

			CTweenManager::getInstance()->addTween(m_tween);
		}
	}
}