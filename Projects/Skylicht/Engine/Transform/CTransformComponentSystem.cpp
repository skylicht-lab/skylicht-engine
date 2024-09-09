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
#include "Culling/CVisibleData.h"
#include "Entity/CEntityManager.h"
#include "CTransform.h"
#include "CWorldTransformData.h"
#include "CTransformComponentData.h"
#include "CTransformComponentSystem.h"

namespace Skylicht
{
	CComponentTransformSystem::CComponentTransformSystem() :
		m_group(NULL)
	{

	}

	CComponentTransformSystem::~CComponentTransformSystem()
	{

	}

	void CComponentTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CTransformComponentData);
			m_group = entityManager->createGroupFromVisible(type, 1);
		}
	}

	void CComponentTransformSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CComponentTransformSystem::init(CEntityManager* entityManager)
	{

	}

	void CComponentTransformSystem::update(CEntityManager* entityManager)
	{
		CEntity** entities = m_group->getEntities();
		u32 numEntity = m_group->getEntityCount();

		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			CTransformComponentData* component = GET_ENTITY_DATA(entity, CTransformComponentData);

			CTransform* transformComponent = component->TransformComponent;

			if (component != NULL
				&& transformComponent != NULL
				&& transformComponent->hasChanged())
			{
				// copy transform to relative matrix
				transformComponent->getRelativeTransform(transform->Relative);
				transformComponent->setChanged(false);

				// notify changed for CWorldTransformSystem, sync in TransformComponent->hasChanged()
				transform->HasChanged = true;

				// sync is world transform
				transform->IsWorldTransform = transformComponent->isWorldTransform();
			}
		}
	}
}