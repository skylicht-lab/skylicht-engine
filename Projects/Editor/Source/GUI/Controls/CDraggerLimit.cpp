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
#include "CDraggerLimit.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDraggerLimit::CDraggerLimit(CBase* parent) :CDragger(parent)
			{

			}

			CDraggerLimit::~CDraggerLimit()
			{

			}

			void CDraggerLimit::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				if (m_disabled)
					return;

				if (!m_pressed)
					return;

				if (!m_enableDrag)
					return;

				SPoint p = SPoint(x - m_holdPosition.X, y - m_holdPosition.Y);

				if (m_target->getParent() != NULL)
					p = m_target->getParent()->canvasPosToLocal(p);

				if (m_callBeginMove)
				{
					m_target->onBeginMoved();
					m_callBeginMove = false;
				}

				if (p.X < m_limitRect.X)
					p.X = m_limitRect.X;
				if (p.X > m_limitRect.X + m_limitRect.Width)
					p.X = m_limitRect.X + m_limitRect.Width;
				if (p.Y < m_limitRect.Y)
					p.Y = m_limitRect.Y;
				if (p.Y > m_limitRect.Y + m_limitRect.Height)
					p.Y = m_limitRect.Y + m_limitRect.Height;

				// clamp limit
				if (m_clampInsideParent)
					m_target->moveTo(p.X, p.Y);
				else
					m_target->dragTo(p.X, p.Y, m_holdPosition.X, m_holdPosition.Y, height());

				m_target->onMoved();
			}
		}
	}
}