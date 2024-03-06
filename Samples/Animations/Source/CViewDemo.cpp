#include "pch.h"
#include "CViewDemo.h"
#include "AnimationDefine.h"
#include "CImguiManager.h"
#include "imgui.h"
#include "Context/CContext.h"

CViewDemo::CViewDemo() :
	m_animController(NULL),
	m_movementSpeed(0.0f),
	m_movementAngle(0.0f),
	m_aim(0.0f),
	m_aimUpDown(0.0f),
	m_aimLeftRight(0.0f),
	m_modify(true),
	m_aimPosition(0.0f, 1.5f, 2.0f)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	core::vector3df back(0.0f, 0.0f, -1.0f);
	core::vector3df forward(0.0f, 0.0f, 1.0f);
	core::vector3df left(-1.0f, 0.0f, 0.0f);
	core::vector3df right(1.0f, 0.0f, 0.0f);

	float walkSpeed = 1.0f;
	float runSpeed = 2.0f;

	m_movementBlending.addSample((int)EAnimationId::Idle, core::vector3df());
	m_movementBlending.addSample((int)EAnimationId::WalkForward, forward * walkSpeed);
	m_movementBlending.addSample((int)EAnimationId::WalkLeft, left * walkSpeed);
	m_movementBlending.addSample((int)EAnimationId::WalkBack, back * walkSpeed);
	m_movementBlending.addSample((int)EAnimationId::WalkRight, right * walkSpeed);
	m_movementBlending.addSample((int)EAnimationId::RunForward, forward * runSpeed);
	m_movementBlending.addSample((int)EAnimationId::RunLeft, left * runSpeed);
	m_movementBlending.addSample((int)EAnimationId::RunBack, back * runSpeed);
	m_movementBlending.addSample((int)EAnimationId::RunRight, right * runSpeed);


	m_aimBlending.addSample((int)EAnimationId::AimStraight, core::vector3df());
	m_aimBlending.addSample((int)EAnimationId::AimUp, core::vector3df(0.0f, 1.0f, 0.0f));
	m_aimBlending.addSample((int)EAnimationId::AimDown, core::vector3df(0.0f, -1.0f, 0.0f));
	m_aimBlending.addSample((int)EAnimationId::AimLeft, core::vector3df(-1.0f, 0.0f, 0.0f));
	m_aimBlending.addSample((int)EAnimationId::AimRight, core::vector3df(1.0f, 0.0f, 0.0f));


	CScene* scene = CContext::getInstance()->getScene();
	scene->getZone(0)->updateIndexSearchObject();
	CGameObject* character = scene->searchObjectInChild(L"Character");
	if (character)
	{
		m_animController = character->getComponent<CAnimationController>();

		// set mask 1.0f for aim joint from spine3
		CSkeleton* aim = m_animController->getSkeleton((int)EAnimationId::Aim);
		aim->setJointWeights(0.0f);
		aim->setJointWeights("Spine", 1.0f, true);
		aim->setLayerType(CSkeleton::Replace);

		m_handIK = character->getComponent<CIKHand>();
	}
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	CImguiManager::getInstance()->onNewFrame();

	if (m_modify && m_animController)
	{
		core::vector3df forward(0.0f, 0.0f, 1.0f);
		core::quaternion rot;
		rot.fromAngleAxis(m_movementAngle * core::DEGTORAD, core::vector3df(0.0f, 1.0f, 0.0f));

		// MOVEMENT
		// calc move speed vector
		core::vector3df moveSpeed = rot * forward;
		moveSpeed.normalize();
		moveSpeed *= m_movementSpeed;

		// calc animation weight
		m_movementBlending.sampleWeightsPolar(moveSpeed);

		// apply weight for movement skeleton
		core::array<CGradientBandInterpolation::SSample*>& movementSamples = m_movementBlending.getSamples();
		for (u32 i = 0, n = movementSamples.size(); i < n; i++)
		{
			CGradientBandInterpolation::SSample* sample = movementSamples[i];

			// update animation weight
			m_animController->getSkeleton(sample->Id)->getTimeline().Weight = sample->Weight;
		}

		// UPPER AIM WEIGHT
		m_animController->getSkeleton((int)EAnimationId::Aim)->getTimeline().Weight = m_aim;

		// IK AIM
		m_handIK->setAimTarget(m_aimPosition);

		// AIM UP/DOWN/LEFT/RIGHT
		m_aimLeftRight = 0.0f;
		m_aimUpDown = 0.0f;

		core::vector3df aimVector(m_aimLeftRight, m_aimUpDown, 0.0f);
		if (aimVector.getLength() > 1.0f)
			aimVector.normalize();

		// calc animation weight for aim
		m_aimBlending.sampleWeightsPolar(aimVector);

		core::array<CGradientBandInterpolation::SSample*>& aimSamples = m_aimBlending.getSamples();
		for (u32 i = 0, n = aimSamples.size(); i < n; i++)
		{
			CGradientBandInterpolation::SSample* sample = aimSamples[i];

			// update animation weight
			m_animController->getSkeleton(sample->Id)->getTimeline().Weight = sample->Weight;
		}

		m_modify = false;
	}
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::onPostRender()
{

}


void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(735, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(420, 200), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Animation Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	float minSize;
	float maxSize;

	if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
	{
		minSize = 0.0f;
		maxSize = 2.0f;
		if (ImGui::SliderFloat("Speed", &m_movementSpeed, minSize, maxSize, "%.3f"))
		{
			m_modify = true;
		}

		minSize = 0.0f;
		maxSize = 360.0f;
		if (ImGui::SliderFloat("Move angle", &m_movementAngle, minSize, maxSize, "%.3f deg"))
		{
			m_modify = true;
		}

		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Aim", ImGuiTreeNodeFlags_DefaultOpen))
	{
		minSize = 0.0f;
		maxSize = 1.0f;

		if (ImGui::SliderFloat("Aim", &m_aim, minSize, maxSize, "%.3f"))
		{
			m_modify = true;
		}

		minSize = 1.0f;
		maxSize = 5.0f;

		float aimPos[3];
		aimPos[0] = m_aimPosition.X;
		aimPos[1] = m_aimPosition.Y;
		aimPos[2] = m_aimPosition.Z;

		if (ImGui::SliderFloat3("Aim position", aimPos, minSize, maxSize, "%.3f"))
		{
			m_aimPosition.X = aimPos[0];
			m_aimPosition.Y = aimPos[1];
			m_aimPosition.Z = aimPos[2];
			m_modify = true;
		}
		ImGui::Spacing();
	}

	ImGui::End();
}