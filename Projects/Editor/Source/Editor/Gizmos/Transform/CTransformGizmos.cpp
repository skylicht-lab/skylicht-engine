/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
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
		CTransformGizmos::CTransformGizmos() :
			m_transform(NULL),
			m_selectObject(NULL)
		{

		}

		CTransformGizmos::~CTransformGizmos()
		{

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

			// show gizmos move position
			m_position = handle->positionHandle(m_position);
			if (handle->endCheck())
			{
				m_transform->setPosition(m_position);
				handle->end();
			}

			/*
			static core::quaternion rot;
			static core::vector3df scale(1.0f, 1.0f, 1.0f);
			*/

			/*
			rot = handle->rotateHandle(rot, core::vector3df(0.0f, 0.0f, 0.0f));
			if (handle->endCheck())
			{
				os::Printer::log("Handles changed!");
				handle->end();
			}
			*/

			/*
			scale = handle->scaleHandle(scale, core::vector3df(1.0f, 0.0f, 1.0f));
			if (handle->endCheck())
			{
				os::Printer::log("Handles changed!");
				handle->end();
			}
			*/
		}
	}
}