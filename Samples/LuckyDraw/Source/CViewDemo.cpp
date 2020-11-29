#include "pch.h"
#include "SkylichtEngine.h"
#include "Context/CContext.h"
#include "CViewDemo.h"

CViewDemo::CViewDemo() :
	m_largeFont(NULL),
	m_smallFont(NULL),
	m_textMediumFont(NULL),
	m_textSmallFont(NULL),
	m_sprite(NULL),
	m_state(0),
	m_spin(NULL),
	m_stop(NULL),
	m_accept(NULL),
	m_ignore(NULL),
	m_left(NULL),
	m_right(NULL),
	m_controller(NULL),
	m_countDown(false),
	m_randomNumber(0)
{
	for (int i = 0; i < MAX_STATE; i++)
		m_peopleCount[i] = 0;

	SkylichtAudio::initSkylichtAudio();

	// add to support read resource on ZIP Bundle
	m_streamFactory = new CZipAudioStreamFactory();
	SkylichtAudio::CAudioEngine::getSoundEngine()->registerStreamFactory(m_streamFactory);
}

CViewDemo::~CViewDemo()
{
	delete m_largeFont;
	delete m_smallFont;
	delete m_textMediumFont;
	delete m_textSmallFont;
	delete m_sprite;
	delete m_controller;

	delete m_spin;
	delete m_stop;
	delete m_accept;
	delete m_ignore;
	delete m_left;
	delete m_right;

	SkylichtAudio::CAudioEngine::getSoundEngine()->unRegisterStreamFactory(m_streamFactory);
	delete m_streamFactory;

	SkylichtAudio::releaseSkylichtAudio();
}

