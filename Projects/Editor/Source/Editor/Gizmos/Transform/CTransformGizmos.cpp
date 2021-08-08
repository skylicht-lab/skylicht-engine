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

namespace Skylicht
{
	namespace Editor
	{
		CSubject<CTransformGizmos::ETransformGizmo> CTransformGizmos::s_transformGizmos(CTransformGizmos::None);

		CTransformGizmos::CTransformGizmos() :
			m_transform(NULL),
			m_selectObject(NULL),
			m_position(core::vector3df()),
			m_rotation(core::quaternion()),
			m_scale(core::vector3df(1.0f, 1.0f, 1.0f)),
			m_lastType(CTransformGizmos::Translate)
		{

		}

		CTransformGizmos::~CTransformGizmos()
		{

		}

		void CTransformGizmos::onNotify(ISubject* subject, IObserver* from)
		{
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

		void CTransformGizmos::onGizmos()
		{
			CHandles* handle = CHandles::getInstance();

			CSelectObject selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject.getID() != m_selectID)
			{
				m_selectID = selectObject.getID();
				m_selectObject = NULL;

				CSceneController* sceneController = CSceneController::getInstance();
				CScene* scene = sceneController->getScene();

				if (selectObject.getType() == CSelectObject::GameObject)
				{
					m_selectObject = scene->searchObjectInChildByID(m_selectID.c_str());
					if (m_selectObject != NULL)
					{
						m_transform = m_selectObject->getComponent<CTransformEuler>();
						m_position = m_transform->getPosition();
						m_rotation = m_transform->getRotationQuaternion();
						m_scale = m_transform->getScale();
					}
				}

				handle->end();
			}

			if (m_selectObject == NULL)
			{
				handle->end();
				return;
			}

			ETransformGizmo type = s_transformGizmos.get();

			if (type == CTransformGizmos::Translate)
			{
				core::vector3df newPos = handle->positionHandle(*m_position, m_transform->getRotationQuaternion());
				if (newPos != *m_position)
					m_position.notify(this);

				m_position = newPos;
				if (handle->endCheck())
				{
					m_transform->setPosition(*m_position);
					handle->end();
				}
			}
			else if (type == CTransformGizmos::Rotate)
			{
				core::quaternion newRot = handle->rotateHandle(*m_rotation, m_transform->getPosition());
				if (newRot != *m_rotation)
					m_rotation.notify(this);

				m_rotation = newRot;
				if (handle->endCheck())
				{
					m_transform->setRotation(*m_rotation);
					handle->end();
				}
			}
			else if (type == CTransformGizmos::Scale)
			{
				core::vector3df newScale = handle->scaleHandle(*m_scale, m_transform->getPosition(), m_transform->getRotationQuaternion());
				if (newScale != *m_scale)
					m_scale.notify(this);

				m_scale = newScale;
				if (handle->endCheck())
				{
					m_transform->setScale(*m_scale);
					handle->end();
				}
			}
			else
			{
				handle->end();
			}

		}

		void CTransformGizmos::onEnable()
		{
			s_transformGizmos.set(m_lastType);
			s_transformGizmos.notify(this);
		}

		void CTransformGizmos::onRemove()
		{
			m_selectObject = NULL;
			m_transform = NULL;
			m_selectID = "";

			m_lastType = s_transformGizmos.get();

			CHandles* handles = CHandles::getInstance();
			if (handles != NULL)
				handles->end();
		}
	}
}