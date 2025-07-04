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

#pragma once

#include "CAssetManager.h"
#include "Scene/CScene.h"

#include "RenderPipeline/CShadowMapRP.h"
#include "Editor/Space/Scene/CDiffuseLightRenderPipeline.h"

namespace Skylicht
{
	namespace Editor
	{
		class CAssetImporter
		{
		protected:
			u32 m_fileID;
			u32 m_deleteID;
			u32 m_total;
			u32 m_totalDeleted;

			std::string m_lastFile;

			std::list<SFileNode*> m_files;

			std::list<SFileNode*>::iterator m_fileIterator;
			std::list<SFileNode*>::iterator m_fileIteratorEnd;

			CAssetManager* m_assetManager;

			std::list<std::string> m_fileDeleted;
			std::list<std::string>::iterator m_deleteIterator;
			std::list<std::string>::iterator m_deleteIteratorEnd;

			CScene* m_scene;
			CCamera* m_camera;

			ITexture* m_rtt;
			CShadowMapRP* m_shadowRP;
			CDiffuseLightRenderPipeline* m_rp;

		public:
			CAssetImporter(std::list<SFileNode*>& listFiles);

			CAssetImporter();

			virtual ~CAssetImporter();

			void initScene();

			bool load(int count);

			void addDeleted(std::list<std::string>& list);

			bool deleteAsset(int count);

			bool needDelete()
			{
				return m_fileDeleted.size() > 0;
			}

			bool isFinish();

			void getImportStatus(float& percent, std::string& last);

			void getDeleteStatus(float& percent, std::string& last);

			void add(const char* path);

			void importAll();

		protected:

			void importPath(const SFileNode* node);

			void saveModelThumbnail(const char* id, const char* path);
		};
	}
}