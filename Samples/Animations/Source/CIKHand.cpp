#include "pch.h"
#include "CIKHand.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

CIKHand::CIKHand() :
	m_gun(NULL),
	m_aimWeight(0.0f)
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
	// WIP not yet done
	/*
	CSceneDebug* debug = CSceneDebug::getInstance();

	core::vector3df left[3];
	left[0] = m_leftHand.Start->WorldTransform->World.getTranslation();
	left[1] = m_leftHand.Mid->WorldTransform->World.getTranslation();
	left[2] = m_leftHand.End->WorldTransform->World.getTranslation();

	core::vector3df right[3];
	right[0] = m_rightHand.Start->WorldTransform->World.getTranslation();
	right[1] = m_rightHand.Mid->WorldTransform->World.getTranslation();
	right[2] = m_rightHand.End->WorldTransform->World.getTranslation();

	core::matrix4 rightHandWorldInv = m_rightHand.Start->WorldTransform->World;
	rightHandWorldInv.makeInverse();

	core::matrix4 leftHandWorldInv = m_leftHand.Start->WorldTransform->World;
	leftHandWorldInv.makeInverse();

	const core::matrix4& handWorld = m_rightHand.End->WorldTransform->World;

	// debug aim vector
	core::vector3df aimCenter = (right[0] + left[0]) * 0.5f;
	CSceneDebug::getInstance()->addSphere(m_aimTarget, 0.02f, SColor(255, 255, 0, 0));

	if (m_gun)
	{
		const core::matrix4& gunRelative = m_gun->WorldTransform->Relative;
		const core::matrix4& gunWorld = m_gun->WorldTransform->World;

		core::vector3df gunFront(1.0f, 0.0f, 0.0f);
		gunWorld.rotateVect(gunFront);
		gunFront.normalize();

		// show current gun vector
		core::vector3df gunPos = gunWorld.getTranslation();
		CSceneDebug::getInstance()->addLine(gunPos, gunPos + gunFront * 0.5f, SColor(255, 100, 100, 100));

		// get aim vector
		core::vector3df currentAimVector = gunPos - right[0];
		core::vector3df aimVec = m_aimTarget - aimCenter;
		aimVec.normalize();

		// predict gun position by aim vector
		core::vector3df predictGunPos = aimCenter + aimVec * 0.58f;
		CSceneDebug::getInstance()->addSphere(predictGunPos, 0.02f, SColor(255, 255, 255, 255));

		core::vector3df gunOffset = -(gunRelative.getTranslation());
		handWorld.rotateVect(gunOffset);

		core::vector3df predictRHandPos = predictGunPos + gunOffset;
		CSceneDebug::getInstance()->addSphere(predictRHandPos, 0.02f, SColor(255, 255, 255, 0));

		for (int i = 0; i < 3; i++)
		{
			rightHandWorldInv.transformVect(right[i]);
			leftHandWorldInv.transformVect(left[i]);
		}

		core::vector3df rightPole = getPoleVec(right);
		rightPole.normalize();

		core::vector3df leftPole = getPoleVec(left);
		leftPole.normalize();

		core::vector3df targetRight;
		rightHandWorldInv.transformVect(targetRight, predictRHandPos);

		// ik in right hand
		core::quaternion qStart, qMid;
		m_ikRightHand.solveIK(right[0], right[1], right[2], targetRight, rightPole, 1.0f, qStart, qMid);

		m_rightHand.Start->AnimRotation *= qStart;
		m_rightHand.Start->updateTransform();

		m_rightHand.Mid->AnimRotation *= qMid;
		m_rightHand.Mid->updateTransform();
	}
	*/
}

core::vector3df CIKHand::getPoleVec(const core::vector3df* p)
{
	core::vector3df pole = p[1] - p[0];
	core::vector3df endVec = p[2] - p[0];

	core::vector3df norm = pole.crossProduct(endVec);
	norm.normalize();

	core::quaternion q;
	q.fromAngleAxis(-45.0f * core::DEGTORAD, norm);

	pole = q * pole;
	return pole;
}