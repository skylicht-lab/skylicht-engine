#include "pch.h"
#include "CViewPopupSetting.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

#include "Graphics2D/CGUIImporter.h"
#include "GameObject/CGameObject.h"

#include "Tween/CTweenManager.h"

CViewPopupSetting::CViewPopupSetting()
{

}

CViewPopupSetting::~CViewPopupSetting()
{

}

void CViewPopupSetting::onInit()
{
	loadGUI("SampleGUIDemo/Setting.gui", NULL);
	m_canvas->setSortDepth(2);

	UI::CUIBase* bg = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Background"));
	setFadeInOutLinear(bg);

	UI::CUIBase* dialog = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Dialog"));
	setFadeInOut(dialog);

	UI::CUIBase* btnClose = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Dialog/BtnClose"));
	setThemeButtonScale(btnClose);
	btnClose->OnPressed = [&](UI::CUIBase* base)
		{
			close();
		};

	UI::CUISlider* slider = new UI::CUISlider(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Dialog/Slider"));
	slider->setValue(0.5f);

	UI::CUISwitch* options = new UI::CUISwitch(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Dialog/BtnSwitch"));
	options->setHandleColor(SColor(255, 255, 255, 255), SColor(255, 200, 200, 200));

	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUI/SampleGUI.spritedata");
	SFrame* frameOn = spriteFrame->getFrameByName("switch-on");
	SFrame* frameOff = spriteFrame->getFrameByName("switch-off");
	if (frameOn && frameOff && options->getBackground())
	{
		options->OnChanged = [options, frameOn, frameOff](UI::CUISwitch* s, bool value) {
			CGUISprite* bg = dynamic_cast<CGUISprite*>(options->getBackground());
			if (bg)
			{
				if (value)
					bg->setFrame(frameOn);
				else
					bg->setFrame(frameOff);
			}
			};
	}

	options->setToggle(true);

	UI::CUICheckbox* checkbox = new UI::CUICheckbox(m_uiContainer, m_canvas->getGUIByPath("Canvas/Container/Dialog/CBCheck"));
	checkbox->setToggle(true);

	m_uiContainer->startInMotion();
}

void CViewPopupSetting::close()
{
	m_uiContainer->startOutMotion();
	m_uiContainer->OnMotionOutFinish = [&]()
		{
			CViewManager::getInstance()->getLayer(2)->removeView(this);
		};
}

void CViewPopupSetting::onDestroy()
{

}

void CViewPopupSetting::onUpdate()
{

}

void CViewPopupSetting::onRender()
{

}

void CViewPopupSetting::onGUI()
{

}

void CViewPopupSetting::onPostRender()
{

}
