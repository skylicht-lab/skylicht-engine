/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "GUI/GUI.h"
#include "Serializable/CSerializableLoader.h"
#include "CSpriteEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/Space/Property/CSpaceProperty.h"

#include "Editor/CEditor.h"

#include "AssetManager/CAssetImporter.h"
#include "AssetManager/CAssetManager.h"

#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"

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
		CSpriteEditor::CSpriteEditor() :
			m_settings(NULL)
		{

		}

		CSpriteEditor::~CSpriteEditor()
		{
			closeGUI();
		}

		void CSpriteEditor::closeGUI()
		{
			CAssetEditor::closeGUI();
			if (m_settings)
			{
				m_settings->saveToFile();

				delete m_settings;
				m_settings = NULL;
			}
		}

		void CSpriteEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			m_settings = createGetSpriteExportSetting(path);

			GUI::CCollapsibleGroup* group = ui->addGroup("Sprite Exporter", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_settings, ui, layout);
			group->setExpand(true);

			group = ui->addGroup("Exporter", this);
			layout = ui->createBoxLayout(group);

			std::string exportPath = path;
			ui->addButton(layout, L"Export Sprite")->OnPress = [&, p = exportPath](GUI::CBase* button)
				{
					exportSprite(p.c_str());
				};

			group->setExpand(true);
		}

		void CSpriteEditor::exportSprite(const char* path)
		{
			if (m_settings == NULL)
				return;

			std::vector<std::string> folders;
			std::vector<SPNGInfo*> pngsInfo;
			std::vector<std::string> pngs;

			CArrayTypeSerializable<CFolderPathProperty>& listFolders = m_settings->ImagesFolder;
			for (int i = 0, n = listFolders.getElementCount(); i < n; i++)
			{
				CFolderPathProperty* value = dynamic_cast<CFolderPathProperty*>(listFolders.getPropertyID(i));
				folders.push_back(value->getString());
			}

			CAssetManager* assetManager = CAssetManager::getInstance();
			for (std::string& folder : folders)
			{
				std::string discovery = assetManager->getAssetFolder() + "/" + folder;
				findAllPNG(discovery.c_str(), pngsInfo);
			}

			// need sort to optimize the atlas image
			std::sort(pngsInfo.begin(), pngsInfo.end(),
				[](SPNGInfo*& a, SPNGInfo*& b)
				{
					int sa = a->Width * a->Height;
					int sb = b->Width * b->Height;
					return sa > sb;
				}
			);

			for (SPNGInfo* info : pngsInfo)
			{
				pngs.push_back(info->Path);
				delete info;
			}
			pngsInfo.clear();

			// show dialog export sprite
			// see CSpaceExportSprite
			CEditor::getInstance()->exportSprite(
				m_settings->GUID.getString(),
				path,
				pngs,
				m_settings->Width.get(),
				m_settings->Height.get(),
				m_settings->Alpha.get()
			);
		}

		void CSpriteEditor::findAllPNG(const char* path, std::vector<SPNGInfo*>& pngs)
		{
			CAssetManager* mgr = CAssetManager::getInstance();

			for (const auto& file : fs::directory_iterator(path))
			{
				std::string path = file.path().generic_u8string();

				if (file.is_directory())
				{
					findAllPNG(path.c_str(), pngs);
				}
				else
				{
					std::string ext = CStringImp::toLower(CPath::getFileNameExt(path));
					if (ext == "png")
					{
						const char* pngPath = path.c_str();
						int w, h;
						if (readWidthHeight(pngPath, w, h))
						{
							SPNGInfo* pngInfo = new SPNGInfo();
							pngInfo->Width = w;
							pngInfo->Height = h;
							pngInfo->Path = mgr->getShortPath(pngPath);
							pngs.push_back(pngInfo);
						}
					}
				}
			}
		}

		uint32_t read_big_endian_32(std::ifstream& file)
		{
			uint32_t value;
			file.read(reinterpret_cast<char*>(&value), 4);
			return ntohl(value);
		}

		bool CSpriteEditor::readWidthHeight(const char* path, int& width, int& height)
		{
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				return false;
			}

			file.seekg(8, std::ios::beg);
			file.seekg(8, std::ios::cur);

			try {
				width = read_big_endian_32(file);
			}
			catch (...) {
				return false;
			}

			try {
				height = read_big_endian_32(file);
			}
			catch (...) {
				return false;
			}

			file.close();
			return true;
		}

		CSpriteExportSettings* CSpriteEditor::createGetSpriteExportSetting(const char* path)
		{
			CSpriteExportSettings* setting = new CSpriteExportSettings();
			CSerializableLoader::loadSerializable(path, setting);
			return setting;
		}

		void CSpriteEditor::onUpdateValue(CObjectSerializable* object)
		{
			m_settings->saveToFile();
		}

		ASSET_EDITOR_REGISTER(CSpriteEditor, sprite);
	}
}