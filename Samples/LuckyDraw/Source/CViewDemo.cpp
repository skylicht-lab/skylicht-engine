#include "pch.h"
#include "SkylichtEngine.h"
#include "Context/CContext.h"
#include "CViewDemo.h"

CViewDemo::CViewDemo() :
	m_largeFont(NULL),
	m_textMediumFont(NULL),
	m_textSmallFont(NULL),
	m_textMedium2Font(NULL),
	m_sprite(NULL),
	m_state(0),
	m_canvas(NULL),
	m_spin(NULL),
	m_stop(NULL),
	m_accept(NULL),
	m_ignore(NULL),
	m_left(NULL),
	m_right(NULL),
	m_controller(NULL),
	m_list(NULL),
	m_countDown(false),
	m_randomNumber(0),
	m_randomPeople(0)
{
	Audio::initSkylichtAudio();

	// add to support read resource on ZIP Bundle
	m_streamFactory = new CZipAudioStreamFactory();
	Audio::CAudioEngine::getSoundEngine()->registerStreamFactory(m_streamFactory);
}

CViewDemo::~CViewDemo()
{
	delete m_largeFont;
	delete m_textMediumFont;
	delete m_textMedium2Font;
	delete m_textSmallFont;
	delete m_sprite;

	for (CScroller* s : m_scrollers)
		delete s;
	m_scrollers.clear();
	delete m_controller;
	delete m_list;

	delete m_spin;
	delete m_stop;
	delete m_accept;
	delete m_ignore;
	delete m_left;
	delete m_right;

	Audio::CAudioEngine::getSoundEngine()->unRegisterStreamFactory(m_streamFactory);
	delete m_streamFactory;

	Audio::releaseSkylichtAudio();
}

