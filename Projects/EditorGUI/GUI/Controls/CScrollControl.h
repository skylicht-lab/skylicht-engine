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
#include "CScrollBar.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CScrollControl : public CBase
			{
			protected:
				CScrollBar* m_vertical;
				CScrollBar* m_horizontal;

				bool m_enableMouseWheelScroll;
				bool m_canScrollV;
				bool m_canScrollH;

				float m_widthScrollExpand;
				float m_heightScrollExpand;

				bool m_modifyChildWidth;

				int m_autoScrollState;
				float m_lastTime;
			public:
				CScrollControl(CBase* parent);

				~CScrollControl();

				virtual void think();

				virtual void layout();

				virtual void onChildBoundsChanged(const SRect& oldChildBounds, CBase* child);

				virtual bool onMouseWheeled(int delta);

				virtual void onScrollBarV(CBase* base);

				virtual void onScrollBarH(CBase* base);

				virtual void onDragItemOver(float x, float y);

				inline void stopAutoScroll()
				{
					m_autoScrollState = 0;
				}

				void scrollToItem(CBase* item);

				void showScrollBar(bool h, bool v);

				void enableScroll(bool h, bool v);

				void enableModifyChildWidth(bool b)
				{
					m_modifyChildWidth = b;
				}

				float getInnerWidth();

				float getInnerHeight();

				float getScrollVertical();

				float getScrollHorizontal();

				void setScrollVertical(float y);

				void setScrollHorizontal(float x);

				inline CScrollBar* getVerticalScroll()
				{
					return m_vertical;
				}

				inline CScrollBar* getHorizontalScroll()
				{
					return m_horizontal;
				}

				void scrollVerticalOffset(float offset);

				void scrollHorizontalOffset(float offset);

				Listener OnLayout;

				inline void enableMouseWheelScroll(bool b)
				{
					m_enableMouseWheelScroll = b;
				}

			protected:

				void updateScrollBar();

			};
		}
	}
}