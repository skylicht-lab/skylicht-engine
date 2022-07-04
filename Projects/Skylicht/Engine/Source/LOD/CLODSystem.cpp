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
	CLODSystem::CLODSystem()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CLODSystem::~CLODSystem()
	{

	}

	void CLODSystem::beginQuery(CEntityManager* entityManager)
	{
		m_lods.set_used(0);
		m_visibles.set_used(0);
		m_transforms.set_used(0);
	}

	void CLODSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CLODData* lod = (CLODData*)entity->getDataByIndex(CLODData::DataTypeIndex);
		if (lod == NULL)
			return;

		CVisibleData* visible = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
		if (!visible->Visible)
			return;

		CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);

		m_lods.push_back(lod);
		m_visibles.push_back(visible);
		m_transforms.push_back(transform);
	}

	void CLODSystem::init(CEntityManager* entityManager)
	{

	}

	void CLODSystem::update(CEntityManager* entityManager)
	{
		IRenderPipeline* rp = entityManager->getRenderPipeline();
		if (rp == NULL)
			return;

		// camera
		CCamera* camera = entityManager->getCamera();
		if (camera == NULL)
			return;

		CWorldTransformData** transforms = m_transforms.pointer();
		CLODData** lods = m_lods.pointer();
		CVisibleData** visibles = m_visibles.pointer();

		core::vector3df cameraPosition = camera->getGameObject()->getPosition();
		core::vector3df distance;

		u32 numEntity = m_lods.size();
		for (u32 i = 0; i < numEntity; i++)
		{
			const f32* m = transforms[i]->World.pointer();

			// distance vector
			float x = cameraPosition.X - m[12];
			float y = 0.0f; // cameraPosition.Y - m[13];
			float z = cameraPosition.Z - m[14];

			// length vector
			float d = x * x + y * y + z * z;

			// culling
			if (d < lods[i]->From || d >= lods[i]->To)
			{
				visibles[i]->Visible = false;
			}
		}
	}

	void CLODSystem::render(CEntityManager* entityManager)
	{

	}

	void CLODSystem::postRender(CEntityManager* entityManager)
	{

	}
}