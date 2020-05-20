#include "pch.h"
#include "SkylichtEngine.h"
#include "CScroller.h"
#include "CButton.h"
#include "SampleLuckyDraw.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLuckyDraw *app = new SampleLuckyDraw();
	getApplication()->registerAppEvent("SampleLuckyDraw", app);
}

SampleLuckyDraw::SampleLuckyDraw() :
	m_scene(NULL),
	m_largeFont(NULL),
	m_smallFont(NULL),
	m_sprite(NULL),
	m_state(0)
{

}

SampleLuckyDraw::~SampleLuckyDraw()
{
	delete m_scene;
	delete m_largeFont;
	delete m_smallFont;
	delete m_sprite;
}

void SampleLuckyDraw::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("LuckyDraw.zip"), false, false);

	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");
	freetypeFont->initFont("LasVegas", "LuckyDraw/LasVegasJackpotRegular.otf");

	// load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// create a Scene
	m_scene = new CScene();

	// create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// create 2D camera
	CGameObject *guiCameraObject = (CGameObject*)zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("LasVegas", 200);

	m_smallFont = new CGlyphFont();
	m_smallFont->setFont("LasVegas", 30);

	// create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// get canvas size
	const core::rectf& screenSize = canvas->getRootElement()->getRect();

	// create background
	m_backgroundImage = canvas->createImage();
	m_backgroundImage->setImage(CTextureManager::getInstance()->getTexture("LuckyDraw/state_01.png"));

	// create number scroller	
	float numberW = 184.0f;
	float numberH = 305.0f;
	float itemH = 280.0f;
	float paddingY = 50.0f;
	float paddingX = 20.0f;
	int numScroller = 4;
	core::rectf scrollerSize(0.0f, 0.0f, numberW, numberH);

	float scrollerPosX = screenSize.getWidth() / 2 - (numScroller * numberW + (numScroller - 1) * paddingX) / 2.0f;
	float scrollerPosY = screenSize.getHeight() / 2 - numberH / 2.0f + paddingY;

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

		scrollerPosX = scrollerPosX + numberW + paddingX;
	}

	// create Button
	m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 1024, 1024);
	SFrame* btnYellowBackground = m_sprite->addFrame("btn_yellow.png", "LuckyDraw/btn_yellow.png");
	SFrame* btnVioletBackground = m_sprite->addFrame("btn_violet.png", "LuckyDraw/btn_violet.png");
	m_sprite->updateTexture();

	core::rectf buttonSize(0.0f, 0.0f,
		btnYellowBackground->BoudingRect.getWidth(),
		btnYellowBackground->BoudingRect.getHeight());

	CGUIElement *buttonSpinGUI = canvas->createElement(buttonSize);
	buttonSpinGUI->setPosition(core::vector3df(0.0f, 0.0f, 0.0f));
	m_spin = new CButton(buttonSpinGUI, btnYellowBackground, "SPIN", m_smallFont, SColor(255, 107, 76, 8));

	CGUIElement *buttonBackGUI = canvas->createElement(buttonSize);
	buttonBackGUI->setPosition(core::vector3df(0.0f, 70.0f, 0.0f));
	m_spin = new CButton(buttonBackGUI, btnVioletBackground, "BACK", m_smallFont, SColor(255, 187, 179, 234));

}

void SampleLuckyDraw::onUpdate()
{
	// gui scroller update
	for (u32 i = 0, n = m_scrollers.size(); i < n; i++)
	{
		//float speed = 0.5f;
		//float f = m_scrollers[i]->getOffset();
		//f = f + getTimeStep() * speed;

		//m_scrollers[i]->setOffset(f);
		m_scrollers[i]->update();
	}

	// update application
	m_scene->update();
}

void SampleLuckyDraw::onRender()
{
	// Render hello,world text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleLuckyDraw::onPostRender()
{
	// post render application
}

bool SampleLuckyDraw::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleLuckyDraw::onResume()
{
	// resume application
}

void SampleLuckyDraw::onPause()
{
	// pause application
}

void SampleLuckyDraw::onQuitApp()
{
	// end application
	delete this;
}

CGUIElement* SampleLuckyDraw::createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect)
{
	CCanvas *canvas = parent->getCanvas();

	CGUIText *textLarge = canvas->createText(parent, itemRect, m_largeFont);
	textLarge->setText("");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Top);
	textLarge->setColor(SColor(255, 255, 255, 255));

	return textLarge;
}

void SampleLuckyDraw::updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID)
{
	int number = core::abs_(itemID % 10);

	CGUIText *textLarge = dynamic_cast<CGUIText*>(item);
	char t[32];
	sprintf(t, "%d", number);
	textLarge->setText(t);
}