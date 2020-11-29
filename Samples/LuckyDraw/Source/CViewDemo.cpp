#include "pch.h"
#include "SkylichtEngine.h"
#include "Context/CContext.h"
#include "CViewDemo.h"

CViewDemo::CViewDemo() :
	m_largeFont(NULL),
	m_smallFont(NULL),
	m_sprite(NULL),
	m_state(0),
	m_spin(NULL),
	m_quit(NULL),
	m_controller(NULL),
	m_countDown(false)
{
	SkylichtAudio::initSkylichtAudio();

	// add to support read resource on ZIP Bundle
	m_streamFactory = new CZipAudioStreamFactory();
	SkylichtAudio::CAudioEngine::getSoundEngine()->registerStreamFactory(m_streamFactory);
}

CViewDemo::~CViewDemo()
{
	delete m_largeFont;
	delete m_smallFont;
	delete m_sprite;
	delete m_spin;
	delete m_quit;
	delete m_controller;

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
	m_largeFont->setFont("LasVegas", 200);

	m_smallFont = new CGlyphFont();
	m_smallFont->setFont("LasVegas", 30);

	// create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// get canvas size
	const core::rectf& screenSize = canvas->getRootElement()->getRect();
	float screenW = screenSize.getWidth();
	float screenH = screenSize.getHeight();

	// create background
	m_backgroundImage = canvas->createImage();
	m_backgroundImage->setImage(CTextureManager::getInstance()->getTexture("LuckyDraw/state_01.png"));

	// create number scroller	
	float numberW = 184.0f;
	float numberH = 305.0f;
	float itemH = 280.0f;
	float paddingY = 0.0f;
	float paddingX = 20.0f;
	int numScroller = 4;
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

	// create Button
	m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 1024, 1024);
	SFrame* btnYellowBackground = m_sprite->addFrame("btn_yellow.png", "LuckyDraw/btn_yellow.png");
	SFrame* btnVioletBackground = m_sprite->addFrame("btn_violet.png", "LuckyDraw/btn_violet.png");
	m_sprite->updateTexture();

	float buttonW = btnYellowBackground->BoudingRect.getWidth();
	float buttonH = btnYellowBackground->BoudingRect.getHeight();
	float buttonPaddingY = 70.0f;
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

	CGUIElement *buttonBackGUI = canvas->createElement(buttonSize);
	buttonBackGUI->setPosition(core::vector3df(buttonX, buttonY + 70.0f, 0.0f));
	m_quit = new CButton(buttonBackGUI, btnVioletBackground, "QUIT", m_smallFont, SColor(255, 187, 179, 234));
	m_quit->OnClick = []() {
		getIrrlichtDevice()->closeDevice();
	};

	// init audio
	SkylichtAudio::CAudioEngine *engine = SkylichtAudio::CAudioEngine::getSoundEngine();
	m_musicBG = engine->createAudioEmitter("LuckyDraw/background.mp3", false);
	m_musicBG->setLoop(true);
	m_soundTada = engine->createAudioEmitter("LuckyDraw/game-tada.mp3", false);
	m_soundSpin = engine->createAudioEmitter("LuckyDraw/sm-spin.mp3", true);
	m_soundStop = engine->createAudioEmitter("LuckyDraw/sm-bet.mp3", true);
	m_soundShowStop = engine->createAudioEmitter("LuckyDraw/game-bonus.mp3", true);
	m_soundCountDown = engine->createAudioEmitter("LuckyDraw/game-countdown.mp3", true);
}

void CViewDemo::onSpinClick()
{
	m_musicBG->playWithFade(1.0f, 500.0f);
	m_controller->beginScroll();

	m_spin->setVisible(false);
	m_quit->setVisible(false);

	m_soundSpin->play();
	m_countDown = false;
}

void CViewDemo::onStopClick()
{
	int random = os::Randomizer::rand() % 9999;
	m_controller->stopOnNumber(random);

	m_stop->setVisible(false);
	printf("SampleLuckyDraw::onStopClick at: %d\n", random);

	m_soundStop->play();
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
		if (m_controller->isFinished() == true && m_spin->isVisible() == false)
		{
			m_soundTada->play();
			m_soundCountDown->stopWithFade(100.0f);

			m_spin->setVisible(true);
			m_quit->setVisible(true);
		}

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