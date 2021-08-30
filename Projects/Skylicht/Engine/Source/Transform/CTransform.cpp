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

namespace Skylicht
{
	core::vector3df CTransform::s_ox = core::vector3df(1.0f, 0.0f, 0.0f);
	core::vector3df CTransform::s_oy = core::vector3df(0.0f, 1.0f, 0.0f);
	core::vector3df CTransform::s_oz = core::vector3df(0.0f, 0.0f, 1.0f);

	CTransform::CTransform() :
		m_hasChanged(true)
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

		CTransform* parent = getParent();
		while (parent != NULL)
		{
			result = parent->getRelativeTransform() * result;
			parent = parent->getParent();
		}

		return result;
	}

	void CTransform::setWorldMatrix(const core::matrix4& world)
	{
		core::matrix4 parentInv;

		CTransform* parent = getParent();
		if (parent != NULL)
		{
			parentInv = parent->calcWorldTransform();
			parentInv.makeInverse();
		}

		core::matrix4 relative = parentInv * world;
		setRelativeTransform(relative);
	}
}