/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CScrollBar.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CScrollBar::CScrollBar(CBase* parent, bool horizontal) :
				CBase(parent),
				m_isHorizontal(horizontal),
				m_viewableContentSize(1.0f),
				m_contentSize(2.0f),
				m_nudgeAmount(50.0f),
				m_scroll(0.0f),
				m_barMinSize(10.0f)
			{
				setSize(6.0f, 6.0f);
				m_bar = new CScrollBarBar(this, horizontal);
				m_bar->OnBarMoved = BIND_LISTENER(&CScrollBar::onBarMoved, this);

				enableRenderFillRect(true);
				setFillRectColor(CThemeConfig::ScrollBarColor);
			}

			CScrollBar::~CScrollBar()
			{

			}

			void CScrollBar::render()
			{
				if (m_bar->isHidden() == false)
					CBase::render();
			}

			void CScrollBar::layout()
			{
				CBase::layout();

				if (m_isHorizontal == true)
				{
					float barWidth = (m_viewableContentSize / m_contentSize) * width();
					float minBarWidth = core::min_<float>(width(), m_barMinSize);
					if (barWidth < m_barMinSize)
						barWidth = m_barMinSize;

					m_bar->setWidth(barWidth);
					m_bar->setHidden(width() <= barWidth);

					float newX = round(m_scroll * (width() - m_bar->width()));
					m_bar->moveTo(newX, m_bar->Y());
				}
				else
				{
					float barHeight = (m_viewableContentSize / m_contentSize) * height();
					float minBarHeight = core::min_<float>(height(), m_barMinSize);
					if (barHeight < m_barMinSize)
						barHeight = m_barMinSize;

					m_bar->setHeight(barHeight);
					m_bar->setHidden(height() <= barHeight);

					float newY = round(m_scroll * (height() - m_bar->height()));
					m_bar->moveTo(m_bar->X(), newY);
				}
			}

			void CScrollBar::setHorizontal()
			{
				m_isHorizontal = true;
				m_bar->setHorizontal();
				invalidate();
			}

			void CScrollBar::setVertical()
			{
				m_isHorizontal = false;
				m_bar->setVertical();
				invalidate();
			}

			float CScrollBar::calculateScrolledAmount()
			{
				if (m_isHorizontal)
					return m_bar->X() / (width() - m_bar->width());
				return m_bar->Y() / (height() - m_bar->height());
			}

			void CScrollBar::onBarMoved(CBase* base)
			{
				setScroll(calculateScrolledAmount());
				invalidateParent();
			}

			void CScrollBar::setScroll(float f)
			{
				m_scroll = f;
				m_scroll = core::clamp(m_scroll, 0.0f, 1.0f);

				if (m_isHorizontal)
				{
					float newX = round(m_scroll * (width() - m_bar->width()));
					m_bar->moveTo(newX, m_bar->Y());
				}
				else
				{
					float newY = round(m_scroll * (height() - m_bar->height()));
					m_bar->moveTo(m_bar->X(), newY);
				}

				if (OnBarMoved != nullptr)
					OnBarMoved(this);
			}

			void CScrollBar::nudgeUp()
			{
				setScroll(m_scroll - getNudgeAmount());
			}

			void CScrollBar::nudgeDown()
			{
				setScroll(m_scroll + getNudgeAmount());
			}

			void CScrollBar::notifyOnBarMoved()
			{
				if (OnBarMoved != nullptr)
					OnBarMoved(this);
			}
		}
	}
}