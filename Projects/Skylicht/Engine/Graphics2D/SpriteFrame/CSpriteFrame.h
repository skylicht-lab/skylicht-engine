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

	struct SKYLICHT_API SImage
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

	struct SKYLICHT_API SModuleRect
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

	struct SKYLICHT_API SModuleOffset
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

	struct SKYLICHT_API SFrame
	{
		std::string ID;
		std::string Name;
		std::vector<SModuleOffset> ModuleOffset;
		SImage* Image;
		core::rectf BoudingRect;

		SFrame()
		{
			ID = -1;
			Image = NULL;
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

	/// @brief The object class holds data on an image, including the locations of sprites and frames on it.
	/// @ingroup Graphics2D
	/// 
	/// You can use the Skylicht-Editor to combine multiple image files into a single sprite file to optimize drawing calls.
	/// Use the CGraphics2D object or the CGUISprite, CGUIFitSprite to draw this sprite and image.
	/// 
	/// **Skylicht-Editor**
	/// 
	/// @image html Graphics2D/CSpriteFrame/create-sprite.jpg "You can create a .sprite from the Asset/Create/Sprite menu."
	/// 
	/// **Editing .sprite properties**
	/// @image html Graphics2D/CSpriteFrame/edit-sprite.jpg "Next, select the .sprite file and fill in its details." width=1200px
	/// 
	/// - Width, Height: These properties define the output image size, which is the result of the sprite atlas.
	/// - Image Folders: Input the Count, then drag Folder Items from your Assets to this property.
	/// 
	/// **Export .spritedata**
	/// 
	/// @image html Graphics2D/CSpriteFrame/export-sprite.jpg "Next, press the Export button to see the resulting .sprite and .png files." width=1200px
	/// 
	/// @code
	/// CSpriteFrame* sprite = CSpriteManager::getInstance()->loadSprite("Assets/SampleGUI/SampleGUI.spritedata");
	/// @endcode
	/// 
	/// @see CGUIFitSprite, CGUISprite
	class SKYLICHT_API CSpriteFrame : public IReferenceCounted
	{
	protected:
		std::vector<SImage*> m_images;
		std::vector<SFrame*> m_frames;
		std::vector<SModuleRect*> m_modules;

		std::map<std::string, SFrame*> m_names;
		std::map<std::string, SFrame*> m_ids;

		bool m_deleteAtlas;

		std::string m_id;
		std::string m_path;

	public:
		CSpriteFrame();

		virtual ~CSpriteFrame();

		bool load(const char* fileName);

		inline std::vector<SFrame*>& getFrames()
		{
			return m_frames;
		}

		inline SFrame* getFrameByName(const char* name)
		{
			return m_names[name];
		}

		inline SFrame* getFrameById(const char* id)
		{
			return m_ids[id];
		}

		inline const char* getId()
		{
			return m_id.c_str();
		}

		inline const char* getPath()
		{
			return m_path.c_str();
		}
	};
}