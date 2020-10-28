/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyRight notice and this permission notice shall be included in all copies or substantial portions of the Software.

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
#include "CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CEditor::CEditor()
		{
			GUI::CCanvas *canvas = GUI::CGUIContext::getRoot();

			GUI::CWindow *window1 = new GUI::CWindow(canvas, 20.0f, 20.0f, 600.0f, 480.0f);
			GUI::CWindow *window2 = new GUI::CWindow(canvas, 400.0f, 20.0f, 600.0f, 480.0f);
			GUI::CWindow *window3 = new GUI::CWindow(window2, 20.0f, 20.0f, 300.0f, 240.0f);
		}

		CEditor::~CEditor()
		{

		}
	}
}