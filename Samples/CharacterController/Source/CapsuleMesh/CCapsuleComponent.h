#pragma once

#include "Material/CMaterial.h"

class CCapsuleComponent : public CComponentSystem
{
protected:
	CMaterial* m_material;

public:
	CCapsuleComponent();

	virtual ~CCapsuleComponent();

	virtual void initComponent();

	virtual void updateComponent();

	void init(float radius, float height);

	inline CMaterial* getMaterial()
	{
		return m_material;
	}
};