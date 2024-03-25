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
#include "CBoxCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CBoxCollider);

		CATEGORY_COMPONENT(CBoxCollider, "Box Collider", "Physics/Collider");

		CBoxCollider::CBoxCollider() :
			m_size(1.0f, 1.0f, 1.0f)
		{
			m_colliderType = CCollider::Box;
		}

		CBoxCollider::~CBoxCollider()
		{

		}

		void CBoxCollider::initComponent()
		{

		}

		void CBoxCollider::updateComponent()
		{

		}

		CObjectSerializable* CBoxCollider::createSerializable()
		{
			CObjectSerializable* obj = CComponentSystem::createSerializable();
			obj->addProperty(new CFloatProperty(obj, "sizeX", m_size.X));
			obj->addProperty(new CFloatProperty(obj, "sizeY", m_size.Y));
			obj->addProperty(new CFloatProperty(obj, "sizeZ", m_size.Z));
			return obj;
		}

		void CBoxCollider::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
			m_size.X = object->get<float>("sizeX", 1.0f);
			m_size.Y = object->get<float>("sizeY", 1.0f);
			m_size.Z = object->get<float>("sizeZ", 1.0f);
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CBoxCollider::initCollisionShape()
		{
			core::vector3df s = m_size * 0.5f;
			m_shape = new btBoxShape(btVector3(s.X, s.Y, s.Z));
			m_shape->setUserPointer(this);
			return m_shape;
		}
#endif
	}
}