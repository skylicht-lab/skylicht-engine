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
#include "CSoftwareSkinningSystem.h"

namespace Skylicht
{
	CSoftwareSkinningSystem::CSoftwareSkinningSystem()
	{

	}

	CSoftwareSkinningSystem::~CSoftwareSkinningSystem()
	{

	}

	void CSoftwareSkinningSystem::beginQuery()
	{
		m_renderers.set_used(0);
	}

	void CSoftwareSkinningSystem::onQuery(CEntityManager *entityManager, CEntity *entity)
	{
		CRenderMeshData *renderer = entity->getData<CRenderMeshData>();
		if (renderer != NULL && renderer->isSoftwareSkinning())
		{
			m_renderers.push_back(renderer);
		}
	}

	void CSoftwareSkinningSystem::init(CEntityManager *entityManager)
	{

	}

	void CSoftwareSkinningSystem::update(CEntityManager *entityManager)
	{
		CRenderMeshData** renderers = m_renderers.pointer();
		for (u32 i = 0, n = m_renderers.size(); i < n; i++)
		{
			CRenderMeshData *renderer = renderers[i];

			CSkinnedMesh *renderMesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());
			CSkinnedMesh *skinMesh = dynamic_cast<CSkinnedMesh*>(renderer->getSkinMesh());

			if (skinMesh->getMeshBuffer(0)->getVertexDescriptor()->getID() == video::EVT_SKIN_TANGENTS)
				softwareSkinningTangent(renderMesh, skinMesh);
			else
				softwareSkinning(renderMesh, skinMesh);
		}
	}

	void CSoftwareSkinningSystem::softwareSkinning(CSkinnedMesh *renderMesh, CSkinnedMesh *originalMesh)
	{

	}

	void CSoftwareSkinningSystem::softwareSkinningTangent(CSkinnedMesh *renderMesh, CSkinnedMesh *originalMesh)
	{
		CSkinnedMesh::SJoint *arrayJoint = originalMesh->Joints.pointer();

		for (u32 i = 0, n = originalMesh->getMeshBufferCount(); i < n; i++)
		{
			// skinned mesh buffer
			IMeshBuffer* originalMeshBuffer = originalMesh->getMeshBuffer(i);			
			IMeshBuffer* skinnedMeshBuffer = renderMesh->getMeshBuffer(i);

			CVertexBuffer<video::S3DVertexSkinTangents>* skinVertexbuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)originalMeshBuffer->getVertexBuffer(0);
			int numVertex = skinVertexbuffer->getVertexCount();

			// current skin vertex
			video::S3DVertexSkinTangents *vertex = (video::S3DVertexSkinTangents*)skinVertexbuffer->getVertices();

			CVertexBuffer<video::S3DVertex>* vertexbuffer = (CVertexBuffer<video::S3DVertex>*)skinnedMeshBuffer->getVertexBuffer(0);
			video::S3DVertex *resultVertex = (video::S3DVertex*)vertexbuffer->getVertices();

			// skinning
			for (int i = 0; i < numVertex; i++)
			{
				resultVertex->Pos.X = 0.0f;
				resultVertex->Pos.Y = 0.0f;
				resultVertex->Pos.Z = 0.0f;

				resultVertex->Normal.X = 0.0f;
				resultVertex->Normal.Y = 0.0f;
				resultVertex->Normal.Z = 0.0f;

				// bone 0
				if (vertex->BoneWeight.X > 0.0f)
					skinVertex(arrayJoint, resultVertex->Pos, resultVertex->Normal, vertex, 0);

				// bone 1
				if (vertex->BoneWeight.Y > 0.0f)
					skinVertex(arrayJoint, resultVertex->Pos, resultVertex->Normal, vertex, 1);

				// bone 2
				if (vertex->BoneWeight.Z > 0.0f)
					skinVertex(arrayJoint, resultVertex->Pos, resultVertex->Normal, vertex, 2);

				// bone 3
				if (vertex->BoneWeight.W > 0.0f)
					skinVertex(arrayJoint, resultVertex->Pos, resultVertex->Normal, vertex, 3);

				// apply skin normal
				float length = resultVertex->Normal.X*resultVertex->Normal.X +
					resultVertex->Normal.Y*resultVertex->Normal.Y +
					resultVertex->Normal.Z*resultVertex->Normal.Z;

				float invLength = 1.0f / sqrtf(length);
				resultVertex->Normal.X = resultVertex->Normal.X*invLength;
				resultVertex->Normal.Y = resultVertex->Normal.Y*invLength;
				resultVertex->Normal.Z = resultVertex->Normal.Z*invLength;

				++resultVertex;
				++vertex;
			}

			renderMesh->setDirty(EBT_VERTEX);
		}
	}

	void CSoftwareSkinningSystem::skinVertex(CSkinnedMesh::SJoint *arrayJoint, core::vector3df &vertex, core::vector3df &normal, video::S3DVertexSkinTangents* src, int boneIndex)
	{
		float *boneID = (float*)(&src->BoneIndex);
		float *boneWeight = (float*)(&src->BoneWeight);

		CSkinnedMesh::SJoint *pJoint = &arrayJoint[(int)boneID[boneIndex]];

		static core::vector3df thisVertexMove, thisNormalMove;

		pJoint->SkinningMatrix.transformVect(thisVertexMove, src->Pos);
		pJoint->SkinningMatrix.rotateVect(thisNormalMove, src->Normal);

		float weight = boneWeight[boneIndex];

		thisVertexMove.X *= weight;
		thisVertexMove.Y *= weight;
		thisVertexMove.Z *= weight;

		thisNormalMove.X *= weight;
		thisNormalMove.Y *= weight;
		thisNormalMove.Z *= weight;

		vertex.X += thisVertexMove.X;
		vertex.Y += thisVertexMove.Y;
		vertex.Z += thisVertexMove.Z;

		normal.X += thisNormalMove.X;
		normal.Y += thisNormalMove.Y;
		normal.Z += thisNormalMove.Z;
	}
}