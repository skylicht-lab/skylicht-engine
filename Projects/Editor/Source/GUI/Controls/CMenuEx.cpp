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
#include "CMenuEx.h"
#include "CCanvas.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMenuEx::CMenuEx(CBase* parent) :
				CBase(parent),
				m_isOpenSubMenu(false),
				m_isOpenPopup(false)
			{
				setBounds(0.0f, 0.0f, 210, 24.0f);
				setHidden(true);
			}

			CMenuEx::~CMenuEx()
			{

			}

			void CMenuEx::layout()
			{
				float menuHeight = 5.0f;
				bool firstMenu = true;

				for (CBase* menuItem : Children)
				{
					if (menuItem->isHidden())
						continue;

					if (firstMenu == true)
					{
						SMargin margin = menuItem->getMargin();
						margin.Top = 2.0f;
						menuItem->setMargin(margin);

						firstMenu = false;
					}
					menuHeight = menuHeight + menuItem->height();
				}

				// if tall menu descends off screen then clamp to screen height
				if (Y() + menuHeight > getCanvas()->height())
				{
					const float dy = getCanvas()->height() - Y();
					const float ytop = core::max_(Y() - dy, 0.0f);

					setPos(X(), ytop);
					menuHeight = core::min_(menuHeight, getCanvas()->height());
				}

				setSize(width(), menuHeight);

				SRect bound = m_parent->getBounds();
				if (X() + width() > bound.X + bound.Width)
				{
					float newX = bound.X + bound.Width - width();
					setPos(newX, Y());
				}

				CBase::layout();
			}

			void CMenuEx::renderUnder()
			{
				CTheme::getTheme()->drawWindowShadow(getRenderBounds());

				CTheme::getTheme()->drawWindow(getRenderBounds(), true);
			}

			CMenuSeparator* CMenuEx::addSeparator()
			{
				CMenuSeparator* item = new CMenuSeparator(this);
				item->dock(EPosition::Top);
				return item;
			}

			void CMenuEx::open(const SPoint& position)
			{
				setHidden(false);
				setPos(position);

				if (OnOpen != nullptr)
					OnOpen(this);

				bringToFront();
				m_isOpenPopup = true;
			}

			void CMenuEx::close()
			{
				if (m_isOpenPopup == true)
				{
					m_isOpenPopup = false;
					setHidden(true);
				}
			}

			void CMenuEx::closeMenu()
			{
				close();
			}
		}
	}
}