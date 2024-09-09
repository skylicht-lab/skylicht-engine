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
#include "CWorldInverseTransformSystem.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Transform/CTransform.h"

namespace Skylicht
{
	CWorldInverseTransformSystem::CWorldInverseTransformSystem() :
		m_group(NULL)
	{
	}

	CWorldInverseTransformSystem::~CWorldInverseTransformSystem()
	{
	}

	void CWorldInverseTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{			
			const u32 type[] = GET_LIST_ENTITY_DATA(CWorldInverseTransformData);
			m_group = entityManager->createGroupFromVisible(type, 1);
		}
	}

	void CWorldInverseTransformSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CWorldInverseTransformSystem::init(CEntityManager* entityManager)
	{

	}

	void CWorldInverseTransformSystem::update(CEntityManager* entityManager)
	{
		CEntity** entities = m_group->getEntities();
		int numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);
			CWorldInverseTransformData* worldInv = GET_ENTITY_DATA(entity, CWorldInverseTransformData);

			if (world->NeedValidate)
			{
				// Get inverse matrix of world
				world->World.getInverse(worldInv->WorldInverse);
			}
		}
	}
}