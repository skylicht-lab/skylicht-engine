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

#pragma once

#include "GUI/CGUIContext.h"
#include "GUI/Controls/CDockableWindow.h"
#include "GUI/Controls/CDockTabControl.h"
#include "GUI/Controls/CSplitter.h"

namespace Skylicht
{
	namespace Editor
	{
		class CEditor
		{
		private:
			GUI::CCanvas *m_canvas;
			GUI::CDockPanel *m_dockPanel;

		public:
			CEditor();

			virtual ~CEditor();

			void saveLayout(const std::string& data);

		protected:

			void initDefaultLayout();

			void initSessionLayout(const std::string& data);

			void readDockLayout(io::IXMLReader* xml, GUI::CDockPanel *panel);

			void readSpliterLayout(io::IXMLReader* xml, GUI::CDockPanel *panel, GUI::CSplitter *spliter, bool isHorizontal);

			void readDockTab(io::IXMLReader* xml, GUI::CDockTabControl *tabcontrol);

			void readBound(io::IXMLReader* xml, GUI::CBase *base);
		};
	}
}