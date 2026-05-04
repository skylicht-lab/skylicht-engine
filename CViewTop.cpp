#include "pch.h"
#include "CViewTop.h"
#include "CViewBaseMenu.h"
#include "CViewAvatar.h"
#include "CViewRename.h"
#include "CViewShop.h"
#include "CViewEnergyAds.h"
#include "CViewCombatPower.h"
#include "Profile/CProfile.h"
#include "Configs/CItemConfig.h"
#include "TextureManager/CTextureManager.h"
#include "Services/CBusConsumer.h"
#include "BusinessConfig.h"
#include "Utils/CDateTimeUtils.h"

CViewTop::CViewTop() :
	m_back(NULL),
	m_btnBack(NULL),
	m_btnAvatar(NULL),
	m_btnHome(NULL),
	m_btnUserName(NULL),
	m_currency(NULL),
	m_btnEnergy(NULL),
	m_btnGold(NULL),
	m_btnDiamond(NULL),
	m_btnPower(NULL),
	m_avatarImage(NULL),
	m_textName(NULL),
	m_textEnergy(NULL),
	m_textGold(NULL),
	m_textDiamond(NULL),
	m_energy(0),
	m_gold(0),
	m_diamond(0),
	m_cp(0.0f),
	m_updateCurrency(false),
	m_updateEnergyTime(0.0f),
	m_productionTime(0)
{

}

CViewTop::~CViewTop()
{

}

void CViewTop::setVisible(bool b)
{
	m_canvas->getGameObject()->setVisible(b);

	if (b)
		updateProductionEnergy();
}

void CViewTop::onInit()
{
	loadGUI("GUI/Top.gui");
	m_canvas->setSortDepth(CANVAS_DEPTH_TOP);

	float animTime = 500.0f;
	m_btnAvatar = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Avatar"));
	m_btnAvatar->setVisible(true);
	setThemeButton(m_btnAvatar);
	m_btnAvatar->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion())->setTime(0.0f, animTime);
	m_btnAvatar->addMotion(UI::EMotionEvent::In, new UI::CPositionMotion(-100.0f, 0.0f, 0.0f))->setInverse(true)->setTime(0.0f, animTime);
	m_btnAvatar->addMotion(UI::EMotionEvent::Out, new UI::CAlphaMotion(0.0f))->setTime(0.0f, animTime);
	m_btnAvatar->addMotion(UI::EMotionEvent::Out, new UI::CPositionMotion(100.0f, 0.0f, 0.0f))->setTime(0.0f, animTime);
	m_btnAvatar->OnPressed = std::bind(&CViewTop::onBtnAvatar, this, std::placeholders::_1);

	m_avatarImage = dynamic_cast<CGUIImage*>(m_canvas->getGUIByPath("Canvas/Safe/Avatar/Frame/Image"));

	m_btnUserName = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Avatar/BtnUserName"));
	m_btnUserName->setLabel(m_profile->Name.c_str());
	setThemeButton(m_btnUserName);
	m_btnUserName->OnPressed = std::bind(&CViewTop::onBtnName, this, std::placeholders::_1);

	m_btnEnergy = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Currency/Energy/Button"));
	setThemeButton(m_btnEnergy);
	m_btnEnergy->OnPressed = std::bind(&CViewTop::onBtnEnergy, this, std::placeholders::_1);

	m_btnDiamond = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Currency/Diamond/Button"));
	setThemeButton(m_btnDiamond);
	m_btnDiamond->OnPressed = std::bind(&CViewTop::onBtnDiamond, this, std::placeholders::_1);

	m_btnGold = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Currency/Gold/Button"));
	setThemeButton(m_btnGold);
	m_btnGold->OnPressed = std::bind(&CViewTop::onBtnGold, this, std::placeholders::_1);

	m_btnHome = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/BtnHome"));
	setThemeButton(m_btnHome);
	m_btnHome->setVisible(false);
	m_btnHome->setSoundId(0, sfx_menu_return);

	CGUIElement* backElement = m_canvas->getGUIByPath("Canvas/Safe/Back");

	m_back = new UI::CUIBase(m_uiContainer, backElement);
	m_back->setVisible(false);
	m_back->addMotion(UI::EMotionEvent::In, new UI::CAlphaMotion())->setTime(0.0f, animTime);
	m_back->addMotion(UI::EMotionEvent::In, new UI::CPositionMotion(-100.0f, 0.0f, 0.0f))->setInverse(true)->setTime(0.0f, animTime);

	m_btnBack = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Back/BtnBack"));
	setThemeButtonScaleAlpha(m_btnBack);
	m_btnBack->setVisible(true);
	m_btnBack->OnPressed = std::bind(&CViewTop::onBtnBack, this, std::placeholders::_1);
	m_btnBack->setSoundId(0, sfx_menu_return);

	m_textName = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Safe/Back/Text"));

	m_currency = new UI::CUIBase(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Currency"));
	m_currency->addMotion(UI::EMotionEvent::In, new UI::CPositionMotion(0.0f, -100.0f, 0.0f))->setInverse(true)->setTime(0.0f, animTime);

	m_energyTime = m_canvas->getGUIByPath("Canvas/Safe/Currency/Energy/Time");
	m_textEnergyTime = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Safe/Currency/Energy/Time/TxtTime"));
	if (m_energyTime)
		m_energyTime->setVisible(false);

	m_textEnergy = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Safe/Currency/Energy/Text"));
	m_textGold = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Safe/Currency/Gold/Text"));
	m_textDiamond = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Safe/Currency/Diamond/Text"));
	m_textCP = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Safe/Avatar/CP/TxtNumber"));

	m_btnPower = new UI::CUIButton(m_uiContainer, m_canvas->getGUIByPath("Canvas/Safe/Avatar/CP"));
	setThemeButton(m_btnPower);
	m_btnPower->OnPressed = std::bind(&CViewTop::onBtnPower, this, std::placeholders::_1);

	setAvatar(m_profile->Avatar.c_str());
	updateConsumer(false);
	updateProductionEnergy();
}

