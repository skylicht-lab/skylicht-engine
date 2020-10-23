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
				m_pressed(false)
			{

			}

			CDragger::CDragger(CBase* parent, float x, float y, float w, float h) :
				CBase(parent),
				m_target(parent),
				m_pressed(false)
			{
				setBounds(x, y, w, h);
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

				SPoint p = SPoint(x - m_holdPosition.X, y - m_holdPosition.Y);

				if (m_target->getParent() != NULL)
					p = m_target->getParent()->canvasPosToLocal(p);

				m_target->dragTo(p.X, p.Y, m_holdPosition.X, m_holdPosition.Y);
			}

			void CDragger::onMouseClickLeft(float x, float y, bool bDown)
			{
				if (m_disabled)
					return;

				m_pressed = bDown;

				if (m_pressed == true)
				{
					m_holdPosition = m_target->canvasPosToLocal(SPoint(x, y));
					CInput::getInput()->setCapture(this);
				}
				else
				{
					CInput::getInput()->setCapture(NULL);
				}
			}
		}
	}
}