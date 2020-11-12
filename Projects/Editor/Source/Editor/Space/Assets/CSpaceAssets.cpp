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
#include "CSpaceAssets.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceAssets::CSpaceAssets(GUI::CDockableWindow *window, CEditor *editor) :
			CSpace(window, editor)
		{
			GUI::CSplitter *spliter = new GUI::CSplitter(window);
			spliter->dock(GUI::EPosition::Fill);
			spliter->setNumberRowCol(1, 2);

			m_folder = new GUI::CTreeControl(spliter);

			GUI::CTreeNode *root = m_folder->addNode(L"../Assets", GUI::ESystemIcon::OpenFolder);
			root->expand();

			GUI::CTreeNode *child2 = root->addNode(L"Child 2", GUI::ESystemIcon::OpenFolder);
			child2->addNode(L"Child a", GUI::ESystemIcon::Folder);
			child2->addNode(L"Child b", GUI::ESystemIcon::Folder);
			child2->expand();

			for (int i = 0; i < 20; i++)
			{
				root->addNode(L"Child _", GUI::ESystemIcon::Folder);
			}

			spliter->setControl(m_folder, 0, 0);


			GUI::CScrollControl *scroll = new GUI::CScrollControl(spliter);
			spliter->setControl(scroll, 0, 1);

			spliter->setColWidth(0, 300.0f);
			spliter->setWeakCol(1);

			GUI::CBase *test = new GUI::CBase(scroll);
			test->setBounds(GUI::SRect(0.0f, 0.0f, 2000.0f, 2000.0f));
		}

		CSpaceAssets::~CSpaceAssets()
		{

		}
	}
}