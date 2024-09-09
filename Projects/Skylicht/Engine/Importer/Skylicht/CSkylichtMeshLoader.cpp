/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CSkylichtMeshLoader.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityPrefab.h"
#include "Exporter/ExportResources.h"

#include "Utils/CMemoryStream.h"
#include "Utils/CActivator.h"

#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	CSkylichtMeshLoader::CSkylichtMeshLoader()
	{

	}

	CSkylichtMeshLoader::~CSkylichtMeshLoader()
	{

	}

	bool CSkylichtMeshLoader::loadModel(const char* resource, CEntityPrefab* output, bool normalMap, bool flipNormalMap, bool texcoord2, bool batching)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IReadFile* readFile = fs->createAndOpenFile(resource);
		if (readFile == NULL)
			return false;

		long size = readFile->getSize();

		unsigned char* data = new unsigned char[size];
		readFile->read(data, (u32)size);
		readFile->drop();

		CMemoryStream stream(data, (u32)size);

		// read header
		SAssetHeader assetHeader;
		stream.readData(&assetHeader, sizeof(SAssetHeader));

		if (strcmp(assetHeader.Sign, "SLT") != 0)
		{
			delete[] data;
			return false;
		}

		if (assetHeader.AssetType != (u32)AssetModel)
		{
			delete[] data;
			return false;
		}

		if (assetHeader.AssetVersion == 1)
			loadVersion(&stream, output, assetHeader.AssetVersion, normalMap, texcoord2, batching);
		else
		{
			delete[] data;
			return false;
		}

		delete[] data;
		return true;
	}

	void CSkylichtMeshLoader::loadVersion(CMemoryStream* stream, CEntityPrefab* output, int version, bool normalMap, bool texcoord2, bool batching)
	{
		std::map<int, int> entityID;
		int depthChange = 0;

		u32 numEntity = stream->readUInt();
		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = output->createEntity();

			int entityIndex = stream->readInt();
			entityID[entityIndex] = entity->getIndex();

			bool entityVisible = stream->readChar() == 1 ? true : false;
			entity->setVisible(entityVisible);

			int entityDataSize = stream->readInt();
			while (entityDataSize != -1)
			{
				std::string entityDataName = stream->readString();
				u32 seek = stream->getPos();

				IEntityData* data = entity->addDataByActivator(entityDataName.c_str());
				if (data != NULL)
				{
					data->deserializable(stream, version);

					// hardcode to fix transform
					if (entityDataName == "CWorldTransformData")
					{
						CWorldTransformData* worldTransform = dynamic_cast<CWorldTransformData*>(data);
						if (worldTransform != NULL)
						{
							if (i == 0)
								depthChange = -worldTransform->Depth;

							worldTransform->Depth += depthChange;

							if (entityID.find(worldTransform->ParentIndex) != entityID.end())
								worldTransform->ParentIndex = entityID[worldTransform->ParentIndex];
						}
					}
				}

				// go next data
				stream->setPos(seek + entityDataSize);
				entityDataSize = stream->readInt();
			}
		}
	}
}
