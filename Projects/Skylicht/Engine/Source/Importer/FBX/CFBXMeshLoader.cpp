/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CFBXMeshLoader.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityPrefab.h"
#include "Exporter/ExportResources.h"

#include "Utils/CMemoryStream.h"
#include "Utils/CActivator.h"

#include "Transform/CWorldTransformData.h"
#include "ufbx.h"

namespace Skylicht
{
	CFBXMeshLoader::CFBXMeshLoader()
	{

	}

	CFBXMeshLoader::~CFBXMeshLoader()
	{

	}

	bool CFBXMeshLoader::loadModel(const char* resource, CEntityPrefab* output, bool normalMap, bool flipNormalMap, bool texcoord2, bool batching)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IReadFile* file = fs->createAndOpenFile(resource);
		if (file == NULL)
		{
			char log[64];
			sprintf(log, "Can not load: %s\n", resource);
			os::Printer::log(log);
			return false;
		}

		const long filesize = file->getSize();
		if (!filesize)
			return 0;

		c8* buf = new c8[filesize];
		memset(buf, 0, filesize);
		file->read((void*)buf, filesize);

		ufbx_load_opts opts;
		memset(&opts, 0, sizeof(ufbx_load_opts));

		opts.load_external_files = true;
		opts.allow_null_material = true;

		// NOTE: We use this _only_ for computing the bounds of the scene!
		// The viewer contains a proper implementation of skinning as well.
		// You probably don't need this.
		opts.evaluate_skinning = true;

		opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
		opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
		opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;

		opts.target_unit_meters = 1.0f;

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_memory(buf, filesize, &opts, &error);
		if (!scene)
		{
			os::Printer::log("Failed to load scene");
			delete buf;
			file->drop();
			return false;
		}

		// import scene

		delete[]buf;
		file->drop();

		ufbx_free_scene(scene);

		return false;
	}
}