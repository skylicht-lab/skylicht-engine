#include "pch.h"
#include "CCapsuleMesh.h"
#include "Debug/CSceneDebug.h"

CCapsuleMesh::CCapsuleMesh()
{

}

CCapsuleMesh::~CCapsuleMesh()
{

}

void CCapsuleMesh::init(float radius, float height)
{
	if (height < radius * 2)
		height = radius * 2;

	initOutline(radius, height);
	initLatheMesh();
}

void CCapsuleMesh::initOutline(float radius, float height)
{
	m_outline.Points.clear();

	core::vector3df center;
	center.set(0.0f, height - radius, 0.0f);

	int step = 5;
	float angle = 0.0f;
	float stepAngle = 90.0f / (float)step;
	core::vector3df p;

	// top capsule
	for (int i = 0; i <= step; i++)
	{
		angle = i * stepAngle;
		p.set(0.0f, radius * cosf(angle * core::DEGTORAD), radius * sinf(angle * core::DEGTORAD));
		m_outline.Points.push_back(center + p);
	}

	// body capsule
	center.set(0.0f, radius, 0.0f);
	p.set(0.0f, 0.0f, radius);
	m_outline.Points.push_back(center + p);

	// bottom capsule
	for (int i = 0; i <= step; i++)
	{
		angle = i * stepAngle;
		p.set(0.0f, -radius * sinf(angle * core::DEGTORAD), radius * cosf(angle * core::DEGTORAD));
		m_outline.Points.push_back(center + p);
	}
}

void CCapsuleMesh::drawOutline()
{
	if (m_outline.Points.size() >= 2)
	{
		CSceneDebug* sceneDebug = CSceneDebug::getInstance();
		sceneDebug->addLinestrip(m_outline.Points, SColor(255, 255, 0, 0));
	}
}