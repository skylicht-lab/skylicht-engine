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

			GUI::CSplitter *spliter = new GUI::CSplitter(window1);
			spliter->dock(GUI::EPosition::Fill);
			spliter->setNumberRowCol(4, 1);

			GUI::CTabControl *tab1 = new GUI::CTabControl(window1);
			GUI::CTabControl *tab2 = new GUI::CTabControl(window1);
			GUI::CTabControl *tab3 = new GUI::CTabControl(window1);
			GUI::CTabControl *tab4 = new GUI::CTabControl(window1);

			spliter->setControl(tab1, 0, 0);
			spliter->setControl(tab2, 1, 0);
			spliter->setControl(tab3, 2, 0);
			spliter->setControl(tab4, 3, 0);

			GUI::CBase *p1 = tab1->addPage(L"Scene", NULL);
			GUI::CBase *p2 = tab1->addPage(L"Particle", NULL);
			GUI::CBase *p3 = tab1->addPage(L"Animation", NULL);

			GUI::CBase *p4 = tab2->addPage(L"Animator", NULL);
			GUI::CBase *p5 = tab2->addPage(L"GUI", NULL);

			GUI::CBase *p6 = tab3->addPage(L"Animator", NULL);
			GUI::CBase *p7 = tab4->addPage(L"GUI", NULL);

			/*
			GUI::CButton *button = new GUI::CButton(p1);
			button->setLabel(L"Test Button");
			button->sizeToContents();
			*/

			/*
			GUI::CLabel *l2 = new GUI::CLabel(p2);
			GUI::CLabel *l3 = new GUI::CLabel(p3);
			GUI::CLabel *l4 = new GUI::CLabel(p4);
			GUI::CLabel *l5 = new GUI::CLabel(p5);

			l1->setString(L"1");
			l1->sizeToContents();

			l2->setString(L"2");
			l2->sizeToContents();

			l3->setString(L"3");
			l3->sizeToContents();

			l4->setString(L"4");
			l4->sizeToContents();

			l5->setString(L"5");
			l5->sizeToContents();
			*/

			tab1->showTabCloseButton(true);
			tab2->showTabCloseButton(true);

			GUI::CWindow *window2 = new GUI::CWindow(canvas, 400.0f, 100.0f, 600.0f, 480.0f);
			GUI::CWindow *window3 = new GUI::CWindow(window2, 20.0f, 20.0f, 300.0f, 240.0f);
		}

		CEditor::~CEditor()
		{

		}
	}
}