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
#include "CResizableControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CResizableControl::CResizableControl(CCanvas* parent, float x, float y, float w, float h) :
				CBase(parent)
			{
				setBounds(x, y, w, h);

				m_resizers[0] = new CResizer(this);
				m_resizers[0]->dock(EPosition::Bottom);
				m_resizers[0]->setResizeDir(EPosition::Bottom);
				m_resizers[0]->setTarget(this);

				// bottom left
				m_resizers[1] = new CResizer(m_resizers[0]);
				m_resizers[1]->dock(EPosition::Left);
				m_resizers[1]->setResizeDir(EPosition::Bottom | EPosition::Left);
				m_resizers[1]->setTarget(this);

				// bottom right
				m_resizers[2] = new CResizer(m_resizers[0]);
				m_resizers[2]->dock(EPosition::Right);
				m_resizers[2]->setResizeDir(EPosition::Bottom | EPosition::Right);
				m_resizers[2]->setTarget(this);

				// top
				m_resizers[3] = new CResizer(this);
				m_resizers[3]->dock(EPosition::Top);
				m_resizers[3]->setResizeDir(EPosition::Top);
				m_resizers[3]->setTarget(this);

				// top left
				m_resizers[4] = new CResizer(m_resizers[3]);
				m_resizers[4]->dock(EPosition::Left);
				m_resizers[4]->setResizeDir(EPosition::Top | EPosition::Left);
				m_resizers[4]->setTarget(this);

				// top right
				m_resizers[5] = new CResizer(m_resizers[3]);
				m_resizers[5]->dock(EPosition::Right);
				m_resizers[5]->setResizeDir(EPosition::Top | EPosition::Right);
				m_resizers[5]->setTarget(this);

				// left
				m_resizers[6] = new CResizer(this);
				m_resizers[6]->dock(EPosition::Left);
				m_resizers[6]->setResizeDir(EPosition::Left);
				m_resizers[6]->setTarget(this);

				// right
				m_resizers[7] = new CResizer(this);
				m_resizers[7]->dock(EPosition::Right);
				m_resizers[7]->setResizeDir(EPosition::Right);
				m_resizers[7]->setTarget(this);
			}

			CResizableControl::~CResizableControl()
			{
			}

			void CResizableControl::setResizable(bool b)
			{
				for (int i = 0; i < 8; i++)
					m_resizers[i]->setDisabled(!b);
			}
		}
	}
}