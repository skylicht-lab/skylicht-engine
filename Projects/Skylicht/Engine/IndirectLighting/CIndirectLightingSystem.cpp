/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "Entity/CEntityManager.h"
#include "CIndirectLightingSystem.h"

namespace Skylicht
{
	CIndirectLightingSystem::CIndirectLightingSystem() :
		m_probeChange(false),
		m_groupLighting(NULL),
		m_groupProbes(NULL)
	{
		m_kdtree = kd_create(3);
	}

	CIndirectLightingSystem::~CIndirectLightingSystem()
	{
		kd_free(m_kdtree);
	}

	void CIndirectLightingSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_groupLighting == NULL)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CIndirectLightingData);
			m_groupLighting = entityManager->createGroupFromVisible(type, 1);
		}

		if (m_groupProbes == NULL)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CLightProbeData);
			m_groupProbes = entityManager->createGroupFromVisible(type, 1);
		}

		m_entities.set_used(0);
		m_entitiesPositions.set_used(0);

		m_probes.set_used(0);
		m_probePositions.set_used(0);
	}

	void CIndirectLightingSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_groupProbes->getEntities();
		numEntity = m_groupProbes->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CLightProbeData* probeData = GET_ENTITY_DATA(entity, CLightProbeData);
			m_probes.push_back(probeData);

			CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);
			m_probePositions.push_back(transformData);

			if (transformData->NeedValidate || probeData->NeedValidate)
			{
				m_probeChange = true;
				probeData->NeedValidate = false;
			}
		}

		entities = m_groupLighting->getEntities();
		numEntity = m_groupLighting->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CIndirectLightingData* lightData = GET_ENTITY_DATA(entity, CIndirectLightingData);
			if (lightData->AutoSH &&
				*lightData->AutoSH &&
				lightData->Type == CIndirectLightingData::SH9)
			{
				CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);
				m_entities.push_back(lightData);
				m_entitiesPositions.push_back(transformData);
			}
		}
	}

	void CIndirectLightingSystem::init(CEntityManager* entityManager)
	{

	}

	void CIndirectLightingSystem::update(CEntityManager* entityManager)
	{
		if (m_probeChange)
		{
			kd_clear(m_kdtree);

			u32 n = m_probePositions.size();

			CWorldTransformData** worlds = m_probePositions.pointer();
			CLightProbeData** data = m_probes.pointer();

			for (u32 i = 0; i < n; i++)
			{
				f32* m = worlds[i]->World.pointer();
				kd_insert3f(m_kdtree, m[12], m[13], m[14], data[i]);
			}
		}

		u32 n = m_entitiesPositions.size();
		CWorldTransformData** worlds = m_entitiesPositions.pointer();
		CIndirectLightingData** data = m_entities.pointer();

		for (u32 i = 0; i < n; i++)
		{
			if (!worlds[i]->NeedValidate &&
				!data[i]->InvalidateProbe &&
				!m_probeChange)
			{
				continue;
			}

			float* m = worlds[i]->World.pointer();

			// query nearst probe
			kdres* res = kd_nearest3f(m_kdtree, m[12], m[13], m[14]);
			if (res != NULL)
			{
				while (!kd_res_end(res))
				{
					// float pos[3];
					// kd_res_itemf(res, pos);

					// get probe data
					CLightProbeData* probe = (CLightProbeData*)kd_res_item_data(res);
					if (probe != NULL)
					{
						// get indirectData
						CIndirectLightingData* indirectData = data[i];

						// copy sh data
						// need interpolate here, todo later
						for (int j = 0; j < 9; j++)
						{
							indirectData->SH[j].set(probe->SH[j]);
						}

						indirectData->InvalidateProbe = false;
					}

					// kd_res_next(res);
					break;
				}
				kd_res_free(res);
			}
		}

		m_probeChange = false;
	}
}