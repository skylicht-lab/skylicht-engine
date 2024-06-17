#include "pch.h"
#include "CLegReplayer.h"

#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"
#include "Animation/CAnimationClip.h"
#include "Animation/CAnimationManager.h"

#include "Debug/CSceneDebug.h"
#include "Utils/CVector.h"

CLegReplayer::CLegReplayer() :
	m_drawDebug(false),
	m_renderMesh(NULL),
	m_root(NULL),
	m_rootMotion(false),
	m_recoder(NULL),
	m_frame(0.0f),
	m_pause(false),
	m_frameFrom(0.0f),
	m_frameTo(0.0f),
	m_duration(0.0f)
{

}

CLegReplayer::~CLegReplayer()
{
	for (CLegIK* leg : m_legs)
		delete leg;
	m_legs.clear();

	CWorldTransformSystem* system = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	system->unRegisterLateUpdate(this);
}

void CLegReplayer::initComponent()
{
	m_renderMesh = m_gameObject->getComponent<CRenderMesh>();

	CWorldTransformSystem* system = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	system->registerLateUpdate(this);
}

void CLegReplayer::setRecorder(CRecorder* recoder)
{
	m_recoder = recoder;
	m_duration = recoder->getDuration();
	m_frameFrom = 0.0f;
	m_frameTo = m_duration;
	m_frame = 0.0f;
}

void CLegReplayer::updateComponent()
{
	if (!m_recoder || m_recoder->getFrameCount() == 0)
		return;

	float* clips = m_recoder->getClip();
	m_frameFrom = m_duration * clips[0];
	m_frameTo = m_duration * clips[1];
	if (m_frameFrom > m_frameTo)
		m_frameFrom = m_frameTo;

	if (!m_pause)
	{
		m_frame = m_frame + getTimeStep() * 0.001f;
		if (m_frame >= m_frameTo || m_frame < m_frameFrom)
			m_frame = m_frameFrom;
	}

	SRecordFrame frame = m_recoder->getFrameData(m_frame);
	int i = 0;

	// hip animation
	if (m_root)
		m_root->Relative.setTranslation(frame.HipRelativePosition);

	// object position/rotation
	CTransformEuler* transform = m_gameObject->getTransformEuler();
	transform->setRotation(frame.ObjectRotation);
	if (m_rootMotion)
		transform->setPosition(frame.ObjectPosition);
	else
		transform->setPosition(core::vector3df());

	// foot animation
	for (CLegIK* leg : m_legs)
	{
		// foot animation
		if (m_rootMotion)
			leg->setFootPosition(frame.FootPosition[i++]);
		else
			leg->setFootPosition(frame.FootPosition[i++] - frame.ObjectPosition);
	}
}

CLegIK* CLegReplayer::addLeg(CWorldTransformData* root, CWorldTransformData* leg)
{
	if (m_renderMesh && leg)
	{
		m_root = root;

		std::vector<CWorldTransformData*> joints;
		joints.push_back(leg);
		m_renderMesh->getChildTransforms(leg, joints);

		if (joints.size() >= 2)
		{
			CLegIK* leg = new CLegIK(root, joints);
			m_legs.push_back(leg);
			return leg;
		}
	}

	return NULL;
}

void CLegReplayer::lateUpdate()
{
	if (m_gameObject->isVisible() == false)
		return;

	for (CLegIK* leg : m_legs)
		leg->lateUpdate();
}

