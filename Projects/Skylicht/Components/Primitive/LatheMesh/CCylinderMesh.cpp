#include "pch.h"
#include "CCylinderMesh.h"

namespace Skylicht
{

	CCylinderMesh::CCylinderMesh()
	{

	}

	CCylinderMesh::~CCylinderMesh()
	{

	}

	void CCylinderMesh::init(float radius, float height, CMaterial* material, bool tangent)
	{
		if (height < 0.0f)
			height = 0.0f;

		initOutline(radius, height);
		initLatheMesh(material, tangent);
	}

	void CCylinderMesh::initOutline(float radius, float height)
	{
		m_outline.Points.clear();
		m_outline.Normals.clear();

		core::vector3df center;

		core::vector3df r(0.0f, 0.0f, radius);

		// top cylinder
		{
			center.set(0.0f, height * 0.5f, 0.0f);

			m_outline.Points.push_back(center);
			m_outline.Normals.push_back(core::vector3df(0.0f, 1.0f, 0.0f));

			m_outline.Points.push_back(center + r);
			m_outline.Normals.push_back(core::vector3df(0.0f, 1.0f, 0.0f));

			m_outline.Points.push_back(center + r);
			m_outline.Normals.push_back(core::vector3df(0.0f, 0.0f, 1.0f));
		}

		// bottom cylinder
		{
			center.set(0.0f, -(height * 0.5f), 0.0f);

			m_outline.Points.push_back(center + r);
			m_outline.Normals.push_back(core::vector3df(0.0f, 0.0f, 1.0f));

			m_outline.Points.push_back(center + r);
			m_outline.Normals.push_back(core::vector3df(0.0f, -1.0f, 0.0f));

			m_outline.Points.push_back(center);
			m_outline.Normals.push_back(core::vector3df(0.0f, -1.0f, 0.0f));
		}
	}

}