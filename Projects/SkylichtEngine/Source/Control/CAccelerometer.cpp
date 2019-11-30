#include "pch.h"
#include "CAccelerometer.h"

namespace Skylicht
{

CAccelerometer::CAccelerometer()
{
	m_support = false;
	m_enable = false;

	m_rotX = 0.0f;
	m_rotY = 0.0f;
}

CAccelerometer::~CAccelerometer()
{
}

// update
void CAccelerometer::update()
{
}

// setVector
void CAccelerometer::setVector(float x, float y, float z)
{
	m_vector.set(x, y, z);
	
	m_rotX = x;
	m_rotY = y;
}

}