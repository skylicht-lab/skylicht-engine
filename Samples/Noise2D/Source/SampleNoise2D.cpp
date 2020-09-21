#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleNoise2D.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleNoise2D *app = new SampleNoise2D();
	getApplication()->registerAppEvent("SampleNoise2D", app);
}

SampleNoise2D::SampleNoise2D() :
	m_scene(NULL),
	m_forwardRP(NULL),
	m_noiseMaterial(NULL)
#if defined(USE_FREETYPE)	
	, m_largeFont(NULL)
#endif
{

}

SampleNoise2D::~SampleNoise2D()
{
	delete m_scene;
#if defined(USE_FREETYPE)	
	delete m_largeFont;
#endif
	delete m_noiseMaterial;
	delete m_forwardRP;
}

void SampleNoise2D::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

#if defined(USE_FREETYPE)
	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");
#endif

	// load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// create a Scene
	m_scene = new CScene();

	// create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// create 2D camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// create 3D camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

#if defined(USE_FREETYPE)
	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 50);

	// create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// create UI Text in Canvas
	CGUIText *textLarge = canvas->createText(m_largeFont);
	textLarge->setText("SampleNoise2D");
	textLarge->setTextAlign(CGUIElement::Left, CGUIElement::Bottom);
#endif

	m_noiseMaterial = new CMaterial("NoiseMaterial", "BuiltIn/Shader/Noise/Noise2D.xml");

	CGUIRect *noiseRect1 = canvas->createRect(core::rectf(0.0f, 0.0f, 256.0f, 256.0f), SColor(255, 255, 255, 255));
	noiseRect1->setMaterial(m_noiseMaterial);

	CGUIRect *noiseRect2 = canvas->createRect(core::rectf(0.0f, 280.0f, 256.0f, 536.0f), SColor(255, 255, 255, 255));
	noiseRect2->setMaterial(m_noiseMaterial);

	// rendering pipe line
	m_forwardRP = new CForwardRP();
}

void SampleNoise2D::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleNoise2D::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleNoise2D::onPostRender()
{
	// post render application
}

bool SampleNoise2D::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleNoise2D::onResume()
{
	// resume application
}

void SampleNoise2D::onPause()
{
	// pause application
}

void SampleNoise2D::onQuitApp()
{
	// end application
	delete this;
}