void CViewTop::onDestroy()
{

}

void CViewTop::onUpdate()
{
	if (m_updateCurrency)
	{
		float energy = (float)m_busConsumer->getAmount(EConsumerItem::Energy);
		float gold = (float)m_busConsumer->getAmount(EConsumerItem::Gold);
		float diamond = (float)m_busConsumer->getAmount(EConsumerItem::Diamond);
		float cp = CProfile::getInstance()->getPower();

		float ts = getTimeScale() * 0.2f;

		float e = energy - m_energy;
		float g = gold - m_gold;
		float d = diamond - m_diamond;
		float c = cp - m_cp;

		m_energy = e <= 50 ? energy : m_energy + e * ts;
		m_gold = g <= 50 ? gold : m_gold + g * ts;
		m_diamond = d <= 50 ? diamond : m_diamond + d * ts;
		m_cp = c <= 50 ? cp : m_cp + c * ts;

		char energyText[64];
		sprintf(energyText, "%d/%d", (int)m_energy, m_busEnergy->getCapacity());

		m_textEnergy->setText(energyText);
		m_textGold->setText(CStringFormat::formatThousand((int)m_gold, true, true).c_str());
		m_textDiamond->setText(CStringFormat::formatThousand((int)m_diamond, true, true).c_str());
		m_textCP->setText(CStringFormat::formatThousand((int)m_cp, true, true).c_str());

		if (m_energy == energy &&
			m_gold == gold &&
			m_diamond == diamond &&
			m_cp == cp)
		{
			m_updateCurrency = false;
		}
	}

	if (m_energyTime && m_textEnergyTime)
		updateEnergyTime();
}

void CViewTop::onRender()
{

}

void CViewTop::setName(const char* name)
{
	if (m_textName)
		m_textName->setTextStrim(name);
}

