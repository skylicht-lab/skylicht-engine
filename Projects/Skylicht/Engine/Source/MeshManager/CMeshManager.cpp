#include "pch.h"
#include "Utils/CPath.h"
#include "CMeshManager.h"

#include "Importer/Collada/CColladaLoader.h"
#include "Importer/WavefrontOBJ/COBJMeshFileLoader.h"
#include "Importer/Skylicht/CSkylichtMeshLoader.h"
#include "Importer/FBX/CFBXMeshLoader.h"

#include "Exporter/Skylicht/CSkylichtMeshExporter.h"

#include "RenderMesh/CRenderMeshData.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/CShader.h"

namespace Skylicht
{
	CMeshManager::CMeshManager()
	{

	}

	CMeshManager::~CMeshManager()
	{
		releaseAllPrefabs();
		releaseAllInstancingMesh();
	}

	void CMeshManager::releasePrefab(CEntityPrefab* prefab)
	{
		std::map<std::string, CEntityPrefab*>::iterator i = m_meshPrefabs.begin(), end = m_meshPrefabs.end();
		while (i != end)
		{
			if ((*i).second == prefab)
			{
				delete (*i).second;
				m_meshPrefabs.erase(i);
				return;
			}
			++i;
		}
	}

	void CMeshManager::releaseAllPrefabs()
	{
		std::map<std::string, CEntityPrefab*>::iterator i = m_meshPrefabs.begin(), end = m_meshPrefabs.end();
		while (i != end)
		{
			delete (*i).second;
			++i;
		}

		m_meshPrefabs.clear();
	}

	void CMeshManager::releaseAllInstancingMesh()
	{
		for (SMeshInstancingData* data : m_instancingData)
		{
			u32 n = data->MeshBuffers.size();
			for (u32 i = 0; i < n; i++)
				data->MeshBuffers[i]->drop();

			n = data->InstancingBuffer.size();
			for (u32 i = 0; i < n; i++)
				data->InstancingBuffer[i]->drop();

			n = data->InstancingMeshBuffers.size();
			for (u32 i = 0; i < n; i++)
				data->InstancingMeshBuffers[i]->drop();

			delete data;
		}
		m_instancingData.clear();
	}

	CEntityPrefab* CMeshManager::loadModel(const char* resource, const char* texturePath, bool loadNormalMap, bool flipNormalMap, bool loadTexcoord2, bool createBatching)
	{
		// find in cached
		std::map<std::string, CEntityPrefab*>::iterator findCache = m_meshPrefabs.find(resource);
		if (findCache != m_meshPrefabs.end())
		{
			return (*findCache).second;
		}

		IMeshImporter* importer = NULL;
		CEntityPrefab* output = NULL;

		// load from file
		std::string ext = CPath::getFileNameExt(resource);
		if (ext == "dae")
			importer = new CColladaLoader();
		else if (ext == "obj")
			importer = new COBJMeshFileLoader();
		else if (ext == "smesh")
			importer = new CSkylichtMeshLoader();
		else if (ext == "fbx")
			importer = new CFBXMeshLoader();

		if (importer != NULL)
		{
			output = new CEntityPrefab();

			// add search texture path
			if (texturePath != NULL)
				importer->addTextureFolder(texturePath);

			// add base folder path
			std::string baseFolderPath = CPath::getFolderPath(resource);
			importer->addTextureFolder(baseFolderPath.c_str());

			// hard code list folder
			CRenderMeshData::ImportTextureFolder = importer->getTextureFolder();

			// load model
			if (importer->loadModel(resource, output, loadNormalMap, flipNormalMap, loadTexcoord2, createBatching) == true)
			{
				// cached resource
				m_meshPrefabs[resource] = output;
			}
			else
			{
				// load failed!
				delete output;
				output = NULL;
			}

			// clear cache load model
			delete importer;
		}

		return output;
	}

