#pragma once

#include "RenderMesh/CMesh.h"
#include "Entity/CEntityPrefab.h"
#include "Utils/CGameSingleton.h"

namespace Skylicht
{
	class CMeshManager : public CGameSingleton<CMeshManager>
	{
	protected:
		std::map<std::string, CEntityPrefab*> m_meshPrefabs;

	public:
		CMeshManager();

		virtual ~CMeshManager();

		CEntityPrefab* loadModel(const char *resource, const char *texturePath, bool loadNormalMap = true, bool loadTexcoord2 = false, bool createBatching = false);

		void releaseAllPrefabs();
	};
}