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

// #define VERTEX_NORMALIZE

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
			IMeshBuffer* meshBuffer = new CMeshBuffer<video::S3DVertex>(
				getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD),
				originalMeshBuffer->getIndexBuffer()->getType());

			// get new index & new vertex buffer
			CVertexBuffer<video::S3DVertex>* vertexBuffer = dynamic_cast<CVertexBuffer<video::S3DVertex>*>(meshBuffer->getVertexBuffer(0));
			IIndexBuffer* indexBuffer = meshBuffer->getIndexBuffer();

			if (originalMeshBuffer->getVertexType() == video::EVT_SKIN_TANGENTS)
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
			IIndexBuffer* originalIndexBuffer = originalMeshBuffer->getIndexBuffer();

			int numIndex = originalIndexBuffer->getIndexCount();
			indexBuffer->set_used(numIndex);
			for (int i = 0; i < numIndex; i++)
				indexBuffer->setIndex(i, originalIndexBuffer->getIndex(i));

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

	CMesh* CSoftwareSkinningUtils::initSoftwareBlendShape(CMesh* originalMesh)
	{
		CMesh* mesh = originalMesh->clone();
		IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();

		for (int i = 0, n = originalMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* originalMeshBuffer = originalMesh->getMeshBuffer(i);

			video::E_VERTEX_TYPE vertexType = originalMeshBuffer->getVertexType();
			video::E_INDEX_TYPE indexType = originalMeshBuffer->getIndexBuffer()->getType();
			video::IVertexDescriptor* vtxDes = getVideoDriver()->getVertexDescriptor(vertexType);

			IMeshBuffer* meshBuffer = NULL;

			if (vertexType == EVT_TANGENTS)
				meshBuffer = new CMeshBuffer<video::S3DVertexTangents>(vtxDes, indexType);
			else if (vertexType == EVT_SKIN_TANGENTS)
				meshBuffer = new CMeshBuffer<video::S3DVertexSkinTangents>(vtxDes, indexType);

			// copy mesh data
			mh->copyVertices(originalMeshBuffer->getVertexBuffer(), 0, vtxDes, meshBuffer->getVertexBuffer(), 0, vtxDes, true);
			mh->copyIndices(originalMeshBuffer->getIndexBuffer(), meshBuffer->getIndexBuffer());

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
			mesh->replaceMeshBuffer(i, meshBuffer);
			meshBuffer->drop();
		}

		return mesh;
	}

	void CSoftwareSkinningUtils::resetVertexID(CMesh* mesh)
	{
		for (int i = 0, n = mesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* meshBuffer = mesh->getMeshBuffer(i);

			if (meshBuffer->getVertexType() == video::EVT_TANGENTS)
			{
				CVertexBuffer<video::S3DVertexTangents>* vertexBuffer = dynamic_cast<CVertexBuffer<video::S3DVertexTangents>*>(meshBuffer->getVertexBuffer());
				u32 vtxCount = vertexBuffer->getVertexCount();
				for (u32 i = 0; i < vtxCount; i++)
				{
					vertexBuffer->getVertex(i).VertexData.Y = (float)i;
				}
			}
			else if (meshBuffer->getVertexType() == video::EVT_SKIN_TANGENTS)
			{
				CVertexBuffer<video::S3DVertexSkinTangents>* vertexBuffer = dynamic_cast<CVertexBuffer<video::S3DVertexSkinTangents>*>(meshBuffer->getVertexBuffer());
				u32 vtxCount = vertexBuffer->getVertexCount();
				for (u32 i = 0; i < vtxCount; i++)
				{
					vertexBuffer->getVertex(i).VertexData.Y = (float)i;
				}
			}

			meshBuffer->setDirty(EBT_VERTEX);
		}
	}

	void CSoftwareSkinningUtils::softwareSkinning(CMesh* skinnedMesh, CSkinnedMesh* originalMesh, CSkinnedMesh* blendShapeMesh)
	{
		CSkinnedMesh::SJoint* arrayJoint = originalMesh->Joints.pointer();

		CSkinnedMesh* sourceMesh = blendShapeMesh ? blendShapeMesh : originalMesh;

		for (u32 i = 0, n = sourceMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* originalMeshBuffer = sourceMesh->getMeshBuffer(i);
			IVertexBuffer* originalVertexbuffer = originalMeshBuffer->getVertexBuffer(0);
			video::S3DVertexSkin* vertex = (video::S3DVertexSkin*)originalVertexbuffer->getVertices();

			int numVertex = originalVertexbuffer->getVertexCount();

			IMeshBuffer* resultMeshBuffer = skinnedMesh->getMeshBuffer(i);
			IVertexBuffer* resultVertexBuffer = resultMeshBuffer->getVertexBuffer(0);
			video::S3DVertex* resultVertex = (video::S3DVertex*)resultVertexBuffer->getVertices();

#ifdef VERTEX_NORMALIZE
			float length, invLength;
#endif
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
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.X].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.X);
				}

				// bone 1
				if (vertex->BoneWeight.Y > 0.0f)
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.Y].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.Y);
				}

				// bone 2
				if (vertex->BoneWeight.Z > 0.0f)
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.Z].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.Z);
				}

				// bone 3
				if (vertex->BoneWeight.W > 0.0f)
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.W].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.W);
				}

				// apply skin normal
