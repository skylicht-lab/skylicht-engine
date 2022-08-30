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
#include "CSpriteFrame.h"

namespace Skylicht
{
	SModuleOffset::SModuleOffset()
	{
		OffsetX = 0;
		OffsetY = 0;
		FlipX = false;
		FlipY = false;

		XAdvance = 0.0f;
		Character = 0;

		Frame = NULL;
		Module = NULL;
	}

	void SModuleOffset::getPositionBuffer(video::S3DVertex* vertices, u16* indices, int vertexOffset, const core::matrix4& mat, float scaleW, float scaleH)
	{
		float x1 = (float)OffsetX;
		float y1 = (float)OffsetY;
		float x2 = (float)(OffsetX + Module->W * scaleW);
		float y2 = (float)(OffsetY + Module->H * scaleH);

		vertices[0].Pos.X = x1;
		vertices[0].Pos.Y = y1;
		vertices[0].Pos.Z = 0.0f;
		mat.transformVect(vertices[0].Pos);

		vertices[1].Pos.X = x2;
		vertices[1].Pos.Y = y1;
		vertices[1].Pos.Z = 0.0f;
		mat.transformVect(vertices[1].Pos);

		vertices[2].Pos.X = x2;
		vertices[2].Pos.Y = y2;
		vertices[2].Pos.Z = 0.0f;
		mat.transformVect(vertices[2].Pos);

		vertices[3].Pos.X = x1;
		vertices[3].Pos.Y = y2;
		vertices[3].Pos.Z = 0.0f;
		mat.transformVect(vertices[3].Pos);

		indices[0] = vertexOffset;
		indices[1] = vertexOffset + 1;
		indices[2] = vertexOffset + 2;
		indices[3] = vertexOffset;
		indices[4] = vertexOffset + 2;
		indices[5] = vertexOffset + 3;
	}

	void SModuleOffset::getPositionBuffer(video::S3DVertex* vertices, u16* indices, int vertexOffset, float offsetX, float offsetY, const core::matrix4& mat, float scaleW, float scaleH)
	{
		float x1 = (float)OffsetX + offsetX;
		float y1 = (float)OffsetY + offsetY;
		float x2 = (float)(OffsetX + offsetX + Module->W * scaleW);
		float y2 = (float)(OffsetY + offsetY + Module->H * scaleH);

		vertices[0].Pos.X = x1;
		vertices[0].Pos.Y = y1;
		vertices[0].Pos.Z = 0.0f;
		mat.transformVect(vertices[0].Pos);

		vertices[1].Pos.X = x2;
		vertices[1].Pos.Y = y1;
		vertices[1].Pos.Z = 0.0f;
		mat.transformVect(vertices[1].Pos);

		vertices[2].Pos.X = x2;
		vertices[2].Pos.Y = y2;
		vertices[2].Pos.Z = 0.0f;
		mat.transformVect(vertices[2].Pos);

		vertices[3].Pos.X = x1;
		vertices[3].Pos.Y = y2;
		vertices[3].Pos.Z = 0.0f;
		mat.transformVect(vertices[3].Pos);

		indices[0] = vertexOffset;
		indices[1] = vertexOffset + 1;
		indices[2] = vertexOffset + 2;
		indices[3] = vertexOffset;
		indices[4] = vertexOffset + 2;
		indices[5] = vertexOffset + 3;
	}

	void SModuleOffset::getTexCoordBuffer(video::S3DVertex* vertices, float texWidth, float texHeight, float scaleW, float scaleH)
	{
		float x1 = Module->X / texWidth;
		float y1 = Module->Y / texHeight;
		float x2 = (Module->X + Module->W * scaleW) / texWidth;
		float y2 = (Module->Y + Module->H * scaleH) / texHeight;

		if (FlipX)
			core::swap<float, float>(x1, x2);
		if (FlipY)
			core::swap<float, float>(y1, y2);

		vertices[0].TCoords.X = x1;
		vertices[0].TCoords.Y = y1;

		vertices[1].TCoords.X = x2;
		vertices[1].TCoords.Y = y1;

		vertices[2].TCoords.X = x2;
		vertices[2].TCoords.Y = y2;

		vertices[3].TCoords.X = x1;
		vertices[3].TCoords.Y = y2;
	}

	void SModuleOffset::getColorBuffer(video::S3DVertex* vertices, const SColor& c)
	{
		vertices[0].Color = c;
		vertices[1].Color = c;
		vertices[2].Color = c;
		vertices[3].Color = c;
	}
}