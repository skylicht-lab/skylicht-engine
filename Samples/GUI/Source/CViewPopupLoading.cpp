#include "pch.h"
#include "CViewPopupLoading.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "Tween/CTweenManager.h"

CViewPopupLoading::CViewPopupLoading() :
	m_progressBar(NULL)
{

}

CViewPopupLoading::~CViewPopupLoading()
{

}

void CViewPopupLoading::onInit()
{
	loadGUI("SampleGUIDemo/Loading.gui", NULL);
	m_canvas->setSortDepth(2);

	UI::CUIBase* disableTouch = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Background"));

	m_progressBar = new UI::CUIProgressBar(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Dialog/ProgressBar"));
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

	m_uiContainer->startInMotion();
}

void CViewPopupLoading::close()
{
	m_uiContainer->startOutMotion();
	m_uiContainer->OnMotionOutFinish = [&]()
		{
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
