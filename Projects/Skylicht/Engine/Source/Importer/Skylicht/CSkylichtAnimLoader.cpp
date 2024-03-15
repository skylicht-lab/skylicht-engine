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
#include "CSkylichtAnimLoader.h"

#include "Exporter/ExportResources.h"

#include "Utils/CMemoryStream.h"
#include "Animation/CAnimationClip.h"

namespace Skylicht
{
	CSkylichtAnimLoader::CSkylichtAnimLoader()
	{

	}

	CSkylichtAnimLoader::~CSkylichtAnimLoader()
	{

	}

	bool CSkylichtAnimLoader::loadAnimation(const char* resource, CAnimationClip* output)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IReadFile* readFile = fs->createAndOpenFile(resource);
		if (readFile == NULL)
			return false;

		u32 size = readFile->getSize();

		unsigned char* data = new unsigned char[size];
		readFile->read(data, size);
		readFile->drop();

		CMemoryStream stream(data, size);

		// read header
		SAssetHeader assetHeader;
		stream.readData(&assetHeader, sizeof(SAssetHeader));

		if (strcmp(assetHeader.Sign, "SLT") != 0)
		{
			delete[] data;
			return false;
		}

		if (assetHeader.AssetType != (u32)AssetAnimation)
		{
			delete[] data;
			return false;
		}

		if (assetHeader.AssetVersion == 1)
			loadVersion1(&stream, output);
		else
		{
			delete[] data;
			return false;
		}

		delete[] data;
		return true;
	}

	void CSkylichtAnimLoader::loadVersion1(CMemoryStream* stream, CAnimationClip* output)
	{
		output->AnimName = stream->readString();
		output->Duration = stream->readFloat();
		output->Loop = stream->readChar() == 1 ? true : false;

		u32 count = stream->readUInt();

		for (u32 i = 0; i < count; i++)
		{
			SEntityAnim* entityAnim = new SEntityAnim();
			entityAnim->Name = stream->readString();

			// positions
			CArrayKeyFrame<core::vector3df>& positions = entityAnim->Data.Positions;

			stream->readFloatArray(&positions.Default.X, 3);

			u32 numKey = stream->readUInt();
			positions.Data.set_used(numKey);

			for (u32 j = 0; j < numKey; j++)
			{
				CPositionKey& posKey = positions.Data[j];
				posKey.Frame = stream->readFloat();
				stream->readFloatArray(&posKey.Value.X, 3);
			}

			// rotations
			CArrayKeyFrame<core::quaternion>& rotations = entityAnim->Data.Rotations;

			stream->readFloatArray(&rotations.Default.X, 4);

			numKey = stream->readUInt();
			rotations.Data.set_used(numKey);

			for (u32 j = 0; j < numKey; j++)
			{
				CRotationKey& rotKey = rotations.Data[j];
				rotKey.Frame = stream->readFloat();
				stream->readFloatArray(&rotKey.Value.X, 4);
			}

			// scales
			CArrayKeyFrame<core::vector3df>& scales = entityAnim->Data.Scales;

			stream->readFloatArray(&scales.Default.X, 3);

			numKey = stream->readUInt();
			scales.Data.set_used(numKey);

			for (u32 j = 0; j < numKey; j++)
			{
				CScaleKey& scaleKey = scales.Data[j];
				scaleKey.Frame = stream->readFloat();
				stream->readFloatArray(&scaleKey.Value.X, 3);
			}

			output->addAnim(entityAnim);
		}
	}
}