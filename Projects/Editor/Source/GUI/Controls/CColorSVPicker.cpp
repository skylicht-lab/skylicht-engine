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
#include "CColorSVPicker.h"
#include "CColorHSVRGBPicker.h"
#include "GUI/Renderer/CRenderer.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CColorSVPicker::CColorSVPicker(CBase* parent) :
				CRawImage(parent),
				m_s(0),
				m_v(0),
				m_mouseDown(false)
			{
				enableClip(true);
				setMouseInputEnabled(true);
			}

			CColorSVPicker::~CColorSVPicker()
			{

			}

			void CColorSVPicker::render()
			{
				CRawImage::render();

				CRenderer* renderer = CRenderer::getRenderer();
				SGUIColor white(255, 255, 255, 255);
				SGUIColor black(255, 0, 0, 0);

				SRect r;
				r.X = (float)m_s - 4.0f;
				r.Y = (float)(255 - m_v) - 4.0f;
				r.Width = 8.0f;
				r.Height = 8.0f;
				renderer->drawBorderRect(r, white, true, true, true, true);

				r.X = (float)m_s - 5.0f;
				r.Y = (float)(255 - m_v) - 5.0f;
				r.Width = 10.0f;
				r.Height = 10.0f;
				renderer->drawBorderRect(r, black, true, true, true, true);
			}

			void CColorSVPicker::onMouseClickLeft(float x, float y, bool down)
			{
				m_mouseDown = down;
				CInput::getInput()->setCapture(m_mouseDown ? this : NULL);

				SPoint in = SPoint(x, y);
				SPoint point = canvasPosToLocal(in);

				m_s = (int)point.X;
				m_v = 255 - (int)point.Y;

				if (m_s < 0)
					m_s = 0;
				if (m_s > 255)
					m_s = 255;

				if (m_v < 0)
					m_v = 0;
				if (m_v > 255)
					m_v = 255;

				((CColorHSVRGBPicker*)m_parent)->changeSV(m_s, m_v);
			}

			void CColorSVPicker::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				if (m_mouseDown)
				{
					SPoint in = SPoint(x, y);
					SPoint point = canvasPosToLocal(in);

					m_s = (int)point.X;
					m_v = 255 - (int)point.Y;

					if (m_s < 0)
						m_s = 0;
					if (m_s > 255)
						m_s = 255;

					if (m_v < 0)
						m_v = 0;
					if (m_v > 255)
						m_v = 255;

					((CColorHSVRGBPicker*)m_parent)->changeSV(m_s, m_v);
				}
			}
		}
	}
}