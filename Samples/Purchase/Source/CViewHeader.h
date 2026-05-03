#pragma once

#include "Graphics2D/GUI/CGUIText.h"
#include "CViewBase.h"

class CViewHeader : public CViewBase
{
protected:
	CGUIText* m_txtUserName;
	CGUIText* m_txtGold;
	CGUIText* m_txtDiamond;

	float m_gold;
	float m_diamond;

	bool m_updateCurrency;

public:
	CViewHeader();

	virtual ~CViewHeader();

	virtual void onInit();

	virtual void onData();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void updateConsumer(bool playAnim);

protected:

	void onGUI();

};
