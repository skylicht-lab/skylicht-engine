#pragma once

#include "CLatheMesh.h"

class CCapsuleMesh : public CLatheMesh
{
protected:

public:
	CCapsuleMesh();

	virtual ~CCapsuleMesh();

	void init(float radius, float height);

	void drawOutline();

protected:

	void initOutline(float radius, float height);

};