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
#pragma once

#include "CBase.h"
#include "CRulerBar.h"
#include "CScrollControl.h"
#include "CTimelineItem.h"
#include "CTimelineValue.h"
#include "CTimelineFocus.h"
#include "CContentSizeControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTimeline : public CBase
			{
			protected:
				CRulerBar* m_ruler;

				CTimelineFocus* m_focus;

				CScrollControl* m_scrollPanel;

				CTimelineContainer* m_itemPanel;

				float m_contentWidth;

				float m_focusPosition;

				CScrollBar* m_timelineScrollBar;

				SDimension m_maxSize;

			public:
				CTimeline(CBase* base);

				virtual ~CTimeline();

				CTimelineItem* addItem(float height);

				virtual void render();

				virtual void layout();

				inline void setFocusPosition(float pos)
				{
					m_focusPosition = pos;
				}

				void onTimelineScroll(CBase* base);

				void onTimelineMouseWheeled(int delta);

			protected:

				void updateFocusPosition();

			};
		}
	}
}