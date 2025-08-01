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
#include "CTransformGizmos.h"
#include "Selection/CSelection.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Handles/CHandles.h"
#include "Debug/CSceneDebug.h"

namespace Skylicht
{
	namespace Editor
	{
		CTransformGizmos::CTransformGizmos() :
			m_transform(NULL),
			m_selectObject(NULL),
			m_position(core::vector3df()),
			m_rotation(core::quaternion()),
			m_scale(core::vector3df(1.0f, 1.0f, 1.0f)),
			m_lastType(ETransformGizmo::Translate),
			m_changed(false)
		{
			getSubjectTransformGizmos().addObserver(this);

			m_lastType = getSubjectTransformGizmos().get();
		}

		CTransformGizmos::~CTransformGizmos()
		{
			getSubjectTransformGizmos().removeObserver(this);
		}

		void CTransformGizmos::onNotify(ISubject* subject, IObserver* from)
		{
			if (from == this)
				return;

			CSubject<ETransformGizmo>* gizmos = (CSubject<ETransformGizmo>*)subject;
			m_lastType = gizmos->get();
		}

		void CTransformGizmos::setPosition(const core::vector3df& pos)
		{
			CHandles::getInstance()->end();
			m_position = pos;
		}

		void CTransformGizmos::setScale(const core::vector3df& scale)
		{
			CHandles::getInstance()->end();
			m_scale = scale;
		}

		void CTransformGizmos::setRotation(const core::vector3df& rotate)
		{
			CHandles::getInstance()->end();
			m_rotation = core::quaternion(rotate * core::DEGTORAD);
		}

		void CTransformGizmos::getSelectedTransform(std::vector<CTransformEuler*>& transforms)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			std::vector<CSelectObject*>& selectObjects = CSelection::getInstance()->getAllSelected();
			for (CSelectObject* obj : selectObjects)
			{
				if (obj->getType() == CSelectObject::GameObject)
				{
					CGameObject* selecteObject = NULL;

					if (m_cacheSelectedObjects.find(obj->getID()) == m_cacheSelectedObjects.end())
					{
						selecteObject = scene->searchObjectInChildByID(obj->getID().c_str());
						if (selecteObject != NULL)
							m_cacheSelectedObjects[obj->getID()] = selecteObject;
					}
					else
					{
						selecteObject = m_cacheSelectedObjects[obj->getID()];
					}

					if (selecteObject != NULL)
					{
						CTransformEuler* t = selecteObject->getComponent<CTransformEuler>();
						if (t != NULL)
							transforms.push_back(t);
					}
				}
			}
		}

		void CTransformGizmos::saveHistorySelectedObject()
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CSceneHistory* history = sceneController->getHistory();

			std::vector<CGameObject*> selectedObject;
			std::vector<CTransformEuler*> transforms;
			getSelectedTransform(transforms);

			for (CTransformEuler* t : transforms)
			{
				CGameObject* gameObject = t->getGameObject();
				selectedObject.push_back(gameObject);

				CGameObject* parent = gameObject->getParentTemplate();
				if (parent != gameObject)
				{
					// if child is changed, we set modify
					parent->setTemplateChanged(true);
					sceneController->updateTreeNode(parent);
				}
			}

