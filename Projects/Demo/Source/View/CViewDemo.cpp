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

bool bake = false;

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

		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// test bake bake irradiance
	if (bake == false)
	{
		CGameObject *bakeCameraObj = scene->getZone(0)->createEmptyObject();		
		CCamera *bakeCamera = bakeCameraObj->addComponent<CCamera>();

		scene->updateAddRemoveObject();

		context->getProbe()->bakeIrradiance(bakeCamera, context->getForwarderRP(), scene->getEntityManager());
		bake = true;
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