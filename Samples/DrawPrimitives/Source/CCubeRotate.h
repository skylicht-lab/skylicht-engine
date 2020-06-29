#pragma once

class CCubeRotate : public CComponentSystem
{
protected:
	core::vector3df m_rotateSpeed;

public:
	CCubeRotate();

	virtual ~CCubeRotate();

	virtual void initComponent();

	virtual void updateComponent();

	void setRotate(float x, float y, float z)
	{
		m_rotateSpeed.X = x;
		m_rotateSpeed.Y = y;
		m_rotateSpeed.Z = z;
	}
};