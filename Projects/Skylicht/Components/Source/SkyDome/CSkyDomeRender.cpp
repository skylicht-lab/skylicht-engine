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
	CSkyDomeRender::CSkyDomeRender()
	{
		m_renderPass = IRenderSystem::Sky;
	}

	CSkyDomeRender::~CSkyDomeRender()
	{

	}

	void CSkyDomeRender::beginQuery(CEntityManager* entityManager)
	{
		m_skydomes.set_used(0);
		m_transforms.set_used(0);
		m_worlds.set_used(0);
	}

	void CSkyDomeRender::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CSkyDomeData* skyDomeData = entity->getData<CSkyDomeData>();

		if (skyDomeData != NULL)
		{
			CVisibleData* visible = entity->getData<CVisibleData>();
			CWorldTransformData* transformData = entity->getData<CWorldTransformData>();

			if (transformData != NULL && visible->Visible)
			{
				m_skydomes.push_back(skyDomeData);
				m_transforms.push_back(transformData);
				m_worlds.push_back(core::IdentityMatrix);
			}
		}
	}

	void CSkyDomeRender::init(CEntityManager* entityManager)
	{

	}

	void CSkyDomeRender::update(CEntityManager* entityManager)
	{
		CCamera* camera = entityManager->getCamera();

		core::vector3df cameraPosition = camera->getGameObject()->getPosition();
		float cameraFar = camera->getFarValue();

		core::matrix4* worlds = m_worlds.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		for (u32 i = 0, n = m_worlds.size(); i < n; i++)
		{
			worlds[i].setTranslation(cameraPosition);
			worlds[i].setScale(cameraFar * 0.9f);
		}
	}

	void CSkyDomeRender::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		CSkyDomeData** skydomes = m_skydomes.pointer();
		core::matrix4* worlds = m_worlds.pointer();

		for (u32 i = 0, n = m_skydomes.size(); i < n; i++)
		{
			IMeshBuffer* buffer = skydomes[i]->Buffer;

			CShaderMaterial::setMaterial(skydomes[i]->SkyDomeMaterial);

			driver->setTransform(video::ETS_WORLD, worlds[i]);
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);
		}
	}
}