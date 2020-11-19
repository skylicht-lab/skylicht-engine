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
#include "CScrollControl.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CScrollControl::CScrollControl(CBase *parent) :
				CBase(parent),
				m_canScrollV(true),
				m_canScrollH(true),
				m_widthScrollExpand(0.0f),
				m_heightScrollExpand(0.0f)
			{
				setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				m_vertical = new CScrollBar(this, false);
				m_vertical->dock(EPosition::Right);
				m_vertical->OnBarMoved = BIND_LISTENER(&CScrollControl::onScrollBarV, this);

				m_horizontal = new CScrollBar(this, true);
				m_horizontal->dock(EPosition::Bottom);
				m_horizontal->OnBarMoved = BIND_LISTENER(&CScrollControl::onScrollBarH, this);

				m_innerPanel = new CBase(this);
				m_innerPanel->setPos(0.0f, 0.0f);
				m_innerPanel->sendToBack();

				enableRenderFillRect(true);
				setFillRectColor(CThemeConfig::ScrollControlBackgroundColor);
				enableClip(true);
			}

			CScrollControl::~CScrollControl()
			{

			}

			void CScrollControl::layout()
			{
				updateScrollBar();
				CBase::layout();
			}

			bool CScrollControl::onMouseWheeled(int delta)
			{
				if (m_canScrollV || !m_vertical->isHidden())
				{
					m_vertical->setScroll(m_vertical->getScroll() + m_vertical->getNudgeAmount() * delta);
					return true;
				}

				if (m_canScrollH || !m_horizontal->isHidden())
				{
					m_horizontal->setScroll(m_horizontal->getScroll() + m_horizontal->getNudgeAmount() * delta);
					return true;
				}

				return false;
			}

			void CScrollControl::onChildBoundsChanged(const SRect& oldChildBounds, CBase* child)
			{
				invalidate();
			}

			void CScrollControl::onScrollBarV(CBase *base)
			{
				invalidate();
			}

			void CScrollControl::onScrollBarH(CBase *base)
			{
				invalidate();
			}

			void CScrollControl::showScrollBar(bool h, bool v)
			{
				m_vertical->setHidden(!v);
				m_horizontal->setHidden(!h);
			}

			void CScrollControl::enableScroll(bool h, bool v)
			{
				m_canScrollH = h;
				m_canScrollV = v;
			}

			float CScrollControl::getInnerWidth()
			{
				return width() - (m_vertical->isHidden() ? 0 : m_vertical->width());
			}

			float CScrollControl::getInnerHeight()
			{
				return height() - (m_horizontal->isHidden() ? 0 : m_horizontal->height());
			}

			void CScrollControl::scrollVerticalOffset(float offset)
			{
				if (m_canScrollV || !m_vertical->isHidden())
				{
					float horizontalHeight = m_horizontal->isVisible() ? m_horizontal->height() : 0.0f;
					float content = (m_innerPanel->height() - height() + horizontalHeight);

					float newScroll = m_vertical->getScroll() + offset / content;
					m_vertical->setScroll(newScroll);
				}
			}

			void CScrollControl::scrollHorizontalOffset(float offset)
			{
				if (m_canScrollH || !m_horizontal->isHidden())
				{
					float verticalWidth = m_vertical->isVisible() ? m_vertical->width() : 0.0f;
					float content = (m_innerPanel->width() - width() + verticalWidth);

					float newScroll = m_horizontal->getScroll() + offset / content;
					m_horizontal->setScroll(newScroll);
				}
			}

			void CScrollControl::updateScrollBar()
			{
				if (m_innerPanel == NULL)
					return;

				float childrenWidth = 0.0f;
				float childrenHeight = 0.0f;

				float w = getInnerWidth();
				float h = getInnerHeight();

				// Get the max size of all our children together
				for (CBase* child : m_innerPanel->Children)
				{
					childrenWidth = core::max_(childrenWidth, child->right());
					childrenHeight = core::max_(childrenHeight, child->bottom());
				}

				m_innerPanel->setSize(
					core::max_(w, childrenWidth) + m_widthScrollExpand,
					core::max_(h, childrenHeight) + m_heightScrollExpand);

				m_vertical->setContentSize(m_innerPanel->height());
				m_vertical->setViewableContentSize(h);

				m_horizontal->setContentSize(m_innerPanel->width());
				m_horizontal->setViewableContentSize(w);

				float newInnerPanelPosX = 0.0f;
				float newInnerPanelPosY = 0.0f;

				if (m_canScrollV || !m_vertical->isHidden())
				{
					float horizontalHeight = m_horizontal->isVisible() ? m_horizontal->height() : 0.0f;
					newInnerPanelPosY = -(m_innerPanel->height() - height() + horizontalHeight) * m_vertical->getScroll();
				}

				if (m_canScrollH || !m_horizontal->isHidden())
				{
					float verticalWidth = m_vertical->isVisible() ? m_vertical->width() : 0.0f;
					newInnerPanelPosX = -(m_innerPanel->width() - width() + verticalWidth) * m_horizontal->getScroll();
				}

				m_innerPanel->setPos(round(newInnerPanelPosX), round(newInnerPanelPosY));
			}
		}
	}
}