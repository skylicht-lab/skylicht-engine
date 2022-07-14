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

	void CSoftwareBlendShapeSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CRenderMeshData* renderer = GET_ENTITY_DATA(entity, CRenderMeshData);
			CCullingData* culling = GET_ENTITY_DATA(entity, CCullingData);

			if (renderer != NULL && renderer->isSoftwareBlendShape())
			{
				bool render = true;
				if (culling != NULL && culling->Visible == false)
					render = false;

				if (render == true)
					m_renderers.push_back(renderer);
			}
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
			blendShape(renderer->getSoftwareBlendShapeMesh(), renderer->getMesh());
		}
	}

	void CSoftwareBlendShapeSystem::blendShape(CMesh* blendShape, CMesh* originalMesh)
	{
		CBlendShape** blendShapeData = originalMesh->BlendShape.pointer();
		u32 numBlendShape = originalMesh->BlendShape.size();

		for (u32 i = 0, n = originalMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* originalMeshBuffer = originalMesh->getMeshBuffer(i);
			CVertexBuffer<video::S3DVertexSkinTangents>* originalVertexbuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)originalMeshBuffer->getVertexBuffer(0);
			video::S3DVertexSkinTangents* vertex = (video::S3DVertexSkinTangents*)originalVertexbuffer->getVertices();

			int numVertex = originalVertexbuffer->getVertexCount();

			IMeshBuffer* skinnedMeshBuffer = blendShape->getMeshBuffer(i);
			CVertexBuffer<video::S3DVertexSkinTangents>* vertexbuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)skinnedMeshBuffer->getVertexBuffer(0);
			video::S3DVertexSkinTangents* resultVertex = (video::S3DVertexSkinTangents*)vertexbuffer->getVertices();

			// morphing
			for (int i = 0; i < numVertex; i++)
			{
				*resultVertex = *vertex;

				int vertexID = (int)vertex->VertexData.Y;
				for (u32 j = 0; j < numBlendShape; j++)
				{
					resultVertex->Pos += blendShapeData[j]->Weight * blendShapeData[j]->Offset[vertexID];
				}

				++resultVertex;
				++vertex;
			}
		}

		blendShape->setDirty(EBT_VERTEX);
	}
}