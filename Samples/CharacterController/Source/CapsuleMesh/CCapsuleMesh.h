#pragma once

#include "CLatheMesh.h"

class CCapsuleMesh : public CLatheMesh
{
protected:

public:
	CCapsuleMesh();

	virtual ~CCapsuleMesh();

	void init(float radius, float height, CMaterial* material);

protected:

	void initOutline(float radius, float height);

};