#pragma once

#include "RenderMesh/CMesh.h"
#include "Entity/CEntityPrefab.h"
#include "Utils/CSingleton.h"

namespace Skylicht
{
	class CMeshManager
	{
	public:
		DECLARE_SINGLETON(CMeshManager)

	protected:
		std::map<std::string, CEntityPrefab*> m_meshPrefabs;

		std::vector<SMeshInstancingData*> m_instancingData;

	public:
		CMeshManager();

		virtual ~CMeshManager();

		CEntityPrefab* loadModel(const char* resource, const char* texturePath, bool loadNormalMap = true, bool flipNormalMap = true, bool loadTexcoord2 = false, bool createBatching = false);

		bool exportModel(CEntity** entities, u32 count, const char* output);

		void releasePrefab(CEntityPrefab* prefab);

		void releaseAllPrefabs();

		void releaseAllInstancingMesh();

		SMeshInstancingData* createGetInstancingMesh(CMesh* mesh);

	protected:

		bool canCreateInstancingMesh(CMesh* mesh);

		bool compareMeshBuffer(CMesh* mesh, SMeshInstancingData* data);

		SMeshInstancingData* createInstancingData(CMesh* mesh);
	};
}