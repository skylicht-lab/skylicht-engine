/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CProgressBar.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CProgressBar::CProgressBar(CBase* parent) :
				CBase(parent),
				m_percent(1.0f)
			{
				setSize(100.0f, 20.0f);

				m_text = new CLabel(this);
				m_text->dock(EPosition::Fill);
				m_text->setMargin(SMargin(0.0f, 3.0f, 0.0f, 0.0f));
				m_text->setTextAlignment(ETextAlign::TextCenter);

				setPercent(1.0f);
			}

			CProgressBar::~CProgressBar()
			{

			}

			void CProgressBar::setPercent(float percent)
			{
				m_percent = percent;
				if (m_percent < 0.0f)
					m_percent = 0.0f;
				if (m_percent > 1.0f)
					m_percent = 1.0f;

				int p = (int)(m_percent * 100.0f);
				wchar_t text[32];
				swprintf(text, 32, L"%d%%", p);
				m_text->setString(text);
			}

			void CProgressBar::renderUnder()
			{
				CTheme* theme = CTheme::getTheme();
				CRenderer* renderer = CRenderer::getRenderer();

				theme->drawTextBox(getRenderBounds(), CThemeConfig::SliderColor);

				SRect clip = m_bounds;
				clip.Width = clip.Width * m_percent;

				if (clip.Width > 0.0f)
				{
					const SRect oldClipRect = renderer->clipRegion();
					bool isEnableClip = renderer->isEnableClip();

					renderer->addClipRegion(clip);
					renderer->enableClip(true);
					renderer->startClip();

					theme->drawTextBox(getRenderBounds(), CThemeConfig::SliderBarColor);

					renderer->setClipRegion(oldClipRect);
					renderer->enableClip(isEnableClip);
					renderer->startClip();
				}

				CBase::renderUnder();
			}
		}
	}
}