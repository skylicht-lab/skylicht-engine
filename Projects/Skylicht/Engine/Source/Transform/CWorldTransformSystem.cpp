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
#include "CWorldTransformSystem.h"
#include "CWorldInverseTransformData.h"
#include "Entity/CEntityManager.h"
#include "Transform/CTransform.h"

namespace Skylicht
{
	CWorldTransformSystem::CWorldTransformSystem()
	{
	}

	CWorldTransformSystem::~CWorldTransformSystem()
	{

	}

	void CWorldTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		SWorldTransformQuery* query = &m_queries;
		query->Count = 0;
	}

	void CWorldTransformSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		SWorldTransformQuery* query = &m_queries;

		CEntity** allEntities = entityManager->getEntities();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* t = GET_ENTITY_DATA(entity, CWorldTransformData);

			t->NeedValidate = false;

			int parentID = t->AttachParentIndex >= 0 ? t->AttachParentIndex : t->ParentIndex;

			if (t->HasChanged == true)
			{
				t->NeedValidate = true;

				// hardcode dynamic array to optimize performance
				if (query->Count + 1 >= query->Alloc)
				{
					int alloc = (query->Count + 1) * 2;
					if (alloc < 32)
						alloc = 32;

					query->Entities.set_used(alloc);
					query->EntitiesPtr = query->Entities.pointer();
					query->Alloc = alloc;
				}

				query->EntitiesPtr[query->Count++] = t;

				// notify recalc inverse matrix
				CWorldInverseTransformData* tInverse = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
				if (tInverse != NULL)
					tInverse->HasChanged = true;
			}
			else if (parentID != -1)
			{
				CWorldTransformData* parent = GET_ENTITY_DATA(allEntities[parentID], CWorldTransformData);
				if (parent->HasChanged == true)
				{
					// this transform changed because parent is changed
					t->HasChanged = true;
					t->NeedValidate = true;

					// notify recalc inverse matrix
					CWorldInverseTransformData* tInverse = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
					if (tInverse != NULL)
						tInverse->HasChanged = true;

					// hardcode dynamic array to optimize performance
					if (query->Count + 1 >= query->Alloc)
					{
						int alloc = (query->Count + 1) * 2;
						if (alloc < 32)
							alloc = 32;

						query->Entities.set_used(alloc);
						query->EntitiesPtr = query->Entities.pointer();
						query->Alloc = alloc;
					}

					query->EntitiesPtr[query->Count++] = t;
				}
			}
		}
	}

	void CWorldTransformSystem::init(CEntityManager* entityManager)
	{
	}

	void CWorldTransformSystem::update(CEntityManager* entityManager)
	{
		SWorldTransformQuery* query = &m_queries;

		CWorldTransformData** entities = query->EntitiesPtr;
		u32 numEntity = query->Count;

		CEntity** allEntities = entityManager->getEntities();

		for (u32 i = 0; i < numEntity; i++)
		{
			CWorldTransformData* t = entities[i];
			t->HasChanged = false;

			if (t->Depth == 0)
			{
				t->World = t->Relative;
			}
			else
			{
				int parentID = t->AttachParentIndex >= 0 ? t->AttachParentIndex : t->ParentIndex;

				// parent entity
				CEntity* parent = allEntities[parentID];
				CWorldTransformData* p = GET_ENTITY_DATA(parent, CWorldTransformData);

				// calc world = parent * relative
				// - relative is copied from CTransformComponentSystem
				// - relative is also defined in CEntityPrefab
				t->World.setbyproduct_nocheck(p->World, t->Relative);
			}
		}
	}
}