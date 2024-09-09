/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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
#include "CCameraBrain.h"
#include "Transform/CWorldTransformSystem.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Utils/CVector.h"

namespace Skylicht
{
	CCameraBrain::CCameraBrain() :
		m_camera(NULL),
		m_targetCamera(NULL),
		m_blend(0.0f)
	{
		m_lookAt.set(0.0f, 0.0f, 1.0f);
		m_upVector.set(0.0f, 1.0f, 0.0f);
	}

	CCameraBrain::~CCameraBrain()
	{
		CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
		transformSystem->unRegisterLateUpdate(this);
	}

	void CCameraBrain::initComponent()
	{
		CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
		transformSystem->registerLateUpdate(this);
	}

	void CCameraBrain::updateComponent()
	{
		if (m_camera == NULL)
			m_camera = m_gameObject->getComponent<CCamera>();
	}

	void CCameraBrain::setTargetCamera(CCamera* cam, float blendTarget)
	{
		m_targetCamera = cam;
		m_lastPosition = m_position;
		m_lastLookAt = m_lookAt;
		m_lastUpVector = m_upVector;
		m_blend = blendTarget;
	}

	void CCameraBrain::lateUpdate()
	{
		if (m_camera == NULL ||
			m_targetCamera == NULL)
			return;

		core::vector3df position = m_targetCamera->getPosition();
		core::vector3df lookAt = m_targetCamera->getLookVector();
		core::vector3df upVector = m_targetCamera->getUpVector();

		if (m_blend < 1.0f)
		{
			m_position = CVector::lerp(m_lastPosition, position, m_blend);
			m_lookAt = CVector::slerp(m_lastLookAt, lookAt, m_blend);
			m_upVector = CVector::slerp(m_lastUpVector, upVector, m_blend);
		}
		else
		{
			m_position = position;
			m_lookAt = lookAt;
			m_upVector = upVector;
		}

		m_camera->lookAt(m_position, m_position + m_lookAt, m_upVector);
		m_camera->recalculateViewMatrix();
	}
}