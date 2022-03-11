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

#include "Utils/CGameSingleton.h"
#include "Editor/Components/CComponentEditor.h"
#include "Editor/EntityData/CEntityDataEditor.h"
#include "Editor/AssetEditor/CAssetEditor.h"


namespace Skylicht
{
	namespace Editor
	{
		typedef CComponentEditor* (*ActivatorCreateEditor)();
		typedef CAssetEditor* (*ActivatorCreateAssetEditor)();
		typedef CEntityDataEditor* (*ActivatorCreateEntityDataEditor)();

#define EDITOR_REGISTER(type, componentType)  \
		CComponentEditor* type##componentType##CreateFunc() { return new type(); } \
		bool type##componentType##Activator = CEditorActivator::createGetInstance()->registerEditor(#componentType, &type##componentType##CreateFunc);

#define ASSET_EDITOR_REGISTER(type, ext)  \
		CAssetEditor* type##_ext_##CreateFunc() { return new type(); } \
		bool type##_ext_##Activator = CEditorActivator::createGetInstance()->registerAssetEditor(#ext, &type##_ext_##CreateFunc);

#define ENTITYDATA_EDITOR_REGISTER(type, ext)  \
		CEntityDataEditor* type##_ext_##CreateFunc() { return new type(); } \
		bool type##_ext_##Activator = CEditorActivator::createGetInstance()->registerEntityDataEditor(#ext, &type##_ext_##CreateFunc);

		class CEditorActivator : public CGameSingleton<CEditorActivator>
		{
		protected:
			std::vector<ActivatorCreateEditor> m_factoryFunc;
			std::vector<ActivatorCreateAssetEditor> m_factoryAssetFunc;
			std::vector<ActivatorCreateEntityDataEditor> m_factoryEntityDataFunc;

			std::map<std::string, int> m_mapComponent;
			std::map<std::string, CComponentEditor*> m_mapInstance;

			std::map<std::string, int> m_mapAsset;
			std::map<std::string, CAssetEditor*> m_mapAssetInstance;

			std::map<std::string, int> m_mapEntityData;
			std::map<std::string, CEntityDataEditor*> m_mapEntityDataInstance;

		public:
			CEditorActivator();

			virtual ~CEditorActivator();

			bool registerEditor(const char* componentType, ActivatorCreateEditor func);

			bool registerAssetEditor(const char* ext, ActivatorCreateAssetEditor func);

			bool registerEntityDataEditor(const char* dataType, ActivatorCreateEntityDataEditor func);

			CComponentEditor* getEditorInstance(const char* componentType);

			CAssetEditor* getAssetEditorInstance(const char* ext);

			CEntityDataEditor* getEntityDataEditorInstance(const char* dataType);

			void releaseAllInstance();
		};
	}
}