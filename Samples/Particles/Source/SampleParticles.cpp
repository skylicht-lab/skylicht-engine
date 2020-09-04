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
	Particle::CZone *zone = group->setZone(factory->createPointZone());
	zone->setPosition(core::vector3df(0.0f, 0.0f, 0.0f));

	// create emitter
	Particle::CEmitter *emitter = group->addEmitter(factory->createRandomEmitter());
	emitter->setTank(0);
	emitter->setFlow(200.0f);

	// create renderer
	Particle::IRenderer *renderer = group->setRenderer(factory->createQuadRenderer());

	ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	renderer->getMaterial()->setUniformTexture("uTexture", texture);
	renderer->getMaterial()->applyMaterial();
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