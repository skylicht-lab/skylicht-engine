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
#include "AssetManager/CAssetManager.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceAssets::CSpaceAssets(GUI::CDockableWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			toolbar->addButton(L"Add", GUI::ESystemIcon::Plus);
			toolbar->addButton(GUI::ESystemIcon::Setting, true);
			toolbar->addSpace(true);
			m_search = new GUI::CTextBox(toolbar);
			m_search->setWidth(200.0f);
			m_search->showIcon();
			m_search->setStringHint(L"Search");
			toolbar->addControl(m_search, true);

			GUI::CSplitter* spliter = new GUI::CSplitter(window);
			spliter->dock(GUI::EPosition::Fill);
			spliter->setNumberRowCol(1, 2);

			m_folder = new GUI::CTreeControl(spliter);

			GUI::CTreeNode* root = m_folder->addNode(L"../Assets", GUI::ESystemIcon::OpenFolder);
			root->expand();

			CAssetManager* assetManager = CAssetManager::getInstance();
			std::vector<SFileInfo> files;

			// add root to tree folder
			assetManager->getRoot(files);
			for (const SFileInfo& f : files)
			{
				if (f.IsFolder == true)
				{
					root->addNode(f.NameW.c_str(), GUI::ESystemIcon::Folder);
				}
			}

			/*
			GUI::CTreeNode* child2 = root->addNode(L"Child 2", GUI::ESystemIcon::OpenFolder);
			child2->addNode(L"File", GUI::ESystemIcon::File);
			child2->addNode(L"Document", GUI::ESystemIcon::FileDocument);
			child2->addNode(L"Image", GUI::ESystemIcon::FileImage);
			child2->expand();
			for (int i = 0; i < 20; i++)
			{
				root->addNode(L"Child _", GUI::ESystemIcon::Folder);
			}
			*/

			spliter->setControl(m_folder, 0, 0);

			GUI::CListBox* listBox = new GUI::CListBox(spliter);
			spliter->setControl(listBox, 0, 1);
			/*
			for (int i = 0; i < 100; i++)
			{
				if (i != 5)
					listBox->addItem(L"File _", GUI::ESystemIcon::File);
				else
					listBox->addItem(L"File _");
			}
			*/
			spliter->setColWidth(0, 300.0f);
			spliter->setWeakCol(1);
		}

		CSpaceAssets::~CSpaceAssets()
		{

		}
	}
}