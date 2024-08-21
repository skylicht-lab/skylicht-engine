#include "pch.h"
#include "CViewDemo.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "Context/CContext.h"
#include "Graphics2D/CGUIImporter.h"

CViewDemo::CViewDemo() :
	m_uiContainer(NULL),
	m_listTab(NULL),
	m_listItems(NULL),
	m_canvas(NULL),
	m_txtTitle(NULL),
	m_btnShowHideItems(NULL),
	m_itemsPanel(NULL),
	m_showItems(true),
	m_tweenShowHide(NULL)
{

}

CViewDemo::~CViewDemo()
{
	if (m_tweenShowHide)
	{
		m_tweenShowHide->stop();
		m_tweenShowHide = NULL;
	}
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

	// Apply mask for panel items
	CGUIMask* mask = dynamic_cast<CGUIMask*>(m_canvas->getGUIByPath("Canvas/Container/MaskItem"));
	m_itemsPanel = m_canvas->getGUIByPath("Canvas/Container/Items");
	if (mask && m_itemsPanel)
		m_itemsPanel->setMask(mask);

	m_uiContainer = leftPanel->addComponent<UI::CUIContainer>();

	m_txtTitle = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Container/Items/txtTabName"));

	// CATEGORY: TAB
	m_listTab = new UI::CUIListView(m_uiContainer,
		m_canvas->getGUIByPath("Canvas/Container/ListTab"),
		m_canvas->getGUIByPath("Canvas/Container/ListTab/Item"));

	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUIDemo/TabIcon.spritedata");
	if (spriteFrame)
	{
		addTabItem(spriteFrame->getFrameByName("ic-hair"), "Hair");
		addTabItem(spriteFrame->getFrameByName("ic-shirt"), "Shirt");
		addTabItem(spriteFrame->getFrameByName("ic-pant"), "Pant");
		addTabItem(spriteFrame->getFrameByName("ic-heels"), "Heels");
	}

	// LIST: ITEMS
	m_listItems = new UI::CUIGridView(m_uiContainer,
		m_canvas->getGUIByPath("Canvas/Container/Items/ListItems"),
		m_canvas->getGUIByPath("Canvas/Container/Items/ListItems/Item"));

	CTextureManager* textureMgr = CTextureManager::getInstance();
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-0.png"));
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-1.png"));
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-2.png"));
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-3.png"));

	// BUTTON: Show/Hide
	m_btnShowHideItems = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Items/btnShowHide"));
	m_btnShowHideItems->addMotion(UI::EMotionEvent::PointerHover, new UI::CScaleMotion(1.2f, 1.2f, 1.0f));
	m_btnShowHideItems->addMotion(UI::EMotionEvent::PointerOut, new UI::CScaleMotion());
	m_btnShowHideItems->addMotion(UI::EMotionEvent::PointerDown, new UI::CScaleMotion(0.9f, 0.9f, 0.9f))->setTime(0.0f, 50.0f);
	m_btnShowHideItems->addMotion(UI::EMotionEvent::PointerUp, new UI::CScaleMotion())->setTime(0.0f, 100.0f);
	m_btnShowHideItems->OnPressed = std::bind(&CViewDemo::onShowHidePanel, this, std::placeholders::_1);
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onData()
{
	onChangeTab();
}

void CViewDemo::addTabItem(SFrame* frame, const char* name)
{
	if (frame)
	{
		CGUIElement* element = m_listTab->addItem();
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
			button->setSkipPointerEventWhenDrag(true);
			button->OnToggle = [bg, view = this, name](UI::CUIBase* button, bool status)
				{
					if (status)
					{
						bg->setColor(SColor(0xff0072fe));
						button->setEnable(false);

						view->onSelectTab(button, name);
					}
					else
					{
						bg->setColor(SColor(0xff201a2b));
					}
				};

			if (m_listTabBtn.size() == 0)
				button->setToggle(true);

			m_listTabBtn.push_back(button);
		}
	}
}

void CViewDemo::onSelectTab(UI::CUIBase* btn, const char* name)
{
	if (m_txtTitle)
		m_txtTitle->setText(name);

	m_tab = name;

	for (UI::CUIButton* b : m_listTabBtn)
	{
		if (b != btn)
		{
			b->setToggle(false);
			b->setEnable(true);
		}
	}

	onChangeTab();
}

void CViewDemo::onChangeTab()
{
	if (m_listItems == NULL)
		return;

	m_listItems->clear();
	m_listItemsBtn.clear();

	if (m_tab == "Hair")
		addRandomItem(6);
	else if (m_tab == "Shirt")
		addRandomItem(4);
	else if (m_tab == "Pant")
		addRandomItem(3);
	else if (m_tab == "Heels")
		addRandomItem(9);
}

