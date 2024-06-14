#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "imgui.h"
#include "CImguiManager.h"

CViewDemo::CViewDemo() :
	m_isRecording(false),
	m_legController(NULL),
	m_recorder(NULL)
{
	m_recorder = new CRecorder();
}

CViewDemo::~CViewDemo()
{
	delete m_recorder;
}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CEventManager::getInstance()->registerEvent("ViewDemo", this);

	CZone* zone = context->getActiveZone();
	zone->updateIndexSearchObject();

	CGameObject* robot = zone->searchObject(L"robot");
	if (robot)
		m_legController = robot->getComponent<CLegController>();
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
	if (m_legController && m_isRecording)
		m_recorder->addFrame(m_legController);

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
		ImGui::Text("Press ASDW or Up,Down,Left,Right to walk");
		ImGui::Spacing();

		if (m_legController && m_legController->getLegs().size() > 0)
		{
			if (m_isRecording)
			{
				if (ImGui::Button("Stop recording"))
				{
					m_isRecording = false;
				}
			}
			else
			{
				if (ImGui::Button("Record animation"))
				{
					m_isRecording = true;
					m_recorder->start();
				}
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
