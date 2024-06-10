#include "pch.h"
#include "CLeg.h"

CLeg::CLeg(std::vector<CWorldTransformData*>& joints, const core::vector3df& targetVector, const core::vector3df& footPos, float footStepLength) :
	m_joints(joints),
	m_targetVector(targetVector),
	m_footTargetPosition(footPos),
	m_footStepLength(footStepLength)
{

}

CLeg::~CLeg()
{

}

core::vector3df CLeg::getPosition()
{
	return m_joints[0]->World.getTranslation();
}

core::vector3df CLeg::getFootPosition()
{
	return m_joints[m_joints.size() - 1]->World.getTranslation();
}

void CLeg::update()
{
	for (CWorldTransformData* joint : m_joints)
		joint->HasLateChanged = true;
}

void CLeg::lateUpdate()
{

}