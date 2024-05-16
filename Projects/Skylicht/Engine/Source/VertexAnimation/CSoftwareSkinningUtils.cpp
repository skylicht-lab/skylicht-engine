/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CSoftwareSkinningUtils.h"

namespace Skylicht
{
	CMesh* CSoftwareSkinningUtils::initSoftwareSkinning(CMesh* originalMesh)
	{
		CMesh* mesh = new CMesh();

		for (int i = 0, n = originalMesh->getMeshBufferCount(); i < n; i++)
		{
			// skinned mesh buffer
			IMeshBuffer* originalMeshBuffer = originalMesh->getMeshBuffer(i);

			// alloc new mesh buffer
			CMeshBuffer<video::S3DVertex>* meshBuffer = new CMeshBuffer<video::S3DVertex>(
				getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD),
				originalMeshBuffer->getIndexBuffer()->getType());

			// get new index & new vertex buffer
			CVertexBuffer<video::S3DVertex>* vertexBuffer = dynamic_cast<CVertexBuffer<video::S3DVertex>*>(meshBuffer->getVertexBuffer(0));
			CIndexBuffer* indexBuffer = dynamic_cast<CIndexBuffer*>(meshBuffer->getIndexBuffer());

			if (originalMeshBuffer->getVertexDescriptor()->getID() == video::EVT_SKIN_TANGENTS)
			{
				// SKIN TANGENT
				CVertexBuffer<video::S3DVertexSkinTangents>* originalTangentVertexBuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)originalMeshBuffer->getVertexBuffer(0);
				int numVertex = originalTangentVertexBuffer->getVertexCount();

				vertexBuffer->set_used(numVertex);
				for (int i = 0; i < numVertex; i++)
				{
					video::S3DVertex& dest = vertexBuffer->getVertex(i);
					video::S3DVertexSkinTangents& src = originalTangentVertexBuffer->getVertex(i);

					dest.Pos = src.Pos;
					dest.Normal = src.Normal;
					dest.Color = src.Color;
					dest.TCoords = src.TCoords;
				}
			}
			else
			{
				// SKIN
				CVertexBuffer<video::S3DVertexSkin>* originalVertexBuffer = (CVertexBuffer<video::S3DVertexSkin>*)originalMeshBuffer->getVertexBuffer(0);
				int numVertex = originalVertexBuffer->getVertexCount();

				vertexBuffer->set_used(numVertex);
				for (int i = 0; i < numVertex; i++)
				{
					video::S3DVertex& dest = vertexBuffer->getVertex(i);
					video::S3DVertexSkin& src = originalVertexBuffer->getVertex(i);

					dest.Pos = src.Pos;
					dest.Normal = src.Normal;
					dest.Color = src.Color;
					dest.TCoords = src.TCoords;
				}
			}

			// copy indices
			int numIndex = originalMeshBuffer->getIndexBuffer()->getIndexCount();
			indexBuffer->set_used(numIndex);
			for (int i = 0; i < numIndex; i++)
			{
				indexBuffer->setIndex(i, originalMeshBuffer->getIndexBuffer()->getIndex(i));
			}

			// copy material
			meshBuffer->getMaterial() = originalMeshBuffer->getMaterial();

			// apply static material
			CShaderManager* shaderMgr = CShaderManager::getInstance();

			if (meshBuffer->getMaterial().getTexture(0) != NULL)
				meshBuffer->getMaterial().MaterialType = shaderMgr->getShaderIDByName("TextureColor");
			else
				meshBuffer->getMaterial().MaterialType = shaderMgr->getShaderIDByName("VertexColor");

			// copy bbox (that fixed unitScale for culling)
			meshBuffer->getBoundingBox() = originalMeshBuffer->getBoundingBox();