#ifdef VERTEX_NORMALIZE
				length = resultVertex->Normal.X * resultVertex->Normal.X +
					resultVertex->Normal.Y * resultVertex->Normal.Y +
					resultVertex->Normal.Z * resultVertex->Normal.Z;

				invLength = 1.0f / sqrtf(length);
				resultVertex->Normal.X = resultVertex->Normal.X * invLength;
				resultVertex->Normal.Y = resultVertex->Normal.Y * invLength;
				resultVertex->Normal.Z = resultVertex->Normal.Z * invLength;
#endif

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
			IVertexBuffer* originalVertexbuffer = originalMeshBuffer->getVertexBuffer(0);
			video::S3DVertexSkinTangents* vertex = (video::S3DVertexSkinTangents*)originalVertexbuffer->getVertices();

			int numVertex = originalVertexbuffer->getVertexCount();

			IMeshBuffer* resultMeshBuffer = skinnedMesh->getMeshBuffer(i);
			IVertexBuffer* resultVertexBuffer = resultMeshBuffer->getVertexBuffer(0);
			video::S3DVertex* resultVertex = (video::S3DVertex*)resultVertexBuffer->getVertices();

#ifdef VERTEX_NORMALIZE
			float length, invLength;
#endif

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
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.X].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.X);
				}

				// bone 1
				if (vertex->BoneWeight.Y > 0.0f)
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.Y].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.Y);
				}

				// bone 2
				if (vertex->BoneWeight.Z > 0.0f)
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.Z].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.Z);
				}

				// bone 3
				if (vertex->BoneWeight.W > 0.0f)
				{
					skinVertex(arrayJoint[(int)vertex->BoneIndex.W].SkinningMatrix,
						resultVertex->Pos,
						resultVertex->Normal,
						vertex->Pos,
						vertex->Normal,
						vertex->BoneWeight.W);
				}

				// apply skin normal
#ifdef VERTEX_NORMALIZE
				length = resultVertex->Normal.X * resultVertex->Normal.X +
					resultVertex->Normal.Y * resultVertex->Normal.Y +
					resultVertex->Normal.Z * resultVertex->Normal.Z;

				invLength = 1.0f / sqrtf(length);
				resultVertex->Normal.X = resultVertex->Normal.X * invLength;
				resultVertex->Normal.Y = resultVertex->Normal.Y * invLength;
				resultVertex->Normal.Z = resultVertex->Normal.Z * invLength;
