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
#include "Editor/CEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/GUIEditor/CGUIEditor.h"
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
			m_rect(core::rectf()),
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
						m_rect = gui->getRect();

						m_changed = false;
					}
				}

				handle->setWorld(m_parentWorld);
				handle->end();
			}

			if (m_gui == NULL)
				return;

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
					updateProperty();
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

			// rect
			{
				core::rectf newRect = CGUIHandles::getInstance()->rectHandle(
					*m_rect,
					m_gui->getPosition(),
					m_gui->getScale(),
					m_gui->getRotationQuaternion());

				if (newRect != *m_rect)
				{

				}

				if (handle->endCheck())
				{
					m_gui->setRect(*m_rect);
					handle->end();

					// save undo/redo
					// if (m_changed)
					//	saveHistorySelectedObject();
				}
			}
		}

		void CGUITransformGizmos::updateProperty()
		{
			// Activator
			CEditorActivator* activator = CEditorActivator::getInstance();

			// GUI property
			CGUIEditor* editor = activator->getGUIEditorInstance(m_gui->getTypeName().c_str());
			if (editor != NULL)
			{
				CObjectSerializable* data = editor->getData();
				if (data)
				{
					CVector3Property* p = dynamic_cast<CVector3Property*>(data->getProperty("position"));
					p->set(m_gui->getPosition());
					p->OnChanged();
					CVector3Property* s = dynamic_cast<CVector3Property*>(data->getProperty("scale"));
					s->set(m_gui->getScale());
					s->OnChanged();
					CVector3Property* r = dynamic_cast<CVector3Property*>(data->getProperty("rotation"));
					r->set(m_gui->getRotation());
					r->OnChanged();
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

		void CGUITransformGizmos::setTransform(const core::vector3df& pos, const core::vector3df& rot, const core::vector3df& scale)
		{
			m_position = pos;
			m_rotation = rot;
			m_scale = scale;

			CGUIHandles::getInstance()->end();

			m_position.notify(this);
			m_rotation.notify(this);
			m_scale.notify(this);
		}
	}
}