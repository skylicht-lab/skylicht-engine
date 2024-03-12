#include "pch.h"
#include "CIKHand.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

CIKHand::CIKHand() :
	m_gun(NULL),
	m_aimWeight(0.0f),
	m_enable(true),
	m_drawSekeleton(false),
	m_skeleton(NULL)
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
	if (!m_enable)
		return;

	// notify late update
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
	if (!m_enable)
		return;

	CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();

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

	core::vector3df aimCenter = (right[0] + left[0]) * 0.5f;

	if (m_gun)
	{
		const core::matrix4& gunRelative = m_gun->WorldTransform->Relative;
		const core::matrix4& gunWorld = m_gun->WorldTransform->World;

		core::vector3df gunFront(1.0f, 0.0f, 0.0f);
		gunWorld.rotateVect(gunFront);
		gunFront.normalize();

		core::vector3df gunPos = gunWorld.getTranslation();
		core::vector3df gunVec = gunPos - aimCenter;

		// core::vector3df gunHandOffset = gunPos - right[2];
		core::vector3df gunHandOffset(-0.0254051760, 0.107825637, 0.114578426); // <- by default animation

		// get aim vector
		core::vector3df aimVec = m_aimTarget - aimCenter;
		aimVec.normalize();

		debug->addLine(aimCenter, aimCenter + aimVec * 2.0f, SColor(255, 0, 0, 100));

		// predict gun position by aim vector
		core::vector3df predictGunPos = aimCenter + aimVec * gunVec.getLength();

		// core::vector3df gunAdjustOffset = predictGunPos - gunPos;
		core::vector3df gunAdjustOffset(-0.0412531942f, -0.0346513987f, 0.00291103125f); // <- by default animation

		// rotate offset by world
		core::vector3df offset = gunHandOffset + gunAdjustOffset;

		core::vector3df upVec(0.0f, 1.0f, 0.0f);
		core::vector3df rightVec = upVec.crossProduct(aimVec);
		upVec = aimVec.crossProduct(rightVec);

		core::matrix4 rotationMatrix;
		f32* matData = rotationMatrix.pointer();

		matData[0] = rightVec.X;
		matData[1] = rightVec.Y;
		matData[2] = rightVec.Z;
		matData[3] = 0.0f;

		matData[4] = upVec.X;
		matData[5] = upVec.Y;
		matData[6] = upVec.Z;
		matData[7] = 0.0f;

		matData[8] = aimVec.X;
		matData[9] = aimVec.Y;
		matData[10] = aimVec.Z;
		matData[11] = 0.0f;

		matData[12] = 0.0f;
		matData[13] = 0.0f;
		matData[14] = 0.0f;
		matData[15] = 1.0f;

		rotationMatrix.rotateVect(offset);

		core::vector3df predictRHandPos = predictGunPos - offset;

		debug->addSphere(predictGunPos, 0.02f, SColor(255, 255, 255, 0));
		debug->addSphere(predictRHandPos, 0.02f, SColor(255, 0, 255, 255));

		// blending by aim ratio
		predictRHandPos = right[2] * (1.0f - m_aimWeight) + predictRHandPos * m_aimWeight;

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

		// solve ik in right hand		
		core::quaternion qStart, qMid;
		m_ikRightHand.solveIK(right[0], right[1], right[2], targetRight, rightPole, 1.0f, qStart, qMid);

		m_rightHand.Start->AnimRotation *= qStart;
		m_rightHand.Start->updateTransform();

		m_rightHand.Mid->AnimRotation *= qMid;
		m_rightHand.Mid->updateTransform();
	}

	if (m_skeleton && m_drawSekeleton)
		m_skeleton->drawDebug(core::IdentityMatrix, SColor(255, 255, 255, 255));
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