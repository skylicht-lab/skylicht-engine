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
#include "CColorHuePicker.h"
#include "CColorHSVRGBPicker.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CColorHuePicker::CColorHuePicker(CBase* parent) :
				CRawImage(parent),
				m_h(0),
				m_mouseDown(false)
			{
				enableClip(true);
				setMouseInputEnabled(true);
			}

			CColorHuePicker::~CColorHuePicker()
			{

			}

			void CColorHuePicker::render()
			{
				CRawImage::render();

				CRenderer* renderer = CRenderer::getRenderer();

				SGUIColor black(255, 0, 0, 0);
				SGUIColor white(255, 255, 255, 255);

				SRect r;
				r.X = 0;
				r.Y = (float)(255 - m_h) - 1.0f;
				r.Width = width();
				r.Height = 3;
				renderer->drawFillRect(r, black);

				r.X = 0;
				r.Y = (float)(255 - m_h);
				r.Width = width();
				r.Height = 1;
				renderer->drawFillRect(r, white);
			}

			void CColorHuePicker::onMouseClickLeft(float x, float y, bool down)
			{
				m_mouseDown = down;
				CInput::getInput()->setCapture(m_mouseDown ? this : NULL);

				SPoint in = SPoint(x, y);
				SPoint point = canvasPosToLocal(in);

				m_h = 255 - (int)point.Y;
				if (m_h < 0)
					m_h = 0;
				if (m_h > 255)
					m_h = 255;

				((CColorHSVRGBPicker*)m_parent)->changeHue(m_h);
			}

			void CColorHuePicker::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				if (m_mouseDown)
				{
					SPoint in = SPoint(x, y);
					SPoint point = canvasPosToLocal(in);

					m_h = 255 - (int)point.Y;
					if (m_h < 0)
						m_h = 0;
					if (m_h > 255)
						m_h = 255;

					((CColorHSVRGBPicker*)m_parent)->changeHue(m_h);
				}
			}
		}
	}
}