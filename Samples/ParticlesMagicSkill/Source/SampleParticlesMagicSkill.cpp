#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleParticlesMagicSkill.h"
#include "imgui.h"
#include "CImguiManager.h"

#include "GridPlane/CGridPlane.h"
#include "ParticleSystem/CParticleComponent.h"
#include "ParticleSystem/CParticleTrailComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleParticlesMagicSkill *app = new SampleParticlesMagicSkill();
	getApplication()->registerAppEvent("SampleParticlesMagicSkill", app);
}

SampleParticlesMagicSkill::SampleParticlesMagicSkill() :
	m_scene(NULL),
	m_vortexSystem(NULL)
{
	CImguiManager::createGetInstance();

	CEventManager::getInstance()->registerEvent("App", this);
}

SampleParticlesMagicSkill::~SampleParticlesMagicSkill()
{
	CEventManager::getInstance()->unRegisterEvent(this);

	delete m_vortexSystem;
	delete m_scene;

	CImguiManager::releaseInstance();
}

void SampleParticlesMagicSkill::onInitApp()
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

	// tower
	Particle::CParticleComponent *tower = zone->createEmptyObject()->addComponent<Particle::CParticleComponent>();
	initTower(tower);

	// projectiles
	m_projectiles = zone->createEmptyObject()->addComponent<Particle::CParticleComponent>();
	initProjectiles(m_projectiles);
	m_projectiles->getGameObject()->setVisible(false);

	// impact
	m_impacts = zone->createEmptyObject()->addComponent<Particle::CParticleComponent>();
	initImpact(m_impacts);
	m_impacts->getGameObject()->setVisible(false);

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
	m_forwardRP = new CForwardRP();
}

bool SampleParticlesMagicSkill::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		if (event.KeyInput.Key == irr::KEY_SPACE && event.KeyInput.PressedDown == false)
		{
			core::vector3df position(0.0f, 1.0f, 0.0f);

			// demo project tiles
			m_projectiles->getGameObject()->setVisible(true);
			m_projectiles->getGameObject()->getTransformEuler()->setPosition(position);
			m_projectiles->Play();

			// demo impact
			m_impacts->getGameObject()->setVisible(true);
			m_impacts->getGameObject()->getTransformEuler()->setPosition(core::vector3df(3.0f, 0.0f, 0.0f));
			m_impacts->Play();

			return true;
		}
	}

	return false;
}

