#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include "Utils/CGameSingleton.h"

namespace Skylicht
{

class CAccelerometer : public CGameSingleton < CAccelerometer >
{
protected:
	bool				m_support;
	bool				m_enable;

	core::vector3df		m_vector;
	float m_rotX;
	float m_rotY;

public:
	CAccelerometer();
	virtual ~CAccelerometer();

	// setSupport
	void setSupport(bool b)
	{
		m_support = b;
	}

	// isSupport
	bool isSupport()
	{
		return m_support;
	}

	// setEnable
	void setEnable(bool b)
	{
		m_enable = b;
	}

	// isEnable
	bool isEnable()
	{
		return m_enable;
	}

	// getVector
	// get phone vector
	const core::vector3df& getVector()
	{
		return m_vector;
	}

	// setVector
	void setVector(float x, float y, float z);

	// update
	void update();

	// getRotX
	inline float getRotX()
	{
		return m_rotX;
	}

	// getRotY
	inline float getRotY()
	{
		return m_rotY;
	}

};

}

#endif