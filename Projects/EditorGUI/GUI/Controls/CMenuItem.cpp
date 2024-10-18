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
#include "CMenuItem.h"
#include "CMenu.h"
#include "CCanvas.h"
#include "CIcon.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMenuItem::CMenuItem(CBase *parent) :
				CButton(parent),
				m_accelerator(NULL),
				m_menu(NULL),
				m_submenuArrow(NULL),
				m_inMenuBar(false),
				m_isOpen(false)
			{
				setLabelColor(CThemeConfig::DefaultTextColor);
			}

			CMenuItem::~CMenuItem()
			{
			}

			void CMenuItem::layout()
			{
				CButton::layout();
				if (m_submenuArrow)
					m_submenuArrow->setPos(width() - m_submenuArrow->width(), m_submenuArrow->Y());
			}

			void CMenuItem::setLabelColor(const SGUIColor& color)
			{
				CButton::setLabelColor(color);
			}

			void CMenuItem::sizeToContents()
			{
				CButton::sizeToContents();

				if (m_accelerator != NULL)
				{
					m_accelerator->sizeToContents();
					setWidth(width() + m_accelerator->width());
				}
			}

			void CMenuItem::renderUnder()
			{
				if (!m_drawBackground || m_disabled)
				{
					if (m_disabled)
					{
						setLabelColor(CThemeConfig::DisableTextColor);
					}

					return;
				}

				if (isHovered() || m_isOpen)
				{
					SGUIColor c;
					c = m_pressColor;

					setLabelColor(CThemeConfig::ButtonTextColor);

					if (m_inMenuBar)
						CTheme::getTheme()->drawButton(getRenderBounds(), c);
					else
					{
						SRect bound = getRenderBounds();
						bound.X = 2.0f;
						bound.Y = 1.0f;
						bound.Height = 18.0f;
						bound.Width = bound.Width - 4.0f;
						CRenderer::getRenderer()->drawFillRect(bound, c);
					}
				}
				else
				{
					setLabelColor(CThemeConfig::DefaultTextColor);
				}
			}

			void CMenuItem::setAccelerator(const std::wstring& accelerator)
			{
				if (m_accelerator != NULL)
				{
					m_accelerator->remove();
					m_accelerator = NULL;
				}

				if (accelerator.empty())
					return;

				m_accelerator = new CTextContainer(this);
				m_accelerator->dock(EPosition::Right);
				m_accelerator->setMargin(SMargin(0.0f, 3.0f, 23.0f, 0.0f));
				m_accelerator->setColor(CThemeConfig::MenuItemAcceleratorColor);
				m_accelerator->setString(accelerator);
				m_accelerator->sizeToContents();
			}

			CMenu* CMenuItem::getMenu()
			{
				if (!m_menu)
				{
					m_menu = new CMenu(getCanvas());
					m_menu->setHidden(true);

					if (!m_inMenuBar)
					{
						m_submenuArrow = new CIcon(this, ESystemIcon::VRight);
						m_submenuArrow->setColor(CThemeConfig::MenuItemAcceleratorColor);
					}

					invalidate();
				}

				return m_menu;
			}

			void CMenuItem::openMenu()
			{
				if (m_menu == NULL)
					return;

				m_isOpen = true;

				m_menu->setHidden(false);
				m_menu->bringToFront();

				if (m_menu->OnOpen != nullptr)
					m_menu->OnOpen(m_menu);

				SPoint position = localPosToCanvas();

				if (m_inMenuBar)
				{
					m_menu->setPos(position.X, position.Y = height() + 2.0f);
				}
				else
				{
					m_menu->setPos(position.X + width(), position.Y - 2.0f);
				}
			}

			void CMenuItem::closeMenu()
			{
				if (m_menu == NULL)
					return;

				m_isOpen = false;
				m_menu->closeMenu();
				m_menu->setHidden(true);
			}
		}
	}
}