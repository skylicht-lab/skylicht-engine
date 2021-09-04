/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CResizerLR.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CResizerLR::CResizerLR(CBase* parent) :
				CDragger(parent),
				m_target2(NULL),
				m_renderBorderPadding(0.0f)
			{
				setSize(3.0f, 3.0f);
				m_resizeDir = EPosition::Left;
			}

			CResizerLR::~CResizerLR()
			{

			}

			void CResizerLR::setResizeDir(EPosition dir)
			{
				m_resizeDir = dir;

				if ((dir & EPosition::Right) || (dir & EPosition::Left))
					setCursor(ECursorType::SizeWE);
			}

			void CResizerLR::renderUnder()
			{
				const SRect& r = getRenderBounds();
				CRenderer* renderer = CRenderer::getRenderer();

				SRect border = r;
				border.X = 1.0f;
				border.Width = 1.0f;
				border.Y = m_renderBorderPadding;
				border.Height = border.Height - 2.0f * m_renderBorderPadding;
				renderer->drawFillRect(border, CThemeConfig::HeaderBorder);
			}

			void CResizerLR::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				if (m_disabled)
					return;

				if (!m_target)
					return;

				if (!m_pressed)
					return;

				SRect bounds = m_target->getBounds();
				SPoint pntMin = m_target->getMinimumSize();
				SPoint cursorPos = m_target->canvasPosToLocal(SPoint(x, y));
				SPoint delta = m_target->localPosToCanvas(m_holdPosition);

				SRect bounds2;
				SPoint pntMin2;

				if (m_target2 != NULL)
				{
					bounds2 = m_target2->getBounds();
					pntMin2 = m_target2->getMinimumSize();
				}

				delta.X = delta.X - x;
				delta.Y = delta.Y - y;

				if (m_resizeDir & EPosition::Left)
				{
					bounds.X = bounds.X - delta.X;
					bounds.Width = bounds.Width + delta.X;

					bounds2.Width = bounds2.Width - delta.X;

					// Conform to minimum size here so we don't
					// go all weird when we snap it in the base conrt
					if (bounds.Width < pntMin.X)
					{
						const float diff = pntMin.X - bounds.Width;
						bounds.Width = bounds.Width + diff;
						bounds.X = bounds.X - diff;

						bounds2.Width = bounds2.Width - diff;
					}

					if (m_target2 != NULL)
					{
						if (bounds2.Width < pntMin2.X)
						{
							const float diff = pntMin2.X - bounds2.Width;
							bounds2.Width = bounds2.Width + diff;

							bounds.Width = bounds.Width - diff;
							bounds.X = bounds.X + diff;
						}
					}
				}

				if (m_resizeDir & EPosition::Right)
				{
					// This is complicated.
					// Basically we want to use the HoldPos, so it doesn't snap to the edge
					// of the control
					// But we need to move the HoldPos with the window movement. Yikes.
					// I actually think this might be a big hack around the way this control
					// works with regards to the holdpos being on the parent panel.
					const float woff = bounds.Width - m_holdPosition.X;
					float diff = bounds.Width;
					bounds.Width = cursorPos.X + woff;

					bounds2.X = bounds2.X - delta.X;
					bounds2.Width = bounds2.Width + delta.X;

					if (bounds.Width < pntMin.X)
					{
						const float diff = pntMin.X - bounds.Width;

						bounds.Width = pntMin.X;

						bounds2.X = bounds2.X + diff;
						bounds2.Width = bounds2.Width - diff;
					}

					if (m_target2 != NULL)
					{
						if (bounds2.Width < pntMin2.X)
						{
							const float diff = pntMin2.X - bounds2.Width;
							bounds.Width = bounds.Width - diff;

							bounds2.X = bounds2.X - diff;
							bounds2.Width = pntMin2.X;
						}
					}

					diff = diff - bounds.Width;
					m_holdPosition.X = m_holdPosition.X - diff;
				}

				m_target->setBounds(bounds);
				m_target->onResized();

				if (m_target2 != NULL)
				{
					m_target2->setBounds(bounds2);
					m_target2->onResized();
				}
			}

			void CResizerLR::onMouseClickLeft(float x, float y, bool down)
			{
				if (!m_target)
					return;

				CDragger::onMouseClickLeft(x, y, down);
			}
		}
	}
}