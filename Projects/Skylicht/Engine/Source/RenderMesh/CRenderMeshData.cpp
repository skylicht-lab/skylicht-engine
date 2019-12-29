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
#include "CRenderMeshData.h"

namespace Skylicht
{
	CRenderMeshData::CRenderMeshData() :
		RenderMesh(NULL),
		SkinMesh(NULL),
		SoftwareSkinning(false)
	{

	}

	CRenderMeshData::~CRenderMeshData()
	{
		if (RenderMesh != NULL)
			RenderMesh->drop();

		if (SkinMesh != NULL)
			SkinMesh->drop();
	}

	void CRenderMeshData::setMesh(CMesh *mesh)
	{
		if (RenderMesh)
		{
			RenderMesh->drop();
			RenderMesh = NULL;
		}

		RenderMesh = mesh->clone();
	}

	void CRenderMeshData::initSoftwareSkinning()
	{
		CSkinnedMesh *mesh = new CSkinnedMesh();

		for (int i = 0, n = RenderMesh->getMeshBufferCount(); i < n; i++)
		{
			// skinned mesh buffer
			IMeshBuffer* skinnedMeshBuffer = RenderMesh->getMeshBuffer(i);

			// alloc new mesh buffer
			CMeshBuffer<video::S3DVertex>* meshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD), video::EIT_16BIT);

			// get new index & new vertex buffer
			CVertexBuffer<video::S3DVertex>* vertexBuffer = dynamic_cast<CVertexBuffer<video::S3DVertex>*>(meshBuffer->getVertexBuffer(0));
			CIndexBuffer* indexBuffer = dynamic_cast<CIndexBuffer*>(meshBuffer->getIndexBuffer());
			
			if (skinnedMeshBuffer->getVertexDescriptor()->getID() == video::EVT_SKIN_TANGENTS)
			{
				// SKIN TANGENT
				CVertexBuffer<video::S3DVertexSkinTangents>* skinTangentVertexBuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)skinnedMeshBuffer->getVertexBuffer(0);
				int numVertex = skinTangentVertexBuffer->getVertexCount();

				vertexBuffer->set_used(numVertex);
				for (int i = 0; i < numVertex; i++)
				{
					video::S3DVertex& dest = vertexBuffer->getVertex(i);
					video::S3DVertexSkinTangents& src = skinTangentVertexBuffer->getVertex(i);

					dest.Pos = src.Pos;
					dest.Normal = src.Normal;
					dest.Color = src.Color;
					dest.TCoords = src.TCoords;
				}
			}
			else
			{
				// SKIN
				CVertexBuffer<video::S3DVertexSkin>* skinVertexBuffer = (CVertexBuffer<video::S3DVertexSkin>*)skinnedMeshBuffer->getVertexBuffer(0);
				int numVertex = skinVertexBuffer->getVertexCount();

				vertexBuffer->set_used(numVertex);
				for (int i = 0; i < numVertex; i++)
				{
					video::S3DVertex& dest = vertexBuffer->getVertex(i);
					video::S3DVertexSkin& src = skinVertexBuffer->getVertex(i);

					dest.Pos = src.Pos;
					dest.Normal = src.Normal;
					dest.Color = src.Color;
					dest.TCoords = src.TCoords;
				}
			}

			// copy indices
			int numIndex = skinnedMeshBuffer->getIndexBuffer()->getIndexCount();
			indexBuffer->set_used(numIndex);
			for (int i = 0; i < numIndex; i++)
			{
				indexBuffer->setIndex(i, skinnedMeshBuffer->getIndexBuffer()->getIndex(i));
			}

			// copy material
			meshBuffer->getMaterial() = skinnedMeshBuffer->getMaterial();
			meshBuffer->setHardwareMappingHint(EHM_STREAM);

			// add to mesh
			mesh->addMeshBuffer(meshBuffer);
			meshBuffer->drop();
		}

		// swap default render mesh to dynamic stream mesh
		// see CSoftwareSkinningSystem todo next
		SkinMesh = RenderMesh;
		RenderMesh = mesh;
	}
}