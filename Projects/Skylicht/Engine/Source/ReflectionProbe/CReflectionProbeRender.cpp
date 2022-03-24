/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CReflectionProbeRender.h"

#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	bool CReflectionProbeRender::s_showProbe = false;

	CReflectionProbeRender::CReflectionProbeRender()
	{
		const IGeometryCreator* geometryCreator = getIrrlichtDevice()->getSceneManager()->getGeometryCreator();
		ProbeMesh = geometryCreator->createSphereMesh(0.3f);
		ProbeMesh->setHardwareMappingHint(EHM_STATIC);

		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/ReflectionProbe.xml");

		shaderVertexColor = shaderMgr->getShaderIDByName("VertexColor");
		shaderReflectionProbe = shaderMgr->getShaderIDByName("ReflectionProbe");

		m_material.MaterialType = shaderReflectionProbe;
	}

	CReflectionProbeRender::~CReflectionProbeRender()
	{
		ProbeMesh->drop();
		ProbeMesh = NULL;
	}

	void CReflectionProbeRender::beginQuery(CEntityManager* entityManager)
	{
		m_probes.set_used(0);
		m_transforms.set_used(0);
	}

	void CReflectionProbeRender::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		if (s_showProbe == false)
			return;

		CReflectionProbeData* probeData = entity->getData<CReflectionProbeData>();
		if (probeData != NULL)
		{
			CWorldTransformData* transformData = entity->getData<CWorldTransformData>();
			if (transformData != NULL)
			{
				m_probes.push_back(probeData);
				m_transforms.push_back(transformData);
			}
		}
	}

	void CReflectionProbeRender::init(CEntityManager* entityManager)
	{

	}

	void CReflectionProbeRender::update(CEntityManager* entityManager)
	{

	}

	void CReflectionProbeRender::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		CReflectionProbeData** probes = m_probes.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		for (u32 i = 0, n = m_probes.size(); i < n; i++)
		{
			driver->setTransform(video::ETS_WORLD, transforms[i]->World);

			if (probes[i]->ReflectionTexture != NULL)
			{
				m_material.setTexture(0, probes[i]->ReflectionTexture);
				m_material.MaterialType = shaderReflectionProbe;
			}
			else
			{
				m_material.MaterialType = shaderVertexColor;
			}

			for (u32 j = 0; j < ProbeMesh->getMeshBufferCount(); j++)
			{
				IMeshBuffer* buffer = ProbeMesh->getMeshBuffer(j);
				driver->setMaterial(m_material);
				driver->drawMeshBuffer(buffer);
			}
		}
	}
}