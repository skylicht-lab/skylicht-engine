#include "pch.h"
#include "CLegReplayer.h"

#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

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