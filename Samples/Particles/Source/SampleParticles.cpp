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
	delete m_font;
}

void SampleParticles::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Particles.zip"), false, false);

	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 70);

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
	float dis = 4.0f;

	const char *zoneName[] = {
		"Point",
		"Sphere",
		"AABox",
		"Cylinder",
		"Line",
		"PolyLine",
		"Ring",
	};

	const char *emitterName[] = {
		"Random",
		"Straight",
		"Spheric",
		"Normal"
	};

	for (int i = 0, n = (int)Particle::NumOfZone; i < n; i++)
	{
		for (int j = 0, m = (int)Particle::NumOfEmitter; j < m; j++)
		{
			core::vector3df position((j - m / 2) * dis, 0.0f, (i - n / 2) * dis);

			initParticle(
				zone->createEmptyObject()->addComponent<Particle::CParticleComponent>(),
				(Particle::EZone)i,
				(Particle::EEmitter)j,
				position
			);

			position.Y = 2.0f;

			std::string label = zoneName[i];
			label += "+";
			label += emitterName[j];

			createCanvasText(label.c_str(), position);
		}
	}

	// Rendering
	m_forwardRP = new CForwardRP();
}

void SampleParticles::createCanvasText(const char *text, const core::vector3df& position)
{
	CGameObject *canvasObject = m_scene->getZone(0)->createEmptyObject();

	CCanvas *canvas = canvasObject->addComponent<CCanvas>();
	CGUIText *guiText = canvas->createText(core::rectf(0.0f, 0.0f, 700.0f, 100.0f), m_font);
	guiText->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
	guiText->setText(text);
	guiText->setPosition(core::vector3df(-350.0f, 0.0f, 0.0f));

	CGUIElement *rootGUI = canvas->getRootElement();
	rootGUI->setScale(core::vector3df(-0.004f, -0.004f, 0.004f));
	rootGUI->setPosition(position);

	canvas->enable3DBillboard(true);
}

void SampleParticles::initParticle(Particle::CParticleComponent *particleComponent, Particle::EZone zoneType, Particle::EEmitter emitterType, const core::vector3df& position)
{
	Particle::CFactory *factory = particleComponent->getParticleFactory();
	Particle::CGroup *group = particleComponent->createParticleGroup();

	Particle::CZone *zone = NULL;
	float flow = 0.0f;

	switch (zoneType)
	{
	case Particle::Point:
		flow = 100.0f;
		zone = group->setZone(factory->createPointZone());
		break;
	case Particle::Sphere:
		flow = 200.0f;
		zone = group->setZone(factory->createSphereZone(core::vector3df(0.0f, 0.0f, 0.0f), 1.0f));
		break;
	case Particle::AABox:
		flow = 200.0f;
		zone = group->setZone(factory->createAABoxZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(1.0f, 1.0f, 1.0f)));
		break;
	case Particle::Cylinder:
		flow = 200.0f;
		zone = group->setZone(factory->createCylinderZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), 0.5f, 1.0f));
		break;
	case Particle::Line:
		flow = 100.0f;
		zone = group->setZone(factory->createLineZone(core::vector3df(-1.0f, 0.0f, 0.0f), core::vector3df(1.0f, 0.0f, 0.0f)));
		break;
	case Particle::PolyLine:
	{
		flow = 200.0f;
		core::array<core::vector3df> points;
		points.push_back(core::vector3df(-1.0f, 0.0f, -1.0f));
		points.push_back(core::vector3df(1.0f, 0.0f, -1.0f));
		points.push_back(core::vector3df(1.0f, 0.0f, 1.0f));
		points.push_back(core::vector3df(-1.0f, 0.0f, 1.0f));
		points.push_back(core::vector3df(-1.0f, 0.0f, -1.0f));
		zone = group->setZone(factory->createPolyLineZone(points));
	}
	break;
	case Particle::Ring:
		flow = 200.0f;
		zone = group->setZone(factory->createRingZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), 0.5, 1.0f));
		break;
	default:
		break;
	}

	Particle::CEmitter *emitter = NULL;

	float minForce = 1.0f;
	float maxForce = 2.0f;

	switch (emitterType)
	{
	case Particle::Random:
		minForce = 0.2f;
		maxForce = 0.5f;
		emitter = group->addEmitter(factory->createRandomEmitter());
		break;
	case Particle::Straight:
		emitter = group->addEmitter(factory->createStraightEmitter(core::vector3df(0.0f, 1.0f, 0.0f)));
		break;
	case Particle::Spheric:
		emitter = group->addEmitter(factory->createSphericEmitter(core::vector3df(0.0f, 1.0f, 0.0f), core::PI * 0.0f, core::PI * 0.5f));
		break;
	case Particle::Normal:
		minForce = 1.0f;
		maxForce = 1.0f;
		emitter = group->addEmitter(factory->createNormalEmitter(false));
		break;
	default:
		break;
	}

	emitter->setTank(0);
	emitter->setFlow(flow);
	emitter->setForce(minForce, maxForce);
	emitter->setEmitFullZone(false);

	// create renderer
	Particle::CQuadRenderer *quadRenderer = factory->createQuadRenderer();
	quadRenderer->setMaterialType(Particle::Addtive, Particle::Camera);
	group->setRenderer(quadRenderer);

	// ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/explosion.png");
	// quadRenderer->setAtlas(2, 2);

	ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	quadRenderer->setAtlas(1, 1);
	quadRenderer->getMaterial()->setUniformTexture("uTexture", texture);
	quadRenderer->getMaterial()->applyMaterial();

	// create model
	group->Gravity.set(0.0f, 0.2f, 0.0f);

	group->createModel(Particle::RotateSpeedZ)->setStart(-2.0f, 2.0f);
	group->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);

	group->createModel(Particle::ScaleX)->setStart(0.1f)->setEnd(0.15f, 0.2f);
	group->createModel(Particle::ScaleY)->setStart(0.1f)->setEnd(0.15f, 0.2f);
	group->createModel(Particle::ScaleZ)->setStart(0.15f)->setEnd(0.15f, 0.2f);

	group->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);

	float r = Particle::random(0.5f, 1.0f);
	float g = Particle::random(0.5f, 1.0f);
	float b = Particle::random(0.5f, 1.0f);

	group->createModel(Particle::ColorR)->setStart(r)->setEnd(1.0f);
	group->createModel(Particle::ColorG)->setStart(g)->setEnd(1.0f);
	group->createModel(Particle::ColorB)->setStart(b)->setEnd(1.0f);

	CTransformEuler *t = particleComponent->getGameObject()->getTransformEuler();
	t->setPosition(position);
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
	CGraphics2D::getInstance()->render(m_camera);
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