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
#include "Entity/CEntityHandleData.h"

namespace Skylicht
{
	namespace Editor
	{
		CWorldTransformDataGizmos::CWorldTransformDataGizmos() :
			m_transform(NULL),
			m_selectEntity(NULL),
			m_position(core::vector3df()),
			m_rotation(core::quaternion()),
			m_scale(core::vector3df(1.0f, 1.0f, 1.0f)),
			m_lastType(ETransformGizmo::Translate),
			changed(false)
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

		void CWorldTransformDataGizmos::saveHistorySelectedObject()
		{
			if (m_selectEntity)
			{
				CEntityHandleData* handler = (CEntityHandleData*)m_selectEntity->getDataByIndex(CEntityHandleData::DataTypeIndex);
				if (handler != NULL)
				{
					// get owner of entity
					CGameObject* gameObject = handler->Handler->getGameObject();

					std::vector<CGameObject*> listGameObject;
					listGameObject.push_back(gameObject);

					// save modify
					CSceneController::getInstance()->getHistory()->saveModifyHistory(listGameObject);
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
				changed = false;
				return;
			}

			if (selectObject->getID() != m_selectID)
			{
				m_selectID = selectObject->getID();
				m_selectEntity = NULL;

				CSceneController* sceneController = CSceneController::getInstance();
				CScene* scene = sceneController->getScene();

				if (selectObject->getType() == CSelectObject::Entity)
				{
					m_selectEntity = scene->getEntityManager()->getEntityByID(m_selectID.c_str());
					m_transform = (CWorldTransformData*)m_selectEntity->getDataByIndex(CWorldTransformData::DataTypeIndex);

					core::vector3df pos, rot, scale;
					splitMatrixData(m_transform->Relative, pos, rot, scale);

					m_position = pos;
					m_rotation = rot;
					m_scale = scale;

					changed = false;

					if (m_transform->ParentIndex > 0)
					{
						CEntity* parent = scene->getEntityManager()->getEntity(m_transform->ParentIndex);
						CWorldTransformData* parentTransform = (CWorldTransformData*)parent->getDataByIndex(CWorldTransformData::DataTypeIndex);

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

			if (m_selectEntity == NULL)
			{
				m_parentWorld.makeIdentity();
				handle->setWorld(m_parentWorld);
				handle->end();
				m_cacheSelectedObjects.clear();
				changed = false;
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
					setMatrix(m_transform, newPos, m_rotation.get(), m_scale.get());

					changed = true;
				}

				m_position = newPos;
				if (handle->endCheck())
				{
					setMatrix(m_transform, m_position.get(), m_rotation.get(), m_scale.get());
					handle->end();

					if (changed)
						saveHistorySelectedObject();

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
					setMatrix(m_transform, m_position.get(), newRot, m_scale.get());

					changed = true;
				}

				m_rotation = newRot;
				if (handle->endCheck())
				{
					setMatrix(m_transform, m_position.get(), m_rotation.get(), m_scale.get());
					handle->end();

					if (changed)
						saveHistorySelectedObject();

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
					setMatrix(m_transform, m_position.get(), m_rotation.get(), newScale);

					changed = true;
				}

				m_scale = newScale;
				if (handle->endCheck())
				{
					setMatrix(m_transform, m_position.get(), m_rotation.get(), m_scale.get());
					handle->end();

					if (changed)
						saveHistorySelectedObject();

					m_cacheSelectedObjects.clear();
				}
			}
			else
			{
				handle->end();
				m_cacheSelectedObjects.clear();
				changed = false;
			}
		}

		void CWorldTransformDataGizmos::splitMatrixData(const core::matrix4& matrix, core::vector3df& pos, core::vector3df& rot, core::vector3df& scale)
		{
			core::vector3df front(0.0f, 0.0f, 1.0f);
			core::vector3df up(0.0f, 1.0f, 0.0f);

			matrix.rotateVect(front);
			matrix.rotateVect(up);

			front.normalize();
			up.normalize();

			core::vector3df right = up.crossProduct(front);
			right.normalize();

			core::matrix4 rotationMatrix;
			f32* matData = rotationMatrix.pointer();
			matData[0] = right.X;
			matData[1] = right.Y;
			matData[2] = right.Z;
			matData[3] = 0.0f;

			matData[4] = up.X;
			matData[5] = up.Y;
			matData[6] = up.Z;
			matData[7] = 0.0f;

			matData[8] = front.X;
			matData[9] = front.Y;
			matData[10] = front.Z;
			matData[11] = 0.0f;

			matData[12] = 0.0f;
			matData[13] = 0.0f;
			matData[14] = 0.0f;
			matData[15] = 1.0f;

			core::quaternion q(rotationMatrix);
			q.toEuler(rot);

			pos = matrix.getTranslation();
			scale = matrix.getScale();
		}

		void CWorldTransformDataGizmos::setMatrix(CWorldTransformData* transform, const core::vector3df& pos, const core::quaternion& rot, const core::vector3df& scale)
		{
			core::matrix4& mat = transform->Relative;
			transform->HasChanged = true;

			mat.makeIdentity();
			mat = rot.getMatrix();

			f32* m = mat.pointer();

			m[0] *= scale.X;
			m[1] *= scale.X;
			m[2] *= scale.X;
			m[3] *= scale.X;

			m[4] *= scale.Y;
			m[5] *= scale.Y;
			m[6] *= scale.Y;
			m[7] *= scale.Y;

			m[8] *= scale.Z;
			m[9] *= scale.Z;
			m[10] *= scale.Z;
			m[11] *= scale.Z;

			m[12] = pos.X;
			m[13] = pos.Y;
			m[14] = pos.Z;
		}

		void CWorldTransformDataGizmos::onEnable()
		{

		}

		void CWorldTransformDataGizmos::onRemove()
		{
			m_selectEntity = NULL;
			m_transform = NULL;
			m_selectID = "";

			m_lastType = getSubjectTransformGizmos().get();

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