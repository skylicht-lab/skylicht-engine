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
#include "CIndirectLighting.h"
#include "TextureManager/CTextureManager.h"

#include "Entity/CEntityHandler.h"
#include "GameObject/CGameObject.h"
#include "RenderMesh/CRenderMesh.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CIndirectLighting);

	CATEGORY_COMPONENT(CIndirectLighting, "Indirect Lighting", "Indirect Lighting");

	CIndirectLighting::CIndirectLighting() :
		m_type(SH9),
		m_autoSH(true)
	{

	}

	CIndirectLighting::~CIndirectLighting()
	{

	}

	void CIndirectLighting::initComponent()
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
				CEntity* entity = entityMgr->getEntity(render->EntityIndex);
				addLightingData(entity);
			}
		}
		else
		{
			CEntityHandler* entityHandler = m_gameObject->getComponent<CEntityHandler>();
			if (entityHandler != NULL)
			{
				std::vector<CEntity*>& entities = entityHandler->getEntities();
				for (CEntity* entity : entities)
					addLightingData(entity);
			}
			else
			{
				// get entity that have render mesh data
				CEntity* entity = m_gameObject->getEntity();
				addLightingData(entity);
			}
		}
	}

	void CIndirectLighting::startComponent()
	{
		initComponent();
	}

	void CIndirectLighting::addLightingData(CEntity* entity)
	{
		// add indirect data info
		CIndirectLightingData* data = entity->addData<CIndirectLightingData>();

		if (m_type == LightmapArray)
		{
			data->Type = CIndirectLightingData::LightmapArray;
		}
		else
		{
			data->Type = CIndirectLightingData::VertexColor;
		}

		data->SH = m_sh;
		data->AutoSH = &m_autoSH;

		m_data.push_back(data);
	}

	void CIndirectLighting::updateComponent()
	{

	}

	CObjectSerializable* CIndirectLighting::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		CEnumProperty<EIndirectType>* enumType = new CEnumProperty<EIndirectType>(object, "type", m_type);
		enumType->addEnumString("Lightmap", EIndirectType::LightmapArray);
		enumType->addEnumString("Vertex Color", EIndirectType::VertexColor);
		enumType->addEnumString("SH9", EIndirectType::SH9);
		object->autoRelease(enumType);

		CArraySerializable* textureArray = new CArraySerializable("LMTextures", object);
		textureArray->OnCreateElement = [textureArray]()
		{
			CFilePathProperty* element = new CFilePathProperty(textureArray, "Element");
			textureArray->autoRelease(element);
			return element;
		};
		object->autoRelease(textureArray);

		// Sync lightmap path
		std::vector<std::string> textureExts = { "tga","png" };
		for (u32 i = 0, n = (u32)m_lightmapPaths.size(); i < n; i++)
		{
			char name[43];
			sprintf(name, "%d", i);

			textureArray->autoRelease(
				new CFilePathProperty(
					textureArray,
					name,
					m_lightmapPaths[i].c_str(),
					textureExts)
			);
		}

		return object;
	}

	void CIndirectLighting::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		EIndirectType type = object->get<EIndirectType>("type", EIndirectType::SH9);

		CArraySerializable* textureArray = (CArraySerializable*)object->getProperty("LMTextures");
		if (textureArray != NULL)
		{
			m_lightmapPaths.clear();

			int count = textureArray->getElementCount();
			for (int i = 0; i < count; i++)
			{
				char name[43];
				sprintf(name, "%d", i);

				std::string path = textureArray->get<std::string>(name, std::string());
				m_lightmapPaths.push_back(path);
			}
		}

		setIndirectLightingType(type);
	}

	void CIndirectLighting::setSH(core::vector3df* sh)
	{
		m_autoSH = false;
		for (int i = 0; i < 9; i++)
			m_sh[i] = sh[i];
	}

	void CIndirectLighting::setLightmap(ITexture* texture)
	{
		m_lightmap = texture;
	}

	void CIndirectLighting::setAutoSH(bool b)
	{
		m_autoSH = b;

		for (CIndirectLightingData* data : m_data)
		{
			data->AutoSH = &m_autoSH;
			data->Init = true;
		}
	}

	void CIndirectLighting::setIndirectLightingType(EIndirectType type)
	{
		m_type = type;

		for (CIndirectLightingData* data : m_data)
		{
			if (m_type == LightmapArray)
			{
				// Load lightmap texture array
				if (m_lightmapPaths.size() > 0 && m_lightmap == NULL)
					m_lightmap = CTextureManager::getInstance()->getTextureArray(m_lightmapPaths);

				data->Type = CIndirectLightingData::LightmapArray;
				data->LightmapTexture = m_lightmap;
			}
			else if (m_type == SH9)
			{
				data->Type = CIndirectLightingData::SH9;
				data->SH = m_sh;
			}
			else
			{
				data->Type = CIndirectLightingData::VertexColor;
			}
		}
	}
}