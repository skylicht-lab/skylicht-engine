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
#include "CCylinderCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CCylinderCollider);

		CATEGORY_COMPONENT(CCylinderCollider, "Cylinder Collider", "Physics/Collider");

		CCylinderCollider::CCylinderCollider() :
			m_halfSize(1.0f, 1.0f, 1.0f)
		{
			m_colliderType = CCollider::Cylinder;
		}

		CCylinderCollider::~CCylinderCollider()
		{

		}

		void CCylinderCollider::updateComponent()
		{

		}

		CObjectSerializable* CCylinderCollider::createSerializable()
		{
			CObjectSerializable* obj = CComponentSystem::createSerializable();
			obj->autoRelease(new CVector3Property(obj, "halfSize", m_halfSize));
			return obj;
		}

		void CCylinderCollider::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
			m_halfSize = object->get<core::vector3df>("halfSize", core::vector3df(1.0f, 1.0f, 1.0f));

			clampSize(m_halfSize);
			initRigidbody();
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CCylinderCollider::initCollisionShape()
		{
			m_shape = new btCylinderShape(btVector3(m_halfSize.X, m_halfSize.Y, m_halfSize.Z));
			m_shape->setUserPointer(this);
			return m_shape;
		}
#endif
	}
}