#include "pch.h"
#include "CDemo.h"

CDemo::CDemo(Verlet::CVerlet* verlet) :
	m_verlet(verlet),
	m_drawingParticle(false),
	m_enableWind(true)
{

}

CDemo::~CDemo()
{

}