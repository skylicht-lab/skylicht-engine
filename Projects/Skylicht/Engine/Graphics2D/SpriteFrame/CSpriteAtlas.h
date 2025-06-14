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

#include "CSpriteFrame.h"

namespace Skylicht
{
	/// @brief The object class holds data on an image, including the locations of sprites and frames on it.
	/// @ingroup Graphics2D
	/// 
	/// @code
	/// CSpriteAtlas* sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 1024, 1024);
	/// SFrame* btnYellowBackground = sprite->addFrame("btn_yellow.png", "LuckyDraw/btn_yellow.png");
	/// @endcode
	/// Then, use the CGraphics2D object or the CGUISprite, CGUIFitSprite to draw this sprite and image.
	class SKYLICHT_API CSpriteAtlas : public CSpriteFrame
	{
	protected:
		int m_width;
		int m_height;
		ECOLOR_FORMAT m_fmt;

	protected:
		SImage* addEmptyAtlas();

	public:
		CSpriteAtlas(ECOLOR_FORMAT format, int width, int height);

		virtual ~CSpriteAtlas();

		SFrame* addFrame(const char* name, const char* path, const char* frameId = NULL);

		SImage* createAtlasRect(int w, int h, core::recti& outRegion);

		void updateTexture();

		inline std::vector<SModuleRect*>& getModules()
		{
			return m_modules;
		}

		inline std::vector<SFrame*>& getFrames()
		{
			return m_frames;
		}

		inline std::vector<SImage*>& getImages()
		{
			return m_images;
		}

		inline int getWidth()
		{
			return m_width;
		}

		inline int getHeight()
		{
			return m_height;
		}
	};
}