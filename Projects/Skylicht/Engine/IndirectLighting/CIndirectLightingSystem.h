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

#pragma once

#include "Entity/IEntityData.h"
#include "Entity/IRenderSystem.h"
#include "Entity/CEntityGroup.h"

#include "Transform/CWorldTransformData.h"
#include "IndirectLighting/CIndirectLightingData.h"
#include "LightProbes/CLightProbeData.h"
#include "Culling/CVisibleData.h"

#include "kdtree.h"

namespace Skylicht
{
	class SKYLICHT_API CIndirectLightingSystem : public IEntitySystem
	{
	protected:
		core::array<CIndirectLightingData*> m_entities;
		core::array<CWorldTransformData*> m_entitiesPositions;
		core::array<CLightProbeData*> m_probes;
		core::array<CWorldTransformData*> m_probePositions;

		kdtree* m_kdtree;

		bool m_probeChange;

		CEntityGroup* m_groupLighting;
		CEntityGroup* m_groupProbes;
	public:
		CIndirectLightingSystem();

		virtual ~CIndirectLightingSystem();

		virtual void beginQuery(CEntityManager* entityManager);

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);

		virtual void init(CEntityManager* entityManager);

		virtual void update(CEntityManager* entityManager);
	};
}