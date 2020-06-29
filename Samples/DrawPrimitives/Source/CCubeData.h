#pragma once

class CCubeData : public CRenderMeshData
{
public:
	IMeshBuffer *CubeMeshBuffer;

public:
	CCubeData();

	virtual ~CCubeData();

	void initCube(float size);
};