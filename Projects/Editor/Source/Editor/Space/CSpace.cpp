/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CSpace.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpace::CSpace(GUI::CDockableWindow *window, CEditor* editor) :
			m_window(window),
			m_editor(editor)
		{
			m_window->OnDestroy = BIND_LISTENER(&CSpace::onDestroy, this);
			m_window->OnResize = BIND_LISTENER(&CSpace::onWindowResize, this);
		}

		CSpace::~CSpace()
		{

		}

		void CSpace::update()
		{

		}

		void CSpace::onDestroy(GUI::CBase *base)
		{
			m_editor->removeWorkspace(this);
		}

		void CSpace::onWindowResize(GUI::CBase *base)
		{
			if (base == m_window)
			{
				onResize(base->width(), base->height());
			}
		}
	}
}