void CViewDemo::onInit()
{
	// create a Scene
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("LasVegas", 200.0f);

	m_textMediumFont = new CGlyphFont();
	m_textMediumFont->setFont("Sans", 40.0f);

	m_textMedium2Font = new CGlyphFont();
	m_textMedium2Font->setFont("Sans", 30.0f);

	m_textSmallFont = new CGlyphFont();
	m_textSmallFont->setFont("Sans", 18.0f);

	// init audio
	Audio::CAudioEngine* engine = Audio::CAudioEngine::getSoundEngine();
	m_musicBG = engine->createAudioEmitter("LuckyDraw/background.mp3", false);
	m_musicBG->setLoop(true);
	m_soundTada = engine->createAudioEmitter("LuckyDraw/game-tada.mp3", false);
	m_soundSpin = engine->createAudioEmitter("LuckyDraw/sm-spin.mp3", true);
	m_soundStop = engine->createAudioEmitter("LuckyDraw/sm-bet.mp3", true);
	m_soundShowStop = engine->createAudioEmitter("LuckyDraw/game-bonus.mp3", true);
	m_soundCountDown = engine->createAudioEmitter("LuckyDraw/game-countdown.mp3", true);
	m_soundAccept = engine->createAudioEmitter("LuckyDraw/fanfare-brass.mp3", true);
	m_soundIgnore = engine->createAudioEmitter("LuckyDraw/game-over.mp3", true);
	m_soundUp = engine->createAudioEmitter("LuckyDraw/game-levelup-s3.mp3", true);

	// atlas frame
	m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 1024, 1024);
	SFrame* btnYellowBackground = m_sprite->addFrame("btn_yellow.png", "LuckyDraw/btn_yellow.png");
	SFrame* btnVioletBackground = m_sprite->addFrame("btn_violet.png", "LuckyDraw/btn_violet.png");

	// button, icon sprite
	SFrame* btnLeft = m_sprite->addFrame("left.png", "LuckyDraw/left.png");
	SFrame* btnRight = m_sprite->addFrame("right.png", "LuckyDraw/right.png");
	SFrame* people = m_sprite->addFrame("people.png", "LuckyDraw/people.png");

	CTextureManager* tm = CTextureManager::getInstance();
	m_prize[0].Background = tm->getTexture("LuckyDraw/state_0.png");
	m_prize[1].Background = tm->getTexture("LuckyDraw/state_1.png");
	m_prize[2].Background = tm->getTexture("LuckyDraw/state_2.png");
	m_prize[3].Background = tm->getTexture("LuckyDraw/state_3.png");

	m_prize[0].Name = CLocalize::get("TXT_PRIZE_0");
	m_prize[1].Name = CLocalize::get("TXT_PRIZE_1");
	m_prize[2].Name = CLocalize::get("TXT_PRIZE_2");
	m_prize[3].Name = CLocalize::get("TXT_PRIZE_3");

	m_prize[0].Icon = m_sprite->addFrame("0.png", "LuckyDraw/0.png");
	m_prize[1].Icon = m_sprite->addFrame("1.png", "LuckyDraw/1.png");
	m_prize[2].Icon = m_sprite->addFrame("2.png", "LuckyDraw/2.png");
	m_prize[3].Icon = m_sprite->addFrame("3.png", "LuckyDraw/3.png");

	m_prize[0].TotalPeopleCount = 1;
	m_prize[1].TotalPeopleCount = 1;
	m_prize[2].TotalPeopleCount = 2;
	m_prize[3].TotalPeopleCount = 3;

	m_sprite->updateTexture();

	// init people
	for (int i = 0; i < 200; i++)
	{
		m_people.push_back(SPeople());
		SPeople& p = m_people.back();
		p.ID = i + 1;
		p.IsLucky = false;
	}

	// create 2D Canvas
	CGameObject* canvasObject = zone->createEmptyObject();
	m_canvas = canvasObject->addComponent<CCanvas>();

	// show debug rectangle
	// m_canvas->IsInEditor = true;

	// create background
	m_backgroundImage = m_canvas->createImage();
	m_backgroundImage->setDock(EGUIDock::DockFill);

	// create icon
	core::rectf iconSize = core::rectf(0.0f, 0.0f, 260.0f, 260.0f);
	m_iconSprite = m_canvas->createSprite(iconSize, NULL);
	m_iconSprite->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	m_iconSprite->setPosition(core::vector3df(130.0f, 130.0f - 300.0f, 0.0f));
	m_iconSprite->setScale(core::vector3df(0.5f, 0.5f, 1.0f));

	// create number scroller	
	float numberW = 184.0f;
	float numberH = 305.0f;
	float itemH = 280.0f;
	float paddingY = 30.0f;
	float paddingX = 20.0f;

	int numScroller = getNumScroller();

	core::rectf scrollerSize(0.0f, 0.0f, numberW, numberH);

	float scrollerPosX = -(numScroller * numberW + (numScroller - 1) * paddingX) * 0.5f + numberW * 0.5f;
	float scrollerPosY = paddingY;

	float startOffset = (numberH - itemH) / 2;

	for (int i = 0; i < numScroller; i++)
	{
		// create scoller background
		CGUIRect* scrollerGUI = m_canvas->createRect(scrollerSize, SColor(150, 0, 0, 0));
		scrollerGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
		scrollerGUI->setPosition(core::vector3df(scrollerPosX, scrollerPosY, 0.0f));

		// create scroller control
		CScroller* scroller = new CScroller(scrollerGUI, itemH, this);
		scroller->setStartOffset(startOffset);
		m_scrollers.push_back(scroller);

		// next scroll position
		scrollerPosX = scrollerPosX + numberW + paddingX;
	}

	m_controller = new CScrollerController(m_scrollers);

	// create list
	float listWidth = 740.0f;
	float listHeight = 300.0f;
	float listY = paddingY;
	core::rectf listSize(0.0f, 0.0f, listWidth, listHeight);

	CGUIRect* listGUI = m_canvas->createRect(listSize, SColor(150, 0, 0, 0));
	listGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	listGUI->setPosition(core::vector3df(0.0f, listY, 0.0f));
	m_list = new CList(listGUI);
	m_list->setVisible(false);

	// create label
	float textWidth = 800.0f;
	float textHeight = 60.0f;
	m_title = m_canvas->createText(m_textMediumFont);
	m_title->setRect(core::rectf(0.0, 0.0, textWidth, textHeight));
	m_title->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	m_title->setPosition(core::vector3df(0.0f, -190.0f, 0.0f));
	m_title->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	m_title->setColor(SColor(200, 255, 255, 255));

	// create spin/stop/accept/inorge button
	float buttonY = numberH * 0.5f + 100.0f;
	float buttonW = btnYellowBackground->getWidth();
	float buttonH = btnYellowBackground->getHeight();
	float buttonAcceptPaddingX = 180.0f;
	core::rectf buttonSize(0.0f, 0.0f, buttonW, buttonH);

	CGUIElement* buttonSpinGUI = m_canvas->createElement(buttonSize);
	buttonSpinGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	buttonSpinGUI->setPosition(core::vector3df(0.0f, buttonY, 0.0f));
	m_spin = new CButton(buttonSpinGUI, btnYellowBackground, CLocalize::get("TXT_SPIN"), m_textMedium2Font, SColor(255, 107, 76, 8));
	m_spin->OnClick = std::bind(&CViewDemo::onSpinClick, this);

	CGUIElement* buttonStopGUI = m_canvas->createElement(buttonSize);
	buttonStopGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	buttonStopGUI->setPosition(core::vector3df(0.0f, buttonY, 0.0f));
	m_stop = new CButton(buttonStopGUI, btnYellowBackground, CLocalize::get("TXT_STOP"), m_textMedium2Font, SColor(255, 107, 76, 8));
	m_stop->OnClick = std::bind(&CViewDemo::onStopClick, this);
	m_stop->setVisible(false);

	CGUIElement* buttonAcceptGUI = m_canvas->createElement(buttonSize);
	buttonAcceptGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	buttonAcceptGUI->setPosition(core::vector3df(-buttonAcceptPaddingX, buttonY, 0.0f));
	m_accept = new CButton(buttonAcceptGUI, btnYellowBackground, CLocalize::get("TXT_ACCEPT"), m_textMedium2Font, SColor(255, 107, 76, 8));
	m_accept->OnClick = std::bind(&CViewDemo::onAcceptClick, this);
	m_accept->setVisible(false);

	CGUIElement* buttonQuitGUI = m_canvas->createElement(buttonSize);
	buttonQuitGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	buttonQuitGUI->setPosition(core::vector3df(buttonAcceptPaddingX, buttonY, 0.0f));
	m_ignore = new CButton(buttonQuitGUI, btnVioletBackground, CLocalize::get("TXT_IGNORE"), m_textMedium2Font, SColor(255, 187, 179, 234));
	m_ignore->OnClick = std::bind(&CViewDemo::onIgnoreClick, this);
	m_ignore->setVisible(false);

	// left/right button to change state
	float leftW = btnLeft->getWidth();
	float leftH = btnLeft->getHeight();
	core::rectf leftBtnSize(0.0f, 0.0f, leftW, leftH);
	buttonY = 20.0f;

