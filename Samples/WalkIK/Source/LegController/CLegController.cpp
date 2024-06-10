#include "pch.h"
#include "CLegController.h"

#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

#include "Debug/CSceneDebug.h"

CLegController::CLegController() :
	m_drawDebug(true),
	m_renderMesh(NULL),
	m_targetDistance(3.3f),
	m_footStepLength(0.3f)
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

void CLegController::addLeg(CWorldTransformData* leg)
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

			CLeg* leg = new CLeg(joints, targetVector, targetPosition, m_footStepLength);
			m_legs.push_back(leg);
		}
	}
}

void CLegController::lateUpdate()
{
	if (m_gameObject->isVisible() == false)
		return;

	core::vector3df legPosition, targetPosition, footPosition, footTarget;
	core::vector3df objectPosition = m_gameObject->getPosition();

	CWorldTransformData* worldTransform = GET_ENTITY_DATA(m_gameObject->getEntity(), CWorldTransformData);

	for (CLeg* leg : m_legs)
	{
		core::vector3df targetVector = leg->getTargetVector();
		worldTransform->World.rotateVect(targetVector);

		targetPosition = objectPosition + targetVector * m_targetDistance;

		if (m_drawDebug)
			CSceneDebug::getInstance()->addPosition(targetPosition, 0.1f, SColor(255, 255, 0, 255));

		targetPosition.Y = 0.0f;

		if (m_drawDebug)
			CSceneDebug::getInstance()->addPosition(targetPosition, 0.1f, SColor(255, 0, 0, 255));

		footPosition = leg->getFootTargetPosition();

		if (footPosition.getDistanceFromSQ(targetPosition) >= m_footStepLength * m_footStepLength)
		{
			core::vector3df moveVector = targetPosition - footPosition;
			moveVector.normalize();

			footTarget = footPosition + moveVector * m_footStepLength * 2.0f;
			leg->setFootTargetPosition(footTarget);
		}

		if (m_drawDebug)
			CSceneDebug::getInstance()->addPosition(leg->getFootTargetPosition(), 0.1f, SColor(255, 0, 255, 0));

		leg->lateUpdate();
	}
}