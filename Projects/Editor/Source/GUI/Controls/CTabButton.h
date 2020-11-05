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
#include "CButton.h"
#include "CIconButton.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTabControl;

			class CTabButton :public CButton
			{
			protected:
				CTabControl *m_control;
				CBase *m_page;

				bool m_focus;
				bool m_showCloseButton;

				SGUIColor m_focusColor;

				CIconButton *m_close;

				SPoint m_holdPosition;

				SPoint m_tabPosition;
				SPoint m_tabStripPosition;

				bool m_dragOutTabStrip;
				bool m_enableRenderOver;

			public:
				CTabButton(CTabControl *control, CBase *parent, CBase *page);

				virtual ~CTabButton();

				virtual void onMouseMoved(float x, float y, float deltaX, float deltaY);

				virtual void onMouseClickLeft(float x, float y, bool down);

				virtual void renderUnder();

				virtual void renderOver();

				virtual void sizeToContents();

				void cancelDragCommand();

				const SPoint& getDragHoldPosition()
				{
					return m_holdPosition;
				}

				CBase* getPage()
				{
					return m_page;
				}

				CIconButton* getCloseButton()
				{
					return m_close;
				}

				inline void showCloseButton(bool b)
				{
					m_showCloseButton = b;
					m_close->setHidden(!b);
					sizeToContents();
				}

				inline void setFocus(bool b)
				{
					m_focus = b;
				}

				inline bool isFocus()
				{
					return m_focus;
				}

				inline void setFocusColor(const SGUIColor& c)
				{
					m_focusColor = c;
				}

				const SGUIColor& getFocusColor()
				{
					return m_focusColor;
				}
			};
		}
	}
}