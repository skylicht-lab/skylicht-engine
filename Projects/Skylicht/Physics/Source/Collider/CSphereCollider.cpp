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
#include "CSphereCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CSphereCollider);

		CATEGORY_COMPONENT(CSphereCollider, "Sphere Collider", "Physics/Collider");

		CSphereCollider::CSphereCollider() :
			m_radius(0.5f)
		{
			m_colliderType = CCollider::Sphere;
		}

		CSphereCollider::~CSphereCollider()
		{

		}

		void CSphereCollider::initComponent()
		{

		}

		void CSphereCollider::updateComponent()
		{

		}

		CObjectSerializable* CSphereCollider::createSerializable()
		{
			CObjectSerializable* obj = CComponentSystem::createSerializable();
			obj->addProperty(new CFloatProperty(obj, "radius", m_radius));
			return obj;
		}

		void CSphereCollider::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
			m_radius = object->get<float>("radius", 0.5f);
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CSphereCollider::initCollisionShape()
		{
			m_shape = new btSphereShape(m_radius);
			m_shape->setUserPointer(this);
			return m_shape;
		}
#endif
	}
}