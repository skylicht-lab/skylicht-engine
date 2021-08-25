/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CButton.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTreeNode;

			class CTreeRowItem : public CButton
			{
			protected:
				CBase* m_root;

				float m_itemOffsetX;

				bool m_drawLineTop;

				bool m_drawLineBottom;

				SGUIColor m_lineColor;

			public:
				CTreeRowItem(CBase* base, CBase* root);

				virtual ~CTreeRowItem();

				virtual void renderUnder();

				virtual void renderBackground();

				virtual void onMouseClickRight(float x, float y, bool down);

				CTreeNode* getNode();

				inline void setItemOffset(float x)
				{
					m_itemOffsetX = x;
				}

				inline void enableDrawLine(bool top, bool bottom)
				{
					m_drawLineTop = top;
					m_drawLineBottom = bottom;
				}

				inline void setDrawLineColor(const SGUIColor& c)
				{
					m_lineColor = c;
				}
			};
		}
	}
}