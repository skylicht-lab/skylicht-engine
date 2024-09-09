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
#include "RenderMesh/CRenderMesh.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CLightmap);

	CATEGORY_COMPONENT(CLightmap, "Lightmap", "Lightmap");

	CLightmap::CLightmap() :
		m_internalLightmap(false),
		m_lightmap(NULL),
		m_lightmapBeginIndex(0)
	{

	}

	CLightmap::~CLightmap()
	{
		if (m_internalLightmap && m_lightmap)
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
		return object;
	}

	void CLightmap::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);
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
		if (m_internalLightmap && m_lightmap)
			CTextureManager::getInstance()->removeTexture(m_lightmap);

		m_lightmap = texture;
		m_lightmapBeginIndex = beginIndex;
		m_internalLightmap = false;

		updateLightmap(false);
	}

	void CLightmap::updateLightmap(bool loadLightmap)
	{
		// Load lightmap texture array
		if (m_lightmapPaths.size() > 0 && loadLightmap)
		{
			if (m_internalLightmap && m_lightmap)
				CTextureManager::getInstance()->removeTexture(m_lightmap);

			m_lightmap = CTextureManager::getInstance()->getTextureArray(m_lightmapPaths);
			m_internalLightmap = true;
		}

		for (CLightmapData* data : m_data)
		{
			data->LightmapTexture = m_lightmap;
			data->LightmapIndex = m_lightmapBeginIndex;
		}
	}
}