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
#include "CSkyDomeRender.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CSkyDomeRender::CSkyDomeRender() :
		m_group(NULL)
	{
		m_renderPass = IRenderSystem::Sky;
	}

	CSkyDomeRender::~CSkyDomeRender()
	{

	}

	void CSkyDomeRender::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 skyGroup[] = GET_LIST_ENTITY_DATA(CSkyDomeData);
			m_group = entityManager->createGroupFromVisible(skyGroup, 1);
		}
	}

	void CSkyDomeRender::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CSkyDomeRender::init(CEntityManager* entityManager)
	{

	}

	void CSkyDomeRender::update(CEntityManager* entityManager)
	{

	}

	void CSkyDomeRender::render(CEntityManager* entityManager)
	{
		CCamera* camera = entityManager->getCamera();
		if (camera == NULL)
			return;

		core::vector3df cameraPosition = camera->getGameObject()->getPosition();
		float cameraFar = camera->getFarValue();

		IVideoDriver* driver = getVideoDriver();

		CEntity** entities = m_group->getEntities();
		int numEntity = m_group->getEntityCount();

		core::matrix4 world;

		for (int i = 0; i < numEntity; i++)
		{
			CSkyDomeData* skydome = GET_ENTITY_DATA(entities[i], CSkyDomeData);

			CShaderMaterial::setMaterial(skydome->SkyDomeMaterial);

			world.makeIdentity();
			world.setTranslation(cameraPosition);
			world.setScale(cameraFar * 0.9f);

			driver->setTransform(video::ETS_WORLD, world);

			IMeshBuffer* buffer = skydome->Buffer;

			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);
		}
	}
}