#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleNoise3D.h"

#include "GridPlane/CGridPlane.h"

#include "CSphereComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleNoise3D* app = new SampleNoise3D();
	getApplication()->registerAppEvent("SampleNoise3D", app);
}

SampleNoise3D::SampleNoise3D() :
	m_scene(NULL),
	m_forwardRP(NULL),
	m_postProcessorRP(NULL),
	m_noiseOffset(3.0f, 0.0f, 0.0f),
	m_largeFont(NULL)
{

}

SampleNoise3D::~SampleNoise3D()
{
	delete m_scene;
	delete m_largeFont;
	delete m_forwardRP;
	delete m_postProcessorRP;
}

void SampleNoise3D::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// load basic shader
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// create a Scene
	m_scene = new CScene();

	// create a Zone in Scene
	CZone* zone = m_scene->createZone();

	// create 2D camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// create 3D camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	// CGameObject *grid = zone->createEmptyObject();
	// grid->addComponent<CGridPlane>();

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// add sphere
	CGameObject* sphereObj;
	CSphereComponent* sphere;

	sphereObj = zone->createEmptyObject();
	sphere = sphereObj->addComponent<CSphereComponent>();
	sphere->getMaterial()->changeShader("BuiltIn/Shader/Noise/Noise3D.xml");
	sphereObj->getTransformEuler()->setPosition(core::vector3df(3.0f, 0.0f, 0.0f));
	m_materials.push_back(sphere->getMaterial());

	sphereObj = zone->createEmptyObject();
	sphere = sphereObj->addComponent<CSphereComponent>();
	sphere->getMaterial()->changeShader("BuiltIn/Shader/Noise/Electric3D.xml");
	sphereObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 0.0f, 0.0f));
	m_materials.push_back(sphere->getMaterial());

	sphereObj = zone->createEmptyObject();
	sphere = sphereObj->addComponent<CSphereComponent>();
	sphere->getMaterial()->changeShader("BuiltIn/Shader/Noise/StarSequence3D.xml");
	sphereObj->getTransformEuler()->setPosition(core::vector3df(-3.0f, 0.0f, 0.0f));
	m_materials.push_back(sphere->getMaterial());

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 50);

	// create 2D Canvas
	CGameObject* canvasObject = zone->createEmptyObject();
	CCanvas* canvas = canvasObject->addComponent<CCanvas>();

	// create UI Text in Canvas
	CGUIText* textLarge = canvas->createText(m_largeFont);
	textLarge->setText("SampleNoise3D");
	textLarge->setTextAlign(EGUIHorizontalAlign::Left, EGUIVerticalAlign::Top);

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);

	m_postProcessorRP = new CPostProcessorRP();
	m_postProcessorRP->enableBloomEffect(true);
	m_postProcessorRP->enableFXAA(true);
	m_postProcessorRP->initRender(w, h);

	m_forwardRP->setPostProcessor(m_postProcessorRP);
}

void SampleNoise3D::onUpdate()
{
	m_noiseOffset = m_noiseOffset + core::vector3df(-0.0003f, 0.0000f, 0.0003f) * getTimeStep();

	float params[4];
	params[0] = m_noiseOffset.X;
	params[1] = m_noiseOffset.Y;
	params[2] = m_noiseOffset.Z;
	params[3] = 8.0f;

	for (CMaterial* m : m_materials)
	{
		m->setUniform4("uNoiseOffset", params);
		m->updateShaderParams();
	}

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

void SampleNoise3D::onResize(int w, int h)
{
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);

	if (m_postProcessorRP != NULL)
		m_postProcessorRP->resize(w, h);
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