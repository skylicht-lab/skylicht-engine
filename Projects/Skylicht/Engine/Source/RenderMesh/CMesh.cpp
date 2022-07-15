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
#include "CMesh.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	CMesh::CMesh() :
		UseInstancing(false)
	{
	}

	CMesh::~CMesh()
	{
		removeAllMeshBuffer();
	}

	CMesh* CMesh::clone()
	{
		CMesh* newMesh = new CMesh();
		newMesh->BoundingBox = BoundingBox;

		for (u32 i = 0, n = MeshBuffers.size(); i < n; i++)
		{
			newMesh->addMeshBuffer(
				MeshBuffers[i],
				MaterialName[i].c_str(),
				Materials[i]
			);
		}

		for (u32 i = 0, n = BlendShape.size(); i < n; i++)
		{
			newMesh->addBlendShape(BlendShape[i]);
		}

		return newMesh;
	}

	u32 CMesh::getMeshBufferCount() const
	{
		return MeshBuffers.size();
	}

	IMeshBuffer* CMesh::getMeshBuffer(u32 nr) const
	{
		return MeshBuffers[nr];
	}

	IMeshBuffer* CMesh::getMeshBuffer(const video::SMaterial& material) const
	{
		for (s32 i = (s32)MeshBuffers.size() - 1; i >= 0; --i)
		{
			if (material == MeshBuffers[i]->getMaterial())
				return MeshBuffers[i];
		}

		return NULL;
	}

	const core::aabbox3d<f32>& CMesh::getBoundingBox() const
	{
		return BoundingBox;
	}

	void CMesh::setBoundingBox(const core::aabbox3df& box)
	{
		BoundingBox = box;
	}

	void CMesh::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
	{
		for (u32 i = 0; i < MeshBuffers.size(); ++i)
			MeshBuffers[i]->getMaterial().setFlag(flag, newvalue);
	}

	void CMesh::setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer)
	{
		for (u32 i = 0; i < MeshBuffers.size(); ++i)
			MeshBuffers[i]->setHardwareMappingHint(newMappingHint, buffer);
	}

	void CMesh::setDirty(E_BUFFER_TYPE buffer)
	{
		for (u32 i = 0; i < MeshBuffers.size(); ++i)
			MeshBuffers[i]->setDirty(buffer);
	}

	void CMesh::recalculateBoundingBox()
	{
		if (MeshBuffers.size())
		{
			BoundingBox = MeshBuffers[0]->getBoundingBox();
			for (u32 i = 1; i < MeshBuffers.size(); ++i)
				BoundingBox.addInternalBox(MeshBuffers[i]->getBoundingBox());
		}
		else
			BoundingBox.reset(0.0f, 0.0f, 0.0f);
	}

	void CMesh::addMeshBuffer(IMeshBuffer* buf, const char* materialName, CMaterial* m)
	{
		if (buf)
		{
			buf->grab();

			if (m)
				m->grab();

			MeshBuffers.push_back(buf);
			MaterialName.push_back(materialName);
			Materials.push_back(m);
		}
	}


	void CMesh::addBlendShape(CBlendShape* blendShape)
	{
		if (blendShape)
		{
			blendShape->grab();
			BlendShape.push_back(blendShape);
		}
	}

	void CMesh::removeAllMeshBuffer()
	{
		for (u32 i = 0; i < MeshBuffers.size(); ++i)
			MeshBuffers[i]->drop();

		for (u32 i = 0; i < Materials.size(); ++i)
		{
			if (Materials[i])
				Materials[i]->drop();
		}

		for (u32 i = 0, n = BlendShape.size(); i < n; i++)
			BlendShape[i]->drop();
	}

	void CMesh::removeMeshBuffer(IMeshBuffer* buf)
	{
		if (buf)
		{
			for (u32 i = 0; i < MeshBuffers.size(); i++)
			{
				if (MeshBuffers[i] == buf)
				{
					buf->drop();

					if (Materials[i])
						Materials[i]->drop();

					MeshBuffers.erase(i);
					MaterialName.erase(MaterialName.begin() + i);
					Materials.erase(Materials.begin() + i);
					return;
				}
			}
		}
	}

	void CMesh::replaceMeshBuffer(int i, IMeshBuffer* buf)
	{
		MeshBuffers[i]->drop();
		MeshBuffers[i] = buf;
		buf->grab();
	}

	IMeshBuffer* CMesh::getBufferByMaterialID(int materialID)
	{
		for (u32 i = 0, n = MeshBuffers.size(); i < n; i++)
		{
			IMeshBuffer* mb = MeshBuffers[i];
			if (mb->getMaterial().MaterialType == materialID)
				return mb;
		}

		return NULL;
	}

	CMesh* CMesh::detachAlphaMeshBuffer()
	{
		CMesh* alphaMesh = NULL;

		IVideoDriver* driver = getVideoDriver();
		u32 numMeshBuffer = MeshBuffers.size();

		for (u32 i = 0; i < numMeshBuffer; i++)
		{
			IMeshBuffer* buffer = MeshBuffers[i];
			video::SMaterial& m = buffer->getMaterial();

			bool isTransparent = driver->isMaterialTransparent(m);
			if (isTransparent == true)
			{
				if (alphaMesh == NULL)
					alphaMesh = new CMesh();

				// add buffer and material to new alpha mesh
				alphaMesh->addMeshBuffer(MeshBuffers[i]);
				alphaMesh->MaterialName.push_back(MaterialName[i]);
				alphaMesh->Materials.push_back(Materials[i]);

				// remove buffer and material
				MeshBuffers[i]->drop();

				if (Materials[i])
					Materials[i]->drop();

				MeshBuffers.erase(i);
				MaterialName.erase(MaterialName.begin() + i);
				Materials.erase(Materials.begin() + i);

				i--;
				numMeshBuffer--;
			}
		}

		if (alphaMesh != NULL)
			alphaMesh->recalculateBoundingBox();

		return alphaMesh;
	}

	void CMesh::applyDoubleSided(IMeshBuffer* meshBuffer)
	{
		video::E_VERTEX_TYPE vtxType = meshBuffer->getVertexType();

		if (vtxType == EVT_TANGENTS)
		{
			CVertexBuffer<video::S3DVertexTangents>* tangentMeshBuffer = (CVertexBuffer<video::S3DVertexTangents>*)(meshBuffer->getVertexBuffer());

			IIndexBuffer* indexBuffer = meshBuffer->getIndexBuffer();

			u32 indexCount = indexBuffer->getIndexCount();
			u32 newIndexCount = indexCount * 2;

			u32 vtxCount = tangentMeshBuffer->getVertexCount();
			u32 newVtxCount = vtxCount * 2;

			if (newVtxCount >= 0xffff)
				indexBuffer->setType(video::EIT_32BIT);

			// double sided mesh
			tangentMeshBuffer->reallocate(newVtxCount);
			indexBuffer->reallocate(newIndexCount);

			std::map<u32, u32> mapIndex;

			// add triangle
			for (u32 i = 0; i < indexCount; i += 3)
			{
				u32 i0 = indexBuffer->getIndex(i);
				u32 i1 = indexBuffer->getIndex(i + 1);
				u32 i2 = indexBuffer->getIndex(i + 2);

				int ni0 = -1;
				int ni1 = -1;
				int ni2 = -1;

				if (mapIndex.find(i0) == mapIndex.end())
				{
					video::S3DVertexTangents v0 = tangentMeshBuffer->getVertex(i0);
					v0.Normal *= -1.0f;

					tangentMeshBuffer->addVertex(v0);
					ni0 = vtxCount++;

					mapIndex[i0] = ni0;
				}
				else
				{
					ni0 = mapIndex[i0];
				}

				if (mapIndex.find(i1) == mapIndex.end())
				{
					video::S3DVertexTangents v1 = tangentMeshBuffer->getVertex(i1);
					v1.Normal *= -1.0f;

					tangentMeshBuffer->addVertex(v1);
					ni1 = vtxCount++;

					mapIndex[i1] = ni1;
				}
				else
				{
					ni1 = mapIndex[i1];
				}

				if (mapIndex.find(i2) == mapIndex.end())
				{
					video::S3DVertexTangents v2 = tangentMeshBuffer->getVertex(i2);
					v2.Normal *= -1.0f;

					tangentMeshBuffer->addVertex(v2);
					ni2 = vtxCount++;

					mapIndex[i2] = ni2;
				}
				else
				{
					ni2 = mapIndex[i2];
				}

				if (ni0 >= 0 && ni1 >= 0 && ni2 >= 0)
				{
					// flip face
					indexBuffer->addIndex(ni2);
					indexBuffer->addIndex(ni1);
					indexBuffer->addIndex(ni0);
				}
			}
		}
	}

	int CMesh::getMeshBufferID(IMeshBuffer* buffer)
	{
		u32 numMeshBuffer = MeshBuffers.size();
		for (u32 i = 0; i < numMeshBuffer; i++)
		{
			if (MeshBuffers[i] == buffer)
				return i;
		}

		return -1;
	}
}