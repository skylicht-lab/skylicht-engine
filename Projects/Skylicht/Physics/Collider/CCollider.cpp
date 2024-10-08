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
#include "CCollider.h"
#include "RigidBody/CRigidbody.h"
#include "GameObject/CGameObject.h"
#include "PhysicsEngine/CPhysicsEngine.h"

namespace Skylicht
{
	namespace Physics
	{
		CCollider::CCollider() :
			m_colliderType(Unknown)
#ifdef USE_BULLET_PHYSIC_ENGINE
			, m_shape(NULL)
#endif
		{

		}

		CCollider::~CCollider()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			dropCollisionShape();
#endif
		}

		void CCollider::initComponent()
		{
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine && engine->IsInEditor)
				initRigidbody();
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		void CCollider::dropCollisionShape()
		{
			if (m_shape)
				m_shape->setUserPointer(NULL);
			m_shape = NULL;
		}
#endif

		void CCollider::initRigidbody()
		{
			CRigidbody* rigidBody = m_gameObject->getComponent<CRigidbody>();
			if (rigidBody)
				rigidBody->initRigidbody();
		}

		void CCollider::clampSize(core::vector3df& size)
		{
			size.X = core::max_(size.X, 0.0f);
			size.Y = core::max_(size.Y, 0.0f);
			size.Z = core::max_(size.Z, 0.0f);
		}
	}
}