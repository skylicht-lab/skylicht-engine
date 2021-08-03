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
#include "CSpaceConsole.h"

#include "ConsoleLog/CConsoleLog.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceConsole::CSpaceConsole(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_lastID(0)
		{
			m_textControl = new GUI::CTextBox(window);
			m_textControl->dock(GUI::EPosition::Fill);
			m_textControl->setWrapMultiline(true);
			m_textControl->showScrollBar(false, true);
			m_textControl->setEditable(false);
			m_textControl->enableDrawTextBox(false);
		}

		CSpaceConsole::~CSpaceConsole()
		{

		}

		void CSpaceConsole::update()
		{
			CSpace::update();

			CConsoleLog* console = CConsoleLog::getInstance();
			if (console->getLogCount() > 0)
			{
				const CConsoleLog::SLogInfo& log = console->getLast();
				if (log.ID != m_lastID)
				{
					const std::string& buffer = console->getBuffer(true);
					std::wstring stringw(buffer.begin(), buffer.end());
					m_textControl->setString(stringw);
					m_textControl->getVerticalSroll()->setScroll(1.0f);
					m_lastID = log.ID;
				}
			}
		}
	}
}