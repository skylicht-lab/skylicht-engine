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
}