	bool CMeshManager::exportModel(CEntity** entities, u32 count, const char* output)
	{
		IMeshExporter* exporter = NULL;

		std::string ext = CPath::getFileNameExt(output);
		if (ext == "smesh")
			exporter = new CSkylichtMeshExporter();

		if (exporter != NULL)
		{
			bool result = exporter->exportModel(entities, count, output);
			delete exporter;
			return true;
		}

		return false;
	}

	SMeshInstancingData* CMeshManager::createGetInstancingMesh(CMesh* mesh)
	{
		if (!canCreateInstancingMesh(mesh))
			return NULL;

		for (SMeshInstancingData* instancingData : m_instancingData)
		{
			if (compareMeshBuffer(mesh, instancingData))
			{
				return instancingData;
			}
		}

		return createInstancingData(mesh);
	}

	SMeshInstancingData* CMeshManager::createInstancingData(CMesh* mesh)
	{
		SMeshInstancingData* data = new SMeshInstancingData();

		u32 mbCount = mesh->getMeshBufferCount();

		for (u32 i = 0; i < mbCount; i++)
		{
			CMaterial* material = mesh->Materials[i];
			if (material == NULL)
				continue;

			if (material->getShader() == NULL)
				continue;

			if (material->getShader()->getInstancing() == NULL)
				continue;

			if (material->getShader()->getInstancingShader() == NULL)
				continue;

			IMeshBuffer* mb = mesh->getMeshBuffer(i);

			data->MeshBuffers.push_back(mb);
			data->Materials.push_back(material);

			mb->grab();
			IShaderInstancing* instancing = material->getShader()->getInstancing();

			IVertexBuffer* instancingBuffer = NULL;

			for (u32 j = 0; j < data->Instancing.size(); j++)
			{
				if (instancing == data->Instancing[j])
				{
					instancingBuffer = data->InstancingBuffer[j];
					break;
				}
			}

			if (instancingBuffer == NULL)
			{
				instancingBuffer = instancing->createInstancingMeshBuffer();
				instancingBuffer->setHardwareMappingHint(EHM_STREAM);

				data->Instancing.push_back(instancing);
				data->InstancingBuffer.push_back(instancingBuffer);
			}

			IMeshBuffer* newMeshBuffer = instancing->copyConvertMeshBuffer(mb);
			if (newMeshBuffer)
			{
				// change vertex descriptor & add instancing buffer
				instancing->applyInstancing(newMeshBuffer, instancingBuffer);

				// set hardware static buffer
				newMeshBuffer->setHardwareMappingHint(EHM_STATIC);

				// save to render this meshbuffer
				data->InstancingMeshBuffers.push_back(newMeshBuffer);
			}
		}

		m_instancingData.push_back(data);
		return data;
	}

	bool CMeshManager::canCreateInstancingMesh(CMesh* mesh)
	{
		u32 mbCount = mesh->getMeshBufferCount();

		for (u32 i = 0; i < mbCount; i++)
		{
			CMaterial* material = mesh->Materials[i];
			if (material == NULL)
				continue;

			if (material->getShader() == NULL)
				continue;

			if (material->getShader()->getInstancing() == NULL)
				continue;

			if (material->getShader()->getInstancingShader() == NULL)
				continue;

			return true;
		}

		return false;
	}

	bool CMeshManager::compareMeshBuffer(CMesh* mesh, SMeshInstancingData* data)
	{
		u32 mbCount = mesh->getMeshBufferCount();
		u32 mbID = 0;

		for (u32 i = 0; i < mbCount; i++)
		{
			if (mbID >= data->MeshBuffers.size())
				return false;

			CMaterial* material = mesh->Materials[i];
			if (material == NULL)
				continue;

			if (material->getShader() == NULL)
				continue;

			if (material->getShader()->getInstancing() == NULL)
				continue;

			if (material->getShader()->getInstancingShader() == NULL)
				continue;

			IMeshBuffer* mb = mesh->getMeshBuffer(i);
			if (data->MeshBuffers[mbID] != mb)
				return false;

			if (data->Materials[mbID] != material)
				return false;

			mbID++;
		}

		return true;
	}
}