#if defined(IOS)
    buttonY = 100.0f;
#endif
    
	CGUIElement* buttonLeftGUI = m_canvas->createElement(leftBtnSize);
	buttonLeftGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Bottom);
	buttonLeftGUI->setPosition(core::vector3df(-150.0f, buttonY, 0.0f));
	m_left = new CButton(buttonLeftGUI, btnLeft);
	m_left->setVisible(false);

	CGUIElement* buttonRightGUI = m_canvas->createElement(leftBtnSize);
	buttonRightGUI->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Bottom);
	buttonRightGUI->setPosition(core::vector3df(150.0f, buttonY, 0.0f));
	m_right = new CButton(buttonRightGUI, btnRight);

	m_left->OnClick = [&, r = m_right, l = m_left]() {
		int state = getState();
		if (state < getNumState() - 1)
		{
			state++;
			setState(state);
			r->setVisible(true);
		}
		if (state == getNumState() - 1)
			l->setVisible(false);
	};

	m_right->OnClick = [&, r = m_right, l = m_left]() {
		int state = getState();
		if (state > 0)
		{
			state--;
			setState(state);
			l->setVisible(true);
		}
		if (state == 0)
			r->setVisible(false);
	};

	m_switchPrize = m_canvas->createText(m_textSmallFont);
	m_switchPrize->setRect(core::rectf(0.0f, 0.0f, 200.0f, leftH));
	m_switchPrize->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Bottom);
	m_switchPrize->setPosition(core::vector3df(-80.0f, buttonY, 0.0f));
	m_switchPrize->setTextAlign(EGUIHorizontalAlign::Right, EGUIVerticalAlign::Middle);

	m_iconPeople = m_canvas->createSprite(
		core::rectf(0.0f, 0.0f, people->getWidth() * 0.5f, people->getHeight() * 0.5f),
		people
	);
	m_iconPeople->setScale(core::vector3df(0.5f, 0.5f, 1.0f));
	m_iconPeople->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Bottom);
	m_iconPeople->setPosition(core::vector3df(60.0f, buttonY, 0.0f));

	m_peopleText = m_canvas->createText(m_textSmallFont);
	m_peopleText->setRect(core::rectf(0.0f, 0.0f, 100.0f, leftH));
	m_peopleText->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Bottom);
	m_peopleText->setPosition(core::vector3df(125.0f, buttonY, 0.0f));
	m_peopleText->setTextAlign(EGUIHorizontalAlign::Left, EGUIVerticalAlign::Middle);

	setState(getNumState() - 1);
}

