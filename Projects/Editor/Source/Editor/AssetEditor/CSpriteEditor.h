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

#include "CAssetEditor.h"
#include "ResourceSettings/SpriteExportSettings.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpriteEditor : public CAssetEditor
		{
		protected:
			SpriteExportSettings* m_settings;

		public:
			CSpriteEditor();

			virtual ~CSpriteEditor();

			virtual void closeGUI();

			virtual void initGUI(const char* path, CSpaceProperty* ui);

			virtual void onUpdateValue(CObjectSerializable* object);

			SpriteExportSettings* createGetSpriteExportSetting(const char* path);

			void exportSprite(const char* path);

			void findAllPNG(const char* path, std::vector<std::string>& pngs);

			DECLARE_GETTYPENAME(CSpriteEditor);
		};
	}
}