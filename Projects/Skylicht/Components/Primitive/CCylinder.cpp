#include "pch.h"
#include "CCylinder.h"
#include "LatheMesh/CCylinderMesh.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Culling/CCullingData.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CCylinder);

	CATEGORY_COMPONENT(CCylinder, "Cylinder", "Renderer/Primitive");

	CCylinder::CCylinder() :
		m_material(NULL),
		m_customMaterial(NULL),
		m_useNormalMap(true),
		m_useCustomMaterial(true),
		m_radius(0.5f),
		m_height(1.0f),
		m_color(255, 200, 200, 200)
	{

	}

	CCylinder::~CCylinder()
	{
		if (m_material)
		{
			m_material->drop();
			m_material = NULL;
		}
	}

	void CCylinder::initComponent()
	{
		m_material = new CMaterial("Capsule", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();

		init();
	}

	void CCylinder::updateComponent()
	{

	}

	CObjectSerializable* CCylinder::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		CBoolProperty* useCustom = new CBoolProperty(object, "custom material", m_useCustomMaterial);
		object->autoRelease(useCustom);

		CBoolProperty* useNormalMap = new CBoolProperty(object, "normal map", m_useNormalMap);
		object->autoRelease(useNormalMap);

		object->autoRelease(new CFloatProperty(object, "radius", m_radius, 0.0f));
		object->autoRelease(new CFloatProperty(object, "height", m_height, 0.0f));

		CColorProperty* color = new CColorProperty(object, "color", m_color);
		color->setUIHeader("Default Material");
		object->autoRelease(color);

		CFilePathProperty* material = new CFilePathProperty(object, "material", m_materialPath.c_str(), CMaterialManager::getMaterialExts());
		material->setUIHeader("Custom Material");
		object->autoRelease(material);

		return object;
	}

	void CCylinder::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		bool forceUpdateMaterial = false;
		bool useMaterial = !m_materialPath.empty();
		bool useNormal = m_useNormalMap;
		bool useCustom = m_useCustomMaterial;

		float radius = m_radius;
		float height = m_height;

		m_useNormalMap = object->get<bool>("normal map", false);
		m_color = object->get<SColor>("color", SColor(255, 180, 180, 180));
		m_materialPath = object->get<std::string>("material", std::string());
		m_useCustomMaterial = object->get<bool>("custom material", false);

		m_radius = object->get<float>("radius", 0.5f);
		m_height = object->get<float>("height", 1.0f);

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

		if (m_customMaterial)
		{
			m_customMaterial->setUniform4("uColor", m_color);
			m_customMaterial->updateShaderParams();
		}

		if (m_height != height ||
			m_radius != radius ||
			useNormal != m_useNormalMap ||
			useCustom != m_useCustomMaterial ||
			forceUpdateMaterial)
		{
			init();
		}
	}

	void CCylinder::init()
	{
		CEntity* entity = m_gameObject->getEntity();

		// init capsule mesh
		CCylinderMesh cylinderMesh;
		cylinderMesh.init(
			m_radius,
			m_height,
			m_useCustomMaterial && m_customMaterial ? m_customMaterial : m_material,
			m_useNormalMap
		);

		// add cube mesh data to entity
		CRenderMeshData* renderMesh = entity->getData<CRenderMeshData>();
		if (renderMesh == NULL)
			renderMesh = entity->addData<CRenderMeshData>();

		renderMesh->setShareMesh(cylinderMesh.getMesh());

		// add culling
		CCullingData* culling = entity->getData<CCullingData>();
		if (culling == NULL)
			culling = entity->addData<CCullingData>();
		culling->Type = CCullingData::BoundingBox;
	}

}