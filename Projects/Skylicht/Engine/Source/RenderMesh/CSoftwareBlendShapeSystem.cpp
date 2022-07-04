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
#include "CSoftwareBlendShapeSystem.h"
#include "Culling/CCullingData.h"

namespace Skylicht
{
	CSoftwareBlendShapeSystem::CSoftwareBlendShapeSystem()
	{

	}

	CSoftwareBlendShapeSystem::~CSoftwareBlendShapeSystem()
	{

	}

	void CSoftwareBlendShapeSystem::beginQuery(CEntityManager* entityManager)
	{
		m_renderers.set_used(0);
	}

	void CSoftwareBlendShapeSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CRenderMeshData* renderer = (CRenderMeshData*)entity->getDataByIndex(CRenderMeshData::DataTypeIndex);
		CCullingData* culling = (CCullingData*)entity->getDataByIndex(CCullingData::DataTypeIndex);

		if (renderer != NULL && renderer->isSoftwareBlendShape())
		{
			bool render = true;
			if (culling != NULL && culling->Visible == false)
				render = false;

			if (render == true)
				m_renderers.push_back(renderer);
		}
	}

	void CSoftwareBlendShapeSystem::init(CEntityManager* entityManager)
	{

	}

	void CSoftwareBlendShapeSystem::update(CEntityManager* entityManager)
	{
		CRenderMeshData** renderers = m_renderers.pointer();
		for (u32 i = 0, n = m_renderers.size(); i < n; i++)
		{
			CRenderMeshData* renderer = renderers[i];

			CSkinnedMesh* renderMesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());
			CSkinnedMesh* originalMesh = dynamic_cast<CSkinnedMesh*>(renderer->getOriginalMesh());


		}
	}
}