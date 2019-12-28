#ifndef _MESH_IMPORTER_H_
#define _MESH_IMPORTER_H_

namespace Skylicht
{
	class CEntityPrefab;

	class IMeshImporter
	{
	public:
		IMeshImporter()
		{

		}

		virtual ~IMeshImporter()
		{

		}

		virtual bool loadModel(const char *resource, CEntityPrefab* output, bool normalMap = true, bool texcoord2 = true, bool batching = false) = 0;
	};
}

#endif