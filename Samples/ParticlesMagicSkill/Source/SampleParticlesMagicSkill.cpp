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
	m_scene(NULL)
{
	CImguiManager::createGetInstance();

	CEventManager::getInstance()->registerEvent("App", this);
}

SampleParticlesMagicSkill::~SampleParticlesMagicSkill()
{
	CEventManager::getInstance()->unRegisterEvent(this);

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
			if (m_projectiles->IsPlaying() == false)
			{
				core::vector3df position(0.0f, 1.0f, 0.0f);

				m_projectiles->getGameObject()->setVisible(true);
				m_projectiles->getGameObject()->getTransformEuler()->setPosition(position);
				m_projectiles->Play();

				return true;
			}
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
	arcaneEmitter->setFlow(100.0f);
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
	sphereGroup->createModel(Particle::RotateSpeedZ)->setStart(3.0f, 5.0f);
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