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

bool bake = false;

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

	// test bake bake irradiance
	if (bake == false)
	{
		CGameObject* bakeCameraObj = context->getActiveZone()->createEmptyObject();
		CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
		scene->updateAddRemoveObject();

		CLightmapper::getInstance()->initBaker(32);

		CLightmapper::getInstance()->bakeProbes(
			context->getProbes(),
			bakeCamera,
			context->getRenderPipeline(),
			scene->getEntityManager()
		);

		bakeCameraObj->remove();
		bake = true;
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
	ImGui::SetNextWindowPos(ImVec2(935, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320, 300), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Sample Sponza", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		ImGui::Text("Press ASDW to move camera");
		ImGui::Text("Drag Right Mouse to rotate camera");
		ImGui::Text("Scroll Middle Mouse to go near/far");
		ImGui::Separator();

		CPostProcessorRP* rp = CContext::getInstance()->getPostProcessorPipeline();

		bool bloom = rp->isEnableBloomEffect();
		ImGui::Checkbox("Bloom", &bloom);
		rp->enableBloomEffect(bloom);
		if (bloom)
		{
			float threshold = rp->getBloomThreshold();
			ImGui::SliderFloat("Bloom Threadhold", &threshold, 0.0f, 1.0f);
			rp->setBloomThreshold(threshold);

			float intensity = rp->getBloomIntensity();
			ImGui::SliderFloat("Bloom Intensity", &intensity, 0.0f, 4.0f);
			rp->setBloomIntensity(intensity);
		}

		ImGui::Separator();

		bool autoExposure = rp->isEnableAutoExposure();
		ImGui::Checkbox("Auto Exposure", &autoExposure);

		if (!autoExposure)
		{
			float exposure = rp->getManualExposure();
			ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);

			rp->enableAutoExposure(false);
			rp->setManualExposure(exposure);
		}
		else
		{
			rp->enableAutoExposure(true);
		}

		ImGui::Separator();

		bool aa = rp->isEnableFXAA();
		ImGui::Checkbox("Enable AA", &aa);
		rp->enableFXAA(aa);

		ImGui::Separator();

		bool reflection = rp->isEnableScreenSpaceReflection();
		ImGui::Checkbox("Enable Reflection", &reflection);
		rp->enableScreenSpaceReflection(reflection);

		ImGui::End();
	}
}

void CViewDemo::onPostRender()
{

}
