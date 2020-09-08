#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleParticles.h"

#include "GridPlane/CGridPlane.h"
#include "ParticleSystem/CParticleComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleParticles *app = new SampleParticles();
	getApplication()->registerAppEvent("SampleParticles", app);
}

SampleParticles::SampleParticles() :
	m_scene(NULL)
{

}

SampleParticles::~SampleParticles()
{
	delete m_scene;
}

void SampleParticles::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Particles.zip"), false, false);

	// Load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// Create 2D camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// Create 3d camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3D grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// Particles
	initFireParticle(zone->createEmptyObject()->addComponent<Particle::CParticleComponent>());

	// Rendering
	m_forwardRP = new CForwardRP();
}

void SampleParticles::initFireParticle(Particle::CParticleComponent *particleComponent)
{
	Particle::CFactory *factory = particleComponent->getParticleFactory();
	Particle::CGroup *group = particleComponent->createParticleGroup();

	// create start point
	// Particle::CZone *zone = group->setZone(factory->createPointZone());
	// Particle::CZone *zone = group->setZone(factory->createSphereZone(core::vector3df(0.0f, 0.0f, 0.0f), 2.0f));
	// Particle::CZone *zone = group->setZone(factory->createCylinderZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), 2.0f, 4.0f));
	// Particle::CZone *zone = group->setZone(factory->createLineZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(2.0f, 2.0f, 2.0f)));
	Particle::CZone *zone = group->setZone(factory->createAABoxZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(2.0f, 2.0f, 2.0f)));
	// Particle::CZone *zone = group->setZone(factory->createRingZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), 1.0, 2.0f));

	// create emitter
	Particle::CEmitter *emitter = group->addEmitter(factory->createRandomEmitter());
	// Particle::CEmitter *emitter = group->addEmitter(factory->createStraightEmitter(core::vector3df(0.0f, 0.0f, 1.0f)));	
	// Particle::CEmitter *emitter = group->addEmitter(factory->createSphericEmitter(core::vector3df(0.0f, 1.0f, 0.0f), core::PI * 0.0f, core::PI * 0.2f));
	emitter->setTank(0);
	emitter->setFlow(2000.0f);
	emitter->setForce(0.0f, 0.5f);
	emitter->setEmitFullZone(false);

	// create renderer
	Particle::CQuadRenderer *quadRenderer = factory->createQuadRenderer();
	group->setRenderer(quadRenderer);

	//ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/explosion.png");
	//quadRenderer->setAtlas(2, 2);
	ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	quadRenderer->setAtlas(1, 1);
	quadRenderer->getMaterial()->setUniformTexture("uTexture", texture);
	quadRenderer->getMaterial()->applyMaterial();

	// create model
	group->Gravity.set(0.0f, 0.1f, 0.0f);

	// group->createModel(Particle::RotateSpeedZ)->setStart(-2.0f, 2.0f);
	// group->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);

	group->createModel(Particle::ScaleX)->setStart(0.1f)->setEnd(0.1f, 0.1f);
	group->createModel(Particle::ScaleY)->setStart(0.1f)->setEnd(0.1f, 0.1f);
	group->createModel(Particle::ScaleZ)->setStart(0.1f)->setEnd(0.1f, 0.1f);

	group->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	group->createModel(Particle::ColorR)->setStart(0.8f)->setEnd(1.0f);
	group->createModel(Particle::ColorG)->setStart(0.4f)->setEnd(1.0f);
	group->createModel(Particle::ColorB)->setStart(0.8f)->setEnd(1.0f);

	CTransformEuler *t = particleComponent->getGameObject()->getTransformEuler();
	t->setPosition(core::vector3df(1.0f, 1.0f, 1.0f));
	// t->setRotation(core::vector3df(0.0f, 0.0f, 45.0f));
}

void SampleParticles::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleParticles::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// Render 2d gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleParticles::onPostRender()
{
	// post render application
}

bool SampleParticles::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleParticles::onResume()
{
	// resume application
}

void SampleParticles::onPause()
{
	// pause application
}

void SampleParticles::onQuitApp()
{
	// end application
	delete this;
}