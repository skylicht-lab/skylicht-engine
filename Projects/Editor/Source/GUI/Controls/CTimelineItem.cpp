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
#include "CTimelineItem.h"
#include "CTimeline.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTimelineContainer::CTimelineContainer(CBase* parent, CTimeline* timeline) :
				CContentSizeControl(parent),
				m_contentSize(250.0f),
				m_timeline(timeline)
			{

			}

			CTimelineContainer::~CTimelineContainer()
			{

			}

			bool CTimelineContainer::onMouseWheeled(int delta)
			{
				SPoint p = CInput::getInput()->getMousePosition();
				SPoint localPos = m_parent->canvasPosToLocal(p);

				if (localPos.X < m_contentSize)
					return CBase::onMouseWheeled(delta); // fallback to scrollcontrol

				m_timeline->onTimelineMouseWheeled(delta);
				return true;
			}


			CTimelineItem::CTimelineItem(CBase* base, CTimeline* timeline) :
				CBase(base),
				m_contentSize(250.0f),
				m_timeline(timeline)
			{
				m_value = new CTimelineValue(this, timeline);
				m_value->dock(EPosition::Fill);
				m_value->setMargin(SMargin(m_contentSize, 0.0f, 0.0f, 0.0f));
			}

			CTimelineItem::~CTimelineItem()
			{

			}

			void CTimelineItem::setContentSize(float size)
			{
				m_contentSize = size;
				m_value->setMargin(SMargin(m_contentSize, 0.0f, 0.0f, 0.0f));
			}

			void CTimelineItem::render()
			{
				CBase::render();
				CRenderer* renderer = CRenderer::getRenderer();

				SRect bound = getRenderBounds();

				bound.Width = m_contentSize;
				renderer->drawFillRect(bound, CThemeConfig::TimelineItemBG);
				renderer->drawBorderRect(bound, CThemeConfig::TimelineItemBorder, true, true, true, true);
			}
		}
	}
}