int CViewDemo::getNumState()
{
	return MAX_STATE;
}

int CViewDemo::getNumScroller()
{
	int maxID = 0;
	for (SPeople& p : m_people)
	{
		maxID = core::max_(maxID, p.ID);
	}

	int num = 0;
	while (maxID > 0)
	{
		maxID = maxID / 10;
		num++;
	}

	if (num < 2)
		num = 2;

	return num;
}

void CViewDemo::setState(int state)
{
	m_state = state;

	m_title->setText(m_prize[state].Name.c_str());
	m_switchPrize->setText(m_prize[state].Name.c_str());
	m_backgroundImage->setImage(m_prize[state].Background);
	m_iconSprite->setFrame(m_prize[state].Icon);
	m_iconSprite->setAlignCenterModule();

	char num[32];
	sprintf(num, "%d/%d", m_prize[state].PeopleCount, m_prize[state].TotalPeopleCount);
	m_peopleText->setText(num);

	m_soundUp->play();

	checkToShowListLuckyPeople();
}

void CViewDemo::checkToShowListLuckyPeople()
{
	if (m_prize[m_state].PeopleCount >= m_prize[m_state].TotalPeopleCount)
	{
		m_controller->setVisible(false);
		m_spin->setVisible(false);
		m_list->setVisible(true);

		m_list->clearAllItem();

		CGUIElement* listElement = m_list->getElement();

		float height = 0.0f;

		for (SPeople& p : m_people)
		{
			if (p.WinPrize == m_state)
			{
				core::rectf itemRect = core::rectf(0.0f, 0.0f, listElement->getWidth(), 50.0f);

				height = height + itemRect.getHeight();

				CGUIElement* element = m_canvas->createElement(listElement, itemRect);

				CGUIText* nameText = m_canvas->createText(element, itemRect, m_textMedium2Font);
				nameText->setPosition(core::vector3df(200.0f, 0.0f, 0.0f));

				itemRect = core::rectf(0.0f, 0.0f, 120, 50.0f);
				CGUIText* numText = m_canvas->createText(element, itemRect, m_textMedium2Font);
				numText->setPosition(core::vector3df(50.0f, 0.0f, 0.0f));
				numText->setTextAlign(EGUIHorizontalAlign::Right, EGUIVerticalAlign::Top);

				char text[512];
				sprintf(text, CLocalize::get("TXT_LUCKY_PEOPLE_ID"), p.ID);

				nameText->setText(text);

				sprintf(text, "%d", p.ID);
				numText->setText(text);

				m_list->addItem(element);
			}
		}

		// center item in list
		if (height < listElement->getHeight())
		{
			float centerY = listElement->getHeight() / 2.0f - height / 2.0f;
			m_list->setBasePosition(centerY);
		}
		else
		{
			m_list->setBasePosition(0.0f);
		}
	}
	else
	{
		m_controller->setVisible(true);
		m_spin->setVisible(true);
		m_list->setVisible(false);
	}
}

