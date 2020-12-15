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
#include "CCheckBox.h"
#include "GUI//Renderer/CRenderer.h"
#include "GUI/Theme/CTheme.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CCheckBox::CCheckBox(CBase* parent) :
				CButton(parent)
			{
				setSize(15.0f, 15.0f);
				setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				setIsToggle(true);

				OnDown = BIND_LISTENER(&CCheckBox::onClickDown, this);

				m_label->setHidden(true);
			}

			CCheckBox::~CCheckBox()
			{

			}

			void CCheckBox::renderUnder()
			{
				CButton::renderUnder();

				if (m_toggleStatus)
				{
					const SPoint& renderOffset = CRenderer::getRenderer()->getRenderOffset();

					SPoint r;
					r.X = renderOffset.X - 2.0f;
					r.Y = renderOffset.Y - 3.0f;

					CRenderer::getRenderer()->setRenderOffset(r);
					CTheme::getTheme()->drawIcon(getRenderBounds(), ESystemIcon::Check, CThemeConfig::DefaultIconColor, false);
					CRenderer::getRenderer()->setRenderOffset(renderOffset);
				}
			}

			void CCheckBox::onClickDown(CBase* sender)
			{
				bool b = !m_toggleStatus;
				setToggle(b);

				if (OnChanged != nullptr)
					OnChanged(sender);
			}
		}
	}
}