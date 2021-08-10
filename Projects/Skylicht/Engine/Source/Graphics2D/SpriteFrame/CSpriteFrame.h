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

#pragma once

namespace Skylicht
{
	class CAtlas;

	struct SImage
	{
		int ID;
		std::string Path;
		ITexture* Texture;
		CAtlas* Atlas;

		SImage()
		{
			ID = -1;
			Texture = NULL;
			Atlas = NULL;
		}
	};

	struct SModuleRect
	{
		int ID;
		std::string Name;

		float X;
		float Y;
		float W;
		float H;

		SModuleRect()
		{
			ID = -1;
			X = -1;
			Y = -1;
			W = -1;
			H = -1;
		}
	};

	struct SFrame;

	struct SModuleOffset
	{
		SModuleRect* Module;
		SFrame* Frame;

		float OffsetX;
		float OffsetY;
		float XAdvance;
		bool FlipX;
		bool FlipY;

		wchar_t Character;

		SModuleOffset();

		void getPositionBuffer(video::S3DVertex* vertices, u16* indices, int vertexOffset, const core::matrix4& mat, float scaleW = 1.0f, float scaleH = 1.0f);

		void getPositionBuffer(video::S3DVertex* vertices, u16* indices, int vertexOffset, float offsetX, float offsetY, const core::matrix4& mat, float scaleW = 1.0f, float scaleH = 1.0f);

		void getTexCoordBuffer(video::S3DVertex* vertices, float texWidth, float texHeight, float scaleW = 1.0f, float scaleH = 1.0f);

		void getColorBuffer(video::S3DVertex* vertices, const SColor& c);
	};

	struct SFrame
	{
		int ID;
		std::string Name;

		std::vector<SModuleOffset> ModuleOffset;

		SImage* Image;

		core::rectf BoudingRect;

		SFrame()
		{
			ID = -1;
		}

		inline float getWidth()
		{
			return BoudingRect.getWidth();
		}

		inline float getHeight()
		{
			return BoudingRect.getHeight();
		}
	};
}