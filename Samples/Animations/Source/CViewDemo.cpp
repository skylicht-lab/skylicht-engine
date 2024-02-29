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
	m_modify(true)
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

	CScene* scene = CContext::getInstance()->getScene();
	scene->getZone(0)->updateIndexSearchObject();
	CGameObject* character = scene->searchObjectInChild(L"Character");
	if (character)
	{
		m_animController = character->getComponent<CAnimationController>();
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

		// calc move speed vector
		core::vector3df moveSpeed = rot * forward;
		moveSpeed.normalize();
		moveSpeed *= m_movementSpeed;

		// calc animation weight
		m_movementBlending.sampleWeightsPolar(moveSpeed);

		// apply weight for movement skeleton
		core::array<CGradientBandInterpolation::SSample*>& allSamples = m_movementBlending.getSamples();
		for (u32 i = 0, n = allSamples.size(); i < n; i++)
		{
			CGradientBandInterpolation::SSample* sample = allSamples[i];

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

	ImGui::End();
}