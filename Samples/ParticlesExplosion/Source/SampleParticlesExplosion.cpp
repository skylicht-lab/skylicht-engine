#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleParticlesExplosion.h"
#include "imgui.h"
#include "CImguiManager.h"

#include "GridPlane/CGridPlane.h"
#include "ParticleSystem/CParticleComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleParticlesExplosion *app = new SampleParticlesExplosion();
	getApplication()->registerAppEvent("SampleParticlesExplosion", app);
}

SampleParticlesExplosion::SampleParticlesExplosion() :
	m_scene(NULL)
{
	CImguiManager::createGetInstance();

	CEventManager::getInstance()->registerEvent("App", this);
}

SampleParticlesExplosion::~SampleParticlesExplosion()
{
	CEventManager::getInstance()->unRegisterEvent(this);

	delete m_scene;
	CImguiManager::releaseInstance();
}

void SampleParticlesExplosion::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// app->setClearColor(video::SColor(255, 100, 100, 100));

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Particles.zip"), false, false);

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

	// create 3d camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// particles pool	
	for (int i = 0; i < MAX_POOL; i++)
	{
		m_particlePool[i] = zone->createEmptyObject()->addComponent<Particle::CParticleComponent>();
		initParticleSystem(m_particlePool[i]);
		m_particlePool[i]->getGameObject()->setVisible(false);
	}

	// initFireSystem(zone->createEmptyObject()->addComponent<Particle::CParticleComponent>());

	// init font
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 30);

	// create 2D Canvas
	CGameObject *canvasObject = zone->createEmptyObject();
	CCanvas *canvas = canvasObject->addComponent<CCanvas>();

	// create UI Text in Canvas
	CGUIText *textLarge = canvas->createText(m_font);
	textLarge->setPosition(core::vector3df(0.0f, -10.0f, 0.0f));
	textLarge->setText("Press SPACE to simulate explosion");
	textLarge->setTextAlign(CGUIElement::Center, CGUIElement::Bottom);

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

bool SampleParticlesExplosion::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		if (event.KeyInput.Key == irr::KEY_SPACE && event.KeyInput.PressedDown == false)
		{
			for (int i = 0; i < MAX_POOL; i++)
			{
				if (m_particlePool[i]->IsPlaying() == false)
				{
					char debugLog[512];
					sprintf(debugLog, "Play particle %d", i);
					os::Printer::log(debugLog);

					float radius = 2.0f;
					float r1 = Particle::random(-1.0f, 1.0f);
					float r2 = Particle::random(0.0f, 1.0f);
					core::vector3df randomPosition(r1 * radius, r2 * radius, r1 * radius);

					m_particlePool[i]->getGameObject()->setVisible(true);
					m_particlePool[i]->getGameObject()->getTransformEuler()->setPosition(randomPosition);
					m_particlePool[i]->Play();

					return true;
				}
			}
		}
	}

	return false;
}

