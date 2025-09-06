/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CLightSystem.h"
#include "Culling/CVisibleData.h"
#include "Culling/CCullingData.h"
#include "Entity/CEntityManager.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	CLightSystem::CLightSystem() :
		m_group(NULL),
		m_currentDLight(NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			m_currentPLight[i] = NULL;
			m_currentSLight[i] = NULL;
		}
		m_pipelineType = IRenderPipeline::Forwarder;
	}

	CLightSystem::~CLightSystem()
	{
	}

	void CLightSystem::beginQuery(CEntityManager* entityManager)
	{
		m_pointLights.set_used(0);
		m_spotLights.set_used(0);
		m_areaLights.set_used(0);
		m_dirLights.set_used(0);

		if (!m_group)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CLightCullingData);
			m_group = entityManager->createGroupFromVisible(type, 1);
		}
	}

	void CLightSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);

			CLightCullingData* lightData = GET_ENTITY_DATA(entity, CLightCullingData);
			if (lightData->Visible)
			{
				switch (lightData->LightType)
				{
				case CLight::DirectionalLight:
					m_dirLights.push_back(lightData);
					break;
				case CLight::PointLight:
					m_pointLights.push_back(lightData);
					break;
				case CLight::SpotLight:
					m_spotLights.push_back(lightData);
					break;
				case CLight::AreaLight:
					m_areaLights.push_back(lightData);
					break;
				default:
					break;
				};
			}
		}
	}

	void CLightSystem::init(CEntityManager* entityManager)
	{

	}

	void CLightSystem::update(CEntityManager* entityManager)
	{

	}

	void CLightSystem::render(CEntityManager* entityManager)
	{

	}

	void CLightSystem::postRender(CEntityManager* entityManager)
	{

	}

	void CLightSystem::onBeginSetupLight(CRenderMeshData* data, CWorldTransformData* transform)
	{
		m_currentDLight = CShaderLighting::getDirectionalLight();
		for (int i = 0; i < 4; i++)
		{
			m_currentPLight[i] = CShaderLighting::getPointLight(i);
			m_currentSLight[i] = CShaderLighting::getSpotLight(i);
			m_currentALight[i] = CShaderLighting::getAreaLight(i);

			CShaderLighting::setPointLight(NULL, i);
			CShaderLighting::setSpotLight(NULL, i);
			CShaderLighting::setAreaLight(NULL, i);
		}

		SDistanceLightEntry entry;

		u32 objLayer = data->getLightLayers();

		// direction light
		m_sorts.set_used(0);
		CLightCullingData** lights = m_dirLights.pointer();
		int lightCount = m_dirLights.size();

		for (int i = 0; i < lightCount; i++)
		{
			CLight* light = lights[i]->Light;

			u32 priority = light->getLightPriority();
			u32 lightLayer = light->getLightLayers();

			if (objLayer & lightLayer)
			{
				entry.Data = lights[i];
				entry.Light = light;
				entry.Distance = 0.0f;

				int n = m_sorts.size();
				if (n == 0 || priority < m_sorts[n - 1].Light->getLightPriority())
					m_sorts.push_back(entry);
				else
				{
					// insert sort by priority
					for (int i = 0; i < n; i++)
					{
						if (priority > m_sorts[i].Light->getLightPriority())
						{
							m_sorts.insert(entry, i);
							break;
						}
					}
				}
			}
		}

		if (m_sorts.size() == 0)
			CShaderLighting::setDirectionalLight(NULL);
		else
			CShaderLighting::setDirectionalLight((CDirectionalLight*)m_sorts[0].Light);

		core::vector3df position = transform->getWorldPosition();

		// point light
		lights = m_pointLights.pointer();
		lightCount = m_pointLights.size();

		sortLights(position, objLayer, lights, lightCount);

		lightCount = m_sorts.size();
		lightCount = core::min_(lightCount, 4);
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setPointLight((CPointLight*)m_sorts[i].Light, i);

		// spotlight
		lights = m_spotLights.pointer();
		lightCount = m_spotLights.size();

		sortLights(position, objLayer, lights, lightCount);

		lightCount = m_sorts.size();
		lightCount = core::min_(lightCount, 4);
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setSpotLight((CSpotLight*)m_sorts[i].Light, i);

		// area light
		lights = m_areaLights.pointer();
		lightCount = m_areaLights.size();

		sortLights(position, objLayer, lights, lightCount);

		lightCount = m_sorts.size();
		lightCount = core::min_(lightCount, 4);
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setAreaLight((CAreaLight*)m_sorts[i].Light, i);
	}

	void CLightSystem::sortLights(const core::vector3df& position, u32 objLayer, CLightCullingData** lights, int lightCount)
	{
		SDistanceLightEntry entry;
		m_sorts.set_used(0);

		for (int i = 0; i < lightCount; i++)
		{
			CLight* light = lights[i]->Light;
			u32 lightLayer = light->getLightLayers();

			if (objLayer & lightLayer)
			{
				entry.Data = lights[i];
				entry.Light = light;
				entry.Distance = lights[i]->LightPosition.getDistanceFromSQ(position);

				int n = m_sorts.size();
				if (n == 0 || entry.Distance > m_sorts[n - 1].Distance)
					m_sorts.push_back(entry);
				else
				{
					// insert sort by distance
					for (int i = 0; i < n; i++)
					{
						if (entry.Distance < m_sorts[i].Distance)
						{
							m_sorts.insert(entry, i);
							break;
						}
					}
				}
			}
		}
	}

	void CLightSystem::onEndSetupLight()
	{
		CShaderLighting::setDirectionalLight(m_currentDLight);

		for (int i = 0; i < 4; i++)
		{
			CShaderLighting::setPointLight(m_currentPLight[i], i);
			CShaderLighting::setSpotLight(m_currentSLight[i], i);
			CShaderLighting::setAreaLight(m_currentALight[i], i);
		}
	}
}