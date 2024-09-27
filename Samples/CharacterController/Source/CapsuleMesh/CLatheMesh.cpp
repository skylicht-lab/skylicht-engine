#include "pch.h"
#include "CLatheMesh.h"

// uv: https://catlikecoding.com/unity/tutorials/procedural-meshes/uv-sphere/

CLatheMesh::CLatheMesh() :
	m_mesh(NULL)
{
	m_mesh = new CMesh();
}

CLatheMesh::~CLatheMesh()
{
	m_mesh->drop();
}

void CLatheMesh::initLatheMesh()
{
	/*
	int numPoint = (int)m_outline.Points.size();
	if (numPoint == 0)
		return;

	int step = 10;
	float angleStep = 360.0f / (float)step;
	float angle = 0.0f;

	for (int i = 0; i < step; i++)
	{
		for (int j = 0; j < numPoint; j++)
		{
			core::vector3df& p = m_outline.Points[j];

		}
	}
	*/
}