void CViewDemo::onInit()
{
	// create a Scene
	CScene *scene = CContext::getInstance()->getScene();
	CZone *zone = scene->getZone(0);

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("LasVegas", 200.0f);

	m_smallFont = new CGlyphFont();
	m_smallFont->setFont("LasVegas", 30.0f);

	m_textMediumFont = new CGlyphFont();
	m_textMediumFont->setFont("Sans", 40.0f);

	m_textSmallFont = new CGlyphFont();
	m_textSmallFont->setFont("Sans", 20.0f);

	// init audio
	SkylichtAudio::CAudioEngine *engine = SkylichtAudio::CAudioEngine::getSoundEngine();
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

	m_icon[0] = m_sprite->addFrame("0.png", "LuckyDraw/0.png");
	m_icon[1] = m_sprite->addFrame("1.png", "LuckyDraw/1.png");
	m_icon[2] = m_sprite->addFrame("2.png", "LuckyDraw/2.png");
	m_icon[3] = m_sprite->addFrame("3.png", "LuckyDraw/3.png");

	SFrame *btnLeft = m_sprite->addFrame("left.png", "LuckyDraw/left.png");
	SFrame *btnRight = m_sprite->addFrame("right.png", "LuckyDraw/right.png");
	SFrame *people = m_sprite->addFrame("people.png", "LuckyDraw/people.png");

	m_sprite->updateTexture();

	CTextureManager *tm = CTextureManager::getInstance();
	m_stateTexture[0] = tm->getTexture("LuckyDraw/state_0.png");
	m_stateTexture[1] = tm->getTexture("LuckyDraw/state_1.png");
	m_stateTexture[2] = tm->getTexture("LuckyDraw/state_2.png");
	m_stateTexture[3] = tm->getTexture("LuckyDraw/state_3.png");

	m_stateName[0] = "PRIZE SPECIAL";
	m_stateName[1] = "PRIZE 1";
	m_stateName[2] = "PRIZE 2";
	m_stateName[3] = "PRIZE 3";

	// create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// get canvas size
	const core::rectf& screenSize = canvas->getRootElement()->getRect();
	float screenW = screenSize.getWidth();
	float screenH = screenSize.getHeight();

	// create background
	m_backgroundImage = canvas->createImage();

	// create icon
	core::rectf iconSize = core::rectf(0.0f, 0.0f, 300.0f, 300.0f);
	m_iconSprite = canvas->createSprite(iconSize, m_icon[0]);
	float iconX = screenW / 2.0f;
	float iconY = screenH / 2.0f - 320.0f;
	m_iconSprite->setPosition(core::vector3df(iconX, iconY, 0.0f));
	m_iconSprite->setScale(core::vector3df(0.5f, 0.5f, 1.0f));

	// create number scroller	
	float numberW = 184.0f;
	float numberH = 305.0f;
	float itemH = 280.0f;
	float paddingY = 0.0f;
	float paddingX = 20.0f;
	int numScroller = 3;
	core::rectf scrollerSize(0.0f, 0.0f, numberW, numberH);

	float scrollerPosX = screenW / 2 - (numScroller * numberW + (numScroller - 1) * paddingX) / 2.0f;
	float scrollerPosY = screenH / 2 - numberH / 2.0f + paddingY;

	float startOffset = (numberH - itemH) / 2;

	for (int i = 0; i < numScroller; i++)
	{
		// create scoller background
		CGUIRect *scrollerGUI = canvas->createRect(scrollerSize, SColor(150, 0, 0, 0));
		scrollerGUI->setPosition(core::vector3df(scrollerPosX, scrollerPosY, 0.0f));

		// create scroller control
		CScroller *scroller = new CScroller(scrollerGUI, itemH, this);
		scroller->setStartOffset(startOffset);
		m_scrollers.push_back(scroller);

		// next scroll position
		scrollerPosX = scrollerPosX + numberW + paddingX;
	}

	m_controller = new CScrollerController(m_scrollers);

	// create label
	float textWidth = 800.0f;
	float textHeight = 60.0f;
	float textX = (screenW - textWidth) / 2.0f;
	float textY = (screenH - textHeight) / 2.0f - 220.0f;
	m_title = canvas->createText(m_textMediumFont);
	m_title->setRect(core::rectf(0.0, 0.0, textWidth, textHeight));
	m_title->setPosition(core::vector3df(textX, textY, 0.0f));
	m_title->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
	m_title->setColor(SColor(200, 255, 255, 255));

	// create Button
	float buttonW = btnYellowBackground->BoudingRect.getWidth();
	float buttonH = btnYellowBackground->BoudingRect.getHeight();
	float buttonPaddingY = 70.0f;
	float buttonAcceptPaddingX = 180.0f;
	core::rectf buttonSize(0.0f, 0.0f, buttonW, buttonH);
	float buttonX = (screenW - buttonW) / 2.0f;
	float buttonY = scrollerPosY + numberH + buttonPaddingY;

	CGUIElement *buttonSpinGUI = canvas->createElement(buttonSize);
	buttonSpinGUI->setPosition(core::vector3df(buttonX, buttonY, 0.0f));
	m_spin = new CButton(buttonSpinGUI, btnYellowBackground, "SPIN", m_smallFont, SColor(255, 107, 76, 8));
	m_spin->OnClick = std::bind(&CViewDemo::onSpinClick, this);

	CGUIElement *buttonStopGUI = canvas->createElement(buttonSize);
	buttonStopGUI->setPosition(core::vector3df(buttonX, buttonY, 0.0f));
	m_stop = new CButton(buttonStopGUI, btnYellowBackground, "STOP", m_smallFont, SColor(255, 107, 76, 8));
	m_stop->OnClick = std::bind(&CViewDemo::onStopClick, this);
	m_stop->setVisible(false);

	CGUIElement *buttonAcceptGUI = canvas->createElement(buttonSize);
	buttonAcceptGUI->setPosition(core::vector3df(buttonX - buttonAcceptPaddingX, buttonY, 0.0f));
	m_accept = new CButton(buttonAcceptGUI, btnYellowBackground, "ACCEPT", m_smallFont, SColor(255, 107, 76, 8));
	m_accept->OnClick = std::bind(&CViewDemo::onAcceptClick, this);
	m_accept->setVisible(false);

	CGUIElement *buttonQuitGUI = canvas->createElement(buttonSize);
	buttonQuitGUI->setPosition(core::vector3df(buttonX + buttonAcceptPaddingX, buttonY, 0.0f));
	m_ignore = new CButton(buttonQuitGUI, btnVioletBackground, "IGNORE", m_smallFont, SColor(255, 187, 179, 234));
	m_ignore->OnClick = std::bind(&CViewDemo::onIgnoreClick, this);
	m_ignore->setVisible(false);

	// button change state
	float leftW = btnLeft->BoudingRect.getWidth();
	float leftH = btnLeft->BoudingRect.getHeight();
	core::rectf leftBtnSize(0.0f, 0.0f, leftW, leftH);
	buttonX = (screenW - leftW) / 2.0f;
	buttonY = buttonY + 100.0f;

	CGUIElement *buttonLeftGUI = canvas->createElement(leftBtnSize);
	buttonLeftGUI->setPosition(core::vector3df(buttonX - 150.0f, buttonY, 0.0f));
	m_left = new CButton(buttonLeftGUI, btnLeft);
	m_left->setVisible(false);

	CGUIElement *buttonRightGUI = canvas->createElement(leftBtnSize);
	buttonRightGUI->setPosition(core::vector3df(buttonX + 150.0f, buttonY, 0.0f));
	m_right = new CButton(buttonRightGUI, btnRight);

	m_left->OnClick = [x = this, r = m_right, l = m_left]() {
		int state = x->getState();
		if (state < MAX_STATE - 1)
		{
			state++;
			x->setState(state);
			r->setVisible(true);
		}
		if (state == MAX_STATE - 1)
			l->setVisible(false);
	};

	m_right->OnClick = [x = this, r = m_right, l = m_left]() {
		int state = x->getState();
		if (state > 0)
		{
			state--;
			x->setState(state);
			l->setVisible(true);
		}
		if (state == 0)
			r->setVisible(false);
	};

	m_switchPrize = canvas->createText(m_textSmallFont);
	m_switchPrize->setRect(core::rectf(0.0f, 0.0f, 200.0f, leftH));
	m_switchPrize->setPosition(core::vector3df((screenW - 200.0f) * 0.5f - 50.0f, buttonY, 0.0f));
	m_switchPrize->setTextAlign(CGUIElement::Right, CGUIElement::Middle);

	m_iconPeople = canvas->createSprite(
		core::rectf(0.0f, 0.0f, people->BoudingRect.getWidth(), people->BoudingRect.getHeight()),
		people
	);
	m_iconPeople->setScale(core::vector3df(0.7f, 0.7f, 1.0f));
	m_iconPeople->setPosition(core::vector3df((screenW - people->BoudingRect.getWidth()) * 0.5f + 90.0f, buttonY + 10.0f, 0.0f));

	m_peopleText = canvas->createText(m_textSmallFont);
	m_peopleText->setRect(core::rectf(0.0f, 0.0f, 100.0f, leftH));
	m_peopleText->setPosition(core::vector3df((screenW - people->BoudingRect.getWidth()) * 0.5f + 135.0f, buttonY, 0.0f));
	m_peopleText->setTextAlign(CGUIElement::Left, CGUIElement::Middle);

	setState(MAX_STATE - 1);
}

