#include "pch.h"
#include "CViewPopupLoading.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "Tween/CTweenManager.h"

CViewPopupLoading::CViewPopupLoading() :
	m_popup(NULL),
	m_progressBar(NULL)
{

}

CViewPopupLoading::~CViewPopupLoading()
{

}

void CViewPopupLoading::onInit()
{
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	m_popup = zone->createEmptyObject();
	CCanvas* canvas = m_popup->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUIDemo/Loading.gui", canvas);
	canvas->applyScaleGUI(1.0f);
	canvas->setSortDepth(2);

	UI::CUIContainer* uiContainer = m_popup->addComponent<UI::CUIContainer>();

	m_progressBar = new UI::CUIProgressBar(uiContainer, canvas->getGUIByPath("Canvas/Container/Dialog/ProgressBar"));
	m_progressBar->setPercent(0.0f);

	// fake load in 2s to demo ProgressBar
	CTweenFloat* t = new CTweenFloat(0.0f, 1.0f, 2000.0f);
	t->setEase(EaseLinear);
	t->setEndDelay(500.0f);
	t->OnUpdate = [&](CTween* t)
		{
			CTweenFloat* f = (CTweenFloat*)t;
			m_progressBar->setPercent(f->getValue());
		};
	t->OnFinish = [&](CTween* t)
		{
			close();
		};
	CTweenManager::getInstance()->addTween(t);

	uiContainer->startInMotion();
}

void CViewPopupLoading::close()
{
	UI::CUIContainer* uiContainer = m_popup->getComponent<UI::CUIContainer>();
	uiContainer->startOutMotion();
	uiContainer->OnMotionOutFinish = [&]()
		{
			m_popup->remove();
			CViewManager::getInstance()->getLayer(2)->removeView(this);
		};
}

void CViewPopupLoading::onDestroy()
{

}

void CViewPopupLoading::onUpdate()
{

}

void CViewPopupLoading::onRender()
{

}

void CViewPopupLoading::onGUI()
{

}

void CViewPopupLoading::onPostRender()
{

}