void SampleParticlesMagicSkill::initTower(Particle::CParticleComponent *ps)
{
	ITexture *texture = NULL;

	// FACTORY & ZONE
	Particle::CFactory *factory = ps->getParticleFactory();

	// GROUP: RING	
	Particle::CGroup *ringGroup = ps->createParticleGroup();

	ringGroup->LifeMin = 3.0f;
	ringGroup->LifeMax = 3.0f;
	ringGroup->Gravity.set(0.0f, 0.0f, 0.0f);
	ringGroup->createModel(Particle::ColorR)->setStart(1.0f);
	ringGroup->createModel(Particle::ColorG)->setStart(0.3f);
	ringGroup->createModel(Particle::ColorB)->setStart(1.0f);
	ringGroup->createModel(Particle::Scale)->setStart(0.0f)->setEnd(4.0f);
	ringGroup->createModel(Particle::RotateZ)->setStart(0.0f, 2.0f * core::PI);

	Particle::CInterpolator *alphaInterpolator = ringGroup->createInterpolator();
	alphaInterpolator->addEntry(0.0f, 0.0f);
	alphaInterpolator->addEntry(0.4f, 0.6f);
	alphaInterpolator->addEntry(0.6f, 1.0f);
	alphaInterpolator->addEntry(1.0f, 0.0f);
	ringGroup->createModel(Particle::ColorA)->setStart(0.0f)->setEnd(1.0f)->setInterpolator(alphaInterpolator);

	ringGroup->OrientationNormal.set(0.0f, 1.0f, 0.0f);
	ringGroup->OrientationUp.set(0.0f, 0.0f, 1.0f);

	Particle::CQuadRenderer *ring = factory->createQuadRenderer();
	ring->SizeX = 2.0f;
	ring->SizeY = 2.0f;
	ring->SizeZ = 2.0f;
	ringGroup->setRenderer(ring);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/modular_aura06.png");
	ring->setMaterialType(Particle::Addtive, Particle::FixOrientation);
	ring->getMaterial()->setTexture(0, texture);
	ring->getMaterial()->applyMaterial();

	Particle::CEmitter *ringEmitter = factory->createRandomEmitter();
	ringEmitter->setFlow(1.0f);
	ringEmitter->setTank(-1);
	ringEmitter->setForce(0.0f, 0.0f);
	ringEmitter->setZone(factory->createPointZone());
	ringGroup->addEmitter(ringEmitter);

	// GROUP: POINT
	Particle::CGroup *pointGroup = ps->createParticleGroup();

	pointGroup->LifeMin = 0.5f;
	pointGroup->LifeMax = 0.7f;
	pointGroup->Gravity.set(0.0f, 0.0f, 0.0f);
	pointGroup->createModel(Particle::Scale)->setStart(0.8f)->setEnd(1.5f);
	pointGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);

	Particle::CQuadRenderer *point = factory->createQuadRenderer();
	point->SizeX = 2.0f;
	point->SizeY = 2.0f;
	point->SizeZ = 2.0f;

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_glow.png");
	point->setMaterialType(Particle::Addtive, Particle::Camera);
	point->getMaterial()->setTexture(0, texture);
	point->getMaterial()->applyMaterial();
	pointGroup->setRenderer(point);

	Particle::CEmitter *pointEmitter = factory->createRandomEmitter();
	pointEmitter->setFlow(7.0f);
	pointEmitter->setTank(-1);
	pointEmitter->setForce(0.0f, 0.0f);
	pointEmitter->setZone(factory->createPointZone(core::vector3df(0.0f, 1.0f, 0.0f)));
	pointGroup->addEmitter(pointEmitter);

	// GROUP: SPHERE
	Particle::CGroup *sphereGroup = ps->createParticleGroup();

	Particle::CQuadRenderer *sphere = factory->createQuadRenderer();
	sphere->SizeX = 0.1f;
	sphere->SizeY = 0.1f;
	sphere->SizeZ = 0.1f;
	sphereGroup->setRenderer(sphere);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_sphere.png");
	sphere->setMaterialType(Particle::Addtive, Particle::Camera);
	sphere->getMaterial()->setTexture(0, texture);
	sphere->getMaterial()->applyMaterial();

	sphereGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	sphereGroup->createModel(Particle::Scale)->setStart(0.8f, 1.0f)->setEnd(0.0f);
	sphereGroup->LifeMin = 2.0f;
	sphereGroup->LifeMax = 3.5f;
	sphereGroup->Friction = 1.0f;

	Particle::CEmitter *sphereEmitter = factory->createRandomEmitter();
	sphereEmitter->setFlow(20.0f);
	sphereEmitter->setTank(-1);
	sphereEmitter->setForce(0.0f, 0.6f);
	sphereEmitter->setZone(factory->createSphereZone(core::vector3df(0.0f, 1.0f, 0.0f), 1.0f));
	sphereGroup->addEmitter(sphereEmitter);

	m_vortexSystem = new Particle::CVortexSystem(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), 0.5f, 0.2f);
	m_vortexSystem->setEyeAttractionSpeed(0.01f);
	m_vortexSystem->enableKillingParticle(true);
	sphereGroup->addSystem(m_vortexSystem);
}

