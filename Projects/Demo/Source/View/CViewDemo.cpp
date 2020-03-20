#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "Graphics2D/CGraphics2D.h"

#include "Material/Shader/CShaderManager.h"
#include "imgui.h"

CViewDemo::CViewDemo()
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext *context = CContext::getInstance();
	CCamera *camera = context->getActiveCamera();
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext *context = CContext::getInstance();
	CScene *scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CCamera *guiCamera = context->getGUICamera();

	CScene *scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->updateDirectionLight();

		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewDemo::onPostRender()
{
	static bool showImguiDemo = false;

	// render ImGui
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Skylicht"))
	{
		if (ImGui::BeginMenu("Renderer"))
		{
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Windows"))
	{
		ImGui::MenuItem("ImGui Demo", NULL, &showImguiDemo);
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (showImguiDemo == true)
	{
		ImGui::ShowDemoWindow(&showImguiDemo);
	}
}