#endif

				++resultVertex;
				++vertex;
			}
		}

		skinnedMesh->setDirty(EBT_VERTEX);
	}

	float px, py, pz, nx, ny, nz;

	void CSoftwareSkinningUtils::skinVertex(const float* m,
		core::vector3df& vertex,
		core::vector3df& normal,
		const core::vector3df& srcPos,
		const core::vector3df& srcNormal,
		const float& weight)
	{
		px = srcPos.X * m[0] + srcPos.Y * m[4] + srcPos.Z * m[8] + m[12];
		py = srcPos.X * m[1] + srcPos.Y * m[5] + srcPos.Z * m[9] + m[13];
		pz = srcPos.X * m[2] + srcPos.Y * m[6] + srcPos.Z * m[10] + m[14];

		nx = srcNormal.X * m[0] + srcNormal.Y * m[4] + srcNormal.Z * m[8];
		ny = srcNormal.X * m[1] + srcNormal.Y * m[5] + srcNormal.Z * m[9];
		nz = srcNormal.X * m[2] + srcNormal.Y * m[6] + srcNormal.Z * m[10];

		px *= weight;
		py *= weight;
		pz *= weight;

		nx *= weight;
		ny *= weight;
		nz *= weight;

		vertex.X += px;
		vertex.Y += py;
		vertex.Z += pz;

		normal.X += nx;
		normal.Y += ny;
		normal.Z += nz;
	}

	void CSoftwareSkinningUtils::softwareBlendShape(CMesh* blendShape, CMesh* originalMesh)
	{
		CBlendShape** blendShapeData = originalMesh->BlendShape.pointer();
		u32 numBlendShape = originalMesh->BlendShape.size();

		for (u32 i = 0, n = originalMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* originalMeshBuffer = originalMesh->getMeshBuffer(i);
			IVertexBuffer* originalVertexbuffer = originalMeshBuffer->getVertexBuffer(0);

			int numVertex = originalVertexbuffer->getVertexCount();

			IMeshBuffer* resultMeshBuffer = blendShape->getMeshBuffer(i);
			IVertexBuffer* resultVertexBuffer = resultMeshBuffer->getVertexBuffer(0);

			video::S3DVertexSkinTangents* vertex = NULL;
			video::S3DVertexSkinTangents* resultVertex = NULL;

			core::vector3df* srcPos, * dstPos, * srcNormal, * dstNormal;

			const core::vector3df* positionOffsets = NULL;
			const core::vector3df* positionOffset = NULL;

			const core::vector3df* normalOffsets = NULL;
			const core::vector3df* normalOffset = NULL;

			const u32* vtxId = NULL;

			float weight = 0.0f;
			u32 ix, size;

#ifdef VERTEX_NORMALIZE
			float length, invLength;
#endif

			for (u32 j = 0; j < numBlendShape; j++)
			{
				CBlendShape* blendShape = blendShapeData[j];

				weight = blendShape->Weight;
				if (weight == 0.0f)
					continue;

				vertex = (video::S3DVertexSkinTangents*)originalVertexbuffer->getVertices();
				resultVertex = (video::S3DVertexSkinTangents*)resultVertexBuffer->getVertices();

				vtxId = blendShape->VtxId.const_pointer();
				size = blendShape->Offset.size() - 1;
				positionOffsets = blendShape->Offset.const_pointer();
				normalOffsets = blendShape->NormalOffset.const_pointer();

				for (int i = 0; i < numVertex; i++)
				{
					ix = vtxId[(int)(vertex->VertexData.Y)];
					if (ix != size)
					{
						positionOffset = &positionOffsets[ix];
						normalOffset = &normalOffsets[ix];

						srcPos = &vertex->Pos;
						dstPos = &resultVertex->Pos;
						srcNormal = &vertex->Normal;
						dstNormal = &resultVertex->Normal;

						dstPos->X = srcPos->X + weight * positionOffset->X;
						dstPos->Y = srcPos->Y + weight * positionOffset->Y;
						dstPos->Z = srcPos->Z + weight * positionOffset->Z;

						dstNormal->X = srcNormal->X + weight * normalOffset->X;
						dstNormal->Y = srcNormal->Y + weight * normalOffset->Y;
						dstNormal->Z = srcNormal->Z + weight * normalOffset->Z;

#ifdef VERTEX_NORMALIZE
						length = dstNormal->X * dstNormal->X +
							dstNormal->Y * dstNormal->Y +
							dstNormal->Z * dstNormal->Z;

						invLength = 1.0f / sqrtf(length);
						dstNormal->X = dstNormal->X * invLength;
						dstNormal->Y = dstNormal->Y * invLength;
						dstNormal->Z = dstNormal->Z * invLength;
#endif
					}
					++resultVertex;
					++vertex;
				}
			}
		}

		blendShape->setDirty(EBT_VERTEX);
	}
}