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
	m_drawDebug(false),
	m_root(root),
	m_joints(joints),
	m_targetVector(targetVector),
	m_footTargetPosition(footPos),
	m_footStepLength(footStepLength),
	m_stepHeight(stepHeight),
	m_stepTime(stepTime),
	m_animTime(stepTime),
	m_link(NULL)
{
	for (CWorldTransformData* t : joints)
	{
		m_worlds.push_back(t->World);
		m_upVector.push_back(Transform::Oy);
	}

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
	m_animTime = m_animTime + getTimeStep() / 1000.0f;
	if (m_animTime > m_stepTime)
		m_animTime = m_stepTime;

	float t = m_animTime / m_stepTime;
	m_footPosition = CVector::lerp(m_lastFootPosition, m_footTargetPosition, t);
	m_footPosition += Transform::Oy * sinf(t * core::PI) * m_stepHeight;
}

void CLeg::initLinkLength()
{
	for (int i = 0, n = (int)m_worlds.size() - 1; i < n; i++)
	{
		core::vector3df a = m_worlds[i].getTranslation();
		core::vector3df b = m_worlds[i + 1].getTranslation();
		m_lengths.push_back(a.getDistanceFrom(b));
	}

	if (m_lengths.size() > 0)
		m_lengths.push_back(0.0f);

	m_scale = m_root->Relative.getScale();
}

void CLeg::lateUpdate()
{
	CSceneDebug* debug = CSceneDebug::getInstance();
	CSceneDebug* debugNoZ = debug->getNoZDebug();

	core::vector3df up;
	std::vector<core::vector3df> upVector;

	for (size_t i = 0, n = m_joints.size(); i < n; i++)
	{
		m_worlds[i] = m_joints[i]->World;

		up.set(0.0f, 1.0f, 0.0f);
		m_worlds[i].rotateVect(up);
		up.normalize();

		m_upVector[i] = up;

		// input transform
		if (m_drawDebug)
			debugNoZ->addTransform(m_worlds[i], 1000.0f);
	}

	if (m_lengths.size() == 0)
		initLinkLength();

	fabricIK(m_footPosition);

	// output transform
	if (m_drawDebug)
	{
		for (int i = 0, n = (int)m_worlds.size() - 1; i < n; i++)
			debugNoZ->addTransform(m_worlds[i], 1500.0f);
	}

	for (size_t i = 0, n = m_joints.size(); i < n; i++)
	{
		m_joints[i]->World = m_worlds[i];
		m_joints[i]->NeedValidate = true;
	}
}

void CLeg::fabricIK(const core::vector3df& target)
{
	core::vector3df tempPos = target;
	core::vector3df basePos = m_worlds[0].getTranslation();

	core::vector3df pos, nextPos, prevPos, front, diff;

	// forward
	for (int i = (int)m_worlds.size() - 1; i >= 0; i--)
	{
		pos = m_worlds[i].getTranslation();

		diff = pos - tempPos;
		diff.normalize();

		pos = tempPos + diff * m_lengths[i];

		setTransform(m_worlds[i], pos, diff, m_upVector[i]);

		tempPos = pos;
	}

	// backward
	m_worlds[0].setTranslation(basePos);
	for (size_t i = 0, n = m_worlds.size(); i < n; i++)
	{
		pos = m_worlds[i].getTranslation();

		if (i == (m_worlds.size() - 1))
		{
			front = target - pos;
			front.normalize();

			setTransform(m_worlds[i], pos, front, m_upVector[i]);
		}
		else
		{
			diff = m_worlds[i + 1].getTranslation() - pos;

			front = diff;
			front.normalize();

			setTransform(m_worlds[i], pos, front, m_upVector[i]);

			nextPos = pos + front * m_lengths[i];
			m_worlds[i + 1].setTranslation(nextPos);
		}
	}
}

void CLeg::setTransform(core::matrix4& mat, const core::vector3df& pos, core::vector3df& front, core::vector3df up)
{
	core::vector3df right = up.crossProduct(front);
	right.normalize();

	up = front.crossProduct(right);
	up.normalize();

	// note: X is front
	f32* matData = mat.pointer();
	matData[0] = -front.X * m_scale.X;
	matData[1] = -front.Y * m_scale.X;
	matData[2] = -front.Z * m_scale.X;
	matData[3] = 0.0f;

	matData[4] = up.X * m_scale.Y;
	matData[5] = up.Y * m_scale.Y;
	matData[6] = up.Z * m_scale.Y;
	matData[7] = 0.0f;

	matData[8] = right.X * m_scale.Z;
	matData[9] = right.Y * m_scale.Z;
	matData[10] = right.Z * m_scale.Z;
	matData[11] = 0.0f;

	matData[12] = pos.X;
	matData[13] = pos.Y;
	matData[14] = pos.Z;
	matData[15] = 1.0f;
}