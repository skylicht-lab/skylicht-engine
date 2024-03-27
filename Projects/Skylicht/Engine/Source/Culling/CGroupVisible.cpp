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
	CGroupVisible::CGroupVisible() :
		CEntityGroup(NULL, 0)
	{
		m_dataTypes.push_back(DATA_TYPE_INDEX(CVisibleData));
	}

	CGroupVisible::~CGroupVisible()
	{

	}

	void CGroupVisible::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		CEntity** allEntities = entityManager->getEntities();

		m_entities.reset();

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
				m_entities.push(entity);
		}

		m_needQuery = false;
		m_needValidate = true;
	}
}