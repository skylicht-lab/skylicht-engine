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
#pragma once

#include "CBase.h"
#include "CDockHintIcon.h"
#include "CSplitter.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CDockableWindow;

			class CDockPanel : public CBase
			{
			public:
				enum EDock
				{
					DockCenter,
					DockLeft,
					DockRight,
					DockTop,
					DockBottom
				};

			protected:
				CSplitter *m_mainSpliter;

				CBase *m_hintWindow;

				CDockHintIcon *m_dockHint[9];
				CDockHintIcon *m_hoverHint;

			public:
				static void setRootPanel(CDockPanel *panel);

				static CDockPanel* getRootPanel();

			public:
				CDockPanel(CBase *parent);

				virtual ~CDockPanel();

				bool isRoot();

				virtual void layout();

				void showDockHint();

				void hideDockHint();

				CDockHintIcon* hitTestDockHint(const SPoint& mousePoint);

				void showDockHintWindow(CDockHintIcon *hint, CDockableWindow *window);

				void hideDockHintWindow();

				void dockChildWindow(CDockableWindow *window, EDock dock);

				SRect getSpaceBounds(CSplitter* &outSpliter);

			protected:

				SRect getSpaceBounds(CSplitter *spliter, float x, float y, CSplitter* &outSpliter);

			};
		}
	}
}