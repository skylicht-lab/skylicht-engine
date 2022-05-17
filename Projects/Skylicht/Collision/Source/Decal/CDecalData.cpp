/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CDecalData.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CDecalRenderData);

	CDecalRenderData::CDecalRenderData() :
		Texture(NULL)
	{
		Material = new CMaterial("DecalRenderer", "BuiltIn/Shader/Basic/TextureColorAlpha.xml");
	}

	CDecalRenderData::~CDecalRenderData()
	{
		delete Material;
	}

	IMPLEMENT_DATA_TYPE_INDEX(CDecalData);

	CDecalData::CDecalData() :
		TextureRotation(0.0f),
		LifeTime(0.0f),
		Distance(0.0f),
		RenderData(NULL),
		Collision(NULL),
		Change(true)
	{
		MeshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD));
		MeshBuffer->setHardwareMappingHint(scene::EHM_STREAM);

		SMaterial& mat = MeshBuffer->getMaterial();
		mat.TextureLayer[0].TextureWrapU = E_TEXTURE_CLAMP::ETC_CLAMP_TO_EDGE;
		mat.TextureLayer[0].TextureWrapV = E_TEXTURE_CLAMP::ETC_CLAMP_TO_EDGE;
	}

	CDecalData::~CDecalData()
	{
		MeshBuffer->drop();
	}
}