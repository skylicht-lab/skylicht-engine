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
#include "CLODSystem.h"
#include "Entity/CEntityManager.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "Camera/CCamera.h"
#include "RenderPipeline/CShadowMapRP.h"

namespace Skylicht
{
	CLODSystem::CLODSystem() :
		m_group(NULL)
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CLODSystem::~CLODSystem()
	{

	}

	void CLODSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 visibleGroupType[] = GET_LIST_ENTITY_DATA(CVisibleData);
			CEntityGroup* visibleGroup = entityManager->findGroup(visibleGroupType, 1);

			const u32 type[] = GET_LIST_ENTITY_DATA(CLODData);
			m_group = entityManager->createGroup(type, 1, visibleGroup);
		}
	}

	void CLODSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CLODSystem::init(CEntityManager* entityManager)
	{

	}

	void CLODSystem::update(CEntityManager* entityManager)
	{
		IRenderPipeline* rp = entityManager->getRenderPipeline();
		if (rp == NULL)
			return;

		CCamera* camera = entityManager->getCamera();
		if (camera == NULL)
			return;

		core::vector3df cameraPosition = camera->getGameObject()->getPosition();
		core::vector3df distance;

		CEntity** entities = m_group->getEntities();
		u32 numEntity = m_group->getEntityCount();

		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
			CLODData* lod = GET_ENTITY_DATA(entity, CLODData);

			const f32* m = transform->World.pointer();

			// distance vector
			float x = cameraPosition.X - m[12];
			float z = cameraPosition.Z - m[14];

			// length vector
			float d = x * x + z * z;

			// culling
			if (d < lod->From || d >= lod->To)
				visible->Culled = true;
			else
				visible->Culled = false;
		}
	}

	void CLODSystem::render(CEntityManager* entityManager)
	{

	}

	void CLODSystem::postRender(CEntityManager* entityManager)
	{

	}
}