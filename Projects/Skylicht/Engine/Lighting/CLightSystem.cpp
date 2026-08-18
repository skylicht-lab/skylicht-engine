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
		m_currentDLight(NULL),
		m_uboPLight(NULL),
		m_uboSLight(NULL)
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

		if (m_uboPLight)
			m_uboPLight->drop();

		if (m_uboSLight)
			m_uboSLight->drop();
	}

	void CLightSystem::beginQuery(CEntityManager* entityManager)
	{
		m_pointLights.set_used(0);
		m_spotLights.set_used(0);
		m_areaLights.set_used(0);
		m_dirLights.set_used(0);
		m_changedPointLights.set_used(0);
		m_changedSpotLights.set_used(0);
		m_changedAreaLights.set_used(0);

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
				lightData->UBOIndex = (int)m_dirLights.size();
				m_dirLights.push_back(lightData);
				addLightSignature(m_dirLightCache, entity, lightData);
				break;
			case CLight::PointLight:
				lightData->UBOIndex = (int)m_pointLights.size();
				m_pointLights.push_back(lightData);
				if (transformData != NULL && transformData->NeedValidate)
					m_changedPointLights.push_back(lightData);
				addLightSignature(m_pointLightCache, entity, lightData);
				break;
			case CLight::SpotLight:
				lightData->UBOIndex = (int)m_spotLights.size();
				m_spotLights.push_back(lightData);
				if (transformData != NULL && transformData->NeedValidate)
					m_changedSpotLights.push_back(lightData);
				addLightSignature(m_spotLightCache, entity, lightData);
				break;
			case CLight::AreaLight:
				lightData->UBOIndex = (int)m_areaLights.size();
				m_areaLights.push_back(lightData);
				if (transformData != NULL && transformData->NeedValidate)
					m_changedAreaLights.push_back(lightData);
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

	void CLightSystem::setUBOPLight(IHardwareBuffer* buffer)
	{
		if (m_uboPLight)
			m_uboPLight->drop();

		m_uboPLight = buffer;

		if (m_uboPLight)
		{
			m_uboPLight->grab();
			updateUBOLight(m_pointLights, m_uboPLight);
		}
	}

	void CLightSystem::setUBOSLight(IHardwareBuffer* buffer)
	{
		if (m_uboSLight)
			m_uboSLight->drop();

		m_uboSLight = buffer;

		if (m_uboSLight)
		{
			m_uboSLight->grab();
			updateUBOLight(m_spotLights, m_uboSLight);
		}
	}

	void CLightSystem::updateUBOLight(core::array<CLightCullingData*>& light, IHardwareBuffer* buffer)
	{
		SUBOLightBuffer lightBuffer;
		lightBuffer.NumLights = core::min_(MAX_UBO_POINT_LIGHTS, (int)m_pointLights.size());

		core::vector3df pos, dir;

		// see more in CShaderLighting::OnSetConstants
		for (int i = 0; i < lightBuffer.NumLights; i++)
		{
			SUBOPointLight& l = lightBuffer.Lights[i];
			CPointLight* light = (CPointLight*)m_pointLights[i]->Light;

			pos = light->getPosition();
			l.Position.X = pos.X;
			l.Position.Y = pos.Y;
			l.Position.Z = pos.Z;

			l.Color = light->getColor();
			l.Color.a = light->getIntensity();

			if (light->getLightTypeId() == (int)CLight::SpotLight)
			{
				dir = -light->getDirection();
				dir.normalize();
				l.Direction.X = dir.X;
				l.Direction.Y = dir.Y;
				l.Direction.Z = dir.Z;

				l.Attenuation.X = cosf(light->getSplotCutoff() * core::DEGTORAD * 0.5f);
				l.Attenuation.Y = cosf(light->getSpotInnerCutof() * core::DEGTORAD * 0.5f);
				l.Attenuation.Z = light->getAttenuation();
				l.Attenuation.W = light->getSpotExponent();
			}
			else
			{
				l.Attenuation.Y = light->getAttenuation();
			}
		}

		// flush to hardware
		buffer->update(&lightBuffer, sizeof(SUBOLightBuffer));
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

		onSetupLightIndex(data, transform);

		if (data->CachedDirectionalLights.Lights[0] == NULL)
			CShaderLighting::setDirectionalLight(NULL);
		else
			CShaderLighting::setDirectionalLight((CDirectionalLight*)data->CachedDirectionalLights.Lights[0]->Light);

		int lightCount = data->CachedPointLights.Count;
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setPointLight((CPointLight*)data->CachedPointLights.Lights[i]->Light, i);

		lightCount = data->CachedSpotLights.Count;
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setSpotLight((CSpotLight*)data->CachedSpotLights.Lights[i]->Light, i);

		lightCount = data->CachedAreaLights.Count;
		for (int i = 0; i < lightCount; i++)
			CShaderLighting::setAreaLight((CAreaLight*)data->CachedAreaLights.Lights[i]->Light, i);

		CShaderLighting::setLightIndex(data->LightIndex);
	}

	void CLightSystem::onSetupLightIndex(CRenderLightData* data, CWorldTransformData* transform)
	{
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
			data->CachedDirectionalLights.Signature = m_dirLightCache.Signature;
		}

		// point light
		lights = m_pointLights.pointer();
		lightCount = m_pointLights.size();

		if (needSortCachedLights(
			data,
			transformChanged,
			objLayer,
			m_changedPointLights.pointer(),
			m_changedPointLights.size(),
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

		// spotlight
		lights = m_spotLights.pointer();
		lightCount = m_spotLights.size();

		if (needSortCachedLights(
			data,
			transformChanged,
			objLayer,
			m_changedSpotLights.pointer(),
			m_changedSpotLights.size(),
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

		// area light
		lights = m_areaLights.pointer();
		lightCount = m_areaLights.size();

		if (needSortCachedLights(
			data,
			transformChanged,
			objLayer,
			m_changedAreaLights.pointer(),
			m_changedAreaLights.size(),
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

		data->CachedLightPosition = position;
		data->CachedLightLayers = objLayer;
		data->CachedLightVersion = m_lightCacheVersion;
		data->LightCacheValid = true;

		// set index
		if (data->CachedPointLights.Count >= 1)
			data->LightIndex.X = (float)(data->CachedPointLights.Lights[0]->UBOIndex);

		if (data->CachedPointLights.Count >= 2)
			data->LightIndex.Y = (float)(data->CachedPointLights.Lights[1]->UBOIndex);

		if (data->CachedSpotLights.Count >= 1)
			data->LightIndex.Z = (float)(data->CachedSpotLights.Lights[0]->UBOIndex);
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
		bool changed = false;

		int dirLightCount = (int)m_dirLights.size();
		int pointLightCount = (int)m_pointLights.size();
		int spotLightCount = (int)m_spotLights.size();
		int areaLightCount = (int)m_areaLights.size();

		// test direction light
		bool cacheChanged = m_dirLightCache.update(dirLightCount);

		// test point light
		changed = m_pointLightCache.update(pointLightCount);
		if (changed && m_uboPLight)
			updateUBOLight(m_pointLights, m_uboPLight);
		cacheChanged |= changed;

		// test spot light
		changed = m_spotLightCache.update(spotLightCount);
		if (changed && m_uboSLight)
			updateUBOLight(m_spotLights, m_uboSLight);
		cacheChanged |= changed;

		// test area light
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
		CLightCullingData** changedLights,
		int changedLightCount,
		CRenderLightData::SCacheLight& cache,
		size_t lightSignature)
	{
		if (!data->LightCacheValid || data->CachedLightLayers != objLayer || transformChanged)
			return true;

		if (cache.Signature != lightSignature)
			return true;

		float maxDistance = cache.Count > 0 ? cache.Distances[cache.Count - 1] : 0.0f;

		for (int i = 0; i < changedLightCount; i++)
		{
			CLightCullingData* lightData = changedLights[i];
			CLight* light = lightData->Light;
			if ((objLayer & light->getLightLayers()) == 0)
				continue;

			bool cached = false;
			for (int j = 0; j < cache.Count; j++)
			{
				if (cache.Lights[j] == lightData)
				{
					cached = true;
					break;
				}
			}

			if (cached)
				return true;

			float distance = lightData->LightPosition.getDistanceFromSQ(data->CachedLightPosition);
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
