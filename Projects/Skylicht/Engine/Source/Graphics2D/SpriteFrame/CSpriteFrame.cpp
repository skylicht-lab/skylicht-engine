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
#include "Graphics2D/Atlas/CAtlas.h"
#include "TextureManager/CTextureManager.h"

#include "Utils/CStringImp.h"

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

	CSpriteFrame::CSpriteFrame() :
		m_deleteAtlas(false)
	{

	}

	CSpriteFrame::~CSpriteFrame()
	{
		CTextureManager* textureMgr = CTextureManager::getInstance();
		if (textureMgr != NULL)
		{
			for (SImage* img : m_images)
			{
				if (img->Atlas == NULL)
				{
					if (img->Texture != NULL)
						textureMgr->removeTexture(img->Texture);
				}
				else
				{
					if (m_deleteAtlas)
					{
						delete img->Atlas;
					}
				}

				delete img;
			}
		}

		for (SFrame* f : m_frames)
			delete f;

		for (SModuleRect* m : m_modules)
			delete m;

		m_frames.clear();
		m_modules.clear();
		m_images.clear();
		m_names.clear();
	}

	bool CSpriteFrame::load(const char* fileName)
	{
		io::IXMLReader* xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(fileName);
		if (xmlReader == NULL)
			return false;

		char text[512];

		int readState = -1;

		SFrame* currentFrame;

		while (xmlReader->read())
		{
			switch (xmlReader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"page")
				{
					m_images.push_back(new SImage());
					SImage* img = m_images.back();

					const wchar_t* id = xmlReader->getAttributeValue(L"id");
					CStringImp::convertUnicodeToUTF8(id, text);
					img->ID = atoi(text);

					const wchar_t* file = xmlReader->getAttributeValue(L"file");
					CStringImp::convertUnicodeToUTF8(file, text);

					char folderPath[128];
					CStringImp::getFolderPath(folderPath, fileName);
					img->Path = folderPath;
					img->Path += "/";
					img->Path += text;
					img->Texture = CTextureManager::getInstance()->getTexture(img->Path.c_str());
				}
				else if (nodeName == L"module")
				{
					// add module
					m_modules.push_back(new SModuleRect());
					SModuleRect* module = m_modules.back();

					// parse id
					const wchar_t* id = xmlReader->getAttributeValue(L"id");
					CStringImp::convertUnicodeToUTF8(id, text);
					module->ID = atoi(text);

					// read module rect
					const wchar_t* x = xmlReader->getAttributeValue(L"x");
					CStringImp::convertUnicodeToUTF8(x, text);
					module->X = core::fast_atof(text);

					const wchar_t* y = xmlReader->getAttributeValue(L"y");
					CStringImp::convertUnicodeToUTF8(y, text);
					module->Y = core::fast_atof(text);

					const wchar_t* w = xmlReader->getAttributeValue(L"w");
					CStringImp::convertUnicodeToUTF8(w, text);
					module->W = core::fast_atof(text);

					const wchar_t* h = xmlReader->getAttributeValue(L"h");
					CStringImp::convertUnicodeToUTF8(h, text);
					module->H = core::fast_atof(text);
				}
				else if (nodeName == L"frame")
				{
					// add frame
					m_frames.push_back(new SFrame());
					SFrame* frame = m_frames.back();

					// parse id
					const wchar_t* id = xmlReader->getAttributeValue(L"id");
					CStringImp::convertUnicodeToUTF8(id, text);
					frame->ID = atoi(text);

					// frame name
					const wchar_t* name = xmlReader->getAttributeValue(L"name");
					CStringImp::convertUnicodeToUTF8(name, text);
					frame->Name = text;

					// read img
					const wchar_t* page = xmlReader->getAttributeValue(L"page");
					CStringImp::convertUnicodeToUTF8(page, text);
					int imageID = atoi(text);
					frame->Image = m_images[imageID];

					currentFrame = frame;
				}
				else if (nodeName == L"module")
				{
					if (currentFrame)
					{
						// module offset
						currentFrame->ModuleOffset.push_back(SModuleOffset());
						SModuleOffset& moduleOffset = currentFrame->ModuleOffset.back();

						// map frame to module
						moduleOffset.Frame = currentFrame;

						// parse id
						const wchar_t* id = xmlReader->getAttributeValue(L"id");
						CStringImp::convertUnicodeToUTF8(id, text);
						int moduleID = atoi(text);
						moduleOffset.Module = m_modules[moduleID];

						// module offset x
						const wchar_t* x = xmlReader->getAttributeValue(L"x");
						CStringImp::convertUnicodeToUTF8(x, text);
						moduleOffset.OffsetX = core::fast_atof(text);

						// module offset y
						const wchar_t* y = xmlReader->getAttributeValue(L"y");
						CStringImp::convertUnicodeToUTF8(y, text);
						moduleOffset.OffsetY = core::fast_atof(text);

						// module flip x
						const wchar_t* flipX = xmlReader->getAttributeValue(L"flipX");
						CStringImp::convertUnicodeToUTF8(flipX, text);
						moduleOffset.FlipX = atoi(text) == 0 ? false : true;

						// module flip y
						const wchar_t* flipY = xmlReader->getAttributeValue(L"flipY");
						CStringImp::convertUnicodeToUTF8(flipY, text);
						moduleOffset.FlipX = atoi(text) == 0 ? false : true;

						// bounding rect
						core::rectf& r = currentFrame->BoudingRect;
						if (currentFrame->ModuleOffset.size() == 1)
						{
							r.UpperLeftCorner.X = moduleOffset.OffsetX;
							r.UpperLeftCorner.Y = moduleOffset.OffsetY;
							r.LowerRightCorner.X = moduleOffset.OffsetX + moduleOffset.Module->W;
							r.LowerRightCorner.Y = moduleOffset.OffsetY + moduleOffset.Module->H;
						}
						else
						{
							r.UpperLeftCorner.X = core::min_(r.UpperLeftCorner.X, moduleOffset.OffsetX);
							r.UpperLeftCorner.Y = core::min_(r.UpperLeftCorner.Y, moduleOffset.OffsetY);
							r.LowerRightCorner.X = core::max_(r.LowerRightCorner.X, moduleOffset.OffsetX + moduleOffset.Module->W);
							r.LowerRightCorner.Y = core::max_(r.LowerRightCorner.Y, moduleOffset.OffsetY + moduleOffset.Module->H);
						}
					}
				}
			}
			break;
			case io::EXN_TEXT:
			{
				readState = -1;
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"frame")
				{
					currentFrame = NULL;
				}
				break;
			}
			break;
			default:
				break;
			}
		};

		xmlReader->drop();

		for (SFrame* frame : m_frames)
			m_names[frame->Name] = frame;

		return true;
	}
}