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
#include "CSkyDomeData.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CSkyDomeData::CSkyDomeData() :
		SkyDomeTexture(NULL),
		SkyDomeColor(255, 255, 255, 255),
		HorizontalResolution(32),
		VerticalResolution(8),
		TexturePercentage(1.0f),
		SpherePercentage(2.0f),
		Radius(1.0f)
	{
		Buffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD));

		SMaterial& mat = Buffer->getMaterial();
		mat.setTexture(0, SkyDomeTexture);
		mat.ZWriteEnable = false;
		mat.MaterialType = CShaderManager::getInstance()->getShaderIDByName("TextureColor");

		generateMesh();
	}

	CSkyDomeData::~CSkyDomeData()
	{
		Buffer->drop();
	}

	void CSkyDomeData::generateMesh()
	{
		f32 azimuth;
		u32 k;

		Buffer->getVertexBuffer()->clear();
		Buffer->getIndexBuffer()->clear();

		const f32 azimuth_step = (core::PI * 2.f) / HorizontalResolution;
		if (SpherePercentage < 0.f)
			SpherePercentage = -SpherePercentage;
		if (SpherePercentage > 2.f)
			SpherePercentage = 2.f;
		const f32 elevation_step = SpherePercentage * core::HALF_PI / (f32)VerticalResolution;

		Buffer->getVertexBuffer()->reallocate((HorizontalResolution + 1) * (VerticalResolution + 1));
		Buffer->getIndexBuffer()->reallocate(3 * (2 * VerticalResolution - 1) * HorizontalResolution);

		video::S3DVertex vtx;
		vtx.Color = SkyDomeColor;
		vtx.Normal.set(0.0f, -1.f, 0.0f);

		const f32 tcV = TexturePercentage / VerticalResolution;
		for (k = 0, azimuth = 0; k <= HorizontalResolution; ++k)
		{
			f32 elevation = core::HALF_PI;
			const f32 tcU = (f32)k / (f32)HorizontalResolution;
			const f32 sinA = sinf(azimuth);
			const f32 cosA = cosf(azimuth);
			for (u32 j = 0; j <= VerticalResolution; ++j)
			{
				const f32 cosEr = Radius * cosf(elevation);
				vtx.Pos.set(cosEr*sinA, Radius*sinf(elevation), cosEr*cosA);
				vtx.TCoords.set(tcU, j*tcV);

				vtx.Normal = -vtx.Pos;
				vtx.Normal.normalize();

				Buffer->getVertexBuffer()->addVertex(&vtx);
				elevation -= elevation_step;
			}
			azimuth += azimuth_step;
		}

		for (k = 0; k < HorizontalResolution; ++k)
		{
			Buffer->getIndexBuffer()->addIndex(VerticalResolution + 2 + (VerticalResolution + 1)*k);
			Buffer->getIndexBuffer()->addIndex(1 + (VerticalResolution + 1)*k);
			Buffer->getIndexBuffer()->addIndex(0 + (VerticalResolution + 1)*k);

			for (u32 j = 1; j < VerticalResolution; ++j)
			{
				Buffer->getIndexBuffer()->addIndex(VerticalResolution + 2 + (VerticalResolution + 1)*k + j);
				Buffer->getIndexBuffer()->addIndex(1 + (VerticalResolution + 1)*k + j);
				Buffer->getIndexBuffer()->addIndex(0 + (VerticalResolution + 1)*k + j);

				Buffer->getIndexBuffer()->addIndex(VerticalResolution + 1 + (VerticalResolution + 1)*k + j);
				Buffer->getIndexBuffer()->addIndex(VerticalResolution + 2 + (VerticalResolution + 1)*k + j);
				Buffer->getIndexBuffer()->addIndex(0 + (VerticalResolution + 1)*k + j);
			}
		}
		Buffer->recalculateBoundingBox();
		Buffer->setHardwareMappingHint(scene::EHM_STATIC);
	}
}