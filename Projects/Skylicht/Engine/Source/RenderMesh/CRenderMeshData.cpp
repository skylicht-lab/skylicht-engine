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

#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/Instancing/CSkinTBNSGInstancing.h"

#include "TextureManager/CTextureManager.h"
#include "MeshManager/CMeshManager.h"
#include "Utils/CPath.h"
#include "Importer/IMeshImporter.h"
#include "VertexAnimation/CSoftwareSkinningUtils.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderMeshData);

	IMPLEMENT_DATA_TYPE_INDEX(CRenderMeshData);

	std::vector<std::string> g_importTextureFolder;

	void CRenderMeshData::setImportTextureFolder(std::vector<std::string>& folders)
	{
		g_importTextureFolder = folders;
	}

	CRenderMeshData::CRenderMeshData() :
		RenderMesh(NULL),
		SoftwareSkinnedMesh(NULL),
		SoftwareBlendShapeMesh(NULL),
		IsSoftwareSkinning(false),
		IsSoftwareBlendShape(false),
		IsSkinnedMesh(false),
		IsInstancing(false),
		IsSkinnedInstancing(false),
		MeshInstancing(NULL),
		Visible(true)
	{

	}

	CRenderMeshData::~CRenderMeshData()
	{
		if (RenderMesh != NULL)
			RenderMesh->drop();

		if (SoftwareSkinnedMesh != NULL)
			SoftwareSkinnedMesh->drop();

		if (SoftwareBlendShapeMesh != NULL)
			SoftwareBlendShapeMesh->drop();
	}

	void CRenderMeshData::setMesh(CMesh* mesh)
	{
		if (RenderMesh)
		{
			RenderMesh->drop();
			RenderMesh = NULL;
		}

		RenderMesh = mesh->clone();
	}

	void CRenderMeshData::setShareMesh(CMesh* mesh)
	{
		// share mesh just use on Static Mesh
		// CSkinnedMesh have Joints, SkinningMatrix. So it will have the bugs.
		if (RenderMesh)
		{
			RenderMesh->drop();
			RenderMesh = NULL;
		}

		RenderMesh = mesh;
		RenderMesh->grab();
	}

	void CRenderMeshData::setInstancing(bool b)
	{
		if (b)
			MeshInstancing = CMeshManager::getInstance()->createGetInstancingMesh(RenderMesh);
		else
			MeshInstancing = NULL;

		IsInstancing = b;
	}

	void CRenderMeshData::setSkinnedInstancing(bool b)
	{
		if (b)
			MeshInstancing = CMeshManager::getInstance()->createGetInstancingMesh(RenderMesh, new CSkinTBNSGInstancing());
		else
			MeshInstancing = NULL;

		IsSkinnedInstancing = b;
	}

	void CRenderMeshData::setMaterial(CMaterial* material)
	{
		CMesh* mesh = RenderMesh;
		if (SoftwareBlendShapeMesh)
			mesh = SoftwareBlendShapeMesh;
		if (SoftwareSkinnedMesh)
			mesh = SoftwareSkinnedMesh;

		const char* name = material->getName();

		int bufferID = 0;
		for (std::string& materialName : mesh->MaterialName)
		{
			if (materialName == name)
			{
				if (mesh->Materials[bufferID])
					mesh->Materials[bufferID]->drop();

				mesh->Materials[bufferID] = material;
				mesh->Materials[bufferID]->grab();
			}

			bufferID++;
		}

		if (MeshInstancing != NULL)
		{
			// apply material for instancing mesh
			CMesh* instancingMesh = dynamic_cast<CMesh*>(MeshInstancing->InstancingMesh);
			if (instancingMesh)
			{
				for (int i = 0, n = (int)instancingMesh->Materials.size(); i < n; i++)
				{
					if (mesh->Materials[i])
					{
						if (instancingMesh->Materials[i])
							instancingMesh->Materials[i]->drop();

						instancingMesh->Materials[i] = mesh->Materials[i];
						instancingMesh->Materials[i]->grab();

						MeshInstancing->Materials[i] = mesh->Materials[i];
					}
				}
			}
		}
	}

	void CRenderMeshData::unusedMaterial(CMaterial* material)
	{
		CMesh* mesh = RenderMesh;
		if (SoftwareBlendShapeMesh)
			mesh = SoftwareBlendShapeMesh;
		if (SoftwareSkinnedMesh)
			mesh = SoftwareSkinnedMesh;

		const char* name = material->getName();

		int bufferID = 0;
		for (std::string& materialName : mesh->MaterialName)
		{
			if (materialName == name)
			{
				if (mesh->Materials[bufferID])
				{
					mesh->Materials[bufferID]->drop();
					mesh->Materials[bufferID] = NULL;
				}
			}

			bufferID++;
		}

		if (MeshInstancing != NULL)
		{
			CMesh* instancingMesh = dynamic_cast<CMesh*>(MeshInstancing->InstancingMesh);
			if (instancingMesh)
			{
				for (int i = 0, n = (int)instancingMesh->Materials.size(); i < n; i++)
				{
					if (instancingMesh->Materials[i])
					{
						instancingMesh->Materials[i]->drop();
						instancingMesh->Materials[i] = NULL;
					}
				}
			}
		}
	}

	void CRenderMeshData::initSoftwareBlendShape()
	{
		CMesh* mesh = CSoftwareSkinningUtils::initSoftwareBlendShape(RenderMesh);

		mesh->setHardwareMappingHint(EHM_STREAM, EBT_VERTEX);
		mesh->setHardwareMappingHint(EHM_STATIC, EBT_INDEX);

		SoftwareBlendShapeMesh = mesh;
		IsSoftwareBlendShape = true;
	}

	void CRenderMeshData::initSoftwareSkinning()
	{
		CMesh* mesh = CSoftwareSkinningUtils::initSoftwareSkinning(RenderMesh);

		mesh->setHardwareMappingHint(EHM_STREAM, EBT_VERTEX);
		mesh->setHardwareMappingHint(EHM_STATIC, EBT_INDEX);

		// swap default render mesh to dynamic stream mesh
		// see CSoftwareSkinningSystem todo next
		SoftwareSkinnedMesh = mesh;
	}

	void CRenderMeshData::releaseSoftwareSkinning()
	{
		if (SoftwareSkinnedMesh != NULL)
		{
			SoftwareSkinnedMesh->drop();
			SoftwareSkinnedMesh = NULL;
		}

		IsSkinnedInstancing = false;
	}

	bool CRenderMeshData::serializable(CMemoryStream* stream)
	{
		stream->writeChar(IsSkinnedMesh ? 1 : 0);

		CMesh* mesh = RenderMesh;

		if (IsSkinnedMesh == true)
		{
			CSkinnedMesh* smesh = dynamic_cast<CSkinnedMesh*>(mesh);

			// write joint data
			u32 numJoint = smesh->Joints.size();
			stream->writeUInt(numJoint);

			for (u32 i = 0; i < numJoint; i++)
			{
				CSkinnedMesh::SJoint& j = smesh->Joints[i];

				stream->writeString(j.Name);
				stream->writeInt(j.EntityIndex);
				stream->writeFloatArray(j.BindPoseMatrix.pointer(), 16);
			}
		}

		// write number buffer
		u32 numMB = mesh->getMeshBufferCount();
		stream->writeUInt(numMB);

		for (u32 i = 0; i < numMB; i++)
		{
			// write bind material name
			stream->writeString(mesh->MaterialName[i]);

			IMeshBuffer* mb = mesh->getMeshBuffer(i);
			IVertexBuffer* vb = mb->getVertexBuffer(0);
			IIndexBuffer* ib = mb->getIndexBuffer();

			// write texture name
			SMaterial& material = mb->getMaterial();
			for (int t = 0; t < 3; t++)
			{
				ITexture* texture = material.TextureLayer[t].Texture;
				if (texture != NULL)
				{
					std::string path = texture->getName().getInternalName().c_str();
					stream->writeString(CPath::getFileName(path).c_str());
				}
				else
					stream->writeString("");
			}

			// write bounding box
			stream->writeFloatArray(&mb->getBoundingBox().MinEdge.X, 3);
			stream->writeFloatArray(&mb->getBoundingBox().MaxEdge.X, 3);

			// write vertices data
			u32 vtxCount = vb->getVertexCount();
			u32 vtxSize = vb->getVertexSize();
			u32 vtxBufferSize = vtxCount * vtxSize;

			u32 idxCount = ib->getIndexCount();
			u32 idxSize = ib->getIndexSize();
			u32 idxBufferSize = idxCount * idxSize;

			stream->writeUInt(vtxCount);
			stream->writeUInt(vtxSize);

			stream->writeUInt(idxCount);
			stream->writeUInt(idxSize);

			// write attribute
			int vertexType = (int)mb->getVertexType();
			stream->writeString(video::sBuiltInVertexTypeNames[vertexType]);

			video::IVertexDescriptor* vtxInfo = mb->getVertexDescriptor();
			u32 numAttribute = vtxInfo->getAttributeCount();
			stream->writeUInt(numAttribute);
			for (u32 j = 0; j < numAttribute; j++)
			{
				IVertexAttribute* attribute = vtxInfo->getAttribute(j);
				stream->writeString(attribute->getName().c_str());
				stream->writeShort(attribute->getOffset());
				stream->writeShort(attribute->getElementCount());
				stream->writeShort(attribute->getTypeSize());
			}

			// write vertex data
			stream->writeData(vb->getVertices(), vtxBufferSize);

			// write indices data
			stream->writeData(ib->getIndices(), idxBufferSize);
		}

		return true;
	}

	bool CRenderMeshData::deserializable(CMemoryStream* stream, int version)
	{
		CShaderManager* shaderMgr = CShaderManager::getInstance();

		IsSkinnedMesh = stream->readChar() == 1 ? true : false;

		if (IsSkinnedMesh == true)
		{
			CSkinnedMesh* smesh = new CSkinnedMesh();

			u32 numJoint = stream->readUInt();

			for (u32 i = 0; i < numJoint; i++)
			{
				smesh->Joints.push_back(CSkinnedMesh::SJoint());
				CSkinnedMesh::SJoint& j = smesh->Joints[i];

				j.Name = stream->readString();
				j.EntityIndex = stream->readInt();
				stream->readFloatArray(j.BindPoseMatrix.pointer(), 16);
			}

			RenderMesh = smesh;
		}
		else
		{
			RenderMesh = new CMesh();
		}

		u32 numMB = stream->readUInt();

		for (u32 i = 0; i < numMB; i++)
		{
			std::string material = stream->readString();

			std::vector<std::string> textures;

			for (int t = 0; t < 3; t++)
				textures.push_back(stream->readString());

			core::aabbox3df bbox;
			stream->readFloatArray(&bbox.MinEdge.X, 3);
			stream->readFloatArray(&bbox.MaxEdge.X, 3);

			u32 vtxCount = stream->readUInt();
			u32 vtxSize = stream->readUInt();
			u32 vtxBufferSize = vtxCount * vtxSize;

			u32 idxCount = stream->readUInt();
			u32 idxSize = stream->readUInt();
			u32 idxBufferSize = idxCount * idxSize;

			E_INDEX_TYPE indexType = video::EIT_16BIT;
			if (idxSize == 4)
				indexType = video::EIT_32BIT;

			std::string vertexTypeName = stream->readString();

			IMeshBuffer* mb = NULL;

			bool vertexCompatible = true;

			IVertexDescriptor* vertexDes = getVideoDriver()->getVertexDescriptor(vertexTypeName.c_str());
			if (vertexDes != NULL)
			{
				if (vertexDes->getVertexSize(0) == vtxSize)
				{
					E_VERTEX_TYPE vtxType = (E_VERTEX_TYPE)vertexDes->getID();
					switch (vtxType)
					{
					case EVT_STANDARD:
						mb = new CMeshBuffer<S3DVertex>(vertexDes, indexType);
						break;
					case EVT_2TCOORDS:
						mb = new CMeshBuffer<S3DVertex2TCoords>(vertexDes, indexType);
						break;
					case EVT_TANGENTS:
						mb = new CMeshBuffer<S3DVertexTangents>(vertexDes, indexType);
						break;
					case EVT_SKIN:
						mb = new CMeshBuffer<S3DVertexSkin>(vertexDes, indexType);
						break;
					case EVT_SKIN_TANGENTS:
						mb = new CMeshBuffer<S3DVertexSkinTangents>(vertexDes, indexType);
						break;
					case EVT_2TCOORDS_TANGENTS:
						mb = new CMeshBuffer<S3DVertex2TCoordsTangents>(vertexDes, indexType);
						break;
					case EVT_SKIN_2TCOORDS_TANGENTS:
						mb = new CMeshBuffer<S3DVertexSkin2TCoordsTangents>(vertexDes, indexType);
						break;
					}
				}
				else
				{
					os::Printer::log("[CRenderMeshData::deserializable] Vertex is not compatible");
					vertexCompatible = false;
				}
			}

			u32 numAttribute = stream->readUInt();
			for (u32 j = 0; j < numAttribute; j++)
			{
				std::string attributeName = stream->readString();
				short offset = stream->readShort();
				short elementCount = stream->readShort();
				short typeSize = stream->readShort();
			}

			if (mb != NULL)
			{
				IVertexBuffer* vtxBuffer = mb->getVertexBuffer();
				IIndexBuffer* idxBuffer = mb->getIndexBuffer();

				vtxBuffer->set_used(vtxCount);
				stream->readData(vtxBuffer->getVertices(), vtxBufferSize);

				idxBuffer->set_used(idxCount);
				stream->readData(idxBuffer->getIndices(), idxBufferSize);

				mb->getBoundingBox() = bbox;
				mb->setHardwareMappingHint(EHM_STATIC);

				video::SMaterial& irrMaterial = mb->getMaterial();

				for (int t = 0; t < 3; t++)
				{
					if (textures[t].empty() == false)
					{
						ITexture* texture = CTextureManager::getInstance()->getTexture(textures[t].c_str(), g_importTextureFolder);
						if (texture != NULL)
							irrMaterial.setTexture(t, texture);

						if (t == 0 && texture != NULL)
						{
							if (IsSkinnedMesh == false)
								irrMaterial.MaterialType = shaderMgr->getShaderIDByName("TextureColor");
							else
								irrMaterial.MaterialType = shaderMgr->getShaderIDByName("Skin");
						}
					}
				}

				RenderMesh->addMeshBuffer(mb, material.c_str());
				RenderMesh->recalculateBoundingBox();

				mb->drop();
			}
		}

		return true;
	}
}