#include "pch.h"
#include "Utils/CPath.h"
#include "CMeshManager.h"

#include "Importer/Collada/CColladaLoader.h"
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

	CEntityPrefab* CMeshManager::loadModel(const char *resource, const char *texturePath, bool loadNormalMap, bool loadTexcoord2, bool createBatching)
	{
		// find in cached
		std::map<std::string, CEntityPrefab*>::iterator findCache = m_meshPrefabs.find(resource);
		if (findCache != m_meshPrefabs.end())
		{
			return (*findCache).second;
		}

		IMeshImporter *importer = NULL;
		CEntityPrefab *output = NULL;

		// load from file
		std::string ext = CPath::getFileNameExt(resource);
		if (ext == "dae")
		{
			importer = new CColladaLoader();
		}

		if (importer != NULL)
		{
			output = new CEntityPrefab();

			// add search texture path
			importer->addTextureFolder(texturePath);

			// load model
			if (importer->loadModel(resource, output, loadNormalMap, loadTexcoord2, createBatching) == true)
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
}