void CViewDemo::showLeftRightButton()
{
	if (m_state == getNumState() - 1)
		m_left->setVisible(false);
	else
		m_left->setVisible(true);

	if (m_state == 0)
		m_right->setVisible(false);
	else
		m_right->setVisible(true);
}

void CViewDemo::onSpinClick()
{
	m_listPeople.set_used(0);
	for (SPeople& p : m_people)
	{
		if (p.IsLucky == false)
			m_listPeople.push_back(p.ID);
	}

	if (m_listPeople.size() == 0)
	{
		m_soundStop->play();
		return;
	}

	m_musicBG->playWithFade(1.0f, 500.0f);
	m_controller->beginScroll();

	m_spin->setVisible(false);
	m_left->setVisible(false);
	m_right->setVisible(false);

	m_soundSpin->play();
	m_countDown = false;
}

void CViewDemo::onStopClick()
{
	m_randomPeople = os::Randomizer::rand() % m_listPeople.size();
	m_randomNumber = m_listPeople[m_randomPeople];

	m_people[m_randomNumber - 1].IsLucky = true;

	m_controller->stopOnNumber(m_randomNumber);

	m_stop->setVisible(false);
	printf("SampleLuckyDraw::onStopClick at: %d\n", m_randomNumber);

	m_soundStop->play();
}

void CViewDemo::onAcceptClick()
{
	m_prize[m_state].PeopleCount++;

	m_people[m_randomNumber - 1].WinPrize = m_state;

	m_title->setText(m_prize[m_state].Name.c_str());

	char num[32];
	sprintf(num, "%d/%d", m_prize[m_state].PeopleCount, m_prize[m_state].TotalPeopleCount);
	m_peopleText->setText(num);

	m_controller->newRound();
	m_soundAccept->play();

	m_spin->setVisible(true);
	showLeftRightButton();

	m_accept->setVisible(false);
	m_ignore->setVisible(false);

	checkToShowListLuckyPeople();
}

void CViewDemo::onIgnoreClick()
{
	m_title->setText(m_prize[m_state].Name.c_str());

	m_controller->newRound();
	m_soundIgnore->play();

	m_spin->setVisible(true);
	showLeftRightButton();

	m_accept->setVisible(false);
	m_ignore->setVisible(false);
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
	{
		// update list lucky people
		m_list->update();

		// update scroller
		m_controller->update();

		// show stop button
		if (m_controller->stopReady() == true && m_stop->isVisible() == false)
		{
			m_stop->setVisible(true);
			m_soundShowStop->play();
		}

		// show spin button when finish
		if (m_controller->isFinished() == true &&
			m_accept->isVisible() == false &&
			m_spin->isVisible() == false)
		{
			char text[512];
			sprintf(text, CLocalize::get("TXT_CONGRAT_PEOPLE_ID"), m_randomNumber);

			m_title->setText(text);

			m_soundTada->play();
			m_soundCountDown->stopWithFade(100.0f);

			m_accept->setVisible(true);
			m_ignore->setVisible(true);
		}

		// stop music and play count down at last stop number
		if (m_controller->isLastStopPosition() == true && m_countDown == false)
		{
			m_musicBG->stopWithFade(1000.0f);
			m_soundCountDown->play();
			m_countDown = true;
		}

		// update application
		scene->update();
	}
}

void CViewDemo::onRender()
{
	CCamera* guiCamera = CContext::getInstance()->getGUICamera();
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewDemo::onPostRender()
{

}

CGUIElement* CViewDemo::createScrollElement(CScroller* scroller, CGUIElement* parent, const core::rectf& itemRect)
{
	CGUIText* textLarge = m_canvas->createText(parent, itemRect, m_largeFont);
	textLarge->setText("");
	textLarge->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Top);
	textLarge->setColor(SColor(255, 255, 255, 255));

	return textLarge;
}

void CViewDemo::updateScrollElement(CScroller* scroller, CGUIElement* item, int itemID)
{
	int number = core::abs_(itemID % 10);

	CGUIText* textLarge = dynamic_cast<CGUIText*>(item);
	char t[32];
	sprintf(t, "%d", number);
	textLarge->setText(t);
}
