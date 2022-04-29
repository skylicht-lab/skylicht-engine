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
#include "CDataHeader.h"
#include "CDataGridView.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDataHeader::CDataHeader(CDataGridView* view) :
				CBase(view),
				m_gridView(view)
			{
				setHeight(24.0f);
				dock(EPosition::Top);

				for (int i = 0, n = view->getNumColumn(); i < n; i++)
				{
					CFlatButton* btn = new CFlatButton(this);
					btn->setLabelColor(CThemeConfig::DefaultTextColor);
					btn->setHoverColor(CThemeConfig::HeaderHover);
					btn->setBorderColor(CThemeConfig::HeaderBorder);
					btn->enableRenderBorder(false, true, false, true);
					btn->enableDrawBackground(false);
					m_label.push_back(btn);
				}

				for (int i = 0, n = view->getNumColumn(); i < n; i++)
				{
					if (i != n - 1 && n >= 2)
					{
						CResizerLR* resize = new CResizerLR(m_label[i]);
						resize->setMargin(SMargin(0.0f, 0.0f, -9.0f));
						resize->setRenderBorderPadding(5.0f);
						resize->dock(EPosition::Right);
						resize->setResizeDir(EPosition::Right);
						resize->setSize(m_gridView->getResizerWidth(), height());
						resize->setTarget(m_label[i]);
						resize->setTarget2(m_label[i + 1]);
					}
				}

				m_fillRectColor = CThemeConfig::Header;
			}

			CDataHeader::~CDataHeader()
			{

			}

			void CDataHeader::setLabel(int col, const std::wstring& label)
			{
				m_label[col]->setLabel(label);
				m_label[col]->setHeight(height());
			}

			void CDataHeader::renderUnder()
			{
				const SRect& r = getRenderBounds();
				CRenderer* renderer = CRenderer::getRenderer();

				renderer->drawFillRect(getRenderBounds(), m_fillRectColor);

				SRect border = r;
				border.Height = 1.0f;
				renderer->drawFillRect(border, CThemeConfig::HeaderBorder);

				border.Y = height() - 1.0f;
				renderer->drawFillRect(border, CThemeConfig::HeaderBorder);
			}

			void CDataHeader::layout()
			{
				CBase::layout();

				float resizeWidth = m_gridView->getResizerWidth();
				float x = 0.0f;
				for (int i = 0, n = m_gridView->getNumColumn(); i < n; i++)
				{
					float width = m_gridView->getColumnWidth(i);

					m_label[i]->setPos(x, 0.0f);
					m_label[i]->setWidth(width);

					if (m_label[i]->OnResize == nullptr)
						m_label[i]->OnResize = BIND_LISTENER(&CDataHeader::onHeaderResize, this);

					x = x + width;
				}
			}

			void CDataHeader::onHeaderResize(CBase* label)
			{
				int n = (int)m_label.size();
				for (int i = 0; i < n; i++)
				{
					if (m_label[i] == label)
					{
						m_gridView->setColumnWidth(i, label->width());
						return;
					}
				}
			}
		}
	}
}