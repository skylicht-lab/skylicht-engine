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
#include "Culling/CVisibleData.h"
#include "Entity/CEntityManager.h"
#include "VertexAnimation/CSoftwareSkinningUtils.h"

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
		CMeshSystem::beginQuery(entityManager);
	}

	void CSoftwareBlendShapeSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CSoftwareBlendShapeSystem::init(CEntityManager* entityManager)
	{

	}

	void CSoftwareBlendShapeSystem::update(CEntityManager* entityManager)
	{
		int numEntity = m_groupMesh->getNumBlendShape();
		CEntity** entities = m_groupMesh->getBlendShapeMeshes();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CCullingData* culling = GET_ENTITY_DATA(entity, CCullingData);
			if (culling != NULL && culling->Visible == false)
				continue;

			CRenderMeshData* renderer = GET_ENTITY_DATA(entity, CRenderMeshData);
			if (renderer != NULL && renderer->isSoftwareBlendShape())
			{
				CSoftwareSkinningUtils::softwareBlendShape(renderer->getSoftwareBlendShapeMesh(), renderer->getMesh());
			}
		}
	}
}