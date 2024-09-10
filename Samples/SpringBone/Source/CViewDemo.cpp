#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "imgui.h"
#include "CImguiManager.h"

CViewDemo::CViewDemo() :
	m_moveParticle(false),
	m_moveId(0),
	m_moveValue(0.0f),
	m_moveRadius(0.0f)
{

}

CViewDemo::~CViewDemo()
{

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

	m_verlet.drawDebug();

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
		if (ImGui::CollapsingHeader("Demo 1", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Move particle", &m_moveParticle);
		}

		if (ImGui::CollapsingHeader("Wind", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static float scale = m_wind.getScale();
			static float strong = m_wind.getStrong();

			ImGui::SliderFloat("Scale", &scale, 0.001f, 5.0f);
			ImGui::SliderFloat("Strong", &strong, 0.0f, 0.1f);

			m_wind.setScale(scale);
			m_wind.setStrong(strong);
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
	int numCol = 32;
	int numRow = 32;

	int numParticles = numCol * numRow;

	m_moveValue = 0.0f;

	m_verlet.clear();
	m_verlet.addParticle(numParticles);

	Verlet::CParticle** particles = m_verlet.getParticles();

	float space = 0.1f;
	core::vector3df offset(0.0f, numRow * space, 0.0f);

	for (int y = 0; y < numRow; y++)
	{
		for (int x = 0; x < numCol; x++)
		{
			int id = y * numCol + x;

			Verlet::CParticle* p = particles[id];
			p->setPosition(offset + core::vector3df(x * space, -y * space, 0.0f));
		}
	}

	m_moveId = numCol - 1;

	particles[0]->IsConstraint = true;
	particles[m_moveId]->IsConstraint = true;

	m_moveRadius = particles[m_moveId]->Position.X;

	for (int y = 0; y < numRow; y++)
	{
		for (int x = 0; x < numCol; x++)
		{
			if (x < numCol - 1)
			{
				m_verlet.addStick(
					particles[y * numCol + x],
					particles[y * numCol + x + 1]
				);
			}

			if (y < numRow - 1)
			{
				m_verlet.addStick(
					particles[y * numCol + x],
					particles[(y + 1) * numCol + x]
				);
			}
		}
	}
}

void CViewDemo::updateDemo()
{
	if (m_moveParticle)
	{
		Verlet::CParticle** particles = m_verlet.getParticles();

		m_moveValue = m_moveValue + 0.001f * getTimeStep();
		m_moveValue = fmodf(m_moveValue, 2.0f * core::PI);

		core::vector3df& pos = particles[m_moveId]->Position;
		pos.X = cosf(m_moveValue) * m_moveRadius;
		pos.Z = sinf(m_moveValue) * m_moveRadius;
	}
}