void SampleParticlesExplosion::initFireSystem(Particle::CParticleComponent *ps)
{
	ITexture *texture = NULL;
	Particle::CFactory *factory = ps->getParticleFactory();

	// GROUP: FIRE
	Particle::CGroup *fireGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *fire = factory->createQuadRenderer();

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/fire2.png");
	fire->setMaterialType(Particle::Addtive, Particle::Camera);
	fire->setAtlas(2, 2);
	fire->SizeX = 0.3f;
	fire->SizeY = 0.3f;
	fire->getMaterial()->setTexture(0, texture);
	fire->getMaterial()->applyMaterial();

	fireGroup->setRenderer(fire);
	fireGroup->createModel(Particle::ColorR)->setStart(0.8f, 0.9f)->setEnd(0.8f, 0.9f);
	fireGroup->createModel(Particle::ColorG)->setStart(0.5f, 0.6f)->setEnd(0.5f, 0.6f);
	fireGroup->createModel(Particle::ColorB)->setStart(0.3f);
	fireGroup->createModel(Particle::ColorA)->setStart(0.4)->setEnd(0.0f);
	fireGroup->createModel(Particle::RotateZ)->setStart(0.0f, 2.0f * core::PI);
	fireGroup->createModel(Particle::RotateSpeedZ)->setStart(1.1f, 2.2f);
	fireGroup->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);
	fireGroup->LifeMin = 1.0f;
	fireGroup->LifeMax = 1.5f;
	fireGroup->Gravity.set(0.0f, 3.0f, 0.0f);

	Particle::CInterpolator *fireSizeInterpolate = fireGroup->createInterpolator();
	fireSizeInterpolate->addEntry(0.0f, 0.0f);
	fireSizeInterpolate->addEntry(0.5f, 3.0f);
	fireSizeInterpolate->addEntry(1.0f, 0.0f);
	fireGroup->createModel(Particle::Scale)->setInterpolator(fireSizeInterpolate);

	// Emitters
	// The emitters are arranged so that the fire looks realistic
	Particle::CStraightEmitter* fireEmitter1 = factory->createStraightEmitter(core::vector3df(0.0f, 1.0f, 0.0f));
	fireEmitter1->setZone(factory->createSphereZone(core::vector3df(0.0f, -1.0f, 0.0f), 0.5f));
	fireEmitter1->setFlow(40);
	fireEmitter1->setForce(1.0f, 2.5f);

	Particle::CStraightEmitter* fireEmitter2 = factory->createStraightEmitter(core::vector3df(1.0f, 0.6f, 0.0f));
	fireEmitter2->setZone(factory->createSphereZone(core::vector3df(0.15f, -1.2f, 0.075f), 0.1f));
	fireEmitter2->setFlow(15);
	fireEmitter2->setForce(0.5f, 1.5f);

	Particle::CStraightEmitter* fireEmitter3 = factory->createStraightEmitter(core::vector3df(-0.6f, 0.8f, -0.8f));
	fireEmitter3->setZone(factory->createSphereZone(core::vector3df(-0.375f, -1.15f, -0.375f), 0.3f));
	fireEmitter3->setFlow(15);
	fireEmitter3->setForce(0.5f, 1.5f);

	Particle::CStraightEmitter* fireEmitter4 = factory->createStraightEmitter(core::vector3df(-0.8f, 0.5f, 0.2f));
	fireEmitter4->setZone(factory->createSphereZone(core::vector3df(-0.255f, -1.2f, 0.225f), 0.2f));
	fireEmitter4->setFlow(10);
	fireEmitter4->setForce(0.5f, 1.5f);

	Particle::CStraightEmitter* fireEmitter5 = factory->createStraightEmitter(core::vector3df(0.1f, 0.8f, -1.0f));
	fireEmitter5->setZone(factory->createSphereZone(core::vector3df(-0.075f, -1.2f, -0.3f), 0.2f));
	fireEmitter5->setFlow(10);
	fireEmitter5->setForce(0.5f, 1.5f);

	fireGroup->addEmitter(fireEmitter1);
	fireGroup->addEmitter(fireEmitter2);
	fireGroup->addEmitter(fireEmitter3);
	fireGroup->addEmitter(fireEmitter4);
	fireGroup->addEmitter(fireEmitter5);
}

