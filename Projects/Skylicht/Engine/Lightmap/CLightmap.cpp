/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CLightmap.h"
#include "TextureManager/CTextureManager.h"

#include "Entity/CEntityHandler.h"
#include "GameObject/CGameObject.h"
#include "GameObject/CContainerObject.h"
#include "RenderMesh/CRenderMesh.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CLightmap);

	CATEGORY_COMPONENT(CLightmap, "Lightmap", "Lightmapper");

	CLightmap::CLightmap() :
		m_autoLoadLightmap(true),
		m_textureOwner(false),
		m_lightmap(NULL),
		m_lightmapBeginIndex(0),
		m_applyChilds(true)
	{

	}

	CLightmap::~CLightmap()
	{
		if (m_textureOwner && m_lightmap)
		{
			CTextureManager::getInstance()->removeTexture(m_lightmap);
		}
	}

	void CLightmap::initComponent()
	{
		m_data.clear();

		CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
		if (renderMesh != NULL)
		{
			CEntityManager* entityMgr = m_gameObject->getEntityManager();
			std::vector<CRenderMeshData*>& renderData = renderMesh->getRenderers();
			for (CRenderMeshData* render : renderData)
			{
				// get entity that have render mesh data
				addLightingData(render->Entity);
			}
		}
		else
		{
			CEntityHandler* entityHandler = m_gameObject->getComponent<CEntityHandler>();
			if (entityHandler != NULL)
			{
				core::array<CEntity*>& entities = entityHandler->getEntities();
				for (u32 i = 0, n = entities.size(); i < n; i++)
					addLightingData(entities[i]);
			}
			else
			{
				// get entity that have render mesh data
				CEntity* entity = m_gameObject->getEntity();
				addLightingData(entity);
			}
		}
	}

	void CLightmap::startComponent()
	{
		initComponent();
	}

	void CLightmap::addLightingData(CEntity* entity)
	{
		// add indirect data info
		CLightmapData* data = entity->addData<CLightmapData>();
		m_data.push_back(data);
	}

	void CLightmap::updateComponent()
	{

	}

	CObjectSerializable* CLightmap::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		object->autoRelease(new CFilePathProperty(object, "lightmapShader", m_lightmapShader.c_str(), "xml"));
		object->autoRelease(new CBoolProperty(object, "autoLoadLightmap", m_autoLoadLightmap));
		object->autoRelease(new CIntProperty(object, "lightmapIndex", m_lightmapBeginIndex, 0, 32));
		object->autoRelease(new CBoolProperty(object, "applyChilds", m_applyChilds));

		CArrayTypeSerializable<CFilePathProperty>* textureArray = new CArrayTypeSerializable<CFilePathProperty>("lightmap", object);
		textureArray->OnCreateElement = [](CValueProperty* p)
			{
				CFilePathProperty* filePath = (CFilePathProperty*)p;
				filePath->Exts = CTextureManager::getTextureExts();
			};
		for (std::string& path : m_lightmapPaths)
			textureArray->autoRelease(new CFilePathProperty(textureArray, "texture", path.c_str(), CTextureManager::getTextureExts()));
		object->autoRelease(textureArray);


		CArrayTypeSerializable<CFilePathProperty>* lightDataArray = new CArrayTypeSerializable<CFilePathProperty>("lightDataExternal", object);
		lightDataArray->OnCreateElement = [](CValueProperty* p)
			{
				CFilePathProperty* filePath = (CFilePathProperty*)p;
				filePath->Exts.push_back("texarray");
			};
		for (std::string& path : m_lightDataExternal)
			lightDataArray->autoRelease(new CFilePathProperty(lightDataArray, "texture", path.c_str(), "texarray"));
		object->autoRelease(lightDataArray);

		return object;
	}

	void CLightmap::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_lightmapShader = object->get<std::string>("lightmapShader", "");
		m_autoLoadLightmap = object->get<bool>("autoLoadLightmap", true);
		m_lightmapBeginIndex = object->get<int>("lightmapIndex", 0);
		m_applyChilds = object->get<bool>("applyChilds", true);

		bool haveLightmap = false;
		CArraySerializable* textureArray = (CArraySerializable*)object->getProperty("lightmap");
		if (textureArray)
		{
			int numTextures = textureArray->getElementCount();
			haveLightmap = numTextures > 0;

			m_lightmapPaths.clear();
			for (int i = 0; i < numTextures; i++)
			{
				CFilePathProperty* file = (CFilePathProperty*)textureArray->getElement(i);
				m_lightmapPaths.push_back(file->get());
			}
		}

		CArraySerializable* lightData = (CArraySerializable*)object->getProperty("lightDataExternal");
		if (lightData)
		{
			int numTextures = lightData->getElementCount();

			m_lightDataExternal.clear();
			for (int i = 0; i < numTextures; i++)
			{
				CFilePathProperty* file = (CFilePathProperty*)lightData->getElement(i);
				m_lightDataExternal.push_back(file->get());
			}
		}

		updateLightmap(m_autoLoadLightmap);
	}

	bool CLightmap::isLightmapEmpty()
	{
		for (std::string& s : m_lightmapPaths)
		{
			if (s.empty())
				return true;
		}
		return false;
	}

	bool CLightmap::isLightmapChanged(const std::vector<std::string>& paths)
	{
		if (paths.size() != m_lightmapPaths.size())
			return true;

		for (int i = 0, n = (int)paths.size(); i < n; i++)
		{
			if (paths[i] != m_lightmapPaths[i])
			{
				return true;
			}
		}

		return false;
	}

	void CLightmap::setLightmap(ITexture* texture, int beginIndex)
	{
		if (m_textureOwner && m_lightmap)
			CTextureManager::getInstance()->removeTexture(m_lightmap);

		m_lightmap = texture;
		m_lightmapBeginIndex = beginIndex;
		m_textureOwner = false;

		updateLightmap(false);
	}

	void CLightmap::changeLightmapShader()
	{
		if (!m_lightmapShader.empty())
			changeLightmapShader(m_lightmapShader.c_str());
	}

	void CLightmap::changeLightmapShader(const char* shader)
	{
		std::vector<CRenderMesh*> allRenderers;

		if (m_applyChilds && m_gameObject->isContainer())
		{
			allRenderers = ((CContainerObject*)m_gameObject)->getComponentsInChild<CRenderMesh>(true);
		}
		else
		{
			CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
			if (renderMesh)
				allRenderers.push_back(renderMesh);
		}

		updateLightmap(true);

		for (CRenderMesh* renderMesh : allRenderers)
		{
			if (renderMesh->getGameObject()->isStatic())
			{
				renderMesh->changeShaderForAllMaterials(shader);
				if (m_lightmap)
				{
					for (int i = 0, n = renderMesh->getMaterialCount(); i < n; i++)
					{
						CMaterial* mat = renderMesh->getMaterial(i);
						mat->setUniformTexture("uTexLightmap", m_lightmap);
					}
				}
			}
		}
	}

	void CLightmap::changeDefaultShader()
	{
		std::vector<CRenderMesh*> allRenderers;

		if (m_applyChilds && m_gameObject->isContainer())
		{
			allRenderers = ((CContainerObject*)m_gameObject)->getComponentsInChild<CRenderMesh>(true);
		}
		else
		{
			CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
			if (renderMesh)
				allRenderers.push_back(renderMesh);
		}

		for (CRenderMesh* renderMesh : allRenderers)
		{
			if (renderMesh->getGameObject()->isStatic())
				renderMesh->refreshModelAndMaterial(false);
		}
	}

	void CLightmap::updateLightmap(bool loadLightmap)
	{
		// Load lightmap texture array
		if (m_lightmapPaths.size() > 0 && loadLightmap)
		{
			if (m_textureOwner && m_lightmap)
				CTextureManager::getInstance()->removeTexture(m_lightmap);

			m_lightmap = CTextureManager::getInstance()->getTextureArray(m_lightmapPaths);
			m_textureOwner = true;
		}

		for (CLightmapData* data : m_data)
		{
			data->LightmapTexture = m_lightmap;
			data->LightmapIndex = m_lightmapBeginIndex;
		}
	}
}