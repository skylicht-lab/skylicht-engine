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
#include "CResizer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CResizer::CResizer(CBase* parent) :
				CDragger(parent)
			{
				setSize(4.0f, 4.0f);
				m_resizeDir = EPosition::Left;
			}

			CResizer::~CResizer()
			{

			}

			void CResizer::setResizeDir(EPosition dir)
			{
				m_resizeDir = dir;

				if (((dir & EPosition::Left) && (dir & EPosition::Top))
					|| ((dir & EPosition::Right) && (dir & EPosition::Bottom)))
					return setCursor(ECursorType::SizeNWSE);

				if (((dir & EPosition::Right) && (dir & EPosition::Top))
					|| ((dir & EPosition::Left) && (dir & EPosition::Bottom)))
					return setCursor(ECursorType::SizeNESW);

				if ((dir & EPosition::Right) || (dir & EPosition::Left))
					return setCursor(ECursorType::SizeWE);

				if ((dir & EPosition::Top) || (dir & EPosition::Bottom))
					return setCursor(ECursorType::SizeNS);
			}

			void CResizer::onMouseMoved(float x, float y, float deltaX, float deltaY)
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

				delta.X = delta.X - x;
				delta.Y = delta.Y - y;

				if (m_resizeDir & EPosition::Left)
				{
					bounds.X = bounds.X - delta.X;
					bounds.Width = bounds.Width + delta.X;

					// Conform to minimum size here so we don't
					// go all weird when we snap it in the base conrt
					if (bounds.Width < pntMin.X)
					{
						const float diff = pntMin.X - bounds.Width;
						bounds.Width = bounds.Width + diff;
						bounds.X = bounds.X - diff;
					}
				}

				if (m_resizeDir & EPosition::Top)
				{
					bounds.Y = bounds.Y - delta.Y;
					bounds.Height = bounds.Height + delta.Y;

					// Conform to minimum size here so we don't
					// go all weird when we snap it in the base conrt
					if (bounds.Height < pntMin.Y)
					{
						const float diff = pntMin.Y - bounds.Height;
						bounds.Height = bounds.Height + diff;
						bounds.Y = bounds.Y - diff;
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

					if (bounds.Width < pntMin.X)
						bounds.Width = pntMin.X;

					diff = diff - bounds.Width;
					m_holdPosition.X = m_holdPosition.X - diff;
				}

				if (m_resizeDir & EPosition::Bottom)
				{
					const float hoff = bounds.Height - m_holdPosition.Y;
					float diff = bounds.Height;
					bounds.Height = cursorPos.Y + hoff;

					if (bounds.Height < pntMin.Y)
						bounds.Height = pntMin.Y;

					diff = diff - bounds.Height;
					m_holdPosition.Y = m_holdPosition.Y - diff;
				}

				m_target->setBounds(bounds);
				m_target->onResized();
			}

			void CResizer::onMouseClickLeft(float x, float y, bool bDown)
			{
				if (!m_target)
					return;

				CDragger::onMouseClickLeft(x, y, bDown);
			}
		}
	}
}