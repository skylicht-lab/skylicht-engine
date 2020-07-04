#pragma once

class CSphereComponent : public CComponentSystem
{
protected:
	CMaterial *m_material;

public:
	CSphereComponent();

	virtual ~CSphereComponent();

	virtual void initComponent();

	virtual void updateComponent();

	inline CMaterial* getMaterial()
	{
		return m_material;
	}
};