void SampleParticlesMagicSkill::initProjectiles(Particle::CParticleComponent *ps)
{
	ITexture *texture = NULL;

	// FACTORY & ZONE
	Particle::CFactory *factory = ps->getParticleFactory();

	Particle::CCylinder* cylinder = factory->createCylinderZone(core::vector3df(), core::vector3df(0.0f, 1.0f, 0.0f), 0.6, 0.1f);
	Particle::CPoint* point = factory->createPointZone();
	Particle::CCylinder* trailCylinder = factory->createCylinderZone(core::vector3df(0.0f, -0.3f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), 0.2, 0.4f);

	// GROUP: SPARK
	Particle::CGroup *sparkGroup = ps->createParticleGroup();

	sparkGroup->LifeMin = 4.0f;
	sparkGroup->LifeMax = 8.0f;
	sparkGroup->Friction = 0.4f;
	sparkGroup->Gravity.set(0.0f, -0.1f, 0.0f);
	sparkGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);

	Particle::CEmitter *sparkEmitter = factory->createNormalEmitter(false);
	sparkEmitter->setFlow(100.0f);
	sparkEmitter->setTank(20);
	sparkEmitter->setForce(2.5f, 6.0f);
	sparkEmitter->setZone(cylinder);
	sparkGroup->addEmitter(sparkEmitter);

	// ADD TRAIL
	Particle::CParticleTrailComponent *psTrail = ps->getGameObject()->addComponent<Particle::CParticleTrailComponent>();
	Particle::CParticleTrail *trail = psTrail->addTrail(sparkGroup);

	CMaterial *material = trail->getMaterial();
	material->setTexture(0, CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_trail.png"));
	trail->applyMaterial();
	trail->setWidth(0.3f);
	trail->setLength(2.0f);


	// SUB GROUP: Arcane
	Particle::CSubGroup *arcaneGroup = ps->createParticleSubGroup(sparkGroup);

	Particle::CQuadRenderer *arcane = factory->createQuadRenderer();
	arcane->SizeX = 0.4f;
	arcane->SizeY = 0.4f;
	arcane->SizeZ = 0.4f;
	arcaneGroup->setRenderer(arcane);
	arcaneGroup->setFollowParentTransform(true);
	arcaneGroup->syncParentParams(true, true);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_twirl.png");
	arcane->setMaterialType(Particle::Addtive, Particle::Camera);
	arcane->getMaterial()->setTexture(0, texture);
	arcane->getMaterial()->applyMaterial();

	arcaneGroup->createModel(Particle::RotateSpeedZ)->setStart(3.0f, 5.0f);
	arcaneGroup->createModel(Particle::RotateZ)->setStart(0.0f, 2.0f * core::PI);
	arcaneGroup->createModel(Particle::Scale)->setStart(1.0f, 1.2f)->setEnd(0.6f);
	arcaneGroup->LifeMin = 4.0f;
	arcaneGroup->LifeMax = 8.0f;

	Particle::CEmitter *arcaneEmitter = factory->createNormalEmitter(false);
	arcaneEmitter->setFlow(-1.0f);
	arcaneEmitter->setTank(4);
	arcaneEmitter->setForce(0.0f, 0.0f);
	arcaneEmitter->setZone(point);
	arcaneGroup->addEmitter(arcaneEmitter);

	// SUB GROUP: Sphere
	Particle::CSubGroup *sphereGroup = ps->createParticleSubGroup(sparkGroup);

	Particle::CQuadRenderer *sphere = factory->createQuadRenderer();
	sphere->SizeX = 0.1f;
	sphere->SizeY = 0.1f;
	sphere->SizeZ = 0.1f;
	sphereGroup->setRenderer(sphere);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_sphere.png");
	sphere->setMaterialType(Particle::Addtive, Particle::Camera);
	sphere->getMaterial()->setTexture(0, texture);
	sphere->getMaterial()->applyMaterial();

	sphereGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	sphereGroup->createModel(Particle::RotateZ)->setStart(0.0f, core::PI);
	sphereGroup->createModel(Particle::Scale)->setStart(0.8f, 1.0f)->setEnd(0.0f);
	sphereGroup->LifeMin = 0.5f;
	sphereGroup->LifeMax = 1.5f;
	sphereGroup->Friction = 1.0f;

	Particle::CEmitter *sphereEmitter = factory->createSphericEmitter(-CTransform::s_oy, 0.0f, 60.0f * core::DEGTORAD);
	sphereEmitter->setFlow(10.0f);
	sphereEmitter->setTank(-1);
	sphereEmitter->setForce(0.0f, 0.6f);
	sphereEmitter->setZone(trailCylinder);
	sphereGroup->addEmitter(sphereEmitter);
}

