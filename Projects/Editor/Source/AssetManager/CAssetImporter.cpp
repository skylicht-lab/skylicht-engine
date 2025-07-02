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
#include "CAssetImporter.h"

#include <filesystem>
#include <chrono>
#include <sstream>
#include <sys/stat.h>

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

#include "TextureManager/CTextureManager.h"
#include "Material/CMaterialManager.h"
#include "MeshManager/CMeshManager.h"

#include "Editor/SpaceController/CSceneController.h"

#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CAssetImporter::CAssetImporter(std::list<SFileNode*>& listFiles) :
			m_fileID(0),
			m_deleteID(0),
			m_totalDeleted(0)
		{
			m_assetManager = CAssetManager::getInstance();

			m_files = listFiles;
			m_total = (u32)m_files.size();
			m_fileIterator = m_files.begin();
			m_fileIteratorEnd = m_files.end();

			m_deleteIterator = m_fileDeleted.begin();
			m_deleteIteratorEnd = m_fileDeleted.end();
		}

		CAssetImporter::CAssetImporter() :
			m_fileID(0),
			m_deleteID(0),
			m_totalDeleted(0)
		{
			m_assetManager = CAssetManager::getInstance();

			m_total = 0;

			m_deleteIterator = m_fileDeleted.begin();
			m_deleteIteratorEnd = m_fileDeleted.end();
		}

		CAssetImporter::~CAssetImporter()
		{

		}

		void CAssetImporter::addDeleted(std::list<std::string>& list)
		{
			m_fileDeleted.insert(m_fileDeleted.end(), list.begin(), list.end());

			m_deleteIterator = m_fileDeleted.begin();
			m_deleteIteratorEnd = m_fileDeleted.end();
			m_totalDeleted = (u32)m_fileDeleted.size();
		}

		bool CAssetImporter::load(int count)
		{
			if (m_fileIterator == m_fileIteratorEnd)
				return true;

			for (int j = 0; j < count; j++)
			{
				SFileNode* node = (*m_fileIterator);

				importPath(node);

				m_lastFile = node->Path;

				++m_fileIterator;
				++m_fileID;

				if (m_fileIterator == m_fileIteratorEnd)
				{
					// finish import
					return true;
				}
			}

			return false;
		}
		void CAssetImporter::getImportStatus(float& percent, std::string& last)
		{
			percent = m_fileID / (float)(m_total);
			last = m_lastFile;
		}

		void CAssetImporter::getDeleteStatus(float& percent, std::string& last)
		{
			percent = m_deleteID / (float)(m_totalDeleted);
			last = m_lastFile;
		}

		bool CAssetImporter::deleteAsset(int count)
		{
			if (m_deleteIterator == m_deleteIteratorEnd)
				return true;

			for (int j = 0; j < count; j++)
			{
				std::string path = (*m_deleteIterator);

				m_assetManager->deleteChildAsset(path.c_str());

				m_lastFile = path;

				++m_deleteIterator;
				++m_deleteID;

				if (m_deleteIterator == m_deleteIteratorEnd)
					return true;
			}

			return false;
		}

		bool CAssetImporter::isFinish()
		{
			if (m_fileIterator == m_fileIteratorEnd && m_deleteIterator == m_deleteIteratorEnd)
				return true;

			return false;
		}

		void CAssetImporter::add(const char* path)
		{
			std::string bundle = m_assetManager->getBundleName(path);
			m_files.push_back(m_assetManager->addFileNode(bundle, path));

			m_fileID = 0;
			m_total = (u32)m_files.size();
			m_fileIterator = m_files.begin();
			m_fileIteratorEnd = m_files.end();
		}

		void CAssetImporter::importAll()
		{
			if (*m_fileIterator == NULL || m_fileIterator == m_fileIteratorEnd)
				return;

			while (m_fileIterator != m_fileIteratorEnd)
			{
				SFileNode* node = (*m_fileIterator);

				importPath(node);

				m_lastFile = node->Path;

				++m_fileIterator;
				++m_fileID;
			}

			m_assetManager->getThumbnail()->save();
		}

		void CAssetImporter::importPath(const SFileNode* node)
		{
			std::string path = node->Path;

			if (fs::exists(path))
			{
				std::string ext = CPath::getFileNameExt(path);
				ext = CStringImp::toLower(ext);

				if (CTextureManager::isTextureExt(ext.c_str()))
				{
					std::string editorPath = "Editor/";
					if (editorPath != path.substr(0, editorPath.size()))
					{
						std::string id = m_assetManager->getGenerateMetaGUID(path.c_str());
						if (m_assetManager->getThumbnail()->updateInfo(id.c_str(), path.c_str(), node->ModifyTime))
							m_assetManager->getThumbnail()->saveThumbnailTexture(id.c_str());
					}

					CTextureManager* textureMgr = CTextureManager::getInstance();
					if (textureMgr->isTextureLoaded(path.c_str()))
					{
						// get old texture
						ITexture* oldTexture = textureMgr->getTexture(path.c_str());
						oldTexture->grab();

						// reload texture
						textureMgr->removeTexture(oldTexture);
						ITexture* newTexture = textureMgr->getTexture(path.c_str());

						// update for all materials
						CMaterialManager::getInstance()->replaceTexture(oldTexture, newTexture);

						oldTexture->drop();
					}
				}
				else if (CMeshManager::isMeshExt(ext.c_str()))
				{
					CMeshManager* meshMgr = CMeshManager::getInstance();

					if (meshMgr->isMeshLoaded(path.c_str()))
					{
						// unload mesh & reload
						meshMgr->releaseResource(path.c_str());
						CSceneController::getInstance()->doMeshChange(path.c_str());
					}
				}
				else if (CMaterialManager::isMaterialExt(ext.c_str()))
				{
					CMaterialManager* materialMgr = CMaterialManager::getInstance();

					if (materialMgr->isMaterialLoaded(path.c_str()))
					{
						// unload material & reload
						materialMgr->unloadMaterial(path.c_str());
						CSceneController::getInstance()->doMaterialChange(path.c_str());
					}
				}
			}
		}
	}
}