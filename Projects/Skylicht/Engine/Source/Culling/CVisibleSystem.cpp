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
#include "CVisibleSystem.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CVisibleSystem::CVisibleSystem()
	{

	}

	CVisibleSystem::~CVisibleSystem()
	{

	}

	void CVisibleSystem::beginQuery(CEntityManager* entityManager)
	{
		m_queries.Count = 0;
	}

	void CVisibleSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		SVisibleQuery* data = &m_queries;

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

			// hardcode dynamic array to optimize performance
			if ((data->Count + 1) >= data->Alloc)
			{
				int alloc = (data->Count + 1) * 2;
				if (alloc < 32)
					alloc = 32;

				data->Visibles.set_used(alloc);
				data->Transforms.set_used(alloc);
				data->Entities.set_used(alloc);

				data->VisiblesPtr = data->Visibles.pointer();
				data->TransformsPtr = data->Transforms.pointer();
				data->EntitiesPtr = data->Entities.pointer();

				data->Alloc = alloc;
			}

			data->TransformsPtr[data->Count] = transform;
			data->VisiblesPtr[data->Count] = visible;
			data->EntitiesPtr[data->Count] = entity;
			data->Count++;
		}
	}

	void CVisibleSystem::init(CEntityManager* entityManager)
	{

	}

	void CVisibleSystem::update(CEntityManager* entityManager)
	{
		SVisibleQuery* data = &m_queries;

		CVisibleData** visibles = data->VisiblesPtr;
		CWorldTransformData** transforms = data->TransformsPtr;
		CEntity** entities = data->EntitiesPtr;

		CEntity** allEntities = entityManager->getEntities();

		u32 numEntity = data->Count;

		for (u32 i = 0; i < numEntity; i++)
		{
			CVisibleData* visible = visibles[i];

			visible->SelfVisible = entities[i]->isVisible();
			visible->Visible = visible->SelfVisible;

			if (visible->Visible == true && transforms[i]->ParentIndex >= 0)
			{
				// link parent visible
				CEntity* parentEntity = allEntities[transforms[i]->ParentIndex];
				CVisibleData* parentVisible = GET_ENTITY_DATA(parentEntity, CVisibleData);
				visible->Visible = parentVisible->Visible;
			}
		}
	}
}