#pragma once

#include "CViewBase.h"

class CViewTop : public CViewBase
{
protected:
	UI::CUIButton* m_btnAvatar;
	UI::CUIButton* m_btnUserName;

	UI::CUIButton* m_btnHome;

	UI::CUIBase* m_back;
	UI::CUIButton* m_btnBack;

	UI::CUIButton* m_btnEnergy;
	UI::CUIButton* m_btnGold;
	UI::CUIButton* m_btnDiamond;
	UI::CUIButton* m_btnPower;

	UI::CUIBase* m_currency;

	CGUIText* m_textName;
	CGUIText* m_textEnergy;
	CGUIText* m_textGold;
	CGUIText* m_textDiamond;
	CGUIText* m_textCP;

	CGUIElement* m_energyTime;
	CGUIText* m_textEnergyTime;

	std::future<std::pair<int, unsigned long>> m_taskEnergyTime;
	unsigned long m_productionTime;

	CGUIImage* m_avatarImage;

	float m_energy;
	float m_gold;
	float m_diamond;
	float m_cp;

	bool m_updateCurrency;
	float m_updateEnergyTime;
public:
	CViewTop();

	virtual ~CViewTop();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	void setVisible(bool b);

	void setName(const char* name);

	void setUserName(const char* name);

	void updateConsumer(bool playAnim = true);

	void setAvatar(const char* id);

	void hideAvatar();

	inline void showCurrency(bool b)
	{
		m_currency->setVisible(b);
	}

	void showAvatar();

	void showCurrency();

	void showBack(const char* name);

	void showBackAnim();

	void hideBack();

	bool isBackButtonVisible()
	{
		return m_back ? m_back->isVisible() : false;
	}

	void updateProductionEnergy();

	void updateEnergyTime();

protected:

	void onBtnBack(UI::CUIBase* btn);

	void onBtnAvatar(UI::CUIBase* btn);

	void onBtnName(UI::CUIBase* btn);

	void onBtnDiamond(UI::CUIBase* btn);

	void onBtnGold(UI::CUIBase* btn);

	void onBtnEnergy(UI::CUIBase* btn);

	void onBtnPower(UI::CUIBase* btn);
};