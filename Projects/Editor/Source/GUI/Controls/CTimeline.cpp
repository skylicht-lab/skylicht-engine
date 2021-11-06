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
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"
#include "CTimeline.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTimeline::CTimeline(CBase* base) :
				CBase(base),
				m_contentWidth(250.0f),
				m_cursorPosition(0.0f),
				m_contentPadding(50.0f)
			{
				enableRenderFillRect(true);
				setFillRectColor(CThemeConfig::TimelineItemBG);

				m_ruler = new CRulerBar(this, true);
				m_ruler->dock(EPosition::Top);
				m_ruler->setMargin(SMargin(m_contentWidth, 0.0f, 0.0f, 0.0f));

				m_timelineScrollBar = new CScrollBar(this, true);
				m_timelineScrollBar->dock(EPosition::Bottom);
				m_timelineScrollBar->setMargin(SMargin(m_contentWidth, 0.0f, 0.0f, 0.0f));
				m_timelineScrollBar->OnBarMoved = BIND_LISTENER(&CTimeline::onTimelineScroll, this);
				m_timelineScrollBar->setNudgeAmount(50.0f);

				m_scrollPanel = new CScrollControl(this);
				m_scrollPanel->dock(EPosition::Fill);
				m_scrollPanel->enableRenderFillRect(true);
				m_scrollPanel->setFillRectColor(CThemeConfig::Timeline);
				m_scrollPanel->enableScroll(false, true);
				m_scrollPanel->showScrollBar(false, true);

				m_itemPanel = new CTimelineContainer(m_scrollPanel, this);
				m_itemPanel->dock(EPosition::Fill);
				m_itemPanel->setFitType(CContentSizeControl::WrapChildren, CContentSizeControl::WrapChildren);

				m_cursorLine = new CTimelineFocus(this, this);
			}

			CTimeline::~CTimeline()
			{

			}

			CTimelineItem* CTimeline::addItem(float height)
			{
				CTimelineItem* item = new CTimelineItem(m_itemPanel, this);
				item->dock(EPosition::Top);
				item->setHeight(height);
				item->setMargin(SMargin(0.0f, 0.0f, 0.0f, 3.0f));
				item->setContentSize(m_contentWidth);
				invalidate();
				return item;
			}

			void CTimeline::render()
			{
				CBase::render();
			}

			void CTimeline::layout()
			{
				CBase::layout();

				m_maxSize = SDimension(0.0f, 0.0f);
				for (CBase* child : m_itemPanel->Children)
				{
					CTimelineItem* item = dynamic_cast<CTimelineItem*>(child);
					if (item != NULL)
					{
						CTimelineValue* value = item->getTimelineValue();
						SDimension size = value->getInnerPanel()->getSize();
						if (m_maxSize.Width < size.Width)
							m_maxSize.Width = size.Width;
						if (m_maxSize.Height < size.Height)
							m_maxSize.Height = size.Height;
					}
				}

				m_timelineScrollBar->setContentSize(m_maxSize.Width + m_contentPadding);
				m_timelineScrollBar->setViewableContentSize(width() - m_contentWidth);

				updateCursorPosition();

				// fix the first time layout the value is zero
				if (m_maxSize.Width == 0.0f)
					invalidate();
			}

			void CTimeline::updateCursorPosition()
			{
				float rulerX = m_ruler->getPos().X;

				float position = rulerX + m_ruler->getBeginOffset() + m_cursorPosition;
				m_cursorLine->setPos(position, 0.0f);

				float maxWidth = m_maxSize.Width + m_ruler->getBeginOffset();
				m_cursorLine->setDragLimitRect(SRect(rulerX, 0.0f, maxWidth, 0.0f));

				if (position < rulerX || m_maxSize.Width == 0.0f)
					m_cursorLine->setHidden(true);
				else
					m_cursorLine->setHidden(false);
			}

			float CTimeline::getCursorPosition()
			{
				float rulerX = m_ruler->getPos().X;
				float position = m_cursorLine->getPos().X;

				m_cursorPosition = position - rulerX - m_ruler->getBeginOffset();
				return m_cursorPosition;
			}

			void CTimeline::enableDragCursor(bool b)
			{
				m_cursorLine->enableDrag(b);
			}

			void CTimeline::onTimelineScroll(CBase* base)
			{
				float scroll = m_timelineScrollBar->getScroll();
				float newInnerPanelPosX = -((m_maxSize.Width + m_contentPadding) - width() + m_contentWidth) * scroll;

				for (CBase* child : m_itemPanel->Children)
				{
					CTimelineItem* item = dynamic_cast<CTimelineItem*>(child);
					if (item != NULL)
					{
						CTimelineValue* value = item->getTimelineValue();
						value->getInnerPanel()->setPos(round(newInnerPanelPosX), 0.0f);

						m_ruler->setBeginOffset(newInnerPanelPosX);

						updateCursorPosition();
					}
				}
			}

			void CTimeline::onTimelineMouseWheeled(int delta)
			{
				if (m_timelineScrollBar->canScroll())
					m_timelineScrollBar->setScroll(m_timelineScrollBar->getScroll() + m_timelineScrollBar->getNudgeAmount() * delta);
			}
		}
	}
}