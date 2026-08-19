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

#pragma once

#include "CLightCullingData.h"
#include "Entity/CEntityManager.h"
#include "Entity/IRenderSystem.h"
#include "Entity/CEntityGroup.h"
#include "Transform/CWorldTransformData.h"
#include "Lighting/CRenderLightData.h"
#include "IndirectLighting/CIndirectLightingData.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"
#include "Lighting/CSpotLight.h"
#include "Lighting/CAreaLight.h"
#include "Utils/CKDTree3f.h"

namespace Skylicht
{
	class SKYLICHT_API CLightSystem : public IRenderSystem, public IEntityManagerCallback
	{
	protected:
		struct SDistanceLightEntry
		{
			CLightCullingData* Data;
			CLight* Light;
			float Distance;
		};

		struct SCacheLight
		{
			size_t Signature;
			size_t LastSignature;
			int LastCount;

			SCacheLight();

			void resetSignature();

			void invalidate();

			bool update(int count);
		};

		core::array<CLightCullingData*> m_pointLights;
		core::array<CLightCullingData*> m_spotLights;
		core::array<CLightCullingData*> m_areaLights;
		core::array<CLightCullingData*> m_dirLights;
		core::array<CLightCullingData*> m_changedPointLights;
		core::array<CLightCullingData*> m_changedSpotLights;
		core::array<CLightCullingData*> m_changedAreaLights;

		SCacheLight m_dirLightCache;
		SCacheLight m_pointLightCache;
		SCacheLight m_spotLightCache;
		SCacheLight m_areaLightCache;
		size_t m_lightCacheVersion;

		CEntityGroup* m_group;
		CEntityManager* m_entityManager;

		CDirectionalLight* m_currentDLight;
		CPointLight* m_currentPLight[4];
		CSpotLight* m_currentSLight[4];
		CAreaLight* m_currentALight[4];

		core::array<SDistanceLightEntry> m_sorts;
		core::array<CKDTree3f::SKDNode*> m_kdNodes;

		CKDTree3f* m_pointLightKDTree;
		CKDTree3f* m_spotLightKDTree;
		CKDTree3f* m_areaLightKDTree;

		int m_pointLightKDTreeCount;
		int m_spotLightKDTreeCount;
		int m_areaLightKDTreeCount;

		size_t m_pointLightKDTreeSignature;
		size_t m_spotLightKDTreeSignature;
		size_t m_areaLightKDTreeSignature;

		IHardwareBuffer* m_uboPLight;
		IHardwareBuffer* m_uboSLight;

		float m_maxRange;
	public:
		CLightSystem();

		virtual ~CLightSystem();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);

		virtual void render(CEntityManager* entityManager);

		virtual void postRender(CEntityManager* entityManager);

		virtual void onEntityRemoved(CEntity* entity);

		virtual void onEntityRemoved(CEntity** entity, int count);

		void onBeginSetupLight(CRenderLightData* data, CIndirectLightingData* indirectData, CWorldTransformData* transform);

		void onEndSetupLight();

		void onSetupLightIndex(CRenderLightData* data, CIndirectLightingData* indirectData, CWorldTransformData* transform);

		void setUBOPLight(IHardwareBuffer* buffer);

		void setUBOSLight(IHardwareBuffer* buffer);

	protected:

		void addLightToList(core::array<CLightCullingData*>& list, CLightCullingData* light);

		void updateUBOLight(core::array<CLightCullingData*>& light, IHardwareBuffer* buffer);

		void sortLights(const core::vector3df& position, u32 objLayer, CKDTree3f* kdtree);

		void rebuildLightKDTree(CKDTree3f* kdtree, core::array<CLightCullingData*>& lights);

		void addLightSignature(SCacheLight& cache, CEntity* entity, CLightCullingData* light);

		void addLightPositionSignature(size_t& signature, CLightCullingData* lightData);

		void updateLightCacheVersion();

		void invalidateLightCacheVersion();

		bool needSortCachedLights(
			CRenderLightData* data,
			bool transformChanged,
			u32 objLayer,
			CLightCullingData** changedLights,
			int changedLightCount,
			CRenderLightData::SCacheLight& cache,
			size_t lightSignature);

		void cacheSortedLights(
			const core::vector3df& position,
			u32 objLayer,
			CKDTree3f* kdtree,
			CRenderLightData::SCacheLight& cache,
			size_t lightSignature);

	};
}
