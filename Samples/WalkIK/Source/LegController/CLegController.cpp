#include "pch.h"
#include "CLegController.h"

#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

#include "Debug/CSceneDebug.h"
#include "Utils/CVector.h"

CLegController::CLegController() :
	m_drawDebug(false),
	m_renderMesh(NULL),
	m_root(NULL),
	m_targetDistance(3.3f),
	m_moveStepDistance(0.1f),
	m_footStepLength(0.3f),
	m_stepHeight(0.2f),
	m_stepTime(0.3f),
	m_moveTime(0.0f),
	m_standTime(0.0f),
	m_rotTime(0.0f),
	m_rotDirection(0.0f)
{

}

CLegController::~CLegController()
{
	for (CLeg* leg : m_legs)
		delete leg;
	m_legs.clear();

	CWorldTransformSystem* system = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	system->unRegisterLateUpdate(this);
}

void CLegController::initComponent()
{
	m_renderMesh = m_gameObject->getComponent<CRenderMesh>();

	CWorldTransformSystem* system = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	system->registerLateUpdate(this);
}

void CLegController::updateComponent()
{
	float minY = FLT_MAX, maxY = FLT_MIN;

	for (CLeg* leg : m_legs)
	{
		leg->update();

		const core::vector3df& footPosition = leg->getFootPosition();
		if (footPosition.Y < minY)
			minY = footPosition.Y;
		if (footPosition.Y > maxY)
			maxY = footPosition.Y;
	}

	float offsetY = maxY - minY;
	if (offsetY < m_footStepLength)
	{
		// move up the body when he is walking
		if (m_root)
			m_root->Relative.setTranslation(core::vector3df(0.0f, minY + offsetY * 0.2f, 0.0f));
	}
}

CLeg* CLegController::addLeg(CWorldTransformData* root, CWorldTransformData* leg)
{
	if (m_renderMesh && leg)
	{
		m_root = root;

		std::vector<CWorldTransformData*> joints;
		joints.push_back(leg);
		m_renderMesh->getChildTransforms(leg, joints);

		if (joints.size() >= 2)
		{
			core::vector3df objectPosition = m_gameObject->getPosition();
			core::vector3df legPosition = leg->World.getTranslation();

			core::vector3df targetVector = legPosition - objectPosition;
			core::vector3df targetPosition = objectPosition + targetVector * m_targetDistance;

			// project target on ground
			targetPosition.Y = 0.0f;

			CLeg* leg = new CLeg(root, joints, targetVector, targetPosition, m_footStepLength, m_stepHeight, m_stepTime);
			m_legs.push_back(leg);
			return leg;
		}
	}

	return NULL;
}

void CLegController::lateUpdate()
{
	if (m_gameObject->isVisible() == false)
		return;

	core::vector3df legPosition, targetPosition, footTarget;
	core::vector3df objectPosition = m_gameObject->getPosition();
	core::quaternion objectRotation = m_gameObject->getRotation();
	core::vector3df up = m_gameObject->getUp();

	const core::matrix4& worldTransform = m_gameObject->getWorldTransform();

	float timestepSec = getTimeStep() / 1000.0f;
	float balanceStandingStep2 = 0.1f * 0.1f;
	float footStep2 = m_footStepLength * m_footStepLength;

	core::vector3df moveVector = objectPosition - m_lastPosition;
	if (moveVector.getLengthSQ() > 0)
	{
		// walk
		moveVector.normalize();
		m_moveTime = m_moveTime + timestepSec;
		m_standTime = 0.0f;
		m_rotTime = 0.0f;
		m_rotDirection = 0.0f;
	}
	else
	{
		m_moveTime = m_moveTime - timestepSec;
		if (objectRotation != m_lastRotation)
		{
			// stand & rotate
			m_standTime = 0;
			m_rotTime = m_rotTime + timestepSec;
		}
		else
		{
			// stand
			m_rotTime = 0.0f;
			m_rotDirection = 0.0f;
			m_standTime = m_standTime + timestepSec;
		}
	}

	m_moveTime = core::clamp(m_moveTime, 0.0f, 0.1f);
	m_rotTime = core::clamp(m_rotTime, 0.0f, 0.1f);

	for (CLeg* leg : m_legs)
	{
		bool allowStep = true;
		std::vector<CLeg*>& link = leg->getLink();
		for (CLeg* l : link)
		{
			// wait near leg finish 80% cycle
			if (l->getAnimTime() < m_stepTime * 0.8)
				allowStep = false;
		}

		if (allowStep)
		{
			// find the target foot step
			const core::vector3df& footTarget = leg->getFootTargetPosition();

			core::vector3df targetVector = leg->getTargetVector();
			worldTransform.rotateVect(targetVector);

			CVector::projectOnPlane(targetVector, up);

			targetPosition = objectPosition + targetVector * m_targetDistance;
			targetPosition += moveVector * (m_moveTime / 0.1f) * m_moveStepDistance;

			if (m_drawDebug)
			{
				CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();
				debug->addPosition(targetPosition, 0.1f, SColor(255, 255, 0, 255));
				debug->addPosition(footTarget, 0.1f, SColor(255, 0, 255, 0));

				if (leg->getAnimTime() > m_stepTime)
					debug->addLine(footTarget, footTarget + moveVector * m_footStepLength * 2.0f, SColor(255, 255, 0, 0));
			}

			// project target on ground
			targetPosition.Y = 0.0f;

			float stepDistance = footTarget.getDistanceFromSQ(targetPosition);
			if (stepDistance >= footStep2)
			{
				if (m_rotTime > 0)
				{
					// just rotate
					core::vector3df test = targetPosition - footTarget;
					core::vector3df cross = targetVector.crossProduct(up);

					m_rotDirection = test.dotProduct(cross);
					if (m_rotDirection < 0.0f)
						cross *= -1.0f;

					moveVector = cross;
					moveVector.normalize();
				}

				// next step position (* 0.98f to fix bug foot target is blink)
				leg->setFootTargetPosition(targetPosition + moveVector * m_footStepLength * 0.98f);
			}
			else if (m_standTime > 0.5f && stepDistance >= balanceStandingStep2)
			{
				// balanced standing
				leg->setFootTargetPosition(targetPosition);
			}
		}

		leg->lateUpdate();
	}

	m_lastPosition = objectPosition;
	m_lastRotation = objectRotation;
}