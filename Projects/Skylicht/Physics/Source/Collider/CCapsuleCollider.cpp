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
#include "CCapsuleCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CCapsuleCollider);

		CATEGORY_COMPONENT(CCapsuleCollider, "Capsule Collider", "Physics/Collider");

		CCapsuleCollider::CCapsuleCollider() :
			m_radius(0.5f),
			m_height(2.0f)
		{
			m_colliderType = CCollider::Capsule;
		}

		CCapsuleCollider::~CCapsuleCollider()
		{

		}

		void CCapsuleCollider::initComponent()
		{

		}

		void CCapsuleCollider::updateComponent()
		{

		}

		CObjectSerializable* CCapsuleCollider::createSerializable()
		{
			CObjectSerializable* obj = CComponentSystem::createSerializable();
			obj->addProperty(new CFloatProperty(obj, "radius", m_radius));
			obj->addProperty(new CFloatProperty(obj, "height", m_height));
			return obj;
		}

		void CCapsuleCollider::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
			m_radius = object->get<float>("radius", 1.0f);
			m_height = object->get<float>("height", 1.0f);
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CCapsuleCollider::initCollisionShape()
		{
			m_shape = new btCapsuleShape(m_radius, m_height);
			m_shape->setUserPointer(this);
			return m_shape;
		}
#endif
	}
}