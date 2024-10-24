#include "pch.h"
#include "CCapsuleMesh.h"

namespace Skylicht
{

	CCapsuleMesh::CCapsuleMesh()
	{

	}

	CCapsuleMesh::~CCapsuleMesh()
	{

	}

	void CCapsuleMesh::init(float radius, float height, CMaterial* material, bool tangent)
	{
		if (height < radius * 2)
			height = radius * 2;

		initOutline(radius, height);
		initLatheMesh(material, tangent);
	}

	void CCapsuleMesh::initOutline(float radius, float height)
	{
		m_outline.Points.clear();
		m_outline.Normals.clear();

		core::vector3df center;
		center.set(0.0f, height * 0.5f, 0.0f);

		int step = 5;
		float angle = 0.0f;
		float stepAngle = 90.0f / (float)step;
		core::vector3df normal;

		// top capsule
		for (int i = 0; i <= step; i++)
		{
			angle = i * stepAngle;
			normal.set(0.0f, radius * cosf(angle * core::DEGTORAD), radius * sinf(angle * core::DEGTORAD));
			m_outline.Points.push_back(center + normal);
			m_outline.Normals.push_back(normal);
		}

		// body capsule
		center.set(0.0f, -(height * 0.5f), 0.0f);
		normal.set(0.0f, 0.0f, radius);
		m_outline.Points.push_back(center + normal);
		m_outline.Normals.push_back(normal);

		// bottom capsule
		for (int i = 0; i <= step; i++)
		{
			angle = i * stepAngle;
			normal.set(0.0f, -radius * sinf(angle * core::DEGTORAD), radius * cosf(angle * core::DEGTORAD));
			m_outline.Points.push_back(center + normal);
			m_outline.Normals.push_back(normal);
		}
	}

}