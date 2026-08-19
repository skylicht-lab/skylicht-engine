#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "CImguiManager.h"
#include "imgui.h"

#include "Primitive/CCube.h"

CViewDemo::CViewDemo()
	: m_cubes(NULL)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CScene* scene = context->getScene();
	scene->updateIndexSearchObject();

	CGameObject* cubesObj = scene->searchObjectInChild(L"Cubes");
	if (cubesObj != NULL)
		m_cubes = cubesObj->getComponent<CCube>();
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

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(280, 120), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Instancing Point Light", &open, window_flags))
	{
		ImGui::End();
		return;
	}

	if (m_cubes != NULL)
	{
		bool instancing = m_cubes->isInstancing();
		if (ImGui::Checkbox("Instancing", &instancing))
			m_cubes->setInstancing(instancing);
	}
	else
	{
		ImGui::Text("Cubes not found");
	}

	ImGui::Separator();
	ImGui::Text("DrawCall: %d", getVideoDriver()->getDrawCall());

	ImGui::End();
}

void CViewDemo::onPostRender()
{

}
