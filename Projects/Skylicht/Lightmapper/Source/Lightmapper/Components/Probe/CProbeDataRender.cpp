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
#include "CProbe.h"

namespace Skylicht
{
	CProbeDataRender::CProbeDataRender()
	{

	}

	CProbeDataRender::~CProbeDataRender()
	{

	}

	void CProbeDataRender::beginQuery()
	{
		m_probes.set_used(0);
		m_transforms.set_used(0);
	}

	void CProbeDataRender::onQuery(CEntityManager *entityManager, CEntity *entity)
	{
		CProbeData *probeData = entity->getData<CProbeData>();
		if (probeData != NULL)
		{
			CWorldTransformData *transformData = entity->getData<CWorldTransformData>();
			if (transformData != NULL)
			{
				m_probes.push_back(probeData);
				m_transforms.push_back(transformData);
			}
		}
	}

	void CProbeDataRender::init(CEntityManager *entityManager)
	{

	}

	void CProbeDataRender::update(CEntityManager *entityManager)
	{

	}

	void CProbeDataRender::render(CEntityManager *entityManager)
	{
		IVideoDriver *driver = getVideoDriver();

		CProbeData** probes = m_probes.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		for (u32 i = 0, n = m_probes.size(); i < n; i++)
		{
			IMesh* mesh = probes[i]->ProbeMesh;
			driver->setTransform(video::ETS_WORLD, transforms[i]->World);

			for (u32 j = 0; j < mesh->getMeshBufferCount(); j++)
			{
				IMeshBuffer *buffer = mesh->getMeshBuffer(j);
				driver->setMaterial(buffer->getMaterial());
				driver->drawMeshBuffer(buffer);
			}
		}
	}
}