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
#include "CStaticPlaneCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CStaticPlaneCollider);

		CATEGORY_COMPONENT(CStaticPlaneCollider, "Plane Collider", "Physics/Collider");

		CStaticPlaneCollider::CStaticPlaneCollider() :
			m_normal(0.0f, 1.0f, 0.0f),
			m_d(0.0f)
		{
			m_colliderType = CCollider::Plane;
		}

		CStaticPlaneCollider::~CStaticPlaneCollider()
		{

		}

		void CStaticPlaneCollider::initComponent()
		{

		}

		void CStaticPlaneCollider::updateComponent()
		{

		}

		void CStaticPlaneCollider::setPlane(const core::vector3df& normal, float d)
		{
			m_d = d;
			m_normal = normal;
			m_normal.normalize();
		}

		void CStaticPlaneCollider::setPlane(const core::vector3df& normal, const core::vector3df& origin)
		{
			core::plane3df plane(origin, normal);
			m_d = plane.D;
			m_normal = normal;
			m_normal.normalize();
		}

		CObjectSerializable* CStaticPlaneCollider::createSerializable()
		{
			CObjectSerializable* obj = CComponentSystem::createSerializable();
			obj->addProperty(new CFloatProperty(obj, "normalX", m_normal.X));
			obj->addProperty(new CFloatProperty(obj, "normalY", m_normal.Y));
			obj->addProperty(new CFloatProperty(obj, "normalZ", m_normal.Z));
			obj->addProperty(new CFloatProperty(obj, "d", m_d));
			return obj;
		}

		void CStaticPlaneCollider::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
			m_normal.X = object->get<float>("normalX", 0.0f);
			m_normal.Y = object->get<float>("normalY", 1.0f);
			m_normal.Z = object->get<float>("normalZ", 0.0f);
			m_d = object->get<float>("d", 0.0f);
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CStaticPlaneCollider::initCollisionShape()
		{
			m_shape = new btStaticPlaneShape(btVector3(m_normal.X, m_normal.Y, m_normal.Z), m_d);
			m_shape->setUserPointer(this);
			return m_shape;
		}
#endif
	}
}