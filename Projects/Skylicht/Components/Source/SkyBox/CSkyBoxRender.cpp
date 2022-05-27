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
	CSkyBoxRender::CSkyBoxRender()
	{
		m_renderPass = IRenderSystem::Sky;
	}

	CSkyBoxRender::~CSkyBoxRender()
	{

	}

	void CSkyBoxRender::beginQuery(CEntityManager* entityManager)
	{
		m_skyboxs.set_used(0);
		m_transforms.set_used(0);
		m_worlds.set_used(0);
	}

	void CSkyBoxRender::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CSkyBoxData* skyDomeData = (CSkyBoxData*)entity->getDataByIndex(CSkyBoxData::DataTypeIndex);

		if (skyDomeData != NULL)
		{
			CVisibleData* visible = (CVisibleData*)entity->getDataByIndex(CVisibleData::DataTypeIndex);
			CWorldTransformData* transformData = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);

			if (visible->Visible)
			{
				m_skyboxs.push_back(skyDomeData);
				m_transforms.push_back(transformData);
				m_worlds.push_back(core::IdentityMatrix);
			}
		}
	}

	void CSkyBoxRender::init(CEntityManager* entityManager)
	{

	}

	void CSkyBoxRender::update(CEntityManager* entityManager)
	{
		CCamera* camera = entityManager->getCamera();
		if (camera == NULL)
			return;

		core::vector3df cameraPosition = camera->getGameObject()->getPosition();
		float cameraFar = camera->getFarValue();

		core::matrix4* worlds = m_worlds.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		for (u32 i = 0, n = m_worlds.size(); i < n; i++)
		{
			worlds[i].setTranslation(cameraPosition);
			worlds[i].setScale(cameraFar * 0.5f);
		}
	}

	void CSkyBoxRender::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		CSkyBoxData** skyboxs = m_skyboxs.pointer();
		core::matrix4* worlds = m_worlds.pointer();

		for (u32 i = 0, n = m_skyboxs.size(); i < n; i++)
		{
			driver->setTransform(video::ETS_WORLD, worlds[i]);

			CSkyBoxData* skybox = skyboxs[i];
			if (skybox->MeshBuffer[0] == NULL)
				continue;

			for (int j = 0; j < 6; j++)
			{
				CShaderMaterial::setMaterial(skybox->Material[j]);

				driver->setMaterial(skybox->MeshBuffer[j]->getMaterial());
				driver->drawMeshBuffer(skybox->MeshBuffer[j]);
			}
		}
	}
}