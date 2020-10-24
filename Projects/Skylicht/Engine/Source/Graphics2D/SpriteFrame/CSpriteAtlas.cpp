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
#include "CSpriteAtlas.h"
#include "Graphics2D/Atlas/CAtlas.h"

namespace Skylicht
{
	CSpriteAtlas::CSpriteAtlas(ECOLOR_FORMAT format, int width, int height) :
		m_width(width),
		m_height(height),
		m_fmt(format)
	{
		addEmptyAtlas();
	}

	CSpriteAtlas::~CSpriteAtlas()
	{
		for (SFrame *&f : m_frames)
			delete f;
		m_frames.clear();

		for (SModuleRect *&m : m_modules)
			delete m;
		m_modules.clear();

		for (SImage *&a : m_images)
		{
			delete a->Atlas;
			delete a;
		}
		m_images.clear();
		m_names.clear();
	}

	SImage* CSpriteAtlas::createAtlasRect(int w, int h, core::recti& outRegion)
	{
		SImage *image = NULL;
		core::recti r;

		for (SImage *&a : m_images)
		{
			r = a->Atlas->createRect(w, h);
			if (r.getWidth() != 0 && r.getHeight() != 0)
			{
				image = a;
				break;
			}
		}

		if (image == NULL)
		{
			image = addEmptyAtlas();
			r = image->Atlas->createRect(w, h);
			if (r.getWidth() == 0 || r.getHeight() == 0)
			{
				return NULL;
			}
		}

		outRegion = r;
		return image;
	}

	SFrame* CSpriteAtlas::addFrame(const char *name, const char *path)
	{
		IImage *img = getVideoDriver()->createImageFromFile(path);
		SFrame *frame = NULL;

		if (img != NULL)
		{
			// Need swap Png RBG -> RBG on DX11
			video::E_DRIVER_TYPE driverType = getVideoDriver()->getDriverType();

			if (driverType == video::EDT_DIRECT3D11 ||
				driverType == video::EDT_OPENGLES)
			{
				img->swapBG();
			}

			int w = img->getDimension().Width;
			int h = img->getDimension().Height;

			int atlasW = w + 2;
			int atlasH = h + 2;

			SImage *image = NULL;
			core::recti r;

			for (SImage *&a : m_images)
			{
				r = a->Atlas->createRect(atlasW, atlasH);
				if (r.getWidth() != 0 && r.getHeight() != 0)
				{
					image = a;
					break;
				}
			}

			if (image == NULL)
			{
				image = addEmptyAtlas();
				r = image->Atlas->createRect(atlasW, atlasH);
				if (r.getWidth() == 0 || r.getHeight() == 0)
				{
					img->drop();
					return NULL;
				}
			}

			// atlas image
			image->Atlas->bitBltImage(img, r.UpperLeftCorner.X, r.UpperLeftCorner.Y);

			// create frame			
			frame = new SFrame();
			frame->Image = image;
			frame->BoudingRect.UpperLeftCorner.set((f32)r.UpperLeftCorner.X, (f32)r.UpperLeftCorner.Y);
			frame->BoudingRect.LowerRightCorner.set((f32)r.LowerRightCorner.X, (f32)r.LowerRightCorner.Y);
			frame->Name = name;
			frame->ID = (int)m_frames.size();

			// create module
			SModuleRect *module = new SModuleRect();
			module->X = (f32)r.UpperLeftCorner.X;
			module->Y = (f32)r.UpperLeftCorner.Y;
			module->W = (f32)w;
			module->H = (f32)h;
			module->ID = (int)m_modules.size();
			m_modules.push_back(module);

			// frame & module with offset
			frame->ModuleOffset.push_back(SModuleOffset());
			SModuleOffset& offset = frame->ModuleOffset.back();
			offset.Frame = frame;
			offset.Module = module;
			offset.OffsetX = 0;
			offset.OffsetY = 0;

			char log[512];
			sprintf(log, "[AtlasFrame] add %s - %dx%d", name, w, h);
			os::Printer::log(log);

			m_frames.push_back(frame);
			img->drop();

			m_names[name] = frame;
		}

		return frame;
	}

	SFrame* CSpriteAtlas::getFrame(const char *name)
	{
		std::map<std::string, SFrame*>::iterator it = m_names.find(name);

		if (it == m_names.end())
			return NULL;

		return it->second;
	}

	void CSpriteAtlas::updateTexture()
	{
		for (SImage *&a : m_images)
		{
			a->Atlas->updateTexture();
			/*
			static int test = 0;
			char name[64];
			sprintf(name, "C:\\SVN\\test_%d.png", test++);
			getVideoDriver()->writeImageToFile(a->Atlas->getImage(), name);
			*/
		}
	}

	SImage* CSpriteAtlas::addEmptyAtlas()
	{
		SImage *image = new SImage();
		image->Atlas = new CAtlas(m_fmt, m_width, m_height);
		image->Texture = image->Atlas->getTexture();

		image->ID = (int)m_images.size();

		m_images.push_back(image);
		return image;
	}
}
