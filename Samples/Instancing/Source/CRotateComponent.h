#pragma once

class CRotateComponent : public CComponentSystem
{
protected:
	core::vector3df m_rotateSpeed;

public:
	CRotateComponent();

	virtual ~CRotateComponent();

	virtual void initComponent();

	virtual void updateComponent();

	void setRotate(float x, float y, float z)
	{
		m_rotateSpeed.X = x;
		m_rotateSpeed.Y = y;
		m_rotateSpeed.Z = z;
	}
};