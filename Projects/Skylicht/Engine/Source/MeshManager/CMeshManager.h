#ifndef _MESH_MANAGER_H_
#define _MESH_MANAGER_H_

#include "RenderMesh/CMesh.h"
#include "Utils/CGameSingleton.h"

namespace Skylicht
{
	class CMeshManager : public CGameSingleton<CMeshManager>
	{
	public:
		CMeshManager();

		virtual ~CMeshManager();
	};
}

#endif