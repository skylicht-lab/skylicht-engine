#include "pch.h"
#include "CViewDemo.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "Context/CContext.h"

#include "Graphics2D/CGUIImporter.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIListView.h"
#include "UserInterface/CUIGridView.h"

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
	scene->updateIndexSearchObject();

	CZone* zone = scene->getZone(0);

	CGameObject* leftPanel = zone->createEmptyObject();
	CCanvas* canvas = leftPanel->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUI/Main.gui", canvas);
	canvas->applyScaleGUI(1.0f);
	canvas->setSortDepth(0);

	UI::CUIContainer* uiContainer = leftPanel->addComponent<UI::CUIContainer>();

	UI::CUIListView* list = new UI::CUIListView(uiContainer,
		canvas->getGUIByPath("Canvas/Container/ListItems"),
		canvas->getGUIByPath("Canvas/Container/ListItems/Item"));

	// list->setVertical(false);

	for (int i = 0; i < 29; i++)
	{
		CGUIElement* element = list->addItem();
		CGUIText* text = (CGUIText*)canvas->getGUIByPath(element, "Name");
		if (text)
		{
			char t[32];
			sprintf(t, "List item: %d", i);
			text->setText(t);
		}
	}
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

	// render all GUI
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

}

void CViewDemo::onPostRender()
{

}
