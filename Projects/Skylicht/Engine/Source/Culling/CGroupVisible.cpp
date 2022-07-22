/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CGroupVisible.h"
#include "CVisibleData.h"

#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	const u32 k_visibleGroupData[] = { CVisibleData::DataTypeIndex };

	CGroupVisible::CGroupVisible() :
		CEntityGroup(k_visibleGroupData, 1)
	{

	}

	CGroupVisible::~CGroupVisible()
	{

	}

	void CGroupVisible::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		CEntity** allEntities = entityManager->getEntities();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

			visible->SelfVisible = entities[i]->isVisible();
			visible->Visible = visible->SelfVisible;
			visible->Culled = false;

			if (visible->Visible == true && transform->ParentIndex >= 0)
			{
				// link parent visible
				CEntity* parentEntity = allEntities[transform->ParentIndex];
				CVisibleData* parentVisible = GET_ENTITY_DATA(parentEntity, CVisibleData);
				visible->Visible = parentVisible->Visible;
			}

			// only select visible
			if (visible->Visible)
			{
				if ((m_count + 1) >= m_alloc)
				{
					int alloc = (m_count + 1) * 2;
					if (alloc < 32)
						alloc = 32;

					m_entities.set_used(alloc);
					m_entitiesPtr = m_entities.pointer();

					m_alloc = alloc;
				}

				m_entitiesPtr[m_count++] = entity;
			}
		}

		m_needQuery = false;
	}
}