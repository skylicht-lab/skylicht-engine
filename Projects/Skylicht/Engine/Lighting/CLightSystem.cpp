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
	static const size_t LightSignatureOffset = 2166136261u;

	CLightSystem::SCacheLight::SCacheLight() :
		Signature(LightSignatureOffset),
		LastSignature(0),
		LastCount(-1)
	{

	}

	void CLightSystem::SCacheLight::resetSignature()
	{
		Signature = LightSignatureOffset;
	}

	void CLightSystem::SCacheLight::invalidate()
	{
		LastSignature = 0;
		LastCount = -1;
	}

	bool CLightSystem::SCacheLight::update(int count)
	{
		if (LastCount == count && LastSignature == Signature)
			return false;

		LastCount = count;
		LastSignature = Signature;
		return true;
	}

	CLightSystem::CLightSystem() :
		m_lightCacheVersion(1),
		m_group(NULL),
		m_entityManager(NULL),
		m_currentDLight(NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			m_currentPLight[i] = NULL;
			m_currentSLight[i] = NULL;
			m_currentALight[i] = NULL;
		}
		m_pipelineType = IRenderPipeline::Forwarder;
	}

	CLightSystem::~CLightSystem()
	{
		if (m_entityManager != NULL)
		{
			m_entityManager->unRegisterCallback(this);
			m_entityManager = NULL;
		}
	}

	void CLightSystem::beginQuery(CEntityManager* entityManager)
	{
		m_pointLights.set_used(0);
		m_spotLights.set_used(0);
		m_areaLights.set_used(0);
		m_dirLights.set_used(0);
		m_pointLightTransforms.set_used(0);
		m_spotLightTransforms.set_used(0);
		m_areaLightTransforms.set_used(0);

		m_dirLightCache.resetSignature();
		m_pointLightCache.resetSignature();
		m_spotLightCache.resetSignature();
		m_areaLightCache.resetSignature();

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

			switch (lightData->LightType)
			{
			case CLight::DirectionalLight:
				m_dirLights.push_back(lightData);
				addLightSignature(m_dirLightCache, entity, lightData);
				break;
			case CLight::PointLight:
				m_pointLights.push_back(lightData);
				m_pointLightTransforms.push_back(transformData);
				addLightSignature(m_pointLightCache, entity, lightData);
				break;
			case CLight::SpotLight:
				m_spotLights.push_back(lightData);
				m_spotLightTransforms.push_back(transformData);
				addLightSignature(m_spotLightCache, entity, lightData);
				break;
			case CLight::AreaLight:
				m_areaLights.push_back(lightData);
				m_areaLightTransforms.push_back(transformData);
				addLightSignature(m_areaLightCache, entity, lightData);
				break;
			default:
				break;
			};

		}

		updateLightCacheVersion();
	}

	void CLightSystem::init(CEntityManager* entityManager)
	{
		if (m_entityManager != entityManager)
		{
			if (m_entityManager != NULL)
				m_entityManager->unRegisterCallback(this);

			m_entityManager = entityManager;
			if (m_entityManager != NULL)
				m_entityManager->registerCallback(this);
		}
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

	void CLightSystem::onEntityRemoved(CEntity* entity)
	{
		if (GET_ENTITY_DATA(entity, CLightCullingData) != NULL)
			invalidateLightCacheVersion();
	}

	void CLightSystem::onEntityRemoved(CEntity** entities, int count)
	{
		for (int i = 0; i < count; i++)
		{
			if (GET_ENTITY_DATA(entities[i], CLightCullingData) != NULL)
			{
				invalidateLightCacheVersion();
				return;
			}
		}
	}

	void CLightSystem::onBeginSetupLight(CRenderLightData* data, CWorldTransformData* transform)
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

		u32 objLayer = data->getLightLayers();
		core::vector3df position = transform->getWorldPosition();
		bool transformChanged = transform->NeedValidate;

		if (data->CachedLightVersion != m_lightCacheVersion)
			data->invalidateLightCache();

		// direction light
		CLightCullingData** lights = m_dirLights.pointer();
		int lightCount = m_dirLights.size();

		if (!data->LightCacheValid ||
			data->CachedLightLayers != objLayer ||
			data->CachedDirectionalLights.VisibleCount != lightCount ||
			data->CachedDirectionalLights.Signature != m_dirLightCache.Signature)
		{
			SDistanceLightEntry entry;
			m_sorts.set_used(0);

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

					bool inserted = false;
					int n = m_sorts.size();
					for (int j = 0; j < n; j++)
					{
						if (priority > m_sorts[j].Light->getLightPriority())
						{
							m_sorts.insert(entry, j);
							inserted = true;
							break;
						}
					}
					if (!inserted)
						m_sorts.push_back(entry);
				}
			}

			data->CachedDirectionalLights.Lights[0] = m_sorts.size() > 0 ? m_sorts[0].Data : NULL;
			data->CachedDirectionalLights.Count = data->CachedDirectionalLights.Lights[0] ? 1 : 0;
			data->CachedDirectionalLights.VisibleCount = lightCount;
			data->CachedDirectionalLights.Signature = m_dirLightCache.Signature;
		}

		if (data->CachedDirectionalLights.Lights[0] == NULL)
			CShaderLighting::setDirectionalLight(NULL);
		else
			CShaderLighting::setDirectionalLight((CDirectionalLight*)data->CachedDirectionalLights.Lights[0]->Light);

		// point light
		lights = m_pointLights.pointer();
		lightCount = m_pointLights.size();

		if (needSortCachedLights(
			data,
			transformChanged,
			objLayer,
			lights,
			m_pointLightTransforms.pointer(),
			lightCount,
			data->CachedPointLights,
			m_pointLightCache.Signature))
		{
			cacheSortedLights(
				position,
				objLayer,
				lights,
				lightCount,
				data->CachedPointLights,
				m_pointLightCache.Signature);
		}

		lightCount = data->CachedPointLights.Count;
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setPointLight((CPointLight*)data->CachedPointLights.Lights[i]->Light, i);

		// spotlight
		lights = m_spotLights.pointer();
		lightCount = m_spotLights.size();

		if (needSortCachedLights(
			data,
			transformChanged,
			objLayer,
			lights,
			m_spotLightTransforms.pointer(),
			lightCount,
			data->CachedSpotLights,
			m_spotLightCache.Signature))
		{
			cacheSortedLights(
				position,
				objLayer,
				lights,
				lightCount,
				data->CachedSpotLights,
				m_spotLightCache.Signature);
		}

		lightCount = data->CachedSpotLights.Count;
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setSpotLight((CSpotLight*)data->CachedSpotLights.Lights[i]->Light, i);

		// area light
		lights = m_areaLights.pointer();
		lightCount = m_areaLights.size();

		if (needSortCachedLights(
			data,
			transformChanged,
			objLayer,
			lights,
			m_areaLightTransforms.pointer(),
			lightCount,
			data->CachedAreaLights,
			m_areaLightCache.Signature))
		{
			cacheSortedLights(
				position,
				objLayer,
				lights,
				lightCount,
				data->CachedAreaLights,
				m_areaLightCache.Signature);
		}

		lightCount = data->CachedAreaLights.Count;
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setAreaLight((CAreaLight*)data->CachedAreaLights.Lights[i]->Light, i);

		data->CachedLightPosition = position;
		data->CachedLightLayers = objLayer;
		data->CachedLightVersion = m_lightCacheVersion;
		data->LightCacheValid = true;
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

				bool inserted = false;
				int n = m_sorts.size();
				for (int j = 0; j < n; j++)
				{
					if (entry.Distance < m_sorts[j].Distance)
					{
						m_sorts.insert(entry, j);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					m_sorts.push_back(entry);
			}
		}
	}

	void CLightSystem::addLightSignature(SCacheLight& cache, CEntity* entity, CLightCullingData* lightData)
	{
		CLight* light = lightData->Light;
		size_t value = (size_t)lightData;
		value ^= (size_t)entity << 1;
		value ^= (size_t)light << 2;
		value ^= (size_t)light->getLightLayers() << 1;
		value ^= (size_t)light->getLightPriority() << 8;

		cache.Signature ^= value;
		cache.Signature *= 16777619u;
	}

	void CLightSystem::updateLightCacheVersion()
	{
		int dirLightCount = (int)m_dirLights.size();
		int pointLightCount = (int)m_pointLights.size();
		int spotLightCount = (int)m_spotLights.size();
		int areaLightCount = (int)m_areaLights.size();

		bool cacheChanged = m_dirLightCache.update(dirLightCount);
		cacheChanged |= m_pointLightCache.update(pointLightCount);
		cacheChanged |= m_spotLightCache.update(spotLightCount);
		cacheChanged |= m_areaLightCache.update(areaLightCount);

		if (cacheChanged)
		{
			++m_lightCacheVersion;
			if (m_lightCacheVersion == 0)
				m_lightCacheVersion = 1;
		}
	}

	void CLightSystem::invalidateLightCacheVersion()
	{
		++m_lightCacheVersion;
		if (m_lightCacheVersion == 0)
			m_lightCacheVersion = 1;

		m_dirLightCache.invalidate();
		m_pointLightCache.invalidate();
		m_spotLightCache.invalidate();
		m_areaLightCache.invalidate();
	}

	bool CLightSystem::needSortCachedLights(
		CRenderLightData* data,
		bool transformChanged,
		u32 objLayer,
		CLightCullingData** lights,
		CWorldTransformData** transforms,
		int lightCount,
		CRenderLightData::SCacheLight& cache,
		size_t lightSignature)
	{
		if (!data->LightCacheValid || data->CachedLightLayers != objLayer || transformChanged)
			return true;

		if (cache.VisibleCount != lightCount || cache.Signature != lightSignature)
			return true;

		float maxDistance = cache.Count > 0 ? cache.Distances[cache.Count - 1] : 0.0f;

		for (int i = 0; i < lightCount; i++)
		{
			CWorldTransformData* lightTransform = transforms[i];
			if (lightTransform == NULL || !lightTransform->NeedValidate)
				continue;

			CLight* light = lights[i]->Light;
			if ((objLayer & light->getLightLayers()) == 0)
				continue;

			bool cached = false;
			for (int j = 0; j < cache.Count; j++)
			{
				if (cache.Lights[j] == lights[i])
				{
					cached = true;
					break;
				}
			}

			if (cached)
				return true;

			float distance = lights[i]->LightPosition.getDistanceFromSQ(data->CachedLightPosition);
			if (cache.Count < 4 || distance < maxDistance)
				return true;
		}

		return false;
	}

	void CLightSystem::cacheSortedLights(
		const core::vector3df& position,
		u32 objLayer,
		CLightCullingData** lights,
		int lightCount,
		CRenderLightData::SCacheLight& cache,
		size_t lightSignature)
	{
		sortLights(position, objLayer, lights, lightCount);

		cache.Count = core::min_((int)m_sorts.size(), 4);
		for (int i = 0; i < 4; i++)
		{
			if (i < cache.Count)
			{
				cache.Lights[i] = m_sorts[i].Data;
				cache.Distances[i] = m_sorts[i].Distance;
			}
			else
			{
				cache.Lights[i] = NULL;
				cache.Distances[i] = 0.0f;
			}
		}

		cache.VisibleCount = lightCount;
		cache.Signature = lightSignature;
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
