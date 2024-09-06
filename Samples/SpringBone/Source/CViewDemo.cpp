#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "imgui.h"
#include "CImguiManager.h"

CViewDemo::CViewDemo()
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	initDemo1();
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
	ImGui::SetNextWindowPos(ImVec2(935, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(310, 270), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Verlet Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		ImGui::End();
	}
}

void CViewDemo::initDemo1()
{
	int numCol = 32;
	int numRow = 32;

	int numParticles = numCol * numRow;

	m_verlet.clear();
	m_verlet.addParticle(numParticles);

	Verlet::CParticle** particles = m_verlet.getParticles();

	float space = 0.1f;
	core::vector3df offset(0.0f, numRow * space, 0.0f);

	for (int y = 0; y < numRow; y++)
	{
		for (int x = 0; x < numCol; x++)
		{
			int id = y * numRow + x;

			Verlet::CParticle* p = particles[id];
			p->setPosition(offset + core::vector3df(x * space, -y * space, 0.0f));
		}
	}

	particles[0]->IsConstraint = true;
	particles[numCol - 1]->IsConstraint = true;

	for (int y = 0; y < numRow; y++)
	{
		for (int x = 0; x < numCol; x++)
		{
			if (x < numCol - 1)
				m_verlet.addStick(particles[y * numRow + x], particles[y * numRow + x + 1]);

			if (y < numRow - 1)
				m_verlet.addStick(particles[y * numRow + x], particles[(y + 1) * numRow + x]);
		}
	}
}