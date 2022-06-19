/*
/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CPrimitive.h"
#include "CPrimitiveRenderer.h"
#include "CPrimitiveRendererInstancing.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Culling/CCullingData.h"
#include "Culling/CCullingBBoxData.h"
#include "IndirectLighting/CIndirectLightingData.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	CPrimitive::CPrimitive() :
		m_type(CPrimiviteData::Unknown),
		m_instancing(false),
		m_useCustomMaterial(false),
		m_color(255, 180, 180, 180),
		m_material(NULL),
		m_customMaterial(NULL)
	{

	}

	CPrimitive::~CPrimitive()
	{
		if (m_material)
			delete m_material;
	}

	void CPrimitive::initComponent()
	{
		CEntityManager* entityMgr = m_gameObject->getEntityManager();
		entityMgr->addRenderSystem<CPrimitiveRenderer>();
		entityMgr->addRenderSystem<CPrimitiveRendererInstancing>();

		// init material
		m_material = new CMaterial("Primitive", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		m_material->setUniform4("uColor", m_color);

		// add default primitive
		if (m_type != CPrimiviteData::Unknown)
		{
			addPrimitive(
				core::vector3df(),
				core::vector3df(),
				core::vector3df(1.0f, 1.0f, 1.0f)
			);
		}
	}

	CObjectSerializable* CPrimitive::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		object->autoRelease(new CBoolProperty(object, "instancing", m_instancing));

		CBoolProperty* useCustom = new CBoolProperty(object, "custom material", m_useCustomMaterial);
		object->autoRelease(useCustom);

		CColorProperty* color = new CColorProperty(object, "color", m_color);
		color->setUIHeader("Default Material");
		object->autoRelease(color);

		std::vector<std::string> exts = { "mat" };
		CFilePathProperty* material = new CFilePathProperty(object, "material", m_materialPath.c_str(), exts);
		material->setUIHeader("Custom Material");
		object->autoRelease(material);

		CArraySerializable* primitives = new CArraySerializable("Primitives");
		object->addProperty(primitives);
		object->autoRelease(primitives);

		int numPrimities = (int)m_entities.size();
		for (int i = 0; i < numPrimities; i++)
		{
			CMatrixProperty* transformData = new CMatrixProperty(primitives, "transform");
			primitives->autoRelease(transformData);

			// get world transform data
			CWorldTransformData* world = (CWorldTransformData*)m_entities[i]->getDataByIndex(CWorldTransformData::DataTypeIndex);
			transformData->set(world->Relative);
		}

		return object;
	}

	void CPrimitive::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_instancing = object->get<bool>("instancing", false);
		bool useCustom = object->get<bool>("custom material", false);
		m_color = object->get<SColor>("color", SColor(255, 180, 180, 180));
		m_materialPath = object->get<std::string>("material", std::string());

		if (m_useCustomMaterial != useCustom)
		{
			if (useCustom == true && !m_materialPath.empty())
			{
				m_customMaterial = NULL;

				ArrayMaterial& loadMaterials = CMaterialManager::getInstance()->loadMaterial(m_materialPath.c_str(), true, std::vector<std::string>());
				if (loadMaterials.size() > 0)
					m_customMaterial = loadMaterials[0];
			}

			m_useCustomMaterial = useCustom;
		}

		m_material->setUniform4("uColor", m_color);
		if (m_customMaterial)
			m_customMaterial->setUniform4("uColor", m_color);

		CArraySerializable* primitives = (CArraySerializable*)object->getProperty("Primitives");
		if (primitives == NULL)
			return;

		int numPrimities = primitives->getElementCount();

		removeAllEntities();

		for (int i = 0; i < numPrimities; i++)
		{
			CMatrixProperty* transformData = (CMatrixProperty*)primitives->getElement(i);
			if (transformData == NULL)
				return;

			CEntity* entity = addPrimitive(
				core::vector3df(),
				core::vector3df(),
				core::vector3df(1.0f, 1.0f, 1.0f)
			);

			// set transform
			CWorldTransformData* world = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
			world->Relative = transformData->get();
		}
	}

	CEntity* CPrimitive::spawn()
	{
		return addPrimitive(core::vector3df(), core::vector3df(), core::vector3df(1.0f, 1.0f, 1.0f));
	}

	CEntity* CPrimitive::addPrimitive(const core::vector3df& pos, const core::vector3df& rotDeg, const core::vector3df& scale)
	{
		CEntity* entity = createEntity();

		CPrimiviteData* primitiveData = entity->addData<CPrimiviteData>();
		primitiveData->Type = m_type;
		primitiveData->Material = m_useCustomMaterial && m_customMaterial ? m_customMaterial : m_material;
		primitiveData->Instancing = m_instancing;

		// Culling
		entity->addData<CWorldInverseTransformData>();
		entity->addData<CCullingData>();
		entity->addData<CIndirectLightingData>();

		CCullingBBoxData* cullingBBox = entity->addData<CCullingBBoxData>();
		cullingBBox->BBox.MinEdge.set(-1.0f, -1.0f, -1.0f);
		cullingBBox->BBox.MaxEdge.set(1.0f, 1.0f, 1.0f);

		// Position
		CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
		transform->Relative.setTranslation(pos);
		transform->Relative.setRotationDegrees(rotDeg);
		transform->Relative.setScale(scale);

		// Indirect lighting
		CIndirectLightingData* indirect = (CIndirectLightingData*)entity->getDataByIndex(CIndirectLightingData::DataTypeIndex);
		indirect->Type = CIndirectLightingData::SH9;
		indirect->AutoSH = new bool();
		indirect->SH = new core::vector3df[9];
		indirect->ReleaseSH = true;

		*indirect->AutoSH = true;
		return entity;
	}
}