#include "pch.h"
#include "CCapsule.h"
#include "LatheMesh/CCapsuleMesh.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Culling/CVisibleData.h"
#include "Culling/CCullingData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CCapsule);

	CATEGORY_COMPONENT(CCapsule, "Capsule", "Renderer/Primitive");

	CCapsule::CCapsule() :
		m_material(NULL),
		m_customMaterial(NULL),
		m_shadowCasting(true),
		m_useNormalMap(true),
		m_useCustomMaterial(true),
		m_radius(0.5f),
		m_height(1.0f),
		m_color(255, 200, 200, 200)
	{

	}

	CCapsule::~CCapsule()
	{
		if (m_material)
		{
			m_material->drop();
			m_material = NULL;
		}
	}

	void CCapsule::initComponent()
	{
		m_material = new CMaterial("Capsule", "BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		m_material->setUniform4("uColor", m_color);
		m_material->updateShaderParams();

		init();
	}

	void CCapsule::updateComponent()
	{

	}

	CObjectSerializable* CCapsule::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		object->autoRelease(new CBoolProperty(object, "shadow casting", m_shadowCasting));
		object->autoRelease(new CBoolProperty(object, "custom material", m_useCustomMaterial));
		object->autoRelease(new CBoolProperty(object, "normal map", m_useNormalMap));

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

	void CCapsule::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		bool forceUpdateMaterial = false;
		bool useMaterial = !m_materialPath.empty();
		bool useNormal = m_useNormalMap;
		bool useCustom = m_useCustomMaterial;

		float radius = m_radius;
		float height = m_height;

		m_shadowCasting = object->get<bool>("shadow casting", true);
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

		setShadowCasting(m_shadowCasting);
	}

	void CCapsule::setShadowCasting(bool b)
	{
		m_shadowCasting = b;

		CEntity* entity = m_gameObject->getEntity();
		CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);
		visible->ShadowCasting = b;
	}

	void CCapsule::init()
	{
		CEntity* entity = m_gameObject->getEntity();

		// init capsule mesh
		CCapsuleMesh capsuleMesh;
		capsuleMesh.init(
			m_radius,
			m_height,
			m_useCustomMaterial && m_customMaterial ? m_customMaterial : m_material,
			m_useNormalMap
		);

		// add cube mesh data to entity
		CRenderMeshData* renderMesh = entity->getData<CRenderMeshData>();
		if (renderMesh == NULL)
			renderMesh = entity->addData<CRenderMeshData>();

		renderMesh->setShareMesh(capsuleMesh.getMesh());

		// add culling
		if (entity->getData<CWorldInverseTransformData>())
			entity->addData<CWorldInverseTransformData>();

		CCullingData* culling = entity->getData<CCullingData>();
		if (culling == NULL)
			culling = entity->addData<CCullingData>();
		culling->Type = CCullingData::BoundingBox;
	}

}