#include "pch.h"
#include "CLathe.h"
#include "LatheMesh/CLatheMesh.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Culling/CVisibleData.h"
#include "Culling/CCullingData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	CLathe::CLathe() :
		m_material(NULL),
		m_customMaterial(NULL),
		m_shadowCasting(true),
		m_useNormalMap(true),
		m_useCustomMaterial(true),
		m_color(255, 200, 200, 200),
		m_needReinit(true)
	{

	}

	CLathe::~CLathe()
	{
		if (m_gameObject)
		{
			CEntity* entity = m_gameObject->getEntity();
			entity->removeData<CRenderMeshData>();
			entity->removeData<CCullingData>();
		}

		if (m_material)
		{
			m_material->drop();
			m_material = NULL;
		}
	}

	void CLathe::initComponent()
	{
		m_material = new CMaterial("Capsule", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();
	}

	void CLathe::updateComponent()
	{

	}

	CObjectSerializable* CLathe::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		object->autoRelease(new CBoolProperty(object, "shadow casting", m_shadowCasting));
		object->autoRelease(new CBoolProperty(object, "custom material", m_useCustomMaterial));
		object->autoRelease(new CBoolProperty(object, "normal map", m_useNormalMap));

		CColorProperty* color = new CColorProperty(object, "color", m_color);
		color->setUIHeader("Default Material");
		object->autoRelease(color);

		CFilePathProperty* material = new CFilePathProperty(object, "material", m_materialPath.c_str(), CMaterialManager::getMaterialExts());
		material->setUIHeader("Custom Material");
		object->autoRelease(material);

		return object;
	}

	void CLathe::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		bool forceUpdateMaterial = false;
		bool useMaterial = !m_materialPath.empty();
		bool useNormal = m_useNormalMap;
		bool useCustom = m_useCustomMaterial;

		m_shadowCasting = object->get<bool>("shadow casting", true);
		m_useNormalMap = object->get<bool>("normal map", false);
		m_color = object->get<SColor>("color", SColor(255, 180, 180, 180));
		m_materialPath = object->get<std::string>("material", std::string());
		m_useCustomMaterial = object->get<bool>("custom material", false);

		if (!m_materialPath.empty() && !useMaterial)
		{
			// force enable material while update new material in editor
			m_useCustomMaterial = true;
			forceUpdateMaterial = true;
		}

		if (forceUpdateMaterial || m_useCustomMaterial != useCustom)
		{
			if (m_useCustomMaterial == true && !m_materialPath.empty())
			{
				m_customMaterial = NULL;

				ArrayMaterial& loadMaterials = CMaterialManager::getInstance()->loadMaterial(m_materialPath.c_str(), true, std::vector<std::string>());
				if (loadMaterials.size() > 0)
					m_customMaterial = loadMaterials[0];
			}
		}

		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();

		if (useNormal != m_useNormalMap ||
			useCustom != m_useCustomMaterial ||
			forceUpdateMaterial)
		{
			m_needReinit = true;
		}
	}

	void CLathe::setShadowCasting(bool b)
	{
		m_shadowCasting = b;

		CEntity* entity = m_gameObject->getEntity();
		CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
		visible->ShadowCasting = b;
	}

	void CLathe::setColor(const SColor& color)
	{
		m_color = color;

		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();
	}

	CMesh* CLathe::getMesh()
	{
		CEntity* entity = m_gameObject->getEntity();

		CRenderMeshData* renderMesh = entity->getData<CRenderMeshData>();
		if (renderMesh == NULL)
			return NULL;

		return renderMesh->getMesh();
	}

	void CLathe::initMesh(CLatheMesh* mesh)
	{
		CEntity* entity = m_gameObject->getEntity();

		// add cube mesh data to entity
		CRenderMeshData* renderMesh = entity->getData<CRenderMeshData>();
		if (renderMesh == NULL)
			renderMesh = entity->addData<CRenderMeshData>();

		renderMesh->setShareMesh(mesh->getMesh());

		// add culling
		// if (entity->getData<CWorldInverseTransformData>())
		//	entity->addData<CWorldInverseTransformData>();

		CCullingData* culling = entity->getData<CCullingData>();
		if (culling == NULL)
			culling = entity->addData<CCullingData>();
		culling->Type = CCullingData::BoundingBox;
	}

}