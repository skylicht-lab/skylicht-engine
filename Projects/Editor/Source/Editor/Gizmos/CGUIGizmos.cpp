/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CGUIGizmos.h"
#include "Handles/CGUIHandles.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIGizmos::CGUIGizmos()
		{

		}

		CGUIGizmos::~CGUIGizmos()
		{

		}

		void CGUIGizmos::onGizmos()
		{
			CGUIHandles* handle = CGUIHandles::getInstance();

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject == NULL)
			{
				handle->end();
				return;
			}

			if (selectObject->getID() != m_selectID)
			{
				m_selectID = selectObject->getID();

				CSceneController* sceneController = CSceneController::getInstance();
				CScene* scene = sceneController->getScene();

				if (selectObject->getType() == CSelectObject::GUIElement)
				{
					// Show GUI Property
					CGameObject* guiCanvas = scene->searchObjectInChild(L"GUICanvas");
					CCanvas* canvas = guiCanvas->getComponent<CCanvas>();
					CGUIElement* gui = canvas->getGUIByID(selectObject->getID().c_str());
					if (gui)
					{
						onEnable();
					}
				}

				// handle->setWorld(m_parentWorld);
				handle->end();
			}
		}

		void CGUIGizmos::onEnable()
		{

		}

		void CGUIGizmos::onRemove()
		{

		}

		void CGUIGizmos::refresh()
		{

		}
	}
}