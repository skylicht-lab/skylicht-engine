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

	}

	CReflectionProbeSystem::~CReflectionProbeSystem()
	{

	}

	void CReflectionProbeSystem::beginQuery(CEntityManager* entityManager)
	{
		m_probes.set_used(0);
		m_probePositions.set_used(0);

		m_entities.set_used(0);
		m_entitiesPositions.set_used(0);
		m_minDistance.set_used(0);
	}

	void CReflectionProbeSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CWorldTransformData* transformData = entity->getData<CWorldTransformData>();
		CIndirectLightingData* lightData = entity->getData<CIndirectLightingData>();

		CReflectionProbeData* probeData = entity->getData<CReflectionProbeData>();
		if (probeData != NULL && probeData->ReflectionTexture != NULL)
		{
			if (transformData != NULL)
			{
				m_probes.push_back(probeData);
				m_probePositions.push_back(transformData);
			}
		}
		else if (lightData != NULL)
		{
			m_entities.push_back(lightData);
			m_entitiesPositions.push_back(transformData);
			m_minDistance.push_back(999999999.9f);
		}
	}

	void CReflectionProbeSystem::init(CEntityManager* entityManager)
	{

	}

	void CReflectionProbeSystem::update(CEntityManager* entityManager)
	{
		for (u32 i = 0, n = m_probes.size(); i < n; i++)
		{
			core::vector3df probePosition = m_probePositions[i]->World.getTranslation();

			for (u32 j = 0, m = m_entities.size(); j < m; j++)
			{
				core::vector3df entityPosition = m_entitiesPositions[j]->World.getTranslation();

				// find nearest probe
				float d = probePosition.getDistanceFromSQ(entityPosition);
				if (d < m_minDistance[j])
				{
					m_minDistance[j] = d;

					// set reflection texture
					m_entities[j]->ReflectionTexture = m_probes[i]->ReflectionTexture;
				}
			}
		}
	}
}