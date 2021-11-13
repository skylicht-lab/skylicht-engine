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
#include "CDragger.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDragger::CDragger(CBase* parent) :
				CBase(parent),
				m_target(parent),
				m_pressed(false),
				m_callBeginMove(false),
				m_clampInsideParent(true),
				m_clampDragPosInsideParent(true),
				m_enableDrag(true),
				m_isMoved(false)
			{

			}

			CDragger::~CDragger()
			{

			}

			void CDragger::onMouseMoved(float x, float y, float deltaX, float deltaY)
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

				if (m_clampInsideParent)
					m_target->moveTo(p.X, p.Y);
				else
					m_target->dragTo(p.X, p.Y, m_holdPosition.X, m_holdPosition.Y, height(), m_clampDragPosInsideParent);

				m_isMoved = true;
				m_target->onMoved();
			}

			void CDragger::onMouseClickLeft(float x, float y, bool down)
			{
				if (m_disabled)
					return;

				m_pressed = down;

				if (m_pressed == true)
				{
					m_holdPosition = m_target->canvasPosToLocal(SPoint(x, y));
					CInput::getInput()->setCapture(this);
					m_callBeginMove = true;
				}
				else
				{
					CInput::getInput()->setCapture(NULL);
					m_callBeginMove = false;

					if (m_isMoved)
						m_target->onEndMoved();
				}

				m_isMoved = false;
			}

			void CDragger::dragMoveCommand(const SPoint& mouseOffset)
			{
				CInput* input = CInput::getInput();

				SPoint p = input->getMousePosition();
				SPoint localPos = m_target->getParent()->canvasPosToLocal(p);

				m_holdPosition = mouseOffset;

				m_target->moveTo(localPos.X - m_holdPosition.X, localPos.Y - m_holdPosition.Y);

				m_pressed = true;
				m_callBeginMove = true;

				input->setCapture(this);
			}
		}
	}
}