void SampleParticlesExplosion::initParticleSystem(Particle::CParticleComponent *ps)
{
	ITexture *texture = NULL;

	// FACTORY & EXPLOSION ZONE
	Particle::CFactory *factory = ps->getParticleFactory();
	Particle::CSphere* sphere = factory->createSphereZone(core::vector3df(), 0.6f);

	// GROUP: SMOKE
	Particle::CGroup *smokeGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *smoke = factory->createQuadRenderer();
	smokeGroup->setRenderer(smoke);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/explosion_alpha.png");
	smoke->setMaterialType(Particle::Transparent, Particle::Camera);
	smoke->setAtlas(2, 2);
	smoke->getMaterial()->setTexture(0, texture);
	smoke->getMaterial()->applyMaterial();

	// create model
	smokeGroup->createModel(Particle::ColorR)->setStart(0.2f);
	smokeGroup->createModel(Particle::ColorG)->setStart(0.2f);
	smokeGroup->createModel(Particle::ColorB)->setStart(0.2f);
	smokeGroup->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);

	// scale & lifetime
	smokeGroup->createModel(Particle::Scale)->setStart(0.6f, 0.8f)->setEnd(1.0f, 1.4f);
	smokeGroup->LifeMin = 2.5f;
	smokeGroup->LifeMax = 3.0f;
	smokeGroup->Gravity.set(0.0f, 0.05f, 0.0f);

	// alpha interpolator
	Particle::CInterpolator *interpolatorSmokeAlpha = smokeGroup->createInterpolator();
	interpolatorSmokeAlpha->addEntry(0.0f, 0.0f);
	interpolatorSmokeAlpha->addEntry(0.4f, 0.6f);
	interpolatorSmokeAlpha->addEntry(0.6f, 0.6f);
	interpolatorSmokeAlpha->addEntry(1.0f, 0.0f);
	smokeGroup->createModel(Particle::ColorA)->setInterpolator(interpolatorSmokeAlpha);

	// create emitter
	Particle::CRandomEmitter *smokeEmitter = NULL;

	smokeEmitter = factory->createRandomEmitter();
	smokeEmitter->setFlow(-1.0f);
	smokeEmitter->setTank(15);
	smokeEmitter->setForce(0.04f, 0.1f);
	smokeEmitter->setZone(sphere);
	smokeGroup->addEmitter(smokeEmitter);

	// GROUP: FLASH
	Particle::CGroup *flashGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *flash = factory->createQuadRenderer();
	flashGroup->setRenderer(flash);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/flash.png");
	flash->setMaterialType(Particle::Addtive, Particle::Camera);
	flash->getMaterial()->setTexture(0, texture);
	flash->getMaterial()->applyMaterial();

	flashGroup->createModel(Particle::RotateZ)->setStart(0.0f, 2.0f * core::PI);
	flashGroup->LifeMin = 0.5f;
	flashGroup->LifeMax = 0.5f;

	Particle::CInterpolator *flashSizeInterpolator = flashGroup->createInterpolator();
	flashSizeInterpolator->addEntry(0.0f, 0.25f);
	flashSizeInterpolator->addEntry(0.1f, 1.5f);
	flashGroup->createModel(Particle::Scale)->setInterpolator(flashSizeInterpolator);

	Particle::CInterpolator *flashAlphaInterpolator = flashGroup->createInterpolator();
	flashAlphaInterpolator->addEntry(0.0f, 1.0f);
	flashAlphaInterpolator->addEntry(0.4f, 0.0f);
	flashGroup->createModel(Particle::ColorA)->setInterpolator(flashAlphaInterpolator);

	Particle::CRandomEmitter* randomEmitter = factory->createRandomEmitter();
	randomEmitter->setZone(factory->createSphereZone(core::vector3df(), 0.1f));
	randomEmitter->setFlow(-1);
	randomEmitter->setTank(3);
	flashGroup->addEmitter(randomEmitter);

	// GROUP: FLAME
	Particle::CGroup *flameGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *flame = factory->createQuadRenderer();
	flameGroup->setRenderer(flame);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/explosion.png");
	flame->setMaterialType(Particle::Addtive, Particle::Camera);
	flame->setAtlas(2, 2);
	flame->getMaterial()->setTexture(0, texture);
	flame->getMaterial()->applyMaterial();

	flameGroup->createModel(Particle::ColorR)->setStart(1.0f)->setEnd(0.2f);
	flameGroup->createModel(Particle::ColorG)->setStart(0.5f)->setEnd(0.2f);
	flameGroup->createModel(Particle::ColorB)->setStart(0.2f)->setEnd(0.2f);
	flameGroup->createModel(Particle::RotateZ)->setStart(0.0f, core::PI * 0.5f);
	flameGroup->createModel(Particle::FrameIndex)->setStart(0, 3);
	flameGroup->LifeMin = 1.5f;
	flameGroup->LifeMax = 2.0f;

	Particle::CInterpolator *flameSizeInterpolator = flameGroup->createInterpolator();
	flameSizeInterpolator->addEntry(0.0f, 0.25f);
	flameSizeInterpolator->addEntry(0.02f, 0.8f);
	flameSizeInterpolator->addEntry(1.0f, 1.4f);
	flameGroup->createModel(Particle::Scale)->setInterpolator(flameSizeInterpolator);

	Particle::CInterpolator *flameAlphaInterpolator = flameGroup->createInterpolator();
	flameAlphaInterpolator->addEntry(0.5f, 1.0f);
	flameAlphaInterpolator->addEntry(1.0f, 0.0f);
	flameGroup->createModel(Particle::ColorA)->setInterpolator(flameAlphaInterpolator);

	Particle::CNormalEmitter *flameEmitter = factory->createNormalEmitter(false);
	flameEmitter->setFlow(-1.0f);
	flameEmitter->setTank(15);
	flameEmitter->setForce(0.06f, 0.1f);
	flameEmitter->setZone(sphere);
	flameGroup->addEmitter(flameEmitter);

	// GROUP: LINE SPARK
	Particle::CGroup *lineSparkGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *lineSpark = factory->createQuadRenderer();
	lineSparkGroup->setRenderer(lineSpark);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/spark1.png");
	lineSpark->setMaterialType(Particle::Addtive, Particle::Velocity);
	lineSpark->getMaterial()->setTexture(0, texture);
	lineSpark->getMaterial()->applyMaterial();
	lineSpark->SizeX = 0.05f;
	lineSpark->SizeY = 1.0f;

	lineSparkGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	lineSparkGroup->createModel(Particle::Scale)->setStart(0.2f)->setEnd(0.4f);
	lineSparkGroup->LifeMin = 0.2f;
	lineSparkGroup->LifeMax = 1.0f;
	lineSparkGroup->Gravity.set(0.0f, -1.5f, 0.0f);

	Particle::CNormalEmitter *lineSparkEmitter = factory->createNormalEmitter(false);
	lineSparkEmitter->setFlow(-1.0f);
	lineSparkEmitter->setTank(20);
	lineSparkEmitter->setForce(2.0f, 3.0f);
	lineSparkEmitter->setZone(sphere);
	lineSparkGroup->addEmitter(lineSparkEmitter);

	// GROUP: POINT SPARK
	Particle::CGroup *pointSparkGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *pointSpark = factory->createQuadRenderer();
	pointSparkGroup->setRenderer(pointSpark);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	pointSpark->setMaterialType(Particle::Addtive, Particle::Camera);
	pointSpark->getMaterial()->setTexture(0, texture);
	pointSpark->getMaterial()->applyMaterial();
	pointSpark->SizeX = 0.02f;
	pointSpark->SizeY = 0.02f;

	pointSparkGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	pointSparkGroup->createModel(Particle::ColorR)->setStart(1.0f);
	pointSparkGroup->createModel(Particle::ColorG)->setStart(1.0f)->setEnd(0.3f, 0.1f);
	pointSparkGroup->createModel(Particle::ColorB)->setStart(0.7f)->setEnd(0.3f);
	pointSparkGroup->LifeMin = 1.0f;
	pointSparkGroup->LifeMax = 3.0f;
	pointSparkGroup->Gravity.set(0.0f, -0.3f, 0.0f);
	pointSparkGroup->Friction = 0.4f;

	Particle::CNormalEmitter *pointSparkEmitter = factory->createNormalEmitter(false);
	pointSparkEmitter->setFlow(-1.0f);
	pointSparkEmitter->setTank(400);
	pointSparkEmitter->setForce(0.4f, 0.8f);
	pointSparkEmitter->setZone(sphere);
	pointSparkGroup->addEmitter(pointSparkEmitter);

	// GROUP: WAVE
	Particle::CGroup *waveGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *wave = factory->createQuadRenderer();
	waveGroup->setRenderer(wave);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/wave_alpha.png");
	wave->setMaterialType(Particle::Transparent, Particle::FixOrientation);
	wave->getMaterial()->setTexture(0, texture);
	wave->getMaterial()->applyMaterial();

	waveGroup->createModel(Particle::ColorA)->setStart(0.2f)->setEnd(0.0f);
	waveGroup->createModel(Particle::Scale)->setStart(0.0f)->setEnd(4.0f);
	waveGroup->LifeMin = 0.8f;
	waveGroup->Gravity.set(0.0f, -0.3f, 0.0f);
	waveGroup->OrientationNormal.set(0.0f, 1.0f, 0.0f);
	waveGroup->OrientationUp.set(1.0f, 0.0f, 0.0f);

	Particle::CRandomEmitter *waveEmitter = factory->createRandomEmitter();
	waveEmitter->setFlow(-1.0f);
	waveEmitter->setTank(1);
	waveEmitter->setForce(0.0f, 0.0f);
	waveEmitter->setZone(factory->createPointZone());
	waveGroup->addEmitter(waveEmitter);
}

void SampleParticlesExplosion::onUpdate()
{
	// update application
	m_scene->update();

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void SampleParticlesExplosion::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// Render 2d gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);

	CImguiManager::getInstance()->onRender();
}

void SampleParticlesExplosion::onPostRender()
{
	// post render application
}

bool SampleParticlesExplosion::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleParticlesExplosion::onResume()
{
	// resume application
}

void SampleParticlesExplosion::onPause()
{
	// pause application
}

void SampleParticlesExplosion::onQuitApp()
{
	// end application
	delete this;
}