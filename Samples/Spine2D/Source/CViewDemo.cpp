#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "CImguiManager.h"
#include "imgui.h"

#include "CSpineResource.h"

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

	CScene* scene = context->getScene();
	CZone* zone = scene->getZone(0);

	CGameObject* canvasObj = zone->createEmptyObject();
	CCanvas* canvas = canvasObj->addComponent<CCanvas>();

	CGUIElement* element = canvas->createElement();
	element->setWidth(800.0f);
	element->setHeight(400.0f);
	element->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	element->setDrawBorder(true);

	io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

	// spine2d
	spine::CSpineResource spineRes;
	spineRes.loadAtlas("SampleSpine2D/spineboy-pma.atlas", "SampleSpine2D");
	spineRes.loadSkeletonJson("SampleSpine2D/spineboy-pro.json");

	scene->updateIndexSearchObject();
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
}

void CViewDemo::onPostRender()
{

}
