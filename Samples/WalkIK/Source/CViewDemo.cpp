#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "CImguiManager.h"

CViewDemo::CViewDemo() :
	m_state(StateController),
	m_robot(NULL),
	m_robotReplay(NULL),
	m_legController(NULL),
	m_legReplayer(NULL),
	m_recorder(NULL),
	m_3rdCamera(NULL)
{
	m_recorder = new CRecorder();
	m_messageBox = "";
	strcpy(m_fileName, "walk.sanim");
}

CViewDemo::~CViewDemo()
{
	delete m_recorder;
}

void CViewDemo::onInit()
{
	CEventManager::getInstance()->registerEvent("ViewDemo", this);

	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	m_3rdCamera = camera->getGameObject()->getComponent<C3rdCamera>();

	CZone* zone = context->getActiveZone();
	zone->updateIndexSearchObject();

	m_robot = zone->searchObject(L"robot");
	if (m_robot)
		m_legController = m_robot->getComponent<CLegController>();

	m_robotReplay = zone->searchObject(L"robot-replay");
	if (m_robotReplay)
		m_legReplayer = m_robotReplay->getComponent<CLegReplayer>();
}

void CViewDemo::onDestroy()
{
	CEventManager::getInstance()->unRegisterEvent(this);
}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
	{
		// update scene
		scene->update();
	}

	CImguiManager::getInstance()->onNewFrame();

	// record frame
	switch (m_state)
	{
	case StateRecording:
		if (m_legController)
			m_recorder->addFrame(m_legController);
		break;
	default:
		break;
	}

	// draw record frame
	if (m_recorder->getFrameCount())
		m_recorder->draw();
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

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 80), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Walk IK Demo", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		// Show FPS
		int fps = getIrrlichtDevice()->getVideoDriver()->getFPS();
		ImGui::Text("FPS: %d", fps);

		if (m_state != StateEdit)
			ImGui::Text("Press ASDW or Up,Down,Left,Right to walk");
		else
			ImGui::Text("Editing the recorded animation");

		ImGui::Spacing();

		if (m_legController &&
			m_legController->getLegs().size() > 0 &&
			m_legReplayer &&
			m_legReplayer->getLegs().size() > 0)
		{
			switch (m_state)
			{
			case StateController:
				if (ImGui::Button("Reset transform"))
				{
					CTransformEuler* euler = m_robot->getTransformEuler();
					euler->setPosition(core::vector3df());
					euler->setRotation(core::vector3df());
				}

				ImGui::Separator();

				if (m_recorder->getFrameCount() > 0)
				{
					if (ImGui::Button("Edit/Export"))
					{
						m_state = StateEdit;
						m_robotReplay->setVisible(true);
						m_robot->setVisible(false);

						m_legReplayer->setRecorder(m_recorder);

						if (m_3rdCamera)
							m_3rdCamera->setFollowTarget(m_robotReplay);
					}
				}

				if (ImGui::Button("Record"))
				{
					m_state = StateRecording;
					m_recorder->start();
				}

				break;
			case StateRecording:
				if (ImGui::Button("Stop"))
				{
					m_state = StateController;
				}
				break;
			case StateEdit:
			{
				bool b = m_legReplayer->isRootMotion();
				if (ImGui::Checkbox("Root motion", &b))
					m_legReplayer->setRootMotion(b);

				ImGui::SliderFloat2("Clip", m_recorder->getClip(), 0.0f, 1.0f);

				ImGui::InputText("Anim name", m_fileName, 100);

				size_t len = strlen(m_fileName);
				bool disable = len == 0;
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disable);
				if (ImGui::Button("Export"))
				{
					if (m_legReplayer->exportAnim(m_fileName, CContext::getInstance()->getScene()) == true)
					{
						m_messageBox = "Export animation success at Bin folder!";
						ImGui::OpenPopup("Message box");
					}
					else
					{
						m_messageBox = "Export animation failed!";
						ImGui::OpenPopup("Message box");
					}
				}
				ImGui::PopItemFlag();

				if (ImGui::BeginPopupModal("Message box", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text(m_messageBox);
					ImGui::Separator();
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					if (ImGui::Button("Close"))
						ImGui::CloseCurrentPopup();
					ImGui::PopStyleVar();
					ImGui::EndPopup();
				}

				if (ImGui::Button("Exit"))
				{
					m_state = StateController;
					m_robotReplay->setVisible(false);
					m_robot->setVisible(true);

					if (m_3rdCamera)
						m_3rdCamera->setFollowTarget(m_robot);
				}
			}
			break;
			default:
				break;
			}
		}

		ImGui::End();
	}
}

void CViewDemo::onPostRender()
{

}

bool CViewDemo::OnEvent(const SEvent& event)
{
	return false;
}
