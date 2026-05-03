#pragma once

#include "CViewBase.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIProgressBar.h"
#include "UserInterface/CUISlider.h"
#include "UserInterface/CUISwitch.h"
#include "UserInterface/CUICheckbox.h"

class CViewPopupSetting : public CViewBase
{
public:
	CViewPopupSetting();

	virtual ~CViewPopupSetting();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void close();

	void onGUI();

};
