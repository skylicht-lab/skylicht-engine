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
	CVisibleSystem::CVisibleSystem() :
		m_maxDepth(0)
	{

	}

	CVisibleSystem::~CVisibleSystem()
	{

	}

	void CVisibleSystem::beginQuery(CEntityManager* entityManager)
	{
		for (int depth = 0; depth < MAX_CHILD_DEPTH; depth++)
			m_entities[depth].set_used(0);

		m_maxDepth = 0;
	}

	void CVisibleSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
		CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

		if (transform->Depth > m_maxDepth)
			m_maxDepth = transform->Depth;

		m_entities[transform->Depth].push_back(SVisibleData());

		SVisibleData& data = m_entities[transform->Depth].getLast();
		data.Transform = transform;
		data.Visible = visible;
		data.Entity = entity;
	}

	void CVisibleSystem::init(CEntityManager* entityManager)
	{

	}

	void CVisibleSystem::update(CEntityManager* entityManager)
	{
		for (int depth = 0; depth <= m_maxDepth; depth++)
		{
			SVisibleData* entities = m_entities[depth].pointer();
			u32 numEntity = m_entities[depth].size();

			for (u32 i = 0; i < numEntity; i++)
			{
				SVisibleData& data = entities[i];

				data.Visible->SelfVisible = data.Entity->isVisible();
				data.Visible->Visible = data.Visible->SelfVisible;

				if (data.Visible->Visible == true && data.Transform->ParentIndex >= 0)
				{
					// link parent visible
					CEntity* parentEntity = entityManager->getEntity(data.Transform->ParentIndex);
					CVisibleData* parentVisible = GET_ENTITY_DATA(parentEntity, CVisibleData);
					data.Visible->Visible = parentVisible->Visible;
				}
			}
		}
	}
}