void CViewTop::setUserName(const char* name)
{
	if (m_btnUserName)
		m_btnUserName->setLabel(name);
}

void CViewTop::updateConsumer(bool playAnim)
{
	if (playAnim)
		m_updateCurrency = true;
	else
	{
		m_energy = (float)m_busConsumer->getAmount(EConsumerItem::Energy);
		m_gold = (float)m_busConsumer->getAmount(EConsumerItem::Gold);
		m_diamond = (float)m_busConsumer->getAmount(EConsumerItem::Diamond);
		m_cp = CProfile::getInstance()->getPower();

		char energyText[64];
		sprintf(energyText, "%d/%d", (int)m_energy, m_busEnergy->getCapacity());

		m_textEnergy->setText(energyText);
		m_textGold->setText(CStringFormat::formatThousand((int)m_gold, true, true).c_str());
		m_textDiamond->setText(CStringFormat::formatThousand((int)m_diamond, true, true).c_str());
		m_cp = CProfile::getInstance()->getPower();
		m_textCP->setText(CStringFormat::formatThousand((int)m_cp, true, true).c_str());
	}
}

void CViewTop::showAvatar()
{
	m_btnAvatar->setVisible(true);
	m_btnAvatar->setEnable(true);
	m_btnUserName->setEnable(true);
	m_btnAvatar->startMotion(UI::EMotionEvent::In);
}

void CViewTop::showCurrency()
{
	m_currency->startMotion(UI::EMotionEvent::In);
	m_back->startMotion(UI::EMotionEvent::In);
}

void CViewTop::hideAvatar()
{
	m_btnAvatar->setEnable(false);
	m_btnUserName->setEnable(false);
	m_btnAvatar->startMotion(UI::EMotionEvent::Out);
	m_btnAvatar->OnMotionFinish[(int)UI::EMotionEvent::Out] = [](UI::CUIBase* ui, UI::EMotionEvent motion) {
		ui->setVisible(false);
		};
}

void CViewTop::setAvatar(const char* id)
{
	if (m_avatarImage)
	{
		CRowData* row = CItemConfig::getInstance()->getItemById(id);
		if (row)
		{
			std::string imagePath = row->Folder.get() + "/" + row->Icon.get();
			m_avatarImage->setImage(CTextureManager::getInstance()->getTexture(imagePath.c_str()));
		}
	}
}

void CViewTop::showBack(const char* name)
{
	m_back->setVisible(true);
	m_back->setEnable(true);
	m_btnBack->setEnable(true);

	CGUIElement* backElement = m_back->getElement();
	SColor backColor = backElement->getColor();
	backColor.setAlpha(0);
	backElement->setColor(backColor);

	m_back->startMotion(UI::EMotionEvent::In);
	m_textName->setTextStrim(name);
}

void CViewTop::showBackAnim()
{
	m_back->setVisible(true);
	m_back->setEnable(true);
	m_btnBack->setEnable(true);

	m_back->startMotion(UI::EMotionEvent::In);
}

void CViewTop::hideBack()
{
	m_back->setEnable(false);
	m_btnBack->setEnable(false);
	m_back->setVisible(false);
}

void CViewTop::onBtnBack(UI::CUIBase* btn)
{
	btn->setEnable(false);
	m_tweenMgr->addDelayCall(200.0f, [&]() {
		m_btnBack->setEnable(true);
		CView* view = m_viewMgr->getLayer(0)->getCurrentView();
		if (view)
		{
			CViewBaseMenu* menuView = dynamic_cast<CViewBaseMenu*>(view);
			if (menuView)
				menuView->onButtonBack();
		}
		});
}

