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
	IMPLEMENT_SINGLETON(CMeshManager);

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
		for (SMeshInstancing* data : m_instancingData)
		{
			u32 n = data->MeshBuffers.size();
			for (u32 i = 0; i < n; i++)
				data->MeshBuffers[i]->drop();

			n = data->MaterialBuffer.size();
			for (u32 i = 0; i < n; i++)
				data->MaterialBuffer[i]->drop();

			data->TransformBuffer->drop();
			data->IndirectLightingBuffer->drop();

			n = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < n; i++)
				data->RenderMeshBuffers[i]->drop();

			n = data->RenderLightMeshBuffers.size();
			for (u32 i = 0; i < n; i++)
				data->RenderLightMeshBuffers[i]->drop();

			data->InstancingMesh->drop();
			delete data;
		}
		m_instancingData.clear();
	}

	CEntityPrefab* CMeshManager::loadModel(const char* resource, const char* texturePath, bool loadNormalMap, bool flipNormalMap, bool loadTexcoord2, bool createBatching)
	{
		IMeshImporter* importer = NULL;

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

		CEntityPrefab* output = loadModel(resource, texturePath, importer, loadNormalMap, flipNormalMap, loadTexcoord2, createBatching);

		if (importer)
			delete importer;

		return output;
	}

	CEntityPrefab* CMeshManager::loadModel(const char* resource, const char* texturePath, IMeshImporter* importer, bool loadNormalMap, bool flipNormalMap, bool loadTexcoord2, bool createBatching)
	{
		// find in cached
		std::map<std::string, CEntityPrefab*>::iterator findCache = m_meshPrefabs.find(resource);
		if (findCache != m_meshPrefabs.end())
		{
			return (*findCache).second;
		}

		CEntityPrefab* output = NULL;

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
			CRenderMeshData::setImportTextureFolder(importer->getTextureFolder());

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
			return result;
		}

		return false;
	}

	bool CMeshManager::exportModel(CEntity** entities, u32 count, const char* output, IMeshExporter* exporter)
	{
		if (exporter != NULL)
			return exporter->exportModel(entities, count, output);
		return false;
	}

	SMeshInstancing* CMeshManager::createGetInstancingMesh(CMesh* mesh)
	{
		if (!canCreateInstancingMesh(mesh))
			return NULL;

		for (SMeshInstancing* instancingData : m_instancingData)
		{
			if (compareMeshBuffer(mesh, instancingData))
			{
				return instancingData;
			}
		}

		return createInstancingData(mesh);
	}

	SMeshInstancing* CMeshManager::createGetInstancingMesh(CMesh* mesh, IShaderInstancing* shaderInstancing)
	{
		for (SMeshInstancing* instancingData : m_instancingData)
		{
			if (compareMeshBuffer(mesh, instancingData))
			{
				return instancingData;
			}
		}

		return createInstancingData(mesh, shaderInstancing);
	}

	SMeshInstancing* CMeshManager::createInstancingData(CMesh* mesh)
	{
		SMeshInstancing* data = new SMeshInstancing();

		u32 mbCount = mesh->getMeshBufferCount();

		// create instancing mesh render the texture albedo, normal
		CMesh* instancingMesh = mesh->clone();
		instancingMesh->UseInstancing = true;
		instancingMesh->removeAllMeshBuffer();
		data->InstancingMesh = instancingMesh;

		// create instancing mesh, that render the indirect lighting
		CMesh* instancingLightingMesh = instancingMesh->clone();
		instancingLightingMesh->UseInstancing = true;
		instancingLightingMesh->removeAllMeshBuffer();
		instancingMesh->IndirectLightingMesh = instancingLightingMesh;

		// create transform & light buffer
		IVertexBuffer* transformBuffer = IShaderInstancing::createTransformVertexBuffer();
		transformBuffer->setHardwareMappingHint(EHM_STREAM);

		IVertexBuffer* lightingBuffer = IShaderInstancing::createIndirectLightingVertexBuffer();
		lightingBuffer->setHardwareMappingHint(EHM_STREAM);

		data->TransformBuffer = transformBuffer;
		data->IndirectLightingBuffer = lightingBuffer;

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

			IShaderInstancing* shaderInstancing = material->getShader()->getInstancing();

			IVertexBuffer* materialBuffer = shaderInstancing->createInstancingVertexBuffer();
			materialBuffer->setHardwareMappingHint(EHM_STREAM);

			data->InstancingShader.push_back(shaderInstancing);
			data->MaterialBuffer.push_back(materialBuffer);

			IMeshBuffer* renderMeshBuffer = shaderInstancing->createLinkMeshBuffer(mb);
			IMeshBuffer* lightingMeshBuffer = shaderInstancing->createLinkMeshBuffer(mb);

			if (renderMeshBuffer && lightingMeshBuffer)
			{
				// INDIRECT LIGHTING MESH				
				lightingMeshBuffer->setHardwareMappingHint(EHM_STATIC);

				instancingLightingMesh->addMeshBuffer(
					lightingMeshBuffer,
					mesh->MaterialName[i].c_str(),
					NULL
				);

				shaderInstancing->applyInstancingForRenderLighting(lightingMeshBuffer, lightingBuffer, transformBuffer);

				// INSTANCING MESH
				renderMeshBuffer->setHardwareMappingHint(EHM_STATIC);

				instancingMesh->addMeshBuffer(
					renderMeshBuffer,
					mesh->MaterialName[i].c_str(),
					mesh->Materials[i]
				);

				shaderInstancing->applyInstancing(renderMeshBuffer, materialBuffer, transformBuffer);

				// save to render this mesh buffer
				data->RenderMeshBuffers.push_back(renderMeshBuffer);
				data->RenderLightMeshBuffers.push_back(lightingMeshBuffer);

				// apply material
				mesh->Materials[i]->applyMaterial(renderMeshBuffer->getMaterial());
			}
		}

		m_instancingData.push_back(data);
		return data;
	}

	SMeshInstancing* CMeshManager::createInstancingData(CMesh* mesh, IShaderInstancing* shaderInstancing)
	{
		SMeshInstancing* data = new SMeshInstancing();

		u32 mbCount = mesh->getMeshBufferCount();

		// create instancing mesh render the texture albedo, normal
		CMesh* instancingMesh = mesh->clone();
		instancingMesh->UseInstancing = true;
		instancingMesh->removeAllMeshBuffer();
		data->InstancingMesh = instancingMesh;
		data->HandleShader = shaderInstancing;

		// create instancing mesh, that render the indirect lighting
		CMesh* instancingLightingMesh = instancingMesh->clone();
		instancingLightingMesh->UseInstancing = true;
		instancingLightingMesh->removeAllMeshBuffer();
		instancingMesh->IndirectLightingMesh = instancingLightingMesh;

		// create transform & light buffer
		IVertexBuffer* transformBuffer = IShaderInstancing::createTransformVertexBuffer();
		transformBuffer->setHardwareMappingHint(EHM_STREAM);

		IVertexBuffer* lightingBuffer = IShaderInstancing::createIndirectLightingVertexBuffer();
		lightingBuffer->setHardwareMappingHint(EHM_STREAM);

		data->TransformBuffer = transformBuffer;
		data->IndirectLightingBuffer = lightingBuffer;

		for (u32 i = 0; i < mbCount; i++)
		{
			CMaterial* material = mesh->Materials[i];
			IMeshBuffer* mb = mesh->getMeshBuffer(i);

			data->MeshBuffers.push_back(mb);
			data->Materials.push_back(material);

			mb->grab();

			IVertexBuffer* materialBuffer = shaderInstancing->createInstancingVertexBuffer();
			materialBuffer->setHardwareMappingHint(EHM_STREAM);

			data->InstancingShader.push_back(shaderInstancing);
			data->MaterialBuffer.push_back(materialBuffer);

			IMeshBuffer* renderMeshBuffer = shaderInstancing->createLinkMeshBuffer(mb);
			IMeshBuffer* lightingMeshBuffer = shaderInstancing->createLinkMeshBuffer(mb);

			if (renderMeshBuffer && lightingMeshBuffer)
			{
				// INDIRECT LIGHTING MESH
				lightingMeshBuffer->setHardwareMappingHint(EHM_STATIC);

				instancingLightingMesh->addMeshBuffer(
					lightingMeshBuffer,
					mesh->MaterialName[i].c_str(),
					NULL
				);

				shaderInstancing->applyInstancingForRenderLighting(lightingMeshBuffer, lightingBuffer, transformBuffer);

				// INSTANCING MESH
				renderMeshBuffer->setHardwareMappingHint(EHM_STATIC);

				instancingMesh->addMeshBuffer(
					renderMeshBuffer,
					mesh->MaterialName[i].c_str(),
					mesh->Materials[i]
				);

				shaderInstancing->applyInstancing(renderMeshBuffer, materialBuffer, transformBuffer);

				// save to render this mesh buffer
				data->RenderMeshBuffers.push_back(renderMeshBuffer);
				data->RenderLightMeshBuffers.push_back(lightingMeshBuffer);
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

	bool CMeshManager::compareMeshBuffer(CMesh* mesh, SMeshInstancing* data)
	{
		u32 mbCount = mesh->getMeshBufferCount();
		u32 mbID = 0;

		for (u32 i = 0; i < mbCount; i++)
		{
			if (mbID >= data->MeshBuffers.size())
				return false;

			IMeshBuffer* mb = mesh->getMeshBuffer(i);
			if (data->MeshBuffers[mbID] != mb)
				return false;

			if (data->Materials[mbID] != mesh->Materials[i])
				return false;

			mbID++;
		}

		return true;
	}

	void CMeshManager::changeInstancingTransformBuffer(SMeshInstancing* data, IVertexBuffer* transform, IVertexBuffer* lighting)
	{
		data->UseShareTransformBuffer = true;

		// drop old transform
		data->TransformBuffer->drop();
		data->IndirectLightingBuffer->drop();

		// grab new transform
		data->TransformBuffer = transform;
		data->TransformBuffer->grab();

		// grab new lighting
		data->IndirectLightingBuffer = lighting;
		data->IndirectLightingBuffer->grab();

		u32 numMeshBuffers = data->MeshBuffers.size();
		for (u32 i = 0; i < numMeshBuffers; i++)
		{
			// shader instancing
			IShaderInstancing* shaderInstancing = data->InstancingShader[i];

			// get instancing mesh buffer
			IMeshBuffer* renderMeshBuffer = data->RenderMeshBuffers[i];
			IMeshBuffer* lightingMeshBuffer = data->RenderLightMeshBuffers[i];

			IVertexBuffer* instancingBuffer = data->MaterialBuffer[i];

			// bind instancing vb to mesh buffer
			shaderInstancing->applyInstancing(renderMeshBuffer, instancingBuffer, transform);
			shaderInstancing->applyInstancingForRenderLighting(lightingMeshBuffer, lighting, transform);
		}
	}

	void CMeshManager::changeInstancingMaterialBuffer(SMeshInstancing* data, IVertexBuffer* materials)
	{
		data->UseShareMaterialsBuffer = true;

		u32 numMeshBuffers = data->MeshBuffers.size();
		for (u32 i = 0; i < numMeshBuffers; i++)
		{
			// shader instancing
			IShaderInstancing* shaderInstancing = data->InstancingShader[i];

			// drop old material
			if (data->MaterialBuffer[i])
			{
				data->MaterialBuffer[i]->drop();
				data->MaterialBuffer[i] = materials;
				data->MaterialBuffer[i]->grab();
			}

			// bind instancing vb to mesh buffer
			shaderInstancing->applyInstancing(data->RenderMeshBuffers[i], materials, data->TransformBuffer);
		}
	}
}