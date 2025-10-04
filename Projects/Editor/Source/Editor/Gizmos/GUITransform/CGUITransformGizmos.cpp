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
#include "Editor/SpaceController/CGUIDesignController.h"
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
				m_selectID = "";
				return;
			}

			if (selectObject->getID() != m_selectID)
			{
				m_selectID = selectObject->getID();
				m_parentWorld.makeIdentity();

				CGUIDesignController* controller = CGUIDesignController::getInstance();
				CScene* scene = controller->getScene();

				if (selectObject->getType() == CSelectObject::GUIElement)
				{
					// Show GUI Property
					CCanvas* canvas = controller->getCanvas();
					CGUIElement* gui = canvas->getGUIByID(selectObject->getID().c_str());
					if (gui)
					{
						m_gui = gui;

						onEnable();

						CGUIElement* parentGUI = gui->getParent();
						if (parentGUI)
							m_parentWorld = parentGUI->getAbsoluteTransform();

						m_position = gui->getAlignPosition();
						m_rotation = gui->getRotationQuaternion();
						m_scale = gui->getScale();
						m_rect = gui->getRect();

						m_changed = false;
					}
				}

				handle->setWorld(m_parentWorld);
				handle->end();
				handle->reset();
			}

			if (m_gui == NULL)
				return;

			if (m_gui->getDock() != EGUIDock::NoDock)
				return;

			// position
			{
				core::vector3df newPos = CGUIHandles::getInstance()->positionHandle(*m_position, m_gui->getRotationQuaternion());
				core::vector3df delta;

				if (newPos != *m_position)
				{
					delta = newPos - *m_position;
					updateSelectedPosition(delta);

					m_position = newPos;
					m_position.notify(this);

					core::vector3df p = m_gui->getPosition() + delta;
					m_gui->setPosition(p);

					m_changed = true;
					updateProperty();
				}

				if (handle->endCheck())
				{
					core::vector3df p = m_gui->getPosition() + delta;
					m_gui->setPosition(p);
					handle->end();

					// save undo/redo
					if (m_changed)
						saveHistorySelectedObject();
				}
			}

			// rect
			{
				core::rectf newRect = CGUIHandles::getInstance()->rectHandle(
					*m_rect,
					m_gui->getAlignPosition(),
					m_gui->getScale(),
					m_gui->getRotationQuaternion());

				if (newRect != *m_rect)
				{
					*m_rect = newRect;
					m_rect.notify(this);

					m_gui->setRect(newRect);

					m_changed = true;
					updateProperty();
				}

				if (handle->endCheck())
				{
					m_gui->setRect(*m_rect);
					handle->end();

					m_selectID = "";

					// save undo/redo
					if (m_changed)
						saveHistorySelectedObject();
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

					core::rectf rect = m_gui->getRect();

					CFloatProperty* rx = dynamic_cast<CFloatProperty*>(data->getProperty("rectX"));
					rx->set(rect.UpperLeftCorner.X);
					CFloatProperty* ry = dynamic_cast<CFloatProperty*>(data->getProperty("rectY"));
					ry->set(rect.UpperLeftCorner.Y);
					CFloatProperty* rw = dynamic_cast<CFloatProperty*>(data->getProperty("rectW"));
					rw->set(rect.getWidth());
					CFloatProperty* rh = dynamic_cast<CFloatProperty*>(data->getProperty("rectH"));
					rh->set(rect.getHeight());

					rx->OnChanged();
					ry->OnChanged();
					rw->OnChanged();
					rh->OnChanged();
				}
			}
		}


		void CGUITransformGizmos::onEnable()
		{

		}

		void CGUITransformGizmos::onRemove()
		{
			CSelection::getInstance()->clear();
			CGUIHandles::getInstance()->end();
			m_selectID = "";
			m_gui = NULL;
		}

		void CGUITransformGizmos::refresh()
		{
			CGUIHandles::getInstance()->end();
			m_selectID = "";
			m_gui = NULL;
		}

		void CGUITransformGizmos::onNotify(ISubject* subject, IObserver* from)
		{

		}

		void CGUITransformGizmos::setTransform(const core::vector3df& pos, const core::vector3df& rot, const core::vector3df& scale, const core::rectf& rect)
		{
			m_position = pos;
			m_rotation = rot;
			m_scale = scale;
			m_rect = rect;

			CGUIHandles::getInstance()->end();

			m_position.notify(this);
			m_rotation.notify(this);
			m_scale.notify(this);
		}

		void CGUITransformGizmos::updateSelectedPosition(const core::vector3df& delta)
		{
			std::vector<CGUIElement*> guis;
			getSelectedGUI(guis);
			for (CGUIElement* gui : guis)
			{
				if (gui != m_gui)
				{
					core::vector3df newPosition = gui->getPosition() + delta;
					gui->setPosition(newPosition);
				}
			}
		}

		void CGUITransformGizmos::getSelectedGUI(std::vector<CGUIElement*>& guis)
		{
			// Show GUI Property
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CScene* scene = controller->getScene();
			CCanvas* canvas = controller->getCanvas();

			std::vector<CGUIElement*> selects;

			std::vector<CSelectObject*>& selectList = CSelection::getInstance()->getAllSelected();
			for (CSelectObject* sel : selectList)
			{
				CGUIElement* gui = canvas->getGUIByID(sel->getID().c_str());
				if (gui)
					selects.push_back(gui);
			}

			std::sort(selects.begin(), selects.end(), [](CGUIElement*& a, CGUIElement*& b)
				{
					return a->getDepth() < b->getDepth();
				});

			guis.clear();

			for (CGUIElement* sel : selects)
			{
				bool add = true;
				for (CGUIElement* del : guis)
				{
					if (sel->isChild(del))
					{
						add = false;
						break;
					}
				}

				if (add)
				{
					guis.push_back(sel);
				}
			}
		}

		void CGUITransformGizmos::saveHistorySelectedObject()
		{
			std::vector<CGUIElement*> selectedObject;
			getSelectedGUI(selectedObject);

			CGUIDesignController::getInstance()->getHistory()->saveModifyHistory(selectedObject);
		}
	}
}