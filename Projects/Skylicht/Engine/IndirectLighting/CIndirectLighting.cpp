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
		m_autoSH(true),
		m_internalIndirectLM(false),
		m_indirectLM(NULL),
		m_intensity(1.0f),
		m_ambientColor(255, 60, 60, 60)
	{

	}

	CIndirectLighting::~CIndirectLighting()
	{
		if (m_internalIndirectLM && m_indirectLM)
		{
			CTextureManager::getInstance()->removeTexture(m_indirectLM);
		}
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

	void CIndirectLighting::startComponent()
	{
		initComponent();
	}

	void CIndirectLighting::addLightingData(CEntity* entity)
	{
		CIndirectLightingData* data = entity->getData<CIndirectLightingData>();
		if (data == NULL)
		{
			// add indirect data info
			data = entity->addData<CIndirectLightingData>();
		}
		else
		{
			data->releaseSH();
		}

		data->Type = (CIndirectLightingData::EType)m_type;
		data->SH = m_sh;
		data->AutoSH = &m_autoSH;
		data->Intensity = &m_intensity;
		data->ReleaseSH = false;

		if (std::find(m_data.begin(), m_data.end(), data) == m_data.end())
			m_data.push_back(data);
	}

	void CIndirectLighting::removeAllData()
	{
		for (CIndirectLightingData* d : m_data)
			d->Entity->removeData<CIndirectLightingData>();
		m_data.clear();
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
		enumType->addEnumString("Ambient Color", EIndirectType::AmbientColor);
		object->autoRelease(enumType);

		// ambient color
		object->autoRelease(new CColorProperty(object, "ambientColor", m_ambientColor));

		// lightmap
		CArrayTypeSerializable<CFilePathProperty>* textureArray = new CArrayTypeSerializable<CFilePathProperty>("lightmap", object);
		textureArray->OnCreateElement = [](CValueProperty* p)
			{
				CFilePathProperty* filePath = (CFilePathProperty*)p;
				filePath->Exts = CTextureManager::getTextureExts();
			};

		for (std::string& path : m_indirectLMPaths)
			textureArray->autoRelease(new CFilePathProperty(textureArray, "texture", path.c_str(), CTextureManager::getTextureExts()));
		object->autoRelease(textureArray);

		return object;
	}

	void CIndirectLighting::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		EIndirectType type = object->get<EIndirectType>("type", EIndirectType::SH9);

		m_ambientColor = object->get<SColor>("ambientColor", SColor(255, 60, 60, 60));

		bool haveLightmap = false;
		CArraySerializable* textureArray = (CArraySerializable*)object->getProperty("lightmap");
		if (textureArray)
		{
			int numTextures = textureArray->getElementCount();
			haveLightmap = numTextures > 0;

			m_indirectLMPaths.clear();
			for (int i = 0; i < numTextures; i++)
			{
				CFilePathProperty* file = (CFilePathProperty*)textureArray->getElement(i);
				m_indirectLMPaths.push_back(file->get());
			}
		}

		setIndirectLightingType(type, haveLightmap);
	}

	bool CIndirectLighting::isLightmapEmpty()
	{
		for (std::string& s : m_indirectLMPaths)
		{
			if (s.empty())
				return true;
		}
		return false;
	}

	bool CIndirectLighting::isLightmapChanged(const std::vector<std::string>& paths)
	{
		if (paths.size() != m_indirectLMPaths.size())
			return true;

		for (int i = 0, n = (int)paths.size(); i < n; i++)
		{
			if (paths[i] != m_indirectLMPaths[i])
			{
				return true;
			}
		}

		return false;
	}

	void CIndirectLighting::setSH(core::vector3df* sh)
	{
		m_autoSH = false;
		for (int i = 0; i < 9; i++)
			m_sh[i] = sh[i];
	}

	void CIndirectLighting::setAmbientColor(const SColor& color)
	{
		m_ambientColor = color;
		for (CIndirectLightingData* data : m_data)
		{
			data->Color = color;
		}
	}

	void CIndirectLighting::setIndirectLightmap(ITexture* texture)
	{
		if (m_internalIndirectLM && m_indirectLM)
			CTextureManager::getInstance()->removeTexture(m_indirectLM);

		m_indirectLM = texture;
		m_internalIndirectLM = false;
	}

	void CIndirectLighting::setAutoSH(bool b)
	{
		m_autoSH = b;

		for (CIndirectLightingData* data : m_data)
		{
			data->AutoSH = &m_autoSH;
			data->InvalidateProbe = true;
		}
	}

	void CIndirectLighting::setIndirectLightingType(EIndirectType type, bool loadLightmap)
	{
		m_type = type;

		for (CIndirectLightingData* data : m_data)
		{
			if (m_type == LightmapArray)
			{
				// Load lightmap texture array
				if (m_indirectLMPaths.size() > 0 && loadLightmap)
				{
					if (m_internalIndirectLM && m_indirectLM)
						CTextureManager::getInstance()->removeTexture(m_indirectLM);

					m_indirectLM = CTextureManager::getInstance()->getTextureArray(m_indirectLMPaths);
					m_internalIndirectLM = true;
				}

				data->Type = CIndirectLightingData::LightmapArray;
				data->IndirectTexture = m_indirectLM;
			}
			else if (m_type == SH9)
			{
				data->Type = CIndirectLightingData::SH9;
				data->SH = m_sh;
			}
			else if (m_type == AmbientColor)
			{
				data->Type = CIndirectLightingData::AmbientColor;
				data->Color = m_ambientColor;
			}
			else
			{
				data->Type = CIndirectLightingData::VertexColor;
			}
		}
	}
}