			// add to mesh
			mesh->addMeshBuffer(meshBuffer, originalMesh->MaterialName[i].c_str());
			meshBuffer->drop();
		}

		mesh->recalculateBoundingBox();
		return mesh;
	}

	void CSoftwareSkinningUtils::softwareSkinning(CMesh* skinnedMesh, CSkinnedMesh* originalMesh, CSkinnedMesh* blendShapeMesh)
	{
		CSkinnedMesh::SJoint* arrayJoint = originalMesh->Joints.pointer();

		CSkinnedMesh* sourceMesh = blendShapeMesh ? blendShapeMesh : originalMesh;

		for (u32 i = 0, n = sourceMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* originalMeshBuffer = sourceMesh->getMeshBuffer(i);
			CVertexBuffer<video::S3DVertexSkin>* originalVertexbuffer = (CVertexBuffer<video::S3DVertexSkin>*)originalMeshBuffer->getVertexBuffer(0);
			video::S3DVertexSkin* vertex = (video::S3DVertexSkin*)originalVertexbuffer->getVertices();

			int numVertex = originalVertexbuffer->getVertexCount();

			IMeshBuffer* skinnedMeshBuffer = skinnedMesh->getMeshBuffer(i);
			CVertexBuffer<video::S3DVertex>* vertexbuffer = (CVertexBuffer<video::S3DVertex>*)skinnedMeshBuffer->getVertexBuffer(0);
			video::S3DVertex* resultVertex = (video::S3DVertex*)vertexbuffer->getVertices();

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
				float length = resultVertex->Normal.X * resultVertex->Normal.X +
					resultVertex->Normal.Y * resultVertex->Normal.Y +
					resultVertex->Normal.Z * resultVertex->Normal.Z;

				float invLength = 1.0f / sqrtf(length);
				resultVertex->Normal.X = resultVertex->Normal.X * invLength;
				resultVertex->Normal.Y = resultVertex->Normal.Y * invLength;
				resultVertex->Normal.Z = resultVertex->Normal.Z * invLength;

				++resultVertex;
				++vertex;
			}

			skinnedMesh->setDirty(EBT_VERTEX);
		}
	}

	void CSoftwareSkinningUtils::softwareSkinningTangent(CMesh* skinnedMesh, CSkinnedMesh* originalMesh, CSkinnedMesh* blendShapeMesh)
	{
		CSkinnedMesh::SJoint* arrayJoint = originalMesh->Joints.pointer();

		CSkinnedMesh* sourceMesh = blendShapeMesh ? blendShapeMesh : originalMesh;

		for (u32 i = 0, n = sourceMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* originalMeshBuffer = sourceMesh->getMeshBuffer(i);
			CVertexBuffer<video::S3DVertexSkinTangents>* originalVertexbuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)originalMeshBuffer->getVertexBuffer(0);
			video::S3DVertexSkinTangents* vertex = (video::S3DVertexSkinTangents*)originalVertexbuffer->getVertices();

			int numVertex = originalVertexbuffer->getVertexCount();

			IMeshBuffer* skinnedMeshBuffer = skinnedMesh->getMeshBuffer(i);
			CVertexBuffer<video::S3DVertex>* vertexbuffer = (CVertexBuffer<video::S3DVertex>*)skinnedMeshBuffer->getVertexBuffer(0);
			video::S3DVertex* resultVertex = (video::S3DVertex*)vertexbuffer->getVertices();

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
				float length = resultVertex->Normal.X * resultVertex->Normal.X +
					resultVertex->Normal.Y * resultVertex->Normal.Y +
					resultVertex->Normal.Z * resultVertex->Normal.Z;

				float invLength = 1.0f / sqrtf(length);
				resultVertex->Normal.X = resultVertex->Normal.X * invLength;
				resultVertex->Normal.Y = resultVertex->Normal.Y * invLength;
				resultVertex->Normal.Z = resultVertex->Normal.Z * invLength;

				++resultVertex;
				++vertex;
			}
		}

		skinnedMesh->setDirty(EBT_VERTEX);
	}

	void CSoftwareSkinningUtils::skinVertex(CSkinnedMesh::SJoint* arrayJoint, core::vector3df& vertex, core::vector3df& normal, video::S3DVertexSkinTangents* src, int boneIndex)
	{
		float* boneID = (float*)(&src->BoneIndex);
		float* boneWeight = (float*)(&src->BoneWeight);

		CSkinnedMesh::SJoint* pJoint = &arrayJoint[(int)boneID[boneIndex]];

		static core::vector3df thisVertexMove, thisNormalMove;

		// static core::matrix4 skinningMat;
		// skinningMat.setM(pJoint->SkinningMatrix);
		// skinningMat.transformVect(thisVertexMove, src->Pos);
		// skinningMat.rotateVect(thisNormalMove, src->Normal);

		float* m = pJoint->SkinningMatrix;
		thisVertexMove.X = src->Pos.X * m[0] + src->Pos.Y * m[4] + src->Pos.Z * m[8] + m[12];
		thisVertexMove.Y = src->Pos.X * m[1] + src->Pos.Y * m[5] + src->Pos.Z * m[9] + m[13];
		thisVertexMove.Z = src->Pos.X * m[2] + src->Pos.Y * m[6] + src->Pos.Z * m[10] + m[14];

		thisNormalMove.X = src->Normal.X * m[0] + src->Normal.Y * m[4] + src->Normal.Z * m[8];
		thisNormalMove.Y = src->Normal.X * m[1] + src->Normal.Y * m[5] + src->Normal.Z * m[9];
		thisNormalMove.Z = src->Normal.X * m[2] + src->Normal.Y * m[6] + src->Normal.Z * m[10];

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

	void CSoftwareSkinningUtils::skinVertex(CSkinnedMesh::SJoint* arrayJoint, core::vector3df& vertex, core::vector3df& normal, video::S3DVertexSkin* src, int boneIndex)
	{
		float* boneID = (float*)(&src->BoneIndex);
		float* boneWeight = (float*)(&src->BoneWeight);

		CSkinnedMesh::SJoint* pJoint = &arrayJoint[(int)boneID[boneIndex]];

		static core::vector3df thisVertexMove, thisNormalMove;

		// static core::matrix4 skinningMat;
		// skinningMat.setM(pJoint->SkinningMatrix);
		// skinningMat.transformVect(thisVertexMove, src->Pos);
		// skinningMat.rotateVect(thisNormalMove, src->Normal);

		float* m = pJoint->SkinningMatrix;
		thisVertexMove.X = src->Pos.X * m[0] + src->Pos.Y * m[4] + src->Pos.Z * m[8] + m[12];
		thisVertexMove.Y = src->Pos.X * m[1] + src->Pos.Y * m[5] + src->Pos.Z * m[9] + m[13];
		thisVertexMove.Z = src->Pos.X * m[2] + src->Pos.Y * m[6] + src->Pos.Z * m[10] + m[14];

		thisNormalMove.X = src->Normal.X * m[0] + src->Normal.Y * m[4] + src->Normal.Z * m[8];
		thisNormalMove.Y = src->Normal.X * m[1] + src->Normal.Y * m[5] + src->Normal.Z * m[9];
		thisNormalMove.Z = src->Normal.X * m[2] + src->Normal.Y * m[6] + src->Normal.Z * m[10];

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

	void CSoftwareSkinningUtils::softwareBlendShape(CMesh* blendShape, CMesh* originalMesh)
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