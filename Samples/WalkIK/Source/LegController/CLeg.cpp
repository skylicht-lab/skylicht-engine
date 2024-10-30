#include "pch.h"
#include "CLeg.h"
#include "Transform/CTransform.h"

#include "Debug/CSceneDebug.h"
#include "Utils/CVector.h"

CLeg::CLeg(
	CWorldTransformData* root,
	std::vector<CWorldTransformData*>& joints,
	const core::vector3df& targetVector,
	const core::vector3df& footPos,
	float footStepLength,
	float stepHeight,
	float stepTime) :
	CLegIK(root, joints),
	m_targetVector(targetVector),
	m_footTargetPosition(footPos),
	m_stepHeight(stepHeight),
	m_stepTime(stepTime),
	m_animTime(stepTime),
	m_waiting(false),
	m_waitingTime(0.0f)
{
	m_footPosition = m_footTargetPosition;
	m_lastFootPosition = m_footTargetPosition;
}

CLeg::~CLeg()
{

}

void CLeg::addLink(CLeg* leg)
{
	m_link.push_back(leg);
	if (leg)
		leg->m_link.push_back(this);
}

void CLeg::update()
{
	float dt = getTimeStep() / 1000.0f;
	m_animTime = m_animTime + dt;

	if (m_waiting)
		m_waitingTime = m_waitingTime + dt;

	float t = m_animTime / m_stepTime;
	t = core::clamp(t, 0.0f, 1.0f);

	m_footPosition = CVector::lerp(m_lastFootPosition, m_footTargetPosition, t);
	m_footPosition += Transform::Oy * sinf(t * core::PI) * m_stepHeight;

	CLegIK::update();
}