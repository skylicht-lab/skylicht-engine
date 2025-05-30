#include "pch.h"
#include "CCylinder.h"
#include "LatheMesh/CCylinderMesh.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Culling/CVisibleData.h"
#include "Culling/CCullingData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CCylinder);

	CATEGORY_COMPONENT(CCylinder, "Cylinder", "Renderer/Primitive");

	CCylinder::CCylinder() :
		m_radius(0.5f),
		m_height(1.0f)
	{

	}

	CCylinder::~CCylinder()
	{

	}

	void CCylinder::initComponent()
	{
		CLathe::initComponent();

		init();
	}

	void CCylinder::updateComponent()
	{

	}

	CObjectSerializable* CCylinder::createSerializable()
	{
		CObjectSerializable* object = CLathe::createSerializable();

		CFloatProperty* radius = new CFloatProperty(object, "radius", m_radius, 0.0f);
		radius->setUIHeader("Cylinder");
		object->autoRelease(radius);
		object->autoRelease(new CFloatProperty(object, "height", m_height, 0.0f));

		return object;
	}

	void CCylinder::loadSerializable(CObjectSerializable* object)
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

	void CCylinder::init()
	{
		// init cylinder mesh
		CCylinderMesh cylinderMesh;
		cylinderMesh.init(
			m_radius,
			m_height,
			m_useCustomMaterial && m_customMaterial ? m_customMaterial : m_material,
			m_useNormalMap
		);

		initMesh(&cylinderMesh);
		m_needReinit = false;
	}

}