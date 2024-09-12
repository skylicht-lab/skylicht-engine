#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "imgui.h"
#include "CImguiManager.h"

CViewDemo::CViewDemo() :
	m_demoId(0),
	m_windDirection(0.0f)
{
	m_demo[0] = new CDemoCloth(&m_verlet);
	m_demo[1] = new CDemoTree(&m_verlet);
}

CViewDemo::~CViewDemo()
{
	delete m_demo[0];
	delete m_demo[1];
}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	initDemo();
}

void CViewDemo::onDestroy()
{

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

	updateDemo();

	m_wind.update(&m_verlet);

	m_verlet.update();

	m_verlet.drawDebug(m_demo[m_demoId]->isDrawingParticle());

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
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
	ImGui::SetNextWindowPos(ImVec2(850, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(280, 270), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Verlet Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		const char* demoName[] = {
			"Cloth",
			"Tree",
		};

		if (ImGui::Combo("Demo", &m_demoId, demoName, IM_ARRAYSIZE(demoName)))
		{
			initDemo();
		}

		if (ImGui::CollapsingHeader("Demo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_demo[m_demoId]->onGUI();
		}

		if (ImGui::CollapsingHeader("Wind", ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool enable = m_wind.isEnable();
			float scale = m_wind.getScale();
			float strong = m_wind.getStrong();
			float timeSpeed = m_wind.getTimeRatio();

			ImGui::Checkbox("Enable", &enable);
			ImGui::SliderFloat("Scale", &scale, 0.001f, 5.0f);
			ImGui::SliderFloat("Strong", &strong, 0.0f, 0.1f);
			ImGui::SliderFloat("Time speed", &timeSpeed, 0.2f, 2.0f);
			ImGui::SliderFloat("Direction", &m_windDirection, 0.0f, 360.0f);

			core::quaternion q;
			q.fromAngleAxis(m_windDirection * core::DEGTORAD, core::vector3df(0.0f, 1.0f, 0.0f));

			core::vector3df v(0.0f, 0.0f, 1.0f);
			v = q * v;

			m_wind.setEnable(enable);
			m_wind.setScale(scale);
			m_wind.setStrong(strong);
			m_wind.setTimeRatio(timeSpeed);
			m_wind.setDirection(v);
		}

		if (ImGui::Button("Reset"))
		{
			initDemo();
		}

		ImGui::End();
	}


	ImGui::SetNextWindowPos(ImVec2(850, 400), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(280, 130), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Wind", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		windowPos.x += 25.0f;
		windowPos.y += 25.0f;

		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.x -= 50.0f;
		windowSize.y -= 50.0f;

		ImVec2 windowEnd = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);

		ImDrawList* drawList = ImGui::GetForegroundDrawList();

		int numPoint = 30;

		ImVec2* points = new ImVec2[numPoint];

		float x = windowPos.x;
		float y = windowPos.y + windowSize.y;
		float step = windowSize.x / numPoint;

		float space = 0.1f;
		for (int i = 0; i < numPoint; i++)
		{
			float nosieY = i * space;
			float noise = m_wind.getNoiseValue(nosieY);
			points[i] = ImVec2(x, y - noise * windowSize.y);
			x += step;
		}

		drawList->AddPolyline(points, numPoint, IM_COL32(255, 255, 255, 255), false, 2.0f);

		delete[]points;

		ImGui::End();
	}
}

void CViewDemo::initDemo()
{
	m_demo[m_demoId]->init();

	m_wind.setEnable(m_demo[m_demoId]->isEnableWind());
}

void CViewDemo::updateDemo()
{
	m_demo[m_demoId]->update();
}