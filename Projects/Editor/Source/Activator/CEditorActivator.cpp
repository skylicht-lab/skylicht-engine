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
#include "CEditorActivator.h"

namespace Skylicht
{
	namespace Editor
	{
		CEditorActivator::CEditorActivator()
		{

		}

		CEditorActivator::~CEditorActivator()
		{
			releaseAllInstance();
		}

		bool CEditorActivator::registerEditor(const char* componentType, ActivatorCreateEditor func)
		{
			std::map<std::string, int>::iterator i = m_mapComponent.find(componentType);
			if (i != m_mapComponent.end())
				return false;

			m_mapComponent[componentType] = (int)m_factoryFunc.size();
			m_factoryFunc.push_back(func);
			return true;
		}

		bool CEditorActivator::registerAssetEditor(const char* ext, ActivatorCreateAssetEditor func)
		{
			std::map<std::string, int>::iterator i = m_mapAsset.find(ext);
			if (i != m_mapAsset.end())
				return false;

			m_mapAsset[ext] = (int)m_factoryAssetFunc.size();
			m_factoryAssetFunc.push_back(func);
			return true;
		}

		bool CEditorActivator::registerEntityDataEditor(const char* dataType, ActivatorCreateEntityDataEditor func)
		{
			std::map<std::string, int>::iterator i = m_mapEntityData.find(dataType);
			if (i != m_mapEntityData.end())
				return false;

			m_mapEntityData[dataType] = (int)m_factoryEntityDataFunc.size();
			m_factoryEntityDataFunc.push_back(func);
			return true;
		}

		bool CEditorActivator::registerGUIEditor(const char* dataType, ActivatorCreateGUIEditor func)
		{
			std::map<std::string, int>::iterator i = m_mapGUI.find(dataType);
			if (i != m_mapGUI.end())
				return false;

			m_mapGUI[dataType] = (int)m_factoryGUIFunc.size();
			m_factoryGUIFunc.push_back(func);
			return true;
		}

		CComponentEditor* CEditorActivator::getEditorInstance(const char* componentType)
		{
			// find componentType
			std::map<std::string, int>::iterator i = m_mapComponent.find(componentType);
			if (i == m_mapComponent.end())
				return NULL;

			// re-used
			std::map<std::string, CComponentEditor*>::iterator j = m_mapInstance.find(componentType);
			if (j != m_mapInstance.end() && (*j).second != NULL)
				return (*j).second;

			// create and cache to re-used
			int id = (*i).second;
			CComponentEditor* result = m_factoryFunc[id]();
			m_mapInstance[componentType] = result;
			return result;
		}

		CAssetEditor* CEditorActivator::getAssetEditorInstance(const char* ext)
		{
			// find componentType
			std::map<std::string, int>::iterator i = m_mapAsset.find(ext);
			if (i == m_mapAsset.end())
				return NULL;

			// re-used
			std::map<std::string, CAssetEditor*>::iterator j = m_mapAssetInstance.find(ext);
			if (j != m_mapAssetInstance.end() && (*j).second != NULL)
				return (*j).second;

			// create and cache to re-used
			int id = (*i).second;
			CAssetEditor* result = m_factoryAssetFunc[id]();
			m_mapAssetInstance[ext] = result;
			return result;
		}

		CEntityDataEditor* CEditorActivator::getEntityDataEditorInstance(const char* dataType)
		{
			// find dataType
			std::map<std::string, int>::iterator i = m_mapEntityData.find(dataType);
			if (i == m_mapEntityData.end())
				return NULL;

			// re-used
			std::map<std::string, CEntityDataEditor*>::iterator j = m_mapEntityDataInstance.find(dataType);
			if (j != m_mapEntityDataInstance.end() && (*j).second != NULL)
				return (*j).second;

			// create and cache to re-used
			int id = (*i).second;
			CEntityDataEditor* result = m_factoryEntityDataFunc[id]();
			m_mapEntityDataInstance[dataType] = result;
			return result;
		}

		CGUIEditor* CEditorActivator::getGUIEditorInstance(const char* dataType)
		{
			// find dataType
			std::map<std::string, int>::iterator i = m_mapGUI.find(dataType);
			if (i == m_mapGUI.end())
				return NULL;

			// re-used
			std::map<std::string, CGUIEditor*>::iterator j = m_mapGUIInstance.find(dataType);
			if (j != m_mapGUIInstance.end() && (*j).second != NULL)
				return (*j).second;

			// create and cache to re-used
			int id = (*i).second;
			CGUIEditor* result = m_factoryGUIFunc[id]();
			m_mapGUIInstance[dataType] = result;
			return result;
		}

		void CEditorActivator::releaseAllInstance()
		{
			// 1
			for (auto i : m_mapInstance)
			{
				delete i.second;
			}
			m_mapInstance.clear();
			m_mapComponent.clear();

			// 2
			for (auto i : m_mapAssetInstance)
			{
				delete i.second;
			}
			m_mapAssetInstance.clear();
			m_mapAsset.clear();

			// 3
			for (auto i : m_mapEntityDataInstance)
			{
				delete i.second;
			}
			m_mapEntityDataInstance.clear();
			m_mapEntityData.clear();

			// 4
			for (auto i : m_mapGUIInstance)
			{
				delete i.second;
			}
			m_mapGUIInstance.clear();
			m_mapGUI.clear();
		}
	}
}