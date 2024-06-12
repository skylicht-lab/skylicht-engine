#include "pch.h"
#include "CLegController.h"

#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

#include "Debug/CSceneDebug.h"
#include "Utils/CVector.h"

CLegController::CLegController() :
	m_drawDebug(false),
	m_renderMesh(NULL),
	m_targetDistance(3.3f),
	m_footStepLength(0.3f),
	m_stepHeight(0.2f),
	m_stepTime(0.25f)
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
	for (CLeg* leg : m_legs)
		leg->update();
}

CLeg* CLegController::addLeg(CWorldTransformData* root, CWorldTransformData* leg)
{
	if (m_renderMesh && leg)
	{
		std::vector<CWorldTransformData*> joints;
		joints.push_back(leg);
		m_renderMesh->getChildTransforms(leg, joints);

		if (joints.size() >= 3)
		{
			core::vector3df objectPosition = m_gameObject->getPosition();

			core::vector3df legPosition = joints[0]->World.getTranslation();
			core::vector3df targetVector = legPosition - objectPosition;

			core::vector3df targetPosition = objectPosition + targetVector * m_targetDistance;
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

	core::vector3df legPosition, targetPosition, currentFootPosition, footTarget;
	core::vector3df objectPosition = m_gameObject->getPosition();

	CWorldTransformData* worldTransform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);

	core::vector3df up = Transform::Oy;
	core::vector3df front = Transform::Oz;
	worldTransform->World.rotateVect(up);
	worldTransform->World.rotateVect(front);

	for (CLeg* leg : m_legs)
	{
		bool allowStep = true;
		std::vector<CLeg*>& link = leg->getLink();
		for (CLeg* l : link)
		{
			if (l->getAnimTime() < m_stepTime * 0.8f)
				allowStep = false;
		}

		core::vector3df targetVector = leg->getTargetVector();
		worldTransform->World.rotateVect(targetVector);

		CVector::projectOnPlane(targetVector, up);

		targetPosition = objectPosition + targetVector * m_targetDistance;

		if (m_drawDebug)
			CSceneDebug::getInstance()->addPosition(targetPosition, 0.1f, SColor(255, 255, 0, 255));

		// project target to ground
		targetPosition.Y = 0.0f;

		core::vector3df moveVector = objectPosition - m_lastPosition;

		if (moveVector.getLengthSQ() > 0)
			moveVector.normalize();

		// change next step
		currentFootPosition = leg->getFootTargetPosition();
		if (currentFootPosition.getDistanceFromSQ(targetPosition) >= m_footStepLength * m_footStepLength && allowStep)
		{
			if (moveVector.getLengthSQ() == 0)
			{
				core::vector3df test = targetPosition - currentFootPosition;
				core::vector3df cross = targetVector.crossProduct(up);

				float f = test.dotProduct(cross);
				if (f < 0.0f)
					cross *= -1.0f;

				moveVector = cross;
				moveVector.normalize();
			}

			// next step position (* 0.98f to fix bug foot target is blink)
			leg->setFootTargetPosition(targetPosition + moveVector * m_footStepLength * 0.98f);
		}

		leg->lateUpdate();
	}

	m_lastPosition = objectPosition;
}