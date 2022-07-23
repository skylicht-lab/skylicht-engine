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
#include "Culling/CVisibleData.h"

namespace Skylicht
{
	CWorldTransformSystem::CWorldTransformSystem() :
		m_visibleGroup(NULL)
	{
	}

	CWorldTransformSystem::~CWorldTransformSystem()
	{

	}

	void CWorldTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_visibleGroup == NULL)
		{
			const u32 visibleGroupType[] = GET_LIST_ENTITY_DATA(CVisibleData);
			m_visibleGroup = entityManager->findGroup(visibleGroupType, 1);
		}
	}

	void CWorldTransformSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
	}

	void CWorldTransformSystem::init(CEntityManager* entityManager)
	{
	}

	void CWorldTransformSystem::update(CEntityManager* entityManager)
	{
		CEntity** entities = m_visibleGroup->getEntities();
		u32 numEntity = m_visibleGroup->getEntityCount();

		CEntity** allEntities = entityManager->getEntities();

		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* t = GET_ENTITY_DATA(entity, CWorldTransformData);
			t->NeedValidate = false;

			// check the parent
			int parentID = t->AttachParentIndex >= 0 ? t->AttachParentIndex : t->ParentIndex;

			// parent entity
			CEntity* parent = NULL;
			CWorldTransformData* parentTransform = NULL;

			if (parentID != -1)
			{
				parent = allEntities[parentID];
				parentTransform = GET_ENTITY_DATA(parent, CWorldTransformData);

				if (parentTransform->NeedValidate)
				{
					// this transform changed because parent is changed
					t->HasChanged = true;
					t->NeedValidate = true;
				}
			}

			if (t->HasChanged)
			{
				// update
				t->HasChanged = false;
				t->NeedValidate = true;

				if (t->Depth == 0)
				{
					t->World = t->Relative;
				}
				else
				{
					// calc world = parent * relative
					// - relative is copied from CTransformComponentSystem
					// - relative is also defined in CEntityPrefab
					t->World.setbyproduct_nocheck(parentTransform->World, t->Relative);
				}
			}
		}
	}
}