void CViewDemo::addRandomItem(int count)
{
	char name[128];

	for (int i = 0; i < count; i++)
	{
		CGUIElement* item = m_listItems->addItem();
		CGUIText* text = dynamic_cast<CGUIText*>(item->getGUIByPath("Text"));
		if (text)
		{
			sprintf(name, "Item %0d", i);
			text->setText(name);
		}

		CGUIImage* image = dynamic_cast<CGUIImage*>(item->getGUIByPath("Image"));
		if (image && m_itemImage.size() > 0)
		{
			s32 randomImg = getIrrlichtDevice()->getRandomizer()->rand() % (int)m_itemImage.size();
			image->setImage(m_itemImage[randomImg]);
		}

		CGUIElement* bg = item->getGUIByPath("Background");

		UI::CUIButton* button = new UI::CUIButton(m_uiContainer, item);
		button->addMotion(UI::EMotionEvent::PointerHover, bg, new UI::CColorMotion(SColor(0xfffe7200)))->setTime(0.0f, 50.0f);
		button->addMotion(UI::EMotionEvent::PointerOut, bg, new UI::CColorMotion())->setTime(0.0f, 50.0f);
		button->setToggleButton(true);
		button->setSkipPointerEventWhenDrag(true);
		button->OnToggle = [bg, view = this](UI::CUIBase* button, bool status)
			{
				if (status)
				{
					bg->setColor(SColor(0xff0072fe));
					button->setEnable(false);

					view->onSelectItem(button);
				}
				else
				{
					bg->setColor(SColor(0xff201a2b));
				}
			};
		m_listItemsBtn.push_back(button);
	}

	selectItem(m_listItemsBtn[0]);
}

void CViewDemo::selectItem(UI::CUIButton* btn)
{
	if (btn->getBackground())
		btn->getBackground()->setColor(SColor(0xff0072fe));
	btn->setEnable(false);

	onSelectItem(btn);
}

void CViewDemo::onSelectItem(UI::CUIBase* btn)
{
	for (UI::CUIButton* b : m_listItemsBtn)
	{
		if (b != btn)
		{
			b->setToggle(false);
			b->setEnable(true);
		}
	}
}

void CViewDemo::onShowHidePanel(UI::CUIBase* btn)
{
	m_showItems = !m_showItems;

	// update margin because m_itemsPanel dock: LEFT
	float showMLeft = 95.0f;
	float hideMLeft = -200.000f;
	float time = 250.0f;

	SMargin m = m_itemsPanel->getMargin();

	CGUISprite* btnIcon = NULL;
	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUI/SampleGUI.spritedata");
	if (spriteFrame)
		btnIcon = dynamic_cast<CGUISprite*>(m_btnShowHideItems->getElement()->getGUIByPath("Icon"));

	if (!m_showItems)
	{
		if (btnIcon)
			btnIcon->setFrame(spriteFrame->getFrameByName("triangle-r"));

		// hide
		m_tweenShowHide = new CTweenFloat(showMLeft, hideMLeft, time);
		m_tweenShowHide->setEase(EEasingFunctions::EaseOutQuad);
		m_tweenShowHide->OnUpdate = [&, m](CTween* tween) {
			SMargin update = m;
			update.Left = ((CTweenFloat*)tween)->getValue();
			m_itemsPanel->setMargin(update);
			m_itemsPanel->invalidate();
			m_btnShowHideItems->setEnable(false);
			};
		m_tweenShowHide->OnFinish = [&](CTween* tween) {
			m_tweenShowHide = NULL;
			m_btnShowHideItems->setEnable(true);
			};
		CTweenManager::getInstance()->addTween(m_tweenShowHide);
	}
	else
	{
		if (btnIcon)
			btnIcon->setFrame(spriteFrame->getFrameByName("triangle-l"));

		// show
		m_tweenShowHide = new CTweenFloat(hideMLeft, showMLeft, time);
		m_tweenShowHide->setEase(EEasingFunctions::EaseOutQuad);
		m_tweenShowHide->OnUpdate = [&, m](CTween* tween) {
			SMargin update = m;
			update.Left = ((CTweenFloat*)tween)->getValue();
			m_itemsPanel->setMargin(update);
			m_itemsPanel->invalidate();
			m_btnShowHideItems->setEnable(false);
			};
		m_tweenShowHide->OnFinish = [&](CTween* tween) {
			m_tweenShowHide = NULL;
			m_btnShowHideItems->setEnable(true);
			};
		CTweenManager::getInstance()->addTween(m_tweenShowHide);
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