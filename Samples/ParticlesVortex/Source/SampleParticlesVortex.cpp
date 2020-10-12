#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleParticlesVortex.h"
#include "imgui.h"
#include "CImguiManager.h"

#include "GridPlane/CGridPlane.h"
#include "ParticleSystem/CParticleComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleParticlesVortex *app = new SampleParticlesVortex();
	getApplication()->registerAppEvent("SampleParticlesVortex", app);
}

SampleParticlesVortex::SampleParticlesVortex() :
	m_scene(NULL),
	m_currentParticleObj(NULL),
	m_vortexSystem(NULL)
{
	CImguiManager::createGetInstance();
}

SampleParticlesVortex::~SampleParticlesVortex()
{
	delete m_vortexSystem;
	delete m_scene;
	CImguiManager::releaseInstance();
}

void SampleParticlesVortex::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Particles.zip"), false, false);

	// ;oad basic shader
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

	// create 3d camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	// CGameObject *grid = zone->createEmptyObject();
	// grid->addComponent<CGridPlane>();

	// particles
	m_currentParticleObj = zone->createEmptyObject();
	initParticleSystem(m_currentParticleObj->addComponent<Particle::CParticleComponent>());

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleParticlesVortex::initParticleSystem(Particle::CParticleComponent *ps)
{
	Particle::CFactory *factory = ps->getParticleFactory();
	Particle::CGroup *group = ps->createParticleGroup();

	// create renderer
	Particle::CQuadRenderer *quadRenderer = factory->createQuadRenderer();
	group->setRenderer(quadRenderer);

	ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	quadRenderer->setMaterialType(Particle::Additive, Particle::Camera);
	quadRenderer->setAtlas(1, 1);
	quadRenderer->getMaterial()->setTexture(0, texture);
	quadRenderer->getMaterial()->applyMaterial();
	quadRenderer->SizeX = 0.05f;
	quadRenderer->SizeY = 0.05f;
	quadRenderer->SizeZ = 0.05f;

	// create model
	group->createModel(Particle::ColorR)->setStart(0.0f, 0.3f)->setEnd(0.5f);
	group->createModel(Particle::ColorG)->setStart(0.0f, 0.3f)->setEnd(0.5f);
	group->createModel(Particle::ColorB)->setStart(1.0f)->setEnd(0.1f);

	// scale & lifetime
	group->createModel(Particle::Scale)->setStart(0.1f, 5.0f);
	group->LifeMin = 35.0f;
	group->LifeMax = 40.0f;
	group->Gravity.set(0.0f, 0.0f, 0.0f);
	group->Friction = 1.0f;

	// create emitter	
	Particle::CRandomEmitter *emitter = NULL;

	emitter = factory->createRandomEmitter();
	emitter->setFlow(100);
	emitter->setForce(0.0f, 3.0f);
	emitter->setZone(factory->createLineZone(core::vector3df(-13.0f, 0.0f, -13.0f), core::vector3df(13.0f, 0.0f, 13.0f)));
	group->addEmitter(emitter);

	emitter = factory->createRandomEmitter();
	emitter->setFlow(100);
	emitter->setForce(0.0f, 3.0f);
	emitter->setZone(factory->createLineZone(core::vector3df(-13.0f, 0.0f, 13.0f), core::vector3df(13.0f, 0.0f, -13.0f)));
	group->addEmitter(emitter);

	// extensions vortexSystem
	m_vortexSystem = new Particle::CVortexSystem(core::vector3df(0.0f, -15.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));
	m_vortexSystem->setRotateSpeed(2.0f);
	m_vortexSystem->setAttractionSpeed(0.1f);
	m_vortexSystem->setEyeAttractionSpeed(0.5f);
	m_vortexSystem->setEyeRadius(0.05f);
	m_vortexSystem->enableKillingParticle(true);
	group->addSystem(m_vortexSystem);
}

void SampleParticlesVortex::onUpdate()
{
	// update application
	m_scene->update();

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void SampleParticlesVortex::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// Render 2d gui camera
	CGraphics2D::getInstance()->render(m_camera);

	CImguiManager::getInstance()->onRender();
}

void SampleParticlesVortex::onPostRender()
{
	// post render application
}

bool SampleParticlesVortex::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleParticlesVortex::onResize(int w, int h)
{
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleParticlesVortex::onResume()
{
	// resume application
}

void SampleParticlesVortex::onPause()
{
	// pause application
}

void SampleParticlesVortex::onQuitApp()
{
	// end application
	delete this;
}