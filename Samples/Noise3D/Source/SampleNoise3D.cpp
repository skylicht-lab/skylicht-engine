#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleNoise3D.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleNoise3D *app = new SampleNoise3D();
	getApplication()->registerAppEvent("SampleNoise3D", app);
}

SampleNoise3D::SampleNoise3D() :
	m_scene(NULL),
	m_forwardRP(NULL)
#if defined(USE_FREETYPE)	
	, m_largeFont(NULL)
#endif
{

}

SampleNoise3D::~SampleNoise3D()
{
	delete m_scene;
#if defined(USE_FREETYPE)	
	delete m_largeFont;
#endif
	delete m_forwardRP;
}

void SampleNoise3D::onInitApp()
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
	// CGUIText *textLarge = canvas->createText(m_largeFont);
	// textLarge->setText("SampleNoise3D");
	// textLarge->setTextAlign(CGUIElement::Left, CGUIElement::Bottom);
#endif

	// rendering pipe line
	m_forwardRP = new CForwardRP();
}

void SampleNoise3D::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleNoise3D::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleNoise3D::onPostRender()
{
	// post render application
}

bool SampleNoise3D::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleNoise3D::onResume()
{
	// resume application
}

void SampleNoise3D::onPause()
{
	// pause application
}

void SampleNoise3D::onQuitApp()
{
	// end application
	delete this;
}