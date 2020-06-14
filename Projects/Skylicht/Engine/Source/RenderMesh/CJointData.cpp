/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CJointData.h"

namespace Skylicht
{
	CJointData::CJointData() :
		BoneRoot(false),
		RootIndex(-1)
	{

	}

	CJointData::~CJointData()
	{

	}

	bool CJointData::serializable(CMemoryStream *stream, IMeshExporter *exporter)
	{
		stream->writeChar(BoneRoot == true ? 1 : 0);

		stream->writeString(SID);
		stream->writeString(BoneName);

		stream->writeFloatArray(AnimationMatrix.pointer(), 16);
		stream->writeFloatArray(DefaultAnimationMatrix.pointer(), 16);
		stream->writeFloatArray(DefaultRelativeMatrix.pointer(), 16);

		return true;
	}

	bool CJointData::deserializable(CMemoryStream *stream, IMeshImporter *importer)
	{
		BoneRoot = stream->readChar() == 1 ? true : false;

		SID = stream->readString();
		BoneName = stream->readString();

		stream->readFloatArray(AnimationMatrix.pointer(), 16);
		stream->readFloatArray(DefaultAnimationMatrix.pointer(), 16);
		stream->readFloatArray(DefaultRelativeMatrix.pointer(), 16);

		return true;
	}

	ACTIVATOR_REGISTER(CJointData)
}