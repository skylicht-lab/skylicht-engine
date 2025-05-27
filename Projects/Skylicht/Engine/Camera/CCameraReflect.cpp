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
#include "CCameraReflect.h"
#include "Transform/CWorldTransformSystem.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Utils/CVector.h"

namespace Skylicht
{
	CCameraReflect::CCameraReflect() :
		m_camera(NULL),
		m_targetCamera(NULL)
	{
		m_plane.setPlane(core::vector3df(), Transform::Oy);
	}

	CCameraReflect::~CCameraReflect()
	{
		CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
		transformSystem->unRegisterLateUpdate(this);
	}

	void CCameraReflect::initComponent()
	{
		CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
		transformSystem->registerLateUpdate(this);
	}

	void CCameraReflect::updateComponent()
	{
		if (m_camera == NULL)
			m_camera = m_gameObject->getComponent<CCamera>();
	}

	void CCameraReflect::setTargetCamera(CCamera* cam)
	{
		m_targetCamera = cam;
	}

	void CCameraReflect::lateUpdate()
	{
		if (m_camera == NULL ||
			m_targetCamera == NULL)
			return;

		core::vector3df position = m_targetCamera->getPosition();
		core::vector3df lookAt = m_targetCamera->getLookVector();

		core::vector3df out;
		if (m_plane.getIntersectionWithLine(position, -m_plane.Normal, out))
		{
			core::vector3df d = out - position;
			core::vector3df p = position + 2.0f * d;

			core::vector3df l = CVector::reflect(lookAt, m_plane.Normal);
			l.normalize();

			m_camera->lookAt(p, p + l, Transform::Oy);
			m_camera->recalculateViewMatrix();
		}
	}
}