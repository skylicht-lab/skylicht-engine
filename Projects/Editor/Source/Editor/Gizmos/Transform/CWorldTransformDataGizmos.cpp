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
#include "CWorldTransformDataGizmos.h"
#include "Selection/CSelection.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		CWorldTransformDataGizmos::CWorldTransformDataGizmos() :
			m_transform(NULL),
			m_selectObject(NULL),
			m_position(core::vector3df()),
			m_rotation(core::quaternion()),
			m_scale(core::vector3df(1.0f, 1.0f, 1.0f)),
			m_lastType(ETransformGizmo::Translate)
		{
			getSubjectTransformGizmos().addObserver(this);

			m_lastType = getSubjectTransformGizmos().get();
		}

		CWorldTransformDataGizmos::~CWorldTransformDataGizmos()
		{
			getSubjectTransformGizmos().removeObserver(this);
		}

		void CWorldTransformDataGizmos::onNotify(ISubject* subject, IObserver* from)
		{
			if (from == this)
				return;

			CSubject<ETransformGizmo>* gizmos = (CSubject<ETransformGizmo>*)subject;
			m_lastType = gizmos->get();
		}

		void CWorldTransformDataGizmos::setPosition(const core::vector3df& pos)
		{
			CHandles::getInstance()->end();
			m_position = pos;
		}

		void CWorldTransformDataGizmos::setScale(const core::vector3df& scale)
		{
			CHandles::getInstance()->end();
			m_scale = scale;
		}

		void CWorldTransformDataGizmos::setRotation(const core::vector3df& rotate)
		{
			CHandles::getInstance()->end();
			m_rotation = core::quaternion(rotate * core::DEGTORAD);
		}

		void CWorldTransformDataGizmos::getSelectedTransform(std::vector<CWorldTransformData*>& transforms)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			std::vector<CSelectObject*>& selectObjects = CSelection::getInstance()->getAllSelected();
			for (CSelectObject* obj : selectObjects)
			{
				if (obj->getType() == CSelectObject::Entity)
				{

				}
			}
		}

		void CWorldTransformDataGizmos::updateSelectedPosition(const core::vector3df& delta)
		{
			std::vector<CWorldTransformData*> transforms;
			getSelectedTransform(transforms);

			for (CWorldTransformData* t : transforms)
			{
				if (t != m_transform)
				{

				}
			}
		}

		void CWorldTransformDataGizmos::updateSelectedScale(const core::vector3df& delta)
		{
			std::vector<CWorldTransformData*> transforms;
			getSelectedTransform(transforms);

			for (CWorldTransformData* t : transforms)
			{
				if (t != m_transform)
				{

				}
			}
		}

		void CWorldTransformDataGizmos::updateSelectedRotation(const core::quaternion& delta)
		{
			std::vector<CWorldTransformData*> transforms;
			getSelectedTransform(transforms);

			for (CWorldTransformData* t : transforms)
			{
				if (t != m_transform)
				{

				}
			}
		}

		void CWorldTransformDataGizmos::onGizmos()
		{
			CHandles* handle = CHandles::getInstance();

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject == NULL)
			{
				m_parentWorld.makeIdentity();
				handle->setWorld(m_parentWorld);
				handle->end();
				return;
			}

			if (selectObject->getID() != m_selectID)
			{
				m_selectID = selectObject->getID();
				m_selectObject = NULL;

				CSceneController* sceneController = CSceneController::getInstance();
				CScene* scene = sceneController->getScene();

				if (selectObject->getType() == CSelectObject::Entity)
				{
					m_selectObject = scene->getEntityManager()->getEntityByID(m_selectID.c_str());
					m_transform = m_selectObject->getData<CWorldTransformData>();

					m_position = m_transform->Relative.getTranslation();
					m_rotation = m_transform->Relative.getRotationDegrees();
					m_scale = m_transform->Relative.getScale();

					if (m_transform->ParentIndex > 0)
					{
						CEntity* parent = scene->getEntityManager()->getEntity(m_transform->ParentIndex);
						CWorldTransformData* parentTransform = parent->getData<CWorldTransformData>();

						m_parentWorld = parentTransform->World;
					}
					else
					{
						m_parentWorld = core::IdentityMatrix;
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
				return;
			}

			ETransformGizmo type = getSubjectTransformGizmos().get();

			if (type == ETransformGizmo::Translate)
			{
				core::vector3df newPos = handle->positionHandle(*m_position, m_rotation.get());
				if (newPos != *m_position)
				{
					core::vector3df delta = newPos - *m_position;
					updateSelectedPosition(delta);

					m_position.notify(this);
					// m_transform->setPosition(newPos);
				}

				m_position = newPos;
				if (handle->endCheck())
				{
					// m_transform->setPosition(*m_position);
					handle->end();
					m_cacheSelectedObjects.clear();
				}
			}
			else if (type == ETransformGizmo::Rotate)
			{
				core::quaternion newRot = handle->rotateHandle(*m_rotation, m_position.get());
				if (newRot != *m_rotation)
				{
					// new = rot * delta
					// delta = new * inverse(rot)

					core::quaternion inverseR = *m_rotation;
					inverseR.makeInverse();

					core::quaternion delta = newRot * inverseR;
					updateSelectedRotation(delta);

					m_rotation.notify(this);
					// m_transform->setRotation(newRot);
				}

				m_rotation = newRot;
				if (handle->endCheck())
				{
					// m_transform->setRotation(*m_rotation);
					handle->end();
					m_cacheSelectedObjects.clear();
				}
			}
			else if (type == ETransformGizmo::Scale)
			{
				core::vector3df newScale = handle->scaleHandle(*m_scale, m_position.get(), m_rotation.get());
				if (newScale != *m_scale)
				{
					core::vector3df delta = newScale / *m_scale;
					updateSelectedScale(delta);

					m_scale.notify(this);
					// m_transform->setScale(newScale);
				}

				m_scale = newScale;
				if (handle->endCheck())
				{
					// m_transform->setScale(*m_scale);
					handle->end();
					m_cacheSelectedObjects.clear();
				}
			}
			else
			{
				handle->end();
				m_cacheSelectedObjects.clear();
			}
		}

		void CWorldTransformDataGizmos::onEnable()
		{

		}

		void CWorldTransformDataGizmos::onRemove()
		{
			m_selectObject = NULL;
			m_transform = NULL;
			m_selectID = "";

			// m_lastType = s_transformGizmos.get();

			CHandles* handles = CHandles::getInstance();
			if (handles != NULL)
				handles->end();
		}

		void CWorldTransformDataGizmos::refresh()
		{
			onRemove();
		}
	}
}