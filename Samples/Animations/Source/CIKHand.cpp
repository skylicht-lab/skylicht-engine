#include "pch.h"
#include "CIKHand.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

CIKHand::CIKHand() :
	m_gun(NULL)
{

}

CIKHand::~CIKHand()
{
	CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	transformSystem->unRegisterLateUpdate(this);
}

void CIKHand::initComponent()
{
	CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	transformSystem->registerLateUpdate(this);
}

void CIKHand::updateComponent()
{
	// notify late update
	/*
	if (m_leftHand.Start)
		m_leftHand.Start->WorldTransform->HasLateChanged = true;
	if (m_leftHand.Mid)
		m_leftHand.Mid->WorldTransform->HasLateChanged = true;
	if (m_leftHand.End)
		m_leftHand.End->WorldTransform->HasLateChanged = true;

	if (m_rightHand.Start)
		m_rightHand.Start->WorldTransform->HasLateChanged = true;
	if (m_rightHand.Mid)
		m_rightHand.Mid->WorldTransform->HasLateChanged = true;
	if (m_rightHand.End)
		m_rightHand.End->WorldTransform->HasLateChanged = true;
	*/
}

void CIKHand::setGun(CAnimationTransformData* gun)
{
	m_gun = gun;
}

void CIKHand::setLeftHand(CAnimationTransformData* start, CAnimationTransformData* mid, CAnimationTransformData* end)
{
	m_leftHand.Start = start;
	m_leftHand.Mid = mid;
	m_leftHand.End = end;
}

void CIKHand::setRightHand(CAnimationTransformData* start, CAnimationTransformData* mid, CAnimationTransformData* end)
{
	m_rightHand.Start = start;
	m_rightHand.Mid = mid;
	m_rightHand.End = end;
}

void CIKHand::lateUpdate()
{
	CSceneDebug* debug = CSceneDebug::getInstance();

	core::vector3df left[3];
	left[0] = m_leftHand.Start->WorldTransform->World.getTranslation();
	left[1] = m_leftHand.Mid->WorldTransform->World.getTranslation();
	left[2] = m_leftHand.End->WorldTransform->World.getTranslation();

	core::vector3df right[3];
	right[0] = m_rightHand.Start->WorldTransform->World.getTranslation();
	right[1] = m_rightHand.Mid->WorldTransform->World.getTranslation();
	right[2] = m_rightHand.End->WorldTransform->World.getTranslation();

	// debug aim vector
	core::vector3df aimCenter = (right[0] + left[0]) * 0.5f;
	CSceneDebug::getInstance()->addLine(aimCenter, m_aimTarget, SColor(255, 255, 255, 255));

	// debug gun vector
	if (m_gun)
	{
		const core::matrix4& gunWorld = m_gun->WorldTransform->World;
		core::vector3df gunFront(1.0f, 0.0f, 0.0f);
		gunWorld.rotateVect(gunFront);
		gunFront.normalize();

		// show current gun vector
		core::vector3df gunPos = gunWorld.getTranslation();
		CSceneDebug::getInstance()->addLine(gunPos, gunPos + gunFront, SColor(255, 255, 0, 0));

		// get aim vector
		core::vector3df currentAimVector = gunPos - right[0];
		core::vector3df aimVec = m_aimTarget - aimCenter;
		aimVec.normalize();

		// predict gun position by aim vector
		core::vector3df predictGunPos = aimCenter + aimVec * currentAimVector.getLength() * 0.83f;
		CSceneDebug::getInstance()->addSphere(predictGunPos, 0.02f, SColor(255, 255, 255, 255));
	}
}