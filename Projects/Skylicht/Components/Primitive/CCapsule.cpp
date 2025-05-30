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
		m_radius(0.5f),
		m_height(1.0f)
	{

	}

	CCapsule::~CCapsule()
	{

	}

	void CCapsule::initComponent()
	{
		CLathe::initComponent();

		init();
	}

	void CCapsule::updateComponent()
	{

	}

	CObjectSerializable* CCapsule::createSerializable()
	{
		CObjectSerializable* object = CLathe::createSerializable();

		CFloatProperty* radius = new CFloatProperty(object, "radius", m_radius, 0.0f);
		radius->setUIHeader("Capsule");
		object->autoRelease(radius);
		object->autoRelease(new CFloatProperty(object, "height", m_height, 0.0f));

		return object;
	}

	void CCapsule::loadSerializable(CObjectSerializable* object)
	{
		float radius = m_radius;
		float height = m_height;

		CLathe::loadSerializable(object);

		m_radius = object->get<float>("radius", 0.5f);
		m_height = object->get<float>("height", 1.0f);

		if (m_height != height ||
			m_radius != radius ||
			m_needReinit)
		{
			init();
		}

		setShadowCasting(m_shadowCasting);
	}

	void CCapsule::init()
	{
		// init capsule mesh
		CCapsuleMesh capsuleMesh;
		capsuleMesh.init(
			m_radius,
			m_height,
			m_useCustomMaterial && m_customMaterial ? m_customMaterial : m_material,
			m_useNormalMap
		);

		initMesh(&capsuleMesh);

		m_needReinit = false;
	}

}