void SampleParticlesMagicSkill::initImpact(Particle::CParticleComponent *ps)
{
	ITexture *texture = NULL;

	// FACTORY & ZONE
	Particle::CFactory *factory = ps->getParticleFactory();

	Particle::CSphere* sphereZone = factory->createSphereZone(core::vector3df(0.0f, 0.0f, 0.0f), 0.1f);
	Particle::CSphere* largeSphereZone = factory->createSphereZone(core::vector3df(0.0f, 0.2f, 0.0f), 0.3f);

	// GROUP: SPARK
	Particle::CGroup *sparkGroup = ps->createParticleGroup();

	sparkGroup->LifeMin = 0.5f;
	sparkGroup->LifeMax = 1.0f;
	sparkGroup->Friction = 0.0f;
	sparkGroup->Gravity.set(0.0f, 0.0f, 0.0f);

	Particle::CEmitter *sparkEmitter = factory->createRandomEmitter();
	sparkEmitter->setFlow(100.0f);
	sparkEmitter->setTank(1);
	sparkEmitter->setForce(0.0f, 0.0f);
	sparkEmitter->setZone(factory->createPointZone());
	sparkGroup->addEmitter(sparkEmitter);

	// SUBGROUP: GLOW
	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_glow.png");

	Particle::CSubGroup *glowGroup = ps->createParticleSubGroup(sparkGroup);

	glowGroup->LifeMin = 0.5f;
	glowGroup->LifeMax = 0.6f;
	glowGroup->Gravity.set(0.0f, 0.0f, 0.0f);
	glowGroup->createModel(Particle::Scale)->setStart(1.0f)->setEnd(0.0f);
	glowGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);

	Particle::CQuadRenderer *glow = factory->createQuadRenderer();
	glow->SizeX = 2.0f;
	glow->SizeY = 2.0f;
	glow->SizeZ = 2.0f;

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_glow.png");
	glow->setMaterialType(Particle::Addtive, Particle::Camera);
	glow->getMaterial()->setTexture(0, texture);
	glow->getMaterial()->applyMaterial();
	glowGroup->setRenderer(glow);

	Particle::CEmitter *pointEmitter = factory->createRandomEmitter();
	pointEmitter->setFlow(-1.0f);
	pointEmitter->setTank(4);
	pointEmitter->setForce(0.0f, 0.0f);
	pointEmitter->setZone(sphereZone);
	glowGroup->addEmitter(pointEmitter);

	// GROUP: LINE SPARK
	Particle::CSubGroup *lineSparkGroup = ps->createParticleSubGroup(sparkGroup);

	Particle::CQuadRenderer *lineSpark = factory->createQuadRenderer();
	lineSparkGroup->setRenderer(lineSpark);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/vertical_glow3.png");
	lineSpark->setMaterialType(Particle::Addtive, Particle::Velocity);
	lineSpark->getMaterial()->setTexture(0, texture);
	lineSpark->getMaterial()->applyMaterial();
	lineSpark->SizeX = 0.5f;
	lineSpark->SizeY = 2.0f;

	lineSparkGroup->createModel(Particle::ColorR)->setStart(1.0f);
	lineSparkGroup->createModel(Particle::ColorG)->setStart(0.6f);
	lineSparkGroup->createModel(Particle::ColorB)->setStart(1.0f);
	lineSparkGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	lineSparkGroup->createModel(Particle::Scale)->setStart(0.2f)->setEnd(0.4f);
	lineSparkGroup->LifeMin = 0.4f;
	lineSparkGroup->LifeMax = 0.5f;
	lineSparkGroup->Gravity.set(0.0f, -1.5f, 0.0f);

	Particle::CEmitter *lineSparkEmitter = factory->createSphericEmitter(core::vector3df(0.0f, 1.0f, 0.0f), 0.0f, 0.7f * core::PI);
	lineSparkEmitter->setFlow(-1.0f);
	lineSparkEmitter->setTank(7);
	lineSparkEmitter->setForce(2.0f, 4.0f);
	lineSparkEmitter->setZone(sphereZone);
	lineSparkGroup->addEmitter(lineSparkEmitter);

	// GROUP: EXPLOSION SPARK
	Particle::CSubGroup *sphereGroup = ps->createParticleSubGroup(sparkGroup);

	Particle::CQuadRenderer *sphere = factory->createQuadRenderer();
	sphere->SizeX = 0.5f;
	sphere->SizeY = 0.5f;
	sphere->SizeZ = 0.5f;
	sphereGroup->setRenderer(sphere);
	sphereGroup->Gravity.set(0.0f, 0.0f, 0.0f);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/Arcane/arcane_sphere.png");
	sphere->setMaterialType(Particle::Addtive, Particle::Camera);
	sphere->getMaterial()->setTexture(0, texture);
	sphere->getMaterial()->applyMaterial();

	sphereGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	sphereGroup->createModel(Particle::Scale)->setStart(0.8f, 1.0f)->setEnd(0.0f);
	sphereGroup->LifeMin = 0.1f;
	sphereGroup->LifeMax = 0.2f;
	sphereGroup->Friction = 0.1f;

	Particle::CEmitter *sphereEmitter = factory->createSphericEmitter(CTransform::s_oy, 0.0f, core::PI);
	sphereEmitter->setFlow(-1);
	sphereEmitter->setTank(8);
	sphereEmitter->setForce(4.0f, 7.0f);
	sphereEmitter->setZone(sphereZone);
	sphereGroup->addEmitter(sphereEmitter);

	// GROUP: LINGER
	Particle::CSubGroup *lingerGroup = ps->createParticleSubGroup(sphereGroup);

	Particle::CQuadRenderer *linger = factory->createQuadRenderer();
	linger->SizeX = 0.3f;
	linger->SizeY = 0.3f;
	linger->SizeZ = 0.3f;
	lingerGroup->setRenderer(linger);
	lingerGroup->Gravity.set(0.0f, 0.0f, 0.0f);

	linger->setMaterialType(Particle::Addtive, Particle::Camera);
	linger->getMaterial()->setTexture(0, texture);
	linger->getMaterial()->applyMaterial();

	lingerGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	lingerGroup->createModel(Particle::Scale)->setStart(0.8f, 1.0f)->setEnd(0.0f);
	lingerGroup->LifeMin = 0.4f;
	lingerGroup->LifeMax = 0.5f;

	Particle::CEmitter *lingerEmitter = factory->createRandomEmitter();
	lingerEmitter->setFlow(30.0f);
	lingerEmitter->setTank(-1);
	lingerEmitter->setForce(0.0f, 0.0f);
	lingerEmitter->setZone(factory->createPointZone());
	lingerGroup->addEmitter(lingerEmitter);

	// GROUP: POINT
	Particle::CSubGroup *pointGroup = ps->createParticleSubGroup(sparkGroup);

	Particle::CQuadRenderer *point = factory->createQuadRenderer();
	point->SizeX = 0.2f;
	point->SizeY = 0.2f;
	point->SizeZ = 0.2f;
	pointGroup->setRenderer(point);
	pointGroup->Gravity.set(0.0f, -0.2f, 0.0f);

	point->setMaterialType(Particle::Addtive, Particle::Camera);
	point->getMaterial()->setTexture(0, texture);
	point->getMaterial()->applyMaterial();

	pointGroup->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);
	pointGroup->createModel(Particle::Scale)->setStart(0.8f, 1.0f)->setEnd(0.0f);
	pointGroup->LifeMin = 1.0f;
	pointGroup->LifeMax = 1.5f;
	pointGroup->Friction = 0.1f;

	Particle::CEmitter *randomEmitter = factory->createSphericEmitter(CTransform::s_oy, 0.0f, core::PI);
	randomEmitter->setFlow(-1);
	randomEmitter->setTank(20);
	randomEmitter->setForce(0.1f, 0.3f);
	randomEmitter->setZone(largeSphereZone);
	pointGroup->addEmitter(randomEmitter);
}

void SampleParticlesMagicSkill::onUpdate()
{
	// update application
	m_scene->update();

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void SampleParticlesMagicSkill::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// Render 2d gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);

	CImguiManager::getInstance()->onRender();
}

void SampleParticlesMagicSkill::onPostRender()
{
	// post render application
}

bool SampleParticlesMagicSkill::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleParticlesMagicSkill::onResume()
{
	// resume application
}

void SampleParticlesMagicSkill::onPause()
{
	// pause application
}

void SampleParticlesMagicSkill::onQuitApp()
{
	// end application
	delete this;
}