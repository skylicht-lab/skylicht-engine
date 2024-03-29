/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "Editor/CEditor.h"
#include "Editor/Space/Sprite/CSpaceSprite.h"
#include "GUI/Utils/CDragAndDrop.h"
#include "CSpriteController.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CSpriteController);

		CSpriteController::CSpriteController()
		{
			CAssetManager::getInstance()->registerFileLoader("spritedata", this);
		}

		CSpriteController::~CSpriteController()
		{
			CAssetManager::getInstance()->unRegisterFileLoader("spritedata", this);
		}

		void CSpriteController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
		}

		void CSpriteController::loadFile(const std::string& path)
		{
			CEditor* editor = CEditor::getInstance();

			CSpace* space = editor->getWorkspaceByName(std::wstring(L"Sprite"));
			if (!space)
				space = editor->openWorkspace(std::wstring(L"Sprite"));
			else
			{
				GUI::CDockableWindow* dockWindow = dynamic_cast<GUI::CDockableWindow*>(space->getWindow());
				if (dockWindow != NULL)
				{
					GUI::CDockTabControl* dockTab = dockWindow->getCurrentDockTab();
					if (dockTab != NULL)
					{
						// activate
						dockTab->setCurrentWindow(dockWindow);
						GUI::CDragAndDrop::cancel();
					}
				}
			}

			if (space)
			{
				CSpaceSprite* spaceSprite = dynamic_cast<CSpaceSprite*>(space);
				spaceSprite->openSprite(path);
			}
		}
	}
}