#pragma once

class CLine3D
{
public:
	core::array<core::vector3df> Points;
	core::array<core::vector3df> Normals;
	core::array<int> VertexCount;

public:
	CLine3D();

	virtual ~CLine3D();
};