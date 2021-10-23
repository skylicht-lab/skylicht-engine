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
#include "CGridPlaneRender.h"
#include "Culling/CVisibleData.h"

namespace Skylicht
{
	void CGridPlaneRender::beginQuery(CEntityManager* entityManager)
	{
		m_gridPlanes.set_used(0);
		m_transforms.set_used(0);
	}

	void CGridPlaneRender::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CGridPlaneData* gridPlane = entity->getData<CGridPlaneData>();

		if (gridPlane != NULL)
		{
			CVisibleData* visible = entity->getData<CVisibleData>();
			CWorldTransformData* transform = entity->getData<CWorldTransformData>();

			if (transform != NULL && visible->Visible)
			{
				m_gridPlanes.push_back(gridPlane);
				m_transforms.push_back(transform);
			}
		}
	}

	void CGridPlaneRender::init(CEntityManager* entityManager)
	{

	}

	void CGridPlaneRender::update(CEntityManager* entityManager)
	{

	}

	void CGridPlaneRender::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		CGridPlaneData** gridPlane = m_gridPlanes.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		for (u32 i = 0, n = m_gridPlanes.size(); i < n; i++)
		{
			IMeshBuffer* buffer = gridPlane[i]->LineBuffer;

			driver->setTransform(video::ETS_WORLD, transforms[i]->World);
			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);
		}
	}
}