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
#include "CScrollControl.h"
#include "CDataHeader.h"

#define GRIDVIEW_MAX_COLUMN 20

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CDataGridView : public CBase
			{
			protected:
				int m_numColumn;

				float m_resizeWidth;

				float m_width[GRIDVIEW_MAX_COLUMN];

				CDataHeader* m_header;
				CScrollControl* m_view;

			public:
				CDataGridView(CBase* parent, int numColumn);

				virtual ~CDataGridView();

				virtual void layout();

				virtual void postLayout();

				void unSelectAll();

				void removeAllItem();

				CDataHeader* getHeader()
				{
					return m_header;
				}

				inline int getNumColumn()
				{
					return m_numColumn;
				}

				void setColumnWidth(CBase* control, int c, float w);

				inline float getColumnWidth(int c)
				{
					return m_width[c];
				}

				inline float getResizerWidth()
				{
					return m_resizeWidth;
				}

				virtual bool onKeyUp(bool down);

				virtual bool onKeyDown(bool down);

				virtual bool onKeyHome(bool down);

				virtual bool onKeyEnd(bool down);

			public:

				Listener OnSelected;
				Listener OnUnselected;
				Listener OnSelectChange;
				Listener OnItemContextMenu;

			protected:

				virtual void onItemDown(CBase* item);

			};
		}
	}
}