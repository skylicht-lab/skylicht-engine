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
#include "GUI/GUIContext.h"
#include "CSpaceLoadScene.h"
#include "Scene/CSceneImporter.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceLoadScene::CSpaceLoadScene(GUI::CWindow* window, CEditor* editor) :
			CSpaceLoading(window, editor),
			m_progressBar(NULL),
			m_statusText(NULL),
			m_startLoading(false)
		{
			m_progressBar = new GUI::CProgressBar(window);
			m_progressBar->dock(GUI::EPosition::Top);
			m_progressBar->setMargin(GUI::SMargin(14.0f, 14.0, 14.0, 0.0f));

			m_statusText = new GUI::CLabel(window);
			m_statusText->dock(GUI::EPosition::Fill);
			m_statusText->setMargin(GUI::SMargin(14.0f, 5.0, 14.0, 0.0f));
			m_statusText->setWrapMultiline(true);
			m_statusText->setString(L"Open Scene...");
		}

		CSpaceLoadScene::~CSpaceLoadScene()
		{

		}

		void CSpaceLoadScene::update()
		{
			CSpace::update();

			if (m_startLoading)
			{
				if (!CSceneImporter::beginImportScene(CSceneController::getInstance()->getScene(), m_scenePath.c_str()))
				{
					// load scene fail
					m_finished = true;
				}
				m_startLoading = false;
			}

			m_progressBar->setPercent(CSceneImporter::getLoadingPercent());

			if (m_finished == false)
			{
				m_finished = CSceneImporter::updateLoadScene();
			}

			if (m_finished == true)
			{
				CSceneController::getInstance()->doFinishLoadScene();
			}
		}

		void CSpaceLoadScene::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}

		void CSpaceLoadScene::loadScene(const char* path)
		{
			m_startLoading = true;
			m_scenePath = path;
		}
	}
}