void CViewTop::updateEnergyTime()
{
	int energy = m_busConsumer->getAmount(EConsumerItem::Energy);
	if (energy < m_busEnergy->getCapacity())
	{
		// call get production time every second
		m_updateEnergyTime = m_updateEnergyTime - getTimeStep();
		if (m_updateEnergyTime < 0.0f)
		{
			if (m_productionTime == 0)
			{
				// call get production time
				if (!m_taskEnergyTime.valid())
					m_taskEnergyTime = m_busEnergy->getProductionTime();
				else
				{
					// wait task finish
					if (m_taskEnergyTime.valid() && CBaseBusiness::getTaskStatus(m_taskEnergyTime) == 1)
					{
						auto ret = m_taskEnergyTime.get();
						if (ret.first == 0)
							m_productionTime = ret.second;
						m_taskEnergyTime = std::future<std::pair<int, unsigned long>>();
					}
				}
			}

			if (m_productionTime > 0)
			{
				unsigned long productionTime = m_productionTime;
				// unsigned long currentTime = CDateTimeUtils::getTimeBySecond();

				ITimer::RealTimeDate t = os::Timer::getRealTimeAndDate();
				unsigned long currentTime = CDateTimeUtils::getSecondByDate(t);
				printf("Update time: %d; %d %d %d\n", currentTime, t.Hour, t.Minute, t.Second);

				int deltaTime = (int)(currentTime - productionTime);
				if (deltaTime < 0)
				{
					m_productionTime = 0;
					m_energyTime->setVisible(false);
					return;
				}

				int numEnergy = (int)(deltaTime / CFG_ENERGY_PRODUCTION_TIME);
				if (numEnergy > 0)
				{
					// need refresh
					updateProductionEnergy();
					m_productionTime = 0;
				}

				int waitTime = (unsigned long)CFG_ENERGY_PRODUCTION_TIME - (int)(deltaTime % (unsigned long)CFG_ENERGY_PRODUCTION_TIME);
				int min = waitTime / 60;
				int sec = waitTime % 60;

				char text[32];
				sprintf(text, "%02d:%02d", min, sec);
				m_textEnergyTime->setText(text);
				m_energyTime->setVisible(true);
			}
			else
			{
				m_energyTime->setVisible(false);
			}

			m_updateEnergyTime = 1000.0f;
		}
	}
	else
	{
		m_energyTime->setVisible(false);
	}
}

void CViewTop::updateProductionEnergy()
{
	m_servicesMgr->addTask(m_busEnergy->updateProduction(), [&](int ret) {
		m_viewMgr->runInUI([&]() {
			CViewBaseMenu::updateTopConsumer();
			});
		});
}

void CViewTop::onBtnAvatar(UI::CUIBase* btn)
{
	m_viewMgr->getLayer(0)->changeView<CViewAvatar>();
}

void CViewTop::onBtnName(UI::CUIBase* btn)
{
	m_viewMgr->getLayer(2)->pushView<CViewRename>();
}

void CViewTop::onBtnDiamond(UI::CUIBase* btn)
{
	CView* view = m_viewMgr->getLayer(0)->getCurrentView();
	CViewShop* shop = dynamic_cast<CViewShop*>(view);
	if (shop)
	{
		shop->setBottomTab(3, true);
	}
	else
	{
		shop = m_viewMgr->getLayer(0)->pushView<CViewShop>();
		shop->setBottomTab(3, false);
	}
}

void CViewTop::onBtnGold(UI::CUIBase* btn)
{
	CView* view = m_viewMgr->getLayer(0)->getCurrentView();
	CViewShop* shop = dynamic_cast<CViewShop*>(view);
	if (shop)
	{
		shop->setBottomTab(2, true);
	}
	else
	{
		shop = m_viewMgr->getLayer(0)->pushView<CViewShop>();
		shop->setBottomTab(2, false);
	}
}

void CViewTop::onBtnEnergy(UI::CUIBase* btn)
{
	m_viewMgr->getLayer(2)->pushView<CViewEnergyAds>();
}

void CViewTop::onBtnPower(UI::CUIBase* btn)
{
	m_viewMgr->getLayer(2)->pushView<CViewCombatPower>();
}