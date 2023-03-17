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
#include "CGUITransformGizmos.h"
#include "Handles/CGUIHandles.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUITransformGizmos::CGUITransformGizmos() :
			m_gui(NULL),
			m_position(core::vector3df()),
			m_rotation(core::quaternion()),
			m_scale(core::vector3df(1.0f, 1.0f, 1.0f)),
			m_changed(false)
		{

		}

		CGUITransformGizmos::~CGUITransformGizmos()
		{

		}

		void CGUITransformGizmos::onGizmos()
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
				m_parentWorld.makeIdentity();

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
						m_gui = gui;

						onEnable();

						CGUIElement* parentGUI = gui->getParent();
						if (parentGUI)
							m_parentWorld = parentGUI->getAbsoluteTransform();

						m_position = gui->getPosition();
						m_rotation = gui->getRotationQuaternion();
						m_scale = gui->getScale();

						m_changed = false;
					}
				}

				handle->setWorld(m_parentWorld);
				handle->end();
			}

			// position
			{
				core::vector3df newPos = CGUIHandles::getInstance()->positionHandle(*m_position, m_gui->getRotationQuaternion());

				if (newPos != *m_position)
				{
					core::vector3df delta = newPos - *m_position;

					// updateSelectedPosition(delta);

					m_position = newPos;
					m_position.notify(this);

					m_gui->setPosition(newPos);

					m_changed = true;
				}

				if (handle->endCheck())
				{
					m_gui->setPosition(*m_position);
					handle->end();

					// save undo/redo
					// if (m_changed)
					//	saveHistorySelectedObject();
				}
			}
		}

		void CGUITransformGizmos::onEnable()
		{

		}

		void CGUITransformGizmos::onRemove()
		{

		}

		void CGUITransformGizmos::refresh()
		{

		}

		void CGUITransformGizmos::onNotify(ISubject* subject, IObserver* from)
		{

		}
	}
}