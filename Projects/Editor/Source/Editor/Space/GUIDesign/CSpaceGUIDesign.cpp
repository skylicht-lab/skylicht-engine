/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CSpaceGUIDesign.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceGUIDesign::CSpaceGUIDesign(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			m_leftRuler = new GUI::CRulerBar(window, false);
			m_leftRuler->setHeight(100.0f);
			m_leftRuler->setBeginOffset(40.0f);

			m_topRuler = new GUI::CRulerBar(window, true);
			m_topRuler->setWidth(200.0f);

			m_leftRuler->setPosition(0.0f);
			m_topRuler->setPosition(0.0f);
		}

		CSpaceGUIDesign::~CSpaceGUIDesign()
		{

		}
	}
}