bool CLegReplayer::exportAnim(const char* anim, CScene* scene)
{
	CEntityManager* entityManager = scene->getEntityManager();

	float* clips = m_recoder->getClip();
	m_frameFrom = m_duration * clips[0];
	m_frameTo = m_duration * clips[1];
	if (m_frameFrom > m_frameTo)
		m_frameFrom = m_frameTo;

	CAnimationClip* clip = new CAnimationClip();

	std::vector<SEntityAnim*> anims;
	std::vector<CWorldTransformData*> transforms;

	SEntityAnim* rootAnim = new SEntityAnim();
	rootAnim->Name = m_root->Name;
	rootAnim->Data.Scales.Default.set(1.0f, 1.0f, 1.0f);
	clip->addAnim(rootAnim);

	anims.push_back(rootAnim);
	transforms.push_back(m_root);

	for (CLegIK* leg : m_legs)
	{
		std::vector<CWorldTransformData*>& joints = leg->getJoints();

		for (CWorldTransformData* j : joints)
		{
			SEntityAnim* jointAnim = new SEntityAnim();
			jointAnim->Name = j->Name;
			jointAnim->Data.Scales.Default.set(1.0f, 1.0f, 1.0f);
			clip->addAnim(jointAnim);

			anims.push_back(jointAnim);
			transforms.push_back(j);
		}
	}

	CRotationKey keyRot;
	CPositionKey keyPos;
	core::matrix4 parentWorldInv, relative;
	core::quaternion rot;
	core::vector3df pos;
	size_t numNodes = anims.size();

	float delta = 1.0f / 30.0f;
	for (float t = m_frameFrom; t < m_frameTo; t = t + delta)
	{
		SRecordFrame frame = m_recoder->getFrameData(t);
		int i = 0;

		// hip animation
		if (m_root)
			m_root->Relative.setTranslation(frame.HipRelativePosition);

		// object position/rotation
		CTransformEuler* transform = m_gameObject->getTransformEuler();
		transform->setRotation(frame.ObjectRotation);
		if (m_rootMotion)
			transform->setPosition(frame.ObjectPosition);
		else
			transform->setPosition(core::vector3df());

		// foot animation
		for (CLegIK* leg : m_legs)
		{
			// foot animation
			if (m_rootMotion)
				leg->setFootPosition(frame.FootPosition[i++]);
			else
				leg->setFootPosition(frame.FootPosition[i++] - frame.ObjectPosition);
		}

		// update frame
		entityManager->update();

		// time frame
		keyRot.Frame = t;
		keyPos.Frame = t;

		// get animation key value
		for (size_t j = 0; j < numNodes; j++)
		{
			if (transforms[j]->Parent)
			{
				parentWorldInv = transforms[j]->Parent->World;
				parentWorldInv.makeInverse();
				relative = transforms[j]->World * parentWorldInv;
			}
			else
			{
				relative = transforms[j]->World;
			}

			getAnimationTransform(relative, keyPos.Value, keyRot.Value);

			// add rotation key
			CAnimationData& animData = anims[j]->Data;
			if (animData.Rotations.size() == 0)
			{
				animData.Rotations.Default = keyRot.Value;
				animData.Rotations.Data.push_back(keyRot);
			}
			else
			{
				if (animData.Rotations.Data.getLast().Value != keyRot.Value)
					animData.Rotations.Data.push_back(keyRot);
			}

			// add position key
			if (animData.Positions.size() == 0)
			{
				animData.Positions.Default = keyPos.Value;
				animData.Positions.Data.push_back(keyPos);
			}
			else
			{
				if (animData.Positions.Data.getLast().Value != keyPos.Value)
					animData.Positions.Data.push_back(keyPos);
			}
		}
	}

	// remove if animation have 1 key
	for (size_t j = 0; j < numNodes; j++)
	{
		CAnimationData& animData = anims[j]->Data;
		if (animData.Positions.size() == 1)
			animData.Positions.Data.clear();
		if (animData.Rotations.size() == 1)
			animData.Rotations.Data.clear();
	}

	bool result = CAnimationManager::getInstance()->exportAnimation(clip, anim);

	delete clip;
	return result;
}

void CLegReplayer::getAnimationTransform(const core::matrix4& mat, core::vector3df& position, core::quaternion& rotation)
{
	core::vector3df front = Transform::Oz;
	core::vector3df up = Transform::Oy;

	mat.rotateVect(front);
	mat.rotateVect(up);

	front.normalize();
	up.normalize();

	core::vector3df right = up.crossProduct(front);
	right.normalize();

	core::matrix4 rotationMatrix;
	f32* matData = rotationMatrix.pointer();
	matData[0] = right.X;
	matData[1] = right.Y;
	matData[2] = right.Z;
	matData[3] = 0.0f;

	matData[4] = up.X;
	matData[5] = up.Y;
	matData[6] = up.Z;
	matData[7] = 0.0f;

	matData[8] = front.X;
	matData[9] = front.Y;
	matData[10] = front.Z;
	matData[11] = 0.0f;

	matData[12] = 0.0f;
	matData[13] = 0.0f;
	matData[14] = 0.0f;
	matData[15] = 1.0f;

	rotation = core::quaternion(rotationMatrix);
	position = mat.getTranslation();
}