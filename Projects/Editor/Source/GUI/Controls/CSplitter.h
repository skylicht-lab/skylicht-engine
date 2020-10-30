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

#include "GUI/Controls/CBase.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{

#define MAX_SPLITER_ROW 20
#define MAX_SPLITER_COL 20

			class CSplitter : public CBase
			{
			public:
				enum EHoverState
				{
					None = 0,
					Row = (1 << 1),
					Col = (1 << 2),
					RowAndCol = (Row | Col)
				};

			protected:
				u32 m_row;
				u32 m_col;

				CBase *m_control[MAX_SPLITER_COL][MAX_SPLITER_ROW];

				float m_rowHeight[MAX_SPLITER_ROW];
				float m_colWidth[MAX_SPLITER_COL];

				float m_adjustRowHeight[MAX_SPLITER_ROW];
				float m_adjustColWidth[MAX_SPLITER_COL];

				float m_expanderSize;
				float m_minSize;

				bool m_pressed;
				u32 m_dragRow;
				u32 m_dragCol;
				EHoverState m_hitState;

				float m_lastSpaceWidth;
				float m_lastSpaceHeight;

				u32 m_weakCol;
				u32 m_weakRow;

				bool m_firstTimeLayout;

			public:
				CSplitter(CBase* parent);

				virtual ~CSplitter();

				virtual void renderUnder();

				virtual void layout();

				void setControl(CBase *base, u32 row, u32 col);

				void setNumberRowCol(u32 row, u32 col);

				void setRowHeight(u32 row, float height);

				void setColWidth(u32 col, float width);

				inline void setExpanderSize(float size)
				{
					m_expanderSize = size;
				}

				inline void setMinSize(float size)
				{
					m_minSize = size;
				}

				inline void setWeakRow(u32 r)
				{
					m_weakRow = r;
				}

				inline void setWeakCol(u32 c)
				{
					m_weakCol = c;
				}

				inline int getMaxRow() { return MAX_SPLITER_ROW; }

				inline int getMaxCol() { return MAX_SPLITER_COL; }

				virtual void onMouseMoved(float x, float y, float deltaX, float deltaY);

				virtual void onMouseClickLeft(float x, float y, bool down);

			protected:

				void predictChildSize();

				void predictChildWidth(float spaceWidth);

				void predictChildHeight(float spaceHeight);

				void updateWidthSmaller(float spaceWidth, float delta);

				void updateHeightSmaller(float spaceHeight, float delta);

				void updateWidthLarger();

				void updateHeightLarger();

				void saveUserExpectedSize();

				void getWeakPriorityRow(std::list < std::pair<u32, float>>& row, bool inverse = false);

				void getWeakPriorityCol(std::list < std::pair<u32, float>>& col, bool inverse = false);

			protected:

				EHoverState mouseHittest(float x, float y, u32& outRow, u32 &outCol);

				SRect getCellRect(u32 fromRow, u32 fromCol, u32 toRow, u32 toCol);
			};
		}
	}
}