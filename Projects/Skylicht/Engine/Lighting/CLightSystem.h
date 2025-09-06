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
#include "Entity/IRenderSystem.h"
#include "Entity/CEntityGroup.h"
#include "Transform/CWorldTransformData.h"
#include "RenderMesh/CRenderMeshData.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"
#include "Lighting/CSpotLight.h"
#include "Lighting/CAreaLight.h"

namespace Skylicht
{
	class SKYLICHT_API CLightSystem : public IRenderSystem
	{
	protected:
		struct SDistanceLightEntry
		{
			CLightCullingData* Data;
			CLight* Light;
			float Distance;
		};

		core::array<CLightCullingData*> m_pointLights;
		core::array<CLightCullingData*> m_spotLights;
		core::array<CLightCullingData*> m_areaLights;
		core::array<CLightCullingData*> m_dirLights;

		CEntityGroup* m_group;

		CDirectionalLight* m_currentDLight;
		CPointLight* m_currentPLight[4];
		CSpotLight* m_currentSLight[4];
		CAreaLight* m_currentALight[4];

		core::array<SDistanceLightEntry> m_sorts;

	public:
		CLightSystem();

		virtual ~CLightSystem();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);

		virtual void render(CEntityManager* entityManager);

		virtual void postRender(CEntityManager* entityManager);

		void onBeginSetupLight(CRenderMeshData* data, CWorldTransformData* transform);

		void onEndSetupLight();

	protected:

		void sortLights(const core::vector3df& position, u32 objLayer, CLightCullingData** lights, int lightCount);

	};
}