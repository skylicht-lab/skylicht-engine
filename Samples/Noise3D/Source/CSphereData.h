#pragma once

class CSphereData : public CRenderMeshData
{
public:
	IMeshBuffer *SphereMeshBuffer;

public:
	CSphereData();

	virtual ~CSphereData();

	void initSphere(float radius, CMaterial *material);
};