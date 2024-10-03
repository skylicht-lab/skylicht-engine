#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "CImguiManager.h"
#include "imgui.h"

#include "CTextureLoader.h"
#include "CSkeletonDrawable.h"

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

	// load atlas
	io::IReadFile* fileAtlas = fs->createAndOpenFile("SampleSpine2D/spineboy-pma.atlas");
	u32 fileSize = fileAtlas->getSize();
	c8* fileData = new c8[fileSize];
	fileAtlas->read(fileData, fileSize);

	spine::CTextureLoader textureLoader;
	spine::Atlas atlas(fileData, fileSize, "SampleSpine2D", &textureLoader);
	spine::AtlasAttachmentLoader attachmentLoader(&atlas);
	spine::SkeletonJson json(&attachmentLoader);

	delete[]fileData;

	// drawable
	io::IReadFile* fileJson = fs->createAndOpenFile("SampleSpine2D/spineboy-pro.json");

	fileSize = fileJson->getSize();
	fileData = new c8[fileSize];
	fileJson->read(fileData, fileSize);

	spine::SkeletonData* skeletonData = json.readSkeletonData(fileData);
	if (skeletonData != NULL)
	{
		spine::CSkeletonDrawable drawable(skeletonData);
	}
	else
	{
		os::Printer::log(json.getError().buffer());
	}

	delete[]fileData;

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
