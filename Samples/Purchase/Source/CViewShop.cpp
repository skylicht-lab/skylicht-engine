#include "pch.h"
#include "CViewShop.h"
#include "CPurchase.h"
#include "Context/CContext.h"
#include "CShopConfig.h"
#include "CShopData.h"
#include "CLocalize.h"
#include "Utils/CStringImp.h"
#include "Graphics2D/CGUIImporter.h"
#include "Graphics2D/CGUIExporter.h"
#include "Tween/CTweenManager.h"

CViewShop::CViewShop() :
	m_listView(NULL),
	m_scrollTween(NULL),
	m_arrow(NULL),
	m_minArrowX(0.0f),
	m_maxArrowX(0.0f)
{

}

CViewShop::~CViewShop()
{

}

void CViewShop::onInit()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	scene->updateIndexSearchObject();

	loadGUI("SamplePurchase/GUI/ShopPurchase.gui", scene, 0.0f);

	m_listView = new UI::CUIListView(m_uiContainer, m_canvas->getGUIByPath("Canvas/View/ListView"), NULL);
	m_listView->setVertical(false);
	m_listView->enableMask(false);
	m_listView->setItemSpacing(100.0f);

	CGUIElement* diamondShop = addShopView(m_listView, m_canvas->getGUIByPath("Canvas/Safe/Subview/Diamond"));
	initIAPShopView(diamondShop, "Diamond");

	CGUIElement* goldShop = addShopView(m_listView, m_canvas->getGUIByPath("Canvas/Safe/Subview/Gold"));
	initIAPShopView(goldShop, "Gold");

	float space = m_listView->getItemSpacing();
	m_tabSize.push_back(0.0f);
	m_tabSize.push_back(diamondShop->getWidth() + space);

	UI::CUIButton* btnDiamondTab = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Bottom/Layout/Diamond"));
	btnDiamondTab->OnPressed = std::bind(&CViewShop::onBtnBottom, this, std::placeholders::_1, 0);

	UI::CUIButton* btnGoldTab = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Bottom/Layout/Gold"));
	btnGoldTab->OnPressed = std::bind(&CViewShop::onBtnBottom, this, std::placeholders::_1, 1);

	m_arrow = m_canvas->getGUIByPath("Canvas/Safe/Bottom/Arrow");
	CGUIElement* layout = btnDiamondTab->getElement()->getParent();
	if (m_arrow && layout)
	{
		float layoutX = layout->getPosition().X;
		m_minArrowX = layoutX + btnDiamondTab->getElement()->getPosition().X + btnDiamondTab->getElement()->getWidth() * 0.5f;
		m_maxArrowX = layoutX + btnGoldTab->getElement()->getPosition().X + btnGoldTab->getElement()->getWidth() * 0.5f;
	}
}

void CViewShop::onDestroy()
{
	if (m_scrollTween)
	{
		m_scrollTween->stop();
		m_scrollTween = NULL;
	}
}

void CViewShop::onUpdate()
{
	updateArrowPosition();

	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewShop::onRender()
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

void CViewShop::onPostRender()
{

}

CGUIElement* CViewShop::addShopView(UI::CUIListView* listView, CGUIElement* view)
{
	if (!view)
		return NULL;

	CObjectSerializable* dataView = CGUIExporter::createSerializable(view);
	CGUIElement* item = listView->addItem(dataView);
	if (item)
		item->setVisible(true);
	delete dataView;

	view->setVisible(false);
	return item;
}

void CViewShop::initIAPShopView(CGUIElement* element, const char* itemType)
{
	if (!element)
		return;

	CGUIElement* gridView = m_canvas->getGUIByPath(element, "GridView");
	CGUIElement* itemTemplate = m_canvas->getGUIByPath(element, "GridView/Item");

	if (!gridView || !itemTemplate)
		return;

	float w = itemTemplate->getWidth();
	float h = itemTemplate->getHeight();

	CObjectSerializable* itemObj = CGUIExporter::createSerializable(itemTemplate);

	float x = 0.0f;
	float y = 0.0f;
	float spaceX = 20.0f;
	float spaceY = 20.0f;

	CShopConfig* config = CShopConfig::getInstance();
	const std::vector<CObjectSerializable*>& data = config->getData();

	int count = 0;
	for (CObjectSerializable* obj : data)
	{
		CShopData* shopData = dynamic_cast<CShopData*>(obj);
		if (!shopData)
			continue;

		if (shopData->Item.get() != itemType)
			continue;

		CGUIElement* item = CGUIImporter::importGUI(m_canvas, gridView, itemObj);
		item->setPosition(core::vector3df(x, y, 0.0f));

		// Set icon
		CGUISprite* icon = dynamic_cast<CGUISprite*>(m_canvas->getGUIByPath(item, "Icon"));
		if (icon)
		{
			icon->setFrameSource(shopData->Sprite.cstr(), shopData->Icon.cstr());
		}

		// Set value
		CGUIText* txtValue = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath(item, "Top/TxtValue"));
		if (txtValue)
		{
			txtValue->setText(CStringImp::formatThousand(shopData->Value.get(), false, false).c_str());
		}

		// Set price
		CGUIText* txtPrice = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath(item, "BtnPurchase/Text"));
		if (txtPrice)
		{
			txtPrice->setText(shopData->PriceLocalize.c_str());
		}

		UI::CUIButton* btn = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath(item, "BtnPurchase"));
		setThemeButton(btn);
		btn->OnPressed = [shopData](UI::CUIBase* b) {
			CPurchase::getInstance()->purchase(shopData->Iap.cstr());
			};

		x = x + w + spaceX;
		if (++count % 3 == 0)
		{
			x = 0.0f;
			y = y + h + spaceY;
		}
	}

	itemTemplate->setVisible(false);
	delete itemObj;
}

void CViewShop::setCurrentTab(int id)
{
	onBtnBottom(NULL, id);
}

void CViewShop::onBtnBottom(UI::CUIBase* btn, int id)
{
	if (m_scrollTween)
	{
		m_scrollTween->stop();
		m_scrollTween = NULL;
	}

	float offsetPosition = -m_tabSize[id];

	m_scrollTween = new CTweenFloat(m_listView->getOffset(), offsetPosition, 500.0f);
	m_scrollTween->setEase(EaseOutCubic);
	m_scrollTween->OnUpdate = [&](CTween* tween)
		{
			CTweenFloat* t = (CTweenFloat*)tween;
			m_listView->setOffset(t->getValue());
		};
	m_scrollTween->OnFinish = [&](CTween* tween)
		{
			m_scrollTween = NULL;
		};
	CTweenManager::getInstance()->addTween(m_scrollTween);
}

void CViewShop::updateArrowPosition()
{
	if (m_arrow)
	{
		float offset = m_listView->getOffset();
		float maxOffset = m_listView->getMaxOffset();
		if (maxOffset == 0.0f)
			maxOffset = 1.0f;

		float arrowX = m_minArrowX + (m_maxArrowX - m_minArrowX) * (-offset / maxOffset);

		core::vector3df arrowPos = m_arrow->getPosition();
		arrowPos.X = arrowX;
		m_arrow->setPosition(arrowPos);
	}
}
