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
#include "GUI/CGUIContext.h"
#include "CSpaceExportSprite.h"
#include "AssetManager/CAssetManager.h"
#include "Editor/CEditor.h"

#include <filesystem>
#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CSpaceExportSprite::CSpaceExportSprite(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_progressBar(NULL),
			m_statusText(NULL),
			m_width(2048),
			m_height(2048),
			m_alpha(true),
			m_atlas(NULL),
			m_position(0),
			m_total(0)
		{
			m_progressBar = new GUI::CProgressBar(window);
			m_progressBar->dock(GUI::EPosition::Top);
			m_progressBar->setMargin(GUI::SMargin(14.0f, 14.0, 14.0, 0.0f));

			m_statusText = new GUI::CLabel(window);
			m_statusText->dock(GUI::EPosition::Fill);
			m_statusText->setMargin(GUI::SMargin(14.0f, 5.0, 14.0, 0.0f));
			m_statusText->setWrapMultiline(true);
			m_statusText->setString(L"Exporting...");
		}

		CSpaceExportSprite::~CSpaceExportSprite()
		{
			if (m_atlas)
				delete m_atlas;
		}

		void CSpaceExportSprite::update()
		{
			if (m_state == Init)
			{
				m_atlas = new CSpriteAtlas(
					m_alpha ? ECF_A8R8G8B8 : ECF_R8G8B8,
					m_width,
					m_height);

				m_position = 0;
				m_total = (int)m_pngs.size();

				if (m_position >= m_total)
					m_state = Finish;
				else
					m_state = Atlas;
			}
			else if (m_state == Atlas)
			{
				m_statusText->setString(L"Atlas...");

				std::string png = m_pngs[m_position++];
				std::string name = CPath::getFileNameNoExt(png);
				m_atlas->addFrame(name.c_str(), png.c_str());

				float percent = m_position / (float)m_total;
				m_progressBar->setPercent(percent);

				if (m_position >= m_total)
				{
					m_position = 0;
					m_total = (int)m_atlas->getImages().size();

					m_state = BeginWrite;
					m_statusText->setString(L"Update texture...");
				}
			}
			else if (m_state == BeginWrite)
			{
				m_atlas->updateTexture();

				m_state = WriteImage;
				m_statusText->setString(L"Write...");
			}
			else if (m_state == WriteImage)
			{
				CAssetManager* assetManager = CAssetManager::getInstance();

				std::string folderPath = assetManager->getAssetFolder() + "/" + CPath::getFolderPath(m_path);
				std::string exportName = CPath::getFileNameNoExt(m_path);

				std::vector<SImage*>& allImages = m_atlas->getImages();

				char targetPath[64];
				if (allImages.size() > 1)
				{
					sprintf(targetPath, "%s%s_%d.png",
						folderPath.c_str(),
						exportName.c_str(),
						m_position);
				}
				else
				{
					sprintf(targetPath, "%s%s.png", folderPath.c_str(), exportName.c_str());
				}

				exportImage(allImages[m_position], targetPath);

				if (++m_position >= m_total)
				{
					m_state = WriteSprite;
					m_statusText->setString(L"Finish");
				}

				m_progressBar->setPercent(1.0f);
			}
			else if (m_state == WriteSprite)
			{
				exportSpriteData();
				m_state = Finish;
			}
			else if (m_state == Finish)
			{
				// done!
				importer.importAll();
				CEditor::getInstance()->refresh();
			}

			CSpace::update();
		}

		void CSpaceExportSprite::exportImage(SImage* image, const char* path)
		{
			CAssetManager* assetManager = CAssetManager::getInstance();

			IImage* img = image->Atlas->getImage();
			if (getVideoDriver()->getDriverType() == EDT_DIRECT3D11)
				img->swapBG();

			getVideoDriver()->writeImageToFile(img, path);

			if (getVideoDriver()->getDriverType() == EDT_DIRECT3D11)
				img->swapBG();

			image->Path = assetManager->getShortPath(path);

			importer.add(image->Path.c_str());
		}

		void CSpaceExportSprite::exportSpriteData()
		{
			std::vector<SImage*>& images = m_atlas->getImages();
			std::vector<SModuleRect*>& modules = m_atlas->getModules();
			std::vector<SFrame*>& frames = m_atlas->getFrames();

			CAssetManager* assetManager = CAssetManager::getInstance();
			std::string folderPath = assetManager->getAssetFolder() + "/" + CPath::getFolderPath(m_path);
			std::string exportName = CPath::getFileNameNoExt(m_path);

			char targetPath[64];
			sprintf(targetPath, "%s%s.spritedata", folderPath.c_str(), exportName.c_str());

			io::IWriteFile* writeFile = getIrrlichtDevice()->getFileSystem()->createAndWriteFile(targetPath);
			if (writeFile == NULL)
				return;

			std::string buffer;

			char data[1024];

			buffer += "<!-- File generated by function CSpaceExportSprite::exportSpriteData -->\n";
			buffer += "<sprite>\n";

			// write image
			buffer += "\t<images>\n";
			for (SImage* image : images)
			{
				sprintf(data, "\t\t<page id=\"%d\" file=\"%s\"/>\n", image->ID, image->Path.c_str());
				buffer += data;
			}
			buffer += "\t</images>\n";

			// write modules
			buffer += "\t<modules>\n";
			for (SModuleRect* module : modules)
			{
				sprintf(data, "\t\t<module id=\"%d\" x=\"%f\" y=\"%f\" w=\"%f\" h=\"%f\"/>\n",
					module->ID,
					module->X,
					module->Y,
					module->W,
					module->H);
				buffer += data;
			}
			buffer += "\t</modules>\n";

			// write frames
			buffer += "\t<frames>\n";
			for (SFrame* frame : frames)
			{
				sprintf(data, "\t\t<frame id=\"%d\" name=\"%s\" page=\"%d\">\n",
					frame->ID,
					frame->Name.c_str(),
					frame->Image->ID);
				buffer += data;

				for (SModuleOffset& module : frame->ModuleOffset)
				{
					sprintf(data, "\t\t\t<module id=\"%d\" x=\"%f\" y=\"%f\" flipX=\"%d\" flipY=\"%d\"/>\n",
						module.Module->ID,
						module.OffsetX,
						module.OffsetY,
						module.FlipX ? 1 : 0,
						module.FlipY ? 1 : 0);
					buffer += data;
				}

				buffer += "\t\t</frame>\n";
			}
			buffer += "\t</frames>\n";

			buffer += "</sprite>";
			writeFile->write(buffer.c_str(), (u32)buffer.size());
			writeFile->drop();

			importer.add(assetManager->getShortPath(targetPath).c_str());
		}

		void CSpaceExportSprite::onDestroy(GUI::CBase* base)
		{
			CSpace::onDestroy(base);
		}

		bool CSpaceExportSprite::isFinish()
		{
			return m_state == Finish;
		}

		void CSpaceExportSprite::exportSprite(const char* path, const std::vector<std::string>& pngs, int width, int height, bool alpha)
		{
			m_path = path;
			m_pngs = pngs;
			m_width = width;
			m_height = height;
			m_alpha = alpha;

			m_progressBar->setPercent(0.0f);

			m_state = Init;
		}
	}
}