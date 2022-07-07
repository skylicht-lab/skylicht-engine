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
#include "CReflectionProbeSystem.h"

namespace Skylicht
{
	CReflectionProbeSystem::CReflectionProbeSystem()
	{
		m_kdtree = kd_create(3);
	}

	CReflectionProbeSystem::~CReflectionProbeSystem()
	{
		kd_free(m_kdtree);
	}

	void CReflectionProbeSystem::beginQuery(CEntityManager* entityManager)
	{
		m_probes.set_used(0);
		m_probePositions.set_used(0);

		m_entities.set_used(0);
		m_entitiesPositions.set_used(0);
	}

	void CReflectionProbeSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);
		CIndirectLightingData* lightData = GET_ENTITY_DATA(entity, CIndirectLightingData);
		CReflectionProbeData* probeData = GET_ENTITY_DATA(entity, CReflectionProbeData);

		if (probeData != NULL && probeData->ReflectionTexture != NULL)
		{
			m_probes.push_back(probeData);
			m_probePositions.push_back(transformData);

			if (transformData->NeedValidate || probeData->Invalidate)
			{
				m_probeChange = true;
				probeData->Invalidate = false;
			}
		}
		else if (lightData != NULL)
		{
			if (transformData->NeedValidate || lightData->Init || m_probeChange)
			{
				m_entities.push_back(lightData);
				m_entitiesPositions.push_back(transformData);
			}
		}
	}

	void CReflectionProbeSystem::init(CEntityManager* entityManager)
	{

	}

	void CReflectionProbeSystem::update(CEntityManager* entityManager)
	{
		if (m_probeChange)
		{
			kd_clear(m_kdtree);

			u32 n = m_probePositions.size();

			CWorldTransformData** worlds = m_probePositions.pointer();
			CReflectionProbeData** data = m_probes.pointer();

			for (u32 i = 0; i < n; i++)
			{
				f32* m = worlds[i]->World.pointer();
				kd_insert3f(m_kdtree, m[12], m[13], m[14], data[i]);
			}

			m_probeChange = false;
		}

		for (u32 i = 0, n = m_entities.size(); i < n; i++)
		{
			core::vector3df position = m_entitiesPositions[i]->World.getTranslation();

			// query nearst probe
			kdres* res = kd_nearest3f(m_kdtree, position.X, position.Y, position.Z);
			if (res != NULL)
			{
				while (!kd_res_end(res))
				{
					// get probe data
					CReflectionProbeData* probe = (CReflectionProbeData*)kd_res_item_data(res);
					if (probe != NULL)
					{
						// get indirectData
						CIndirectLightingData* indirectData = m_entities[i];
						indirectData->ReflectionTexture = probe->ReflectionTexture;
						indirectData->Init = false;
					}

					// kd_res_next(res);
					break;
				}
				kd_res_free(res);
			}
		}
	}
}