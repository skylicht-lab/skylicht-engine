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
#include "CSkyBoxRender.h"
#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CSkyBoxRender::CSkyBoxRender() :
		m_group(NULL)
	{
		m_renderPass = IRenderSystem::Sky;
	}

	CSkyBoxRender::~CSkyBoxRender()
	{

	}

	void CSkyBoxRender::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 skyGroup[] = GET_LIST_ENTITY_DATA(CSkyBoxData);
			m_group = entityManager->createGroupFromVisible(skyGroup, 1);
		}
	}

	void CSkyBoxRender::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CSkyBoxRender::init(CEntityManager* entityManager)
	{

	}

	void CSkyBoxRender::update(CEntityManager* entityManager)
	{

	}

	void CSkyBoxRender::render(CEntityManager* entityManager)
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
			CSkyBoxData* skybox = GET_ENTITY_DATA(entities[i], CSkyBoxData);

			if (skybox->MeshBuffer[0] == NULL)
				continue;

			world.makeIdentity();
			world.setTranslation(cameraPosition);
			world.setScale(cameraFar * 0.5f);

			driver->setTransform(video::ETS_WORLD, world);

			for (int j = 0; j < 6; j++)
			{
				CMaterial* material = skybox->Material[j];
				SMaterial& mat = skybox->MeshBuffer[j]->getMaterial();

				material->updateTexture(mat);

				CShaderMaterial::setMaterial(material);

				driver->setMaterial(mat);
				driver->drawMeshBuffer(skybox->MeshBuffer[j]);
			}
		}
	}
}