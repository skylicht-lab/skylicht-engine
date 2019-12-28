#ifndef _MESH_MANAGER_H_
#define _MESH_MANAGER_H_

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

		CEntityPrefab* loadModel(const char *resource, bool loadNormalMap = true, bool loadTexcoord2 = true, bool createBatching = false);

		void releaseAllPrefabs();
	};
}

#endif