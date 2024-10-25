/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CSpaceApplyTemplate.h"
#include "Scene/CSceneImporter.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceApplyTemplate::CSpaceApplyTemplate(GUI::CWindow* window, CEditor* editor) :
			CSpaceLoading(window, editor),
			m_startLoading(false),
			m_progressBar(NULL),
			m_statusText(NULL),
			m_data(NULL)
		{
			m_progressBar = new GUI::CProgressBar(window);
			m_progressBar->dock(GUI::EPosition::Top);
			m_progressBar->setMargin(GUI::SMargin(14.0f, 14.0, 14.0, 0.0f));

			m_statusText = new GUI::CLabel(window);
			m_statusText->dock(GUI::EPosition::Fill);
			m_statusText->setMargin(GUI::SMargin(14.0f, 5.0, 14.0, 0.0f));
			m_statusText->setWrapMultiline(true);
			m_statusText->setString(L"Apply Template...");
		}

		CSpaceApplyTemplate::~CSpaceApplyTemplate()
		{

		}

		void CSpaceApplyTemplate::update()
		{
			CSpace::update();

			if (m_startLoading)
			{
				int numObject = CSceneImporter::beginReloadTemplate(CSceneController::getInstance()->getScene(), m_data);
				if (numObject == 0)
				{
					m_finished = true;
				}
				m_startLoading = false;
			}

			m_progressBar->setPercent(0.0f);

			if (m_finished == false)
			{
				m_finished = CSceneImporter::reloadTemplate();
			}

			if (m_finished == true)
			{
				CSceneController::getInstance()->doFinishApplyTemplate(CSceneImporter::getObjects());
			}
		}

		void CSpaceApplyTemplate::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}

		void CSpaceApplyTemplate::applyTemplate(const char* path)
		{
			CSceneController* sceneController = CSceneController::getInstance();

			m_data = CSceneImporter::importTemplateToObject(sceneController->getZone(), path);
			if (m_data)
			{
				m_startLoading = true;
			}
			else
			{
				m_finished = true;
			}
		}
	}
}