			history->saveModifyHistory(selectedObject);
		}

		void CTransformGizmos::updateSelectedPosition(const core::vector3df& delta)
		{
			std::vector<CTransformEuler*> transforms;
			getSelectedTransform(transforms);

			for (CTransformEuler* t : transforms)
			{
				if (t != m_transform)
				{
					core::vector3df offset = delta;

					core::matrix4 world = m_transform->getParent()->calcWorldTransform();
					world.makeInverse();
					world.rotateVect(offset);

					core::vector3df newPosition = t->getPosition() + offset;
					t->setPosition(newPosition);
				}
			}
		}

		void CTransformGizmos::updateSelectedScale(const core::vector3df& delta)
		{
			std::vector<CTransformEuler*> transforms;
			getSelectedTransform(transforms);

			for (CTransformEuler* t : transforms)
			{
				if (t != m_transform)
				{
					core::vector3df newScale = t->getScale() * delta;
					t->setScale(newScale);
				}
			}
		}

		void CTransformGizmos::updateSelectedRotation(const core::quaternion& delta)
		{
			std::vector<CTransformEuler*> transforms;
			getSelectedTransform(transforms);

			for (CTransformEuler* t : transforms)
			{
				if (t != m_transform)
				{
					core::quaternion newRotation = t->getRotationQuaternion() * delta;
					t->setRotation(newRotation);
				}
			}
		}

		void CTransformGizmos::onGizmos()
		{
			CHandles* handle = CHandles::getInstance();

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject == NULL)
			{
				m_parentWorld.makeIdentity();
				handle->setWorld(m_parentWorld);
				handle->end();
				m_changed = false;
				m_selectID = "";
				return;
			}

			ETransformGizmo type = getSubjectTransformGizmos().get();

			if (selectObject->getID() != m_selectID && type != ETransformGizmo::None)
			{
				m_selectID = selectObject->getID();
				m_selectObject = NULL;

				CSceneController* sceneController = CSceneController::getInstance();
				CScene* scene = sceneController->getScene();

				if (selectObject->getType() == CSelectObject::GameObject)
				{
					m_selectObject = scene->searchObjectInChildByID(m_selectID.c_str());
					if (m_selectObject != NULL)
					{
						m_transform = m_selectObject->getComponent<CTransformEuler>();

						m_parentWorld.makeIdentity();
						CTransform* parentTransform = m_transform->getParent();
						if (parentTransform != NULL)
							m_parentWorld = parentTransform->calcWorldTransform();

						m_position = m_transform->getPosition();
						m_rotation = m_transform->getRotationQuaternion();
						m_scale = m_transform->getScale();
						m_changed = false;

						onEnable();
					}
				}

				handle->setWorld(m_parentWorld);
				handle->end();
			}

			if (m_selectObject == NULL)
			{
				m_parentWorld.makeIdentity();
				handle->setWorld(m_parentWorld);
				handle->end();
				m_cacheSelectedObjects.clear();
				m_selectID = "";
				return;
			}

			if (type == ETransformGizmo::Translate)
			{
				core::vector3df newPos = handle->positionHandle(*m_position, m_transform->getRotationQuaternion());
				if (newPos != *m_position)
				{
					core::vector3df p1 = m_transform->getWorldPosition();
					m_transform->setPosition(newPos);
					core::vector3df p2 = m_transform->getWorldPosition();

					core::vector3df delta = p2 - p1;
					updateSelectedPosition(delta);

					m_position = newPos;
					m_position.notify(this);
					m_changed = true;
				}

				if (handle->endCheck())
				{
					m_transform->setPosition(*m_position);
					handle->end();

					// save undo/redo
					if (m_changed)
						saveHistorySelectedObject();

					m_cacheSelectedObjects.clear();
				}
			}
			else if (type == ETransformGizmo::Rotate)
			{
				core::quaternion newRot = handle->rotateHandle(*m_rotation, m_transform->getPosition());
				if (newRot != *m_rotation)
				{
					// new = rot * delta
					// delta = new * inverse(rot)

					core::quaternion inverseR = *m_rotation;
					inverseR.makeInverse();

					core::quaternion delta = newRot * inverseR;
					updateSelectedRotation(delta);

					m_rotation = newRot;
					m_rotation.notify(this);

					m_transform->setRotation(newRot);
					m_changed = true;
				}

				if (handle->endCheck())
				{
					m_transform->setRotation(*m_rotation);
					handle->end();

					// save undo/redo
					if (m_changed)
						saveHistorySelectedObject();

					m_cacheSelectedObjects.clear();
				}
			}
			else if (type == ETransformGizmo::Scale)
			{
				core::vector3df newScale = handle->scaleHandle(*m_scale, m_transform->getPosition(), m_transform->getRotationQuaternion());
				if (newScale != *m_scale)
				{
					core::vector3df delta = newScale / *m_scale;
					updateSelectedScale(delta);

					m_scale = newScale;
					m_scale.notify(this);

					m_transform->setScale(newScale);
					m_changed = true;
				}

				if (handle->endCheck())
				{
					m_transform->setScale(*m_scale);
					handle->end();

					// save undo/redo
					if (m_changed)
						saveHistorySelectedObject();

					m_cacheSelectedObjects.clear();
				}
			}
			else
			{
				handle->end();
				m_cacheSelectedObjects.clear();
				m_selectID = "";
				m_selectObject = NULL;
				m_transform = NULL;
				m_changed = false;
			}

		}

		void CTransformGizmos::onEnable()
		{
			getSubjectTransformGizmos().set(m_lastType);
			getSubjectTransformGizmos().notify(this);
		}

		void CTransformGizmos::onRemove()
		{
			m_selectObject = NULL;
			m_transform = NULL;
			m_selectID = "";

			m_lastType = getSubjectTransformGizmos().get();

			CHandles* handles = CHandles::getInstance();
			if (handles != NULL)
				handles->end();
		}

		void CTransformGizmos::refresh()
		{
			onRemove();
		}
	}
}
