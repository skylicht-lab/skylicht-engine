#pragma once

#include "CLine3D.h"
#include "RenderMesh/CMesh.h"

class CLatheMesh
{
protected:
	CLine3D m_outline;
	CMesh* m_mesh;

public:
	CLatheMesh();

	virtual ~CLatheMesh();

	inline CMesh* getMesh()
	{
		return m_mesh;
	}
protected:

	void initLatheMesh();
};