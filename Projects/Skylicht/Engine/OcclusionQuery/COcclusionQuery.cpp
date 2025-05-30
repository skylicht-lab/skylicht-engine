/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "COcclusionQuery.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntity.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(COcclusionQuery);

	CATEGORY_COMPONENT(COcclusionQuery, "Occlusion Query", "Renderer");

	COcclusionQuery::COcclusionQuery() :
		m_queryData(NULL)
	{

	}

	COcclusionQuery::~COcclusionQuery()
	{
		if (m_gameObject)
			m_gameObject->getEntity()->removeData<COcclusionQueryData>();
	}

	void COcclusionQuery::initComponent()
	{
		m_queryData = m_gameObject->getEntity()->addData<COcclusionQueryData>();
	}

	void COcclusionQuery::updateComponent()
	{

	}

	void COcclusionQuery::onEnable(bool b)
	{
		m_queryData->Enable = b;
	}

	CObjectSerializable* COcclusionQuery::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();
		object->autoRelease(new CVector3Property(object, "boxMin", m_queryData->getAABBox().MinEdge));
		object->autoRelease(new CVector3Property(object, "boxMax", m_queryData->getAABBox().MaxEdge));
		return object;
	}

	void COcclusionQuery::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);
		core::aabbox3df box;
		box.MinEdge = object->get<core::vector3df>("boxMin", core::vector3df());
		box.MaxEdge = object->get<core::vector3df>("boxMax", core::vector3df());
		m_queryData->setAABBox(box);
	}
}