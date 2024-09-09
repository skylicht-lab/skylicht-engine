/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "Entity/CEntity.h"
#include "Exporter/ExportResources.h"

#include "CSkylichtAnimExporter.h"
#include "Utils/CMemoryStream.h"

namespace Skylicht
{
	CSkylichtAnimExporter::CSkylichtAnimExporter()
	{

	}

	CSkylichtAnimExporter::~CSkylichtAnimExporter()
	{

	}

	bool CSkylichtAnimExporter::exportAnim(CAnimationClip* clip, const char* output)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IWriteFile* writeFile = fs->createAndWriteFile(output);
		if (writeFile == NULL)
			return false;

		// write header
		SAssetHeader assetHeader;
		strcpy(assetHeader.Sign, "SLT");
		assetHeader.AssetType = (u32)AssetAnimation;
		assetHeader.AssetVersion = 1;
		writeFile->write(&assetHeader, sizeof(SAssetHeader));

		// init memory (it will grow later)
		CMemoryStream memoryAnim(512);

		memoryAnim.writeString(clip->AnimName);
		memoryAnim.writeFloat(clip->Duration);
		memoryAnim.writeChar(clip->Loop ? 1 : 0);

		u32 count = (u32)clip->AnimInfo.size();
		memoryAnim.writeUInt(count);

		// flush to file
		writeFile->write(memoryAnim.getData(), memoryAnim.getSize());

		for (u32 i = 0; i < count; i++)
		{
			SEntityAnim* entityAnim = clip->AnimInfo[i];

			memoryAnim.resetWrite();
			memoryAnim.writeString(entityAnim->Name);

			// position
			CArrayKeyFrame<core::vector3df>& positions = entityAnim->Data.Positions;

			memoryAnim.writeFloatArray(&positions.Default.X, 3);

			u32 numKey = positions.size();
			memoryAnim.writeUInt(numKey);

			for (u32 j = 0; j < numKey; j++)
			{
				const CPositionKey& posKey = positions.Data[j];
				memoryAnim.writeFloat(posKey.Frame);
				memoryAnim.writeFloatArray(&posKey.Value.X, 3);
			}

			// rotation
			CArrayKeyFrame<core::quaternion>& rotations = entityAnim->Data.Rotations;

			memoryAnim.writeFloatArray(&rotations.Default.X, 4);

			numKey = rotations.size();
			memoryAnim.writeUInt(numKey);

			for (u32 j = 0; j < numKey; j++)
			{
				const CRotationKey& rotKey = rotations.Data[j];
				memoryAnim.writeFloat(rotKey.Frame);
				memoryAnim.writeFloatArray(&rotKey.Value.X, 4);
			}

			// scale
			CArrayKeyFrame<core::vector3df>& scales = entityAnim->Data.Scales;

			memoryAnim.writeFloatArray(&scales.Default.X, 3);

			numKey = scales.size();
			memoryAnim.writeUInt(numKey);

			for (u32 j = 0; j < numKey; j++)
			{
				const CScaleKey& scaleKey = scales.Data[j];
				memoryAnim.writeFloat(scaleKey.Frame);
				memoryAnim.writeFloatArray(&scaleKey.Value.X, 3);
			}

			// flush data to file
			writeFile->write(memoryAnim.getData(), memoryAnim.getSize());
		}

		writeFile->drop();
		return true;
	}
}
