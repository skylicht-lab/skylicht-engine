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
	m_noiseMaterial(NULL),
	m_noiseOffset(3.0f, 0.0f, 0.0f)
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
	delete m_electricMaterial;
	delete m_electricLightningMaterial;
	delete m_burnMaterial;
	delete m_electricCircleMaterial;

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
	/*
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();
	*/

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
	textLarge->setTextAlign(CGUIElement::Left, CGUIElement::Top);
#endif

	m_noiseMaterial = new CMaterial("NoiseMaterial", "BuiltIn/Shader/Noise/Noise2D.xml");
	m_electricMaterial = new CMaterial("NoiseMaterial", "BuiltIn/Shader/Noise/Electric2D.xml");
	m_electricLightningMaterial = new CMaterial("NoiseMaterial", "BuiltIn/Shader/Noise/ElectricLightning2D.xml");
	m_burnMaterial = new CMaterial("NoiseMaterial", "BuiltIn/Shader/Noise/Burn2D.xml");
	m_electricCircleMaterial = new CMaterial("NoiseMaterial", "BuiltIn/Shader/Noise/ElectricCircle2D.xml");

	u32 w = app->getWidth();
	u32 h = app->getHeight();

	f32 cx = w * 0.5f;
	f32 cy = h * 0.5f;
	f32 rectSize = 256.0f;
	f32 offset = rectSize * 0.5f;
	f32 paddingX = 1.5f;
	f32 paddingY = 0.7f;

	// noise
	CGUIRect *noiseRect1 = canvas->createRect(core::rectf(0.0f, 0.0f, rectSize, rectSize), SColor(255, 255, 255, 255));
	noiseRect1->setMaterial(m_noiseMaterial);
	noiseRect1->setPosition(core::vector3df(cx - offset - rectSize * paddingX, cy - offset - rectSize * paddingY, 0.0f));

	// electric
	CGUIRect *noiseRect2 = canvas->createRect(core::rectf(0.0f, 0.0f, rectSize, rectSize), SColor(255, 255, 255, 255));
	noiseRect2->setMaterial(m_electricMaterial);
	noiseRect2->setPosition(core::vector3df(cx - offset, cy - offset - rectSize * paddingY, 0.0f));

	// lightning
	CGUIRect *noiseRect3 = canvas->createRect(core::rectf(0.0f, 0.0f, rectSize, rectSize), SColor(255, 255, 255, 255));
	noiseRect3->setMaterial(m_electricLightningMaterial);
	noiseRect3->setPosition(core::vector3df(cx - offset + rectSize * paddingX, cy - offset - rectSize * paddingY, 0.0f));

	// burn
	CGUIRect *noiseRect4 = canvas->createRect(core::rectf(0.0f, 0.0f, rectSize, rectSize), SColor(255, 255, 255, 255));
	noiseRect4->setMaterial(m_burnMaterial);
	noiseRect4->setPosition(core::vector3df(cx - offset - rectSize * paddingX, cy - offset + rectSize * paddingY, 0.0f));

	// circle electric
	CGUIRect *noiseRect6 = canvas->createRect(core::rectf(0.0f, 0.0f, rectSize, rectSize), SColor(255, 255, 255, 255));
	noiseRect6->setMaterial(m_electricCircleMaterial);
	noiseRect6->setPosition(core::vector3df(cx - offset + rectSize * paddingX, cy - offset + rectSize * paddingY, 0.0f));

	// rendering pipe line
	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleNoise2D::onUpdate()
{
	m_noiseOffset = m_noiseOffset + core::vector3df(0.002f, 0.000f, 0.0000f) * getTimeStep();

	float params[4];
	params[0] = m_noiseOffset.X;
	params[1] = m_noiseOffset.Y;
	params[2] = m_noiseOffset.Z;
	params[3] = 12.0f;

	m_noiseMaterial->setUniform4("uNoiseOffset", params);
	m_noiseMaterial->updateShaderParams();

	m_electricMaterial->setUniform4("uNoiseOffset", params);
	m_electricMaterial->updateShaderParams();

	m_electricLightningMaterial->setUniform4("uNoiseOffset", params);
	m_electricLightningMaterial->updateShaderParams();

	m_burnMaterial->setUniform4("uNoiseOffset", params);
	m_burnMaterial->updateShaderParams();

	m_electricCircleMaterial->setUniform4("uNoiseOffset", params);
	m_electricCircleMaterial->updateShaderParams();

	// update application
	m_scene->update();
}

void SampleNoise2D::onRender()
{
	// render 3d scene
	// m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

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

void SampleNoise2D::onResize(int w, int h)
{
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleNoise2D::onQuitApp()
{
	// end application
	delete this;
}