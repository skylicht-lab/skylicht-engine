/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CTransform.h"
#include "GameObject/CGameObject.h"

#include "Entity/CEntityManager.h"

#include "CWorldTransformData.h"
#include "Entity/CEntity.h"
#include "Entity/CEntityManager.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	namespace Transform
	{
		core::vector3df Ox = core::vector3df(1.0f, 0.0f, 0.0f);
		core::vector3df Oy = core::vector3df(0.0f, 1.0f, 0.0f);
		core::vector3df Oz = core::vector3df(0.0f, 0.0f, 1.0f);

		void getNonScaleTransform(const core::matrix4& src, core::matrix4& dst)
		{
			core::vector3df pos = src.getTranslation();
			core::vector3df x = Transform::Ox;
			core::vector3df y = Transform::Oy;
			core::vector3df z = Transform::Oz;
			src.rotateVect(x);
			src.rotateVect(y);
			src.rotateVect(z);
			x.normalize();
			y.normalize();
			z.normalize();

			f32* matData = dst.pointer();

			matData[0] = x.X;
			matData[1] = x.Y;
			matData[2] = x.Z;
			matData[3] = 0.0f;

			matData[4] = y.X;
			matData[5] = y.Y;
			matData[6] = y.Z;
			matData[7] = 0.0f;

			matData[8] = z.X;
			matData[9] = z.Y;
			matData[10] = z.Z;
			matData[11] = 0.0f;

			matData[12] = pos.X;
			matData[13] = pos.Y;
			matData[14] = pos.Z;
			matData[15] = 1.0f;
		}
	}

	CTransform::CTransform() :
		m_hasChanged(true),
		m_attached(false),
		m_isWorldTransform(false)
	{

	}

	CTransform::~CTransform()
	{

	}

	void CTransform::initComponent()
	{
		m_gameObject->getEntityManager()->addTransformDataToEntity(m_gameObject->getEntity(), this);
	}

	void CTransform::updateComponent()
	{

	}

	void CTransform::attachTransform(CEntity* entity)
	{
		CWorldTransformData* transform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);
		CWorldTransformData* parent = GET_ENTITY_DATA(entity, CWorldTransformData);

		if (entity)
		{
			transform->AttachParentIndex = entity->getIndex();
			m_attached = true;
		}
		else
		{
			CEntity* defaultParent = m_gameObject->getEntityManager()->getEntity(transform->ParentIndex);
			parent = GET_ENTITY_DATA(defaultParent, CWorldTransformData);

			transform->AttachParentIndex = -1;
			m_attached = false;
		}

		// notify sort update
		m_gameObject->getEntityManager()->notifyUpdateSortEntities();
	}

	CTransform* CTransform::getParent()
	{
		CGameObject* parent = getParentObject();
		if (parent != NULL)
			return parent->getTransform();
		return NULL;
	}

	CGameObject* CTransform::getParentObject()
	{
		return m_gameObject->getParent();
	}

	CEntity* CTransform::getParentEntity()
	{
		CGameObject* parent = getParentObject();
		if (parent != NULL)
			return parent->getEntity();
		return NULL;
	}

	core::matrix4 CTransform::calcWorldTransform()
	{
		core::matrix4 result = getRelativeTransform();

		if (!isAttached())
		{
			CTransform* parent = getParent();
			while (parent != NULL)
			{
				result = parent->getRelativeTransform() * result;
				parent = parent->getParent();
			}
		}
		else
		{
			CWorldTransformData* transform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);

			CEntity* parent = m_gameObject->getEntityManager()->getEntity(transform->AttachParentIndex);
			CWorldTransformData* parentTransform = GET_ENTITY_DATA(parent, CWorldTransformData);

			result = parentTransform->World * result;
		}

		return result;
	}

	core::vector3df CTransform::getWorldPosition()
	{
		core::vector3df p = getRelativePosition();

		if (!isAttached())
		{
			CTransform* parent = getParent();
			while (parent != NULL)
			{
				parent->getRelativeTransform().transformVect(p);
				parent = parent->getParent();
			}
		}
		else
		{
			CWorldTransformData* transform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);

			CEntity* parent = m_gameObject->getEntityManager()->getEntity(transform->AttachParentIndex);
			CWorldTransformData* parentTransform = GET_ENTITY_DATA(parent, CWorldTransformData);

			parentTransform->World.transformVect(p);
		}

		return p;
	}

	void CTransform::setWorldMatrix(const core::matrix4& world)
	{
		if (m_isWorldTransform)
		{
			setRelativeTransform(world);
			return;
		}

		core::matrix4 parentInv;

		if (!isAttached())
		{
			CTransform* parent = getParent();
			if (parent != NULL)
			{
				parentInv = parent->calcWorldTransform();
				parentInv.makeInverse();
			}
		}
		else
		{
			CWorldTransformData* transform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);

			CEntity* parent = m_gameObject->getEntityManager()->getEntity(transform->AttachParentIndex);
			CWorldTransformData* parentTransform = GET_ENTITY_DATA(parent, CWorldTransformData);
			parentInv = parentTransform->World;
			parentInv.makeInverse();
		}

		core::matrix4 relative = parentInv * world;
		setRelativeTransform(relative);
	}
}