void CViewDemo::setState(int state)
{
	m_state = state;
	m_title->setText(m_stateName[state].c_str());
	m_switchPrize->setText(m_stateName[state].c_str());
	m_backgroundImage->setImage(m_stateTexture[state]);
	m_iconSprite->setFrame(m_icon[state]);
	m_iconSprite->setAlignCenterModule();

	char num[32];
	sprintf(num, "%d", m_peopleCount[state]);
	m_peopleText->setText(num);

	m_soundUp->play();
}

void CViewDemo::showLeftRightButton()
{
	if (m_state == MAX_STATE - 1)
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
	m_randomNumber = os::Randomizer::rand() % 999;
	m_controller->stopOnNumber(m_randomNumber);

	m_stop->setVisible(false);
	printf("SampleLuckyDraw::onStopClick at: %d\n", m_randomNumber);

	m_soundStop->play();
}

void CViewDemo::onAcceptClick()
{
	m_title->setText(m_stateName[m_state].c_str());

	m_peopleCount[m_state]++;
	char num[32];
	sprintf(num, "%d", m_peopleCount[m_state]);
	m_peopleText->setText(num);


	m_controller->newRound();
	m_soundAccept->play();

	m_spin->setVisible(true);
	showLeftRightButton();

	m_accept->setVisible(false);
	m_ignore->setVisible(false);
}

void CViewDemo::onIgnoreClick()
{
	m_title->setText(m_stateName[m_state].c_str());

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
	CContext *context = CContext::getInstance();
	CScene *scene = context->getScene();
	if (scene != NULL)
	{
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
			sprintf(text, "Congratulation!\nPeople with number: %d", m_randomNumber);
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
	CCamera *guiCamera = CContext::getInstance()->getGUICamera();
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewDemo::onPostRender()
{

}

CGUIElement* CViewDemo::createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect)
{
	CCanvas *canvas = parent->getCanvas();

	CGUIText *textLarge = canvas->createText(parent, itemRect, m_largeFont);
	textLarge->setText("");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Top);
	textLarge->setColor(SColor(255, 255, 255, 255));

	return textLarge;
}

void CViewDemo::updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID)
{
	int number = core::abs_(itemID % 10);

	CGUIText *textLarge = dynamic_cast<CGUIText*>(item);
	char t[32];
	sprintf(t, "%d", number);
	textLarge->setText(t);
}