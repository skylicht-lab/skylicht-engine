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

#include "Entity/CEntityHandler.h"
#include "GameObject/CGameObject.h"
#include "RenderMesh/CRenderMesh.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CIndirectLighting);

	CATEGORY_COMPONENT(CIndirectLighting, "Indirect Lighting", "Indirect Lighting");

	CIndirectLighting::CIndirectLighting() :
		m_type(LightmapArray),
		m_autoSH(false)
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

	void CIndirectLighting::setSH(core::vector3df* sh)
	{
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
		}
	}

	void CIndirectLighting::setIndirectLightingType(EIndirectType type)
	{
		m_type = type;

		for (CIndirectLightingData* data : m_data)
		{
			if (m_type == LightmapArray)
			{
				data->Type = CIndirectLightingData::LightmapArray;
				data->LightmapTexture = m_lightmap;
			}
			else if (m_type == SH4)
			{
				data->Type = CIndirectLightingData::SH4;
				data->SH = m_sh;
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