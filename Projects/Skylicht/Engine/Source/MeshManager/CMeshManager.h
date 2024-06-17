#pragma once

#include "RenderMesh/CMesh.h"
#include "Entity/CEntityPrefab.h"
#include "Utils/CSingleton.h"
#include "Instancing/SMeshInstancing.h"

#include "Importer/IMeshImporter.h"
#include "Exporter/IMeshExporter.h"

namespace Skylicht
{
	class SKYLICHT_API CMeshManager
	{
	public:
		DECLARE_SINGLETON(CMeshManager)

	protected:
		std::map<std::string, CEntityPrefab*> m_meshPrefabs;

		std::vector<SMeshInstancing*> m_instancingData;

	public:
		CMeshManager();

		virtual ~CMeshManager();

		CEntityPrefab* loadModel(const char* resource, const char* texturePath, bool loadNormalMap = true, bool flipNormalMap = true, bool loadTexcoord2 = false, bool createBatching = false);

		CEntityPrefab* loadModel(const char* resource, const char* texturePath, IMeshImporter* importer, bool loadNormalMap = true, bool flipNormalMap = true, bool loadTexcoord2 = false, bool createBatching = false);

		bool exportModel(CEntity** entities, u32 count, const char* output);

		bool exportModel(CEntity** entities, u32 count, const char* output, IMeshExporter* exporter);

		void releasePrefab(CEntityPrefab* prefab);

		void releaseAllPrefabs();

		void releaseAllInstancingMesh();

		SMeshInstancing* createGetInstancingMesh(CMesh* mesh);

		SMeshInstancing* createGetInstancingMesh(CMesh* mesh, IShaderInstancing* shaderInstancing);

		void changeInstancingTransformBuffer(SMeshInstancing* mesh, IVertexBuffer* transform, IVertexBuffer* lighting);

		void changeInstancingMaterialBuffer(SMeshInstancing* mesh, IVertexBuffer* materials);

	protected:

		bool canCreateInstancingMesh(CMesh* mesh);

		bool compareMeshBuffer(CMesh* mesh, SMeshInstancing* data);

		SMeshInstancing* createInstancingData(CMesh* mesh);

		SMeshInstancing* createInstancingData(CMesh* mesh, IShaderInstancing* shaderInstancing);
	};
}