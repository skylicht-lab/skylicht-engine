#include "pch.h"
#include "SkylichtEngine.h"
#include "CCubeRotate.h"
#include "CCubeData.h"

CCubeRotate::CCubeRotate()
{

}

CCubeRotate::~CCubeRotate()
{

}

void CCubeRotate::initComponent()
{

}

void CCubeRotate::updateComponent()
{
	CTransformEuler *transform = m_gameObject->getTransformEuler();

	float timestep = getTimeStep();
	core::vector3df rot = transform->getRotation();

	rot.X = rot.X + m_rotateSpeed.X * timestep;
	rot.Y = rot.Y + m_rotateSpeed.Y * timestep;
	rot.Z = rot.Z + m_rotateSpeed.Z * timestep;

	rot.X = fmodf(rot.X, 360.0f);
	rot.Y = fmodf(rot.Y, 360.0f);
	rot.Z = fmodf(rot.Z, 360.0f);

	transform->setRotation(rot);
}