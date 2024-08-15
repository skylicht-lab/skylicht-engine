#include "pch.h"
#include "CViewDemo.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "Context/CContext.h"
#include "Graphics2D/CGUIImporter.h"

CViewDemo::CViewDemo() :
	m_uiContainer(NULL),
	m_listUniform(NULL),
	m_canvas(NULL)
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
	m_canvas = leftPanel->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUIDemo/Main.gui", m_canvas);
	m_canvas->applyScaleGUI(1.0f);
	m_canvas->setSortDepth(0);

	m_uiContainer = leftPanel->addComponent<UI::CUIContainer>();

	m_listUniform = new UI::CUIListView(m_uiContainer,
		m_canvas->getGUIByPath("Canvas/Container/ListItems"),
		m_canvas->getGUIByPath("Canvas/Container/ListItems/Item"));

	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUIDemo/TabIcon.spritedata");
	if (spriteFrame)
	{
		addListIconItem(spriteFrame->getFrameByName("ic-hair"));
		addListIconItem(spriteFrame->getFrameByName("ic-shirt"));
		addListIconItem(spriteFrame->getFrameByName("ic-pant"));
		addListIconItem(spriteFrame->getFrameByName("ic-heels"));
	}
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::addListIconItem(SFrame* frame)
{
	if (frame)
	{
		CGUIElement* element = m_listUniform->addItem();
		CGUISprite* iconGUI = (CGUISprite*)m_canvas->getGUIByPath(element, "Icon");
		if (iconGUI)
			iconGUI->setFrame(frame);

		CGUIElement* bg = m_canvas->getGUIByPath(element, "Background");
		if (bg)
		{
			UI::CUIButton* button = new UI::CUIButton(m_uiContainer, element);
			button->addMotion(UI::EMotionEvent::PointerHover, bg, new UI::CColorMotion(SColor(0xfffe7200)))->setTime(0.0f, 50.0f);
			button->addMotion(UI::EMotionEvent::PointerOut, bg, new UI::CColorMotion())->setTime(0.0f, 50.0f);
			button->setToggleButton(true);
			button->OnToggle = [bg, view = this](UI::CUIBase* button, bool status)
				{
					if (status)
					{
						bg->setColor(SColor(0xff0072fe));
						button->setEnable(false);
						view->onChangeUniform(button);
					}
					else
					{
						bg->setColor(SColor(0xff201a2b));
					}
				};

			if (m_listUniformBtn.size() == 0)
				button->setToggle(true);

			m_listUniformBtn.push_back(button);
		}
	}
}

void CViewDemo::onChangeUniform(UI::CUIBase* btn)
{
	for (UI::CUIButton* b : m_listUniformBtn)
	{
		if (b != btn)
		{
			b->setToggle(false);
			b->setEnable(true);
		}
	}
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
