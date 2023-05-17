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
#include "CGUISelecting.h"
#include "CSelection.h"
#include "CSelection.h"
#include "GUI/Input/CInput.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Editor/SpaceController/CGUIDesignController.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUISelecting::CGUISelecting()
		{

		}

		CGUISelecting::~CGUISelecting()
		{

		}

		void CGUISelecting::end()
		{

		}

		bool CGUISelecting::OnEvent(const SEvent& event)
		{
			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				int mouseX = event.MouseInput.X;
				int mouseY = event.MouseInput.Y;

				if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
				{

				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{

				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					CScene* scene = CSceneController::getInstance()->getScene();
					CGUIDesignController* controller = CGUIDesignController::getInstance();
					CSelection* selection = CSelection::getInstance();

					if (scene)
					{
						CGameObject* canvasObj = scene->searchObjectInChild(L"GUICanvas");
						CCanvas* canvas = canvasObj->getComponent<CCanvas>();

						CGameObject* cameraObj = scene->searchObjectInChild(L"GUICamera");
						CCamera* camera = cameraObj->getComponent<CCamera>();

						CGUIElement* gui = canvas->getHitTest(camera, (float)mouseX, (float)mouseY, m_viewport);
						if (gui)
						{
							// select a gui
							if (!GUI::CInput::getInput()->isKeyDown(GUI::EKey::KEY_CONTROL))
							{
								// need clear current selection
								if (selection->getSelected(gui) == NULL)
								{
									controller->deselectAllOnHierachy();
									selection->clear();
								}
								controller->selectOnHierachy(gui);
							}
							else
							{
								// hold control && re-pick this object
								if (selection->getSelected(gui) != NULL)
								{
									controller->deselectOnHierachy(gui);
								}
								else
								{
									controller->selectOnHierachy(gui);
								}
							}
						}
						else
						{
							// no select gui							
							controller->deselectAllOnHierachy();
							selection->clear();
						}
					}
				}

				return true;
			}

			return false;
		}
	}
}