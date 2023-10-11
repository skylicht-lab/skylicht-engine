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

#pragma once

#include "SkylichtEngine.h"

#include "Editor/Space/CSpace.h"
#include "AssetManager/CAssetImporter.h"
#include "AssetManager/CAssetManager.h"

#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceExportSprite : public CSpace
		{
		protected:
			enum EImportState
			{
				None = 0,
				Init,
				Atlas,
				BeginWrite,
				WriteImage,
				WriteSprite,
				Finish,
			};

		protected:
			GUI::CProgressBar* m_progressBar;

			GUI::CLabel* m_statusText;

			EImportState m_state;

			std::string m_id;

			std::string m_path;

			std::vector<std::string> m_pngs;

			int m_width;

			int m_height;

			bool m_alpha;

			CSpriteAtlas* m_atlas;

			int m_position;

			int m_total;

			CAssetImporter importer;

		public:
			CSpaceExportSprite(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceExportSprite();

			virtual void update();

			virtual void onDestroy(GUI::CBase* base);

			bool isFinish();

			void exportSprite(const char* id, const char* path, const std::vector<std::string>& pngs, int width, int height, bool alpha);

		protected:

			void exportImage(SImage* image, const char* path);

			void exportSpriteData();
		};
	}
}