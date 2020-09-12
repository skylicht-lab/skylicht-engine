#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleParticles.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "GridPlane/CGridPlane.h"
#include "ParticleSystem/CParticleComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleParticles *app = new SampleParticles();
	getApplication()->registerAppEvent("SampleParticles", app);
}

SampleParticles::SampleParticles() :
	m_scene(NULL),
	m_label(NULL),
	m_currentParticleObj(NULL)
{
	CImguiManager::createGetInstance();
}

SampleParticles::~SampleParticles()
{
	delete m_scene;
	delete m_font;

	CImguiManager::releaseInstance();
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
	m_currentParticleObj = zone->createEmptyObject();
	Particle::CParticleComponent *particleComponent = m_currentParticleObj->addComponent<Particle::CParticleComponent>();

	updateParticleZone(particleComponent, Particle::Sphere);
	updateParticleEmitter(particleComponent, Particle::Random);
	updateParticleRenderer(particleComponent);

	// 3D text
	createCanvasText("", core::vector3df(0.0f, 2.0f, 0.0f));

	// Rendering pipe line
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

	m_label = guiText;
}

Particle::CParticleBufferData* SampleParticles::getParticleData(CGameObject *obj)
{
	return obj->getComponent<Particle::CParticleComponent>()->getData();
}

Particle::CZone* SampleParticles::updateParticleZone(Particle::CParticleComponent *particleComponent, Particle::EZone zoneType)
{
	Particle::CFactory *factory = particleComponent->getParticleFactory();

	Particle::CGroup *group = NULL;
	if (particleComponent->getNumOfGroup() == 0)
		group = particleComponent->createParticleGroup();
	else
		group = particleComponent->getGroup(0);

	Particle::CZone *zone = group->getZone();

	// delete old zone
	if (zone != NULL)
	{
		group->setZone(NULL);
		factory->deleteZone(zone);
		zone = NULL;
	}

	float size = 1.5f;

	switch (zoneType)
	{
	case Particle::Point:
		zone = group->setZone(factory->createPointZone());
		break;
	case Particle::Sphere:
		zone = group->setZone(factory->createSphereZone(core::vector3df(0.0f, 0.0f, 0.0f), size));
		break;
	case Particle::AABox:
		zone = group->setZone(factory->createAABoxZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(size, size, size)));
		break;
	case Particle::Cylinder:
		zone = group->setZone(factory->createCylinderZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, size, 0.0f), size * 0.5f, 1.0f));
		break;
	case Particle::Line:
		zone = group->setZone(factory->createLineZone(core::vector3df(-size, 0.0f, 0.0f), core::vector3df(size, 0.0f, 0.0f)));
		break;
	case Particle::PolyLine:
	{
		core::array<core::vector3df> points;
		points.push_back(core::vector3df(-size, 0.0f, -size));
		points.push_back(core::vector3df(size, 0.0f, -size));
		points.push_back(core::vector3df(size, 0.0f, size));
		points.push_back(core::vector3df(-size, 0.0f, size));
		points.push_back(core::vector3df(-size, 0.0f, -size));
		zone = group->setZone(factory->createPolyLineZone(points));
	}
	break;
	case Particle::Ring:
		zone = group->setZone(factory->createRingZone(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f), size, size));
		break;
	default:
		break;
	}

	return zone;
}

Particle::CEmitter* SampleParticles::updateParticleEmitter(Particle::CParticleComponent *particleComponent, Particle::EEmitter emitterType)
{
	Particle::CFactory *factory = particleComponent->getParticleFactory();

	Particle::CGroup *group = NULL;
	if (particleComponent->getNumOfGroup() == 0)
		group = particleComponent->createParticleGroup();
	else
		group = particleComponent->getGroup(0);

	Particle::CEmitter *emitter = NULL;

	if (group->getEmitters().size() > 0)
		emitter = group->getEmitters()[0];

	// delete old emitter
	if (emitter != NULL)
	{
		group->removeEmitter(emitter);
		factory->deleteEmitter(emitter);
		emitter = NULL;
	}

	float minForce = 0.0f;
	float maxForce = 0.2f;
	float flow = 2000.0f;

	switch (emitterType)
	{
	case Particle::Random:
		emitter = group->addEmitter(factory->createRandomEmitter());
		break;
	case Particle::Straight:
		emitter = group->addEmitter(factory->createStraightEmitter(core::vector3df(0.0f, 1.0f, 0.0f)));
		break;
	case Particle::Spheric:
		emitter = group->addEmitter(factory->createSphericEmitter(core::vector3df(0.0f, 1.0f, 0.0f), core::PI * 0.0f, core::PI * 0.5f));
		break;
	case Particle::Normal:
		emitter = group->addEmitter(factory->createNormalEmitter(false));
		break;
	default:
		break;
	}

	emitter->setTank(-1);
	emitter->setFlow(flow);
	emitter->setForce(minForce, maxForce);
	emitter->setEmitFullZone(true);

	return emitter;
}

Particle::IRenderer* SampleParticles::updateParticleRendererType(Particle::CParticleComponent *particleComponent, int typeId)
{
	Particle::CFactory *factory = particleComponent->getParticleFactory();

	Particle::CGroup *group = NULL;
	if (particleComponent->getNumOfGroup() == 0)
		return NULL;
	else
		group = particleComponent->getGroup(0);

	Particle::IRenderer *r = group->getRenderer();
	if (r->getType() != Particle::Quad)
		return r;

	Particle::CQuadRenderer *quadRenderer = (Particle::CQuadRenderer*)r;

	if (typeId == 0)
	{
		// point
		ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
		quadRenderer->setMaterialType(Particle::Addtive, Particle::Camera);
		quadRenderer->setAtlas(1, 1);
		quadRenderer->getMaterial()->setUniformTexture("uTexture", texture);
		quadRenderer->getMaterial()->applyMaterial();
		quadRenderer->SizeX = 1.0f;
		quadRenderer->SizeY = 1.0f;
		quadRenderer->SizeZ = 1.0f;
	}
	else if (typeId == 1)
	{
		// sprite
		ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/explosion.png");
		quadRenderer->setAtlas(2, 2);
		quadRenderer->getMaterial()->setUniformTexture("uTexture", texture);
		quadRenderer->getMaterial()->applyMaterial();
		quadRenderer->SizeX = 2.0f;
		quadRenderer->SizeY = 2.0f;
		quadRenderer->SizeZ = 2.0f;
	}
	else if (typeId == 2)
	{
		// line
		ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/spark1.png");
		quadRenderer->setMaterialType(Particle::Addtive, Particle::Velocity);
		quadRenderer->setAtlas(1, 1);
		quadRenderer->getMaterial()->setUniformTexture("uTexture", texture);
		quadRenderer->getMaterial()->applyMaterial();
		quadRenderer->SizeX = 0.5f;
		quadRenderer->SizeY = 2.0f;
		quadRenderer->SizeZ = 1.0f;
	}

	return r;
}

Particle::IRenderer* SampleParticles::updateParticleRenderer(Particle::CParticleComponent *particleComponent)
{
	Particle::CFactory *factory = particleComponent->getParticleFactory();

	Particle::CGroup *group = NULL;
	if (particleComponent->getNumOfGroup() == 0)
		group = particleComponent->createParticleGroup();
	else
		group = particleComponent->getGroup(0);

	// delete old emitter
	if (group->getRenderer() != NULL)
	{
		factory->deleteRenderer(group->getRenderer());
		group->setRenderer(NULL);
	}

	// create renderer
	Particle::CQuadRenderer *quadRenderer = factory->createQuadRenderer();
	group->setRenderer(quadRenderer);

	ITexture *texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	quadRenderer->setMaterialType(Particle::Addtive, Particle::Camera);
	quadRenderer->setAtlas(1, 1);
	quadRenderer->getMaterial()->setUniformTexture("uTexture", texture);
	quadRenderer->getMaterial()->applyMaterial();

	// create model
	group->Gravity.set(0.0f, -0.2f, 0.0f);

	group->createModel(Particle::RotateSpeedZ)->setStart(-2.0f, 2.0f);
	group->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);

	group->createModel(Particle::Scale)->setStart(0.05f)->setEnd(0.1f, 0.13f);
	group->createModel(Particle::ColorA)->setStart(1.0f)->setEnd(0.0f);

	float r = Particle::random(0.5f, 0.7f);
	float g = Particle::random(0.1f, 0.2f);
	float b = Particle::random(0.7f, 1.0f);

	group->createModel(Particle::ColorR)->setStart(r)->setEnd(0.0f);
	group->createModel(Particle::ColorG)->setStart(g)->setEnd(0.0f);
	group->createModel(Particle::ColorB)->setStart(b)->setEnd(1.0f);

	return quadRenderer;
}

void SampleParticles::onUpdate()
{
	// update application
	m_scene->update();

	// write current particle
	static float textUpdate = 0.0f;

	textUpdate = textUpdate - getTimeStep();
	if (textUpdate < 0.0f)
	{
		Particle::CParticleComponent *psComponent = m_currentParticleObj->getComponent<Particle::CParticleComponent>();
		u32 totalParticle = psComponent->getGroup(0)->getNumParticles();
		char text[64];
		sprintf(text, "%ld", totalParticle);
		m_label->setText(text);
		textUpdate = 300.0f;
	}

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void SampleParticles::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// Render 2d gui camera
	CGraphics2D::getInstance()->render(m_camera);

	// ImGUI render
	onGUI();

	CImguiManager::getInstance()->onRender();
}

void SampleParticles::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(935, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(340, 760), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Particle System", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		onGUIParticle();
	}

	ImGui::End();

	// DEMO HOW TO USE IMGUI (SEE THIS FUNCTION)
	// ImGui::ShowDemoWindow(&open);
}

void SampleParticles::onGUIParticle()
{
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

	static int currentZone = 1; // default is sphere
	static int currentEmitter = 0;

	bool changeZone = false;
	bool changeEmitter = false;

	if (ImGui::CollapsingHeader("Zone & Emitter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int lastZone = currentZone;
		int lastEmitter = currentEmitter;

		ImGui::Indent();
		ImGui::Combo("Zone", &currentZone, zoneName, IM_ARRAYSIZE(zoneName));

		ImGui::SameLine();
		imguiHelpMarker("The shape that spawn particles");

		if (lastZone != currentZone)
			changeZone = true;

		ImGui::Combo("Emiter", &currentEmitter, emitterName, IM_ARRAYSIZE(emitterName));
		ImGui::SameLine();
		imguiHelpMarker("The force type spawn particle");

		if (lastEmitter != currentEmitter)
			changeEmitter = true;

		if (changeZone == true)
			updateParticleZone(m_currentParticleObj->getComponent<Particle::CParticleComponent>(), (Particle::EZone)currentZone);

		if (changeEmitter == true)
			updateParticleEmitter(m_currentParticleObj->getComponent<Particle::CParticleComponent>(), (Particle::EEmitter)currentEmitter);

		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Particle Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		onGUIZoneNode(currentZone);

		onGUIEmitterNode(currentEmitter);

		onGUIRendererNode();

		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Particle Function", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		ImGui::PushItemWidth(-1);

		float width = ImGui::CalcItemWidth() - 21.0f;
		ImVec2 btnSize(width, 0.0f);

		if (ImGui::TreeNode("Stop Emitter"))
		{
			if (ImGui::Button("Stop Particle", btnSize))
			{
				Particle::CEmitter *emitter = getParticleData(m_currentParticleObj)->Groups[0]->getEmitters()[0];
				emitter->setFlow(0);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Tank Emitter"))
		{
			static float flow = 100;
			static int tank = 100;

			ImGui::SliderFloat("Tank flow", &flow, 0.0f, 10000.0f, "flow = %.3f");
			ImGui::SliderInt("Tank", &tank, 0, 10000, "tank = %d");

			if (ImGui::Button("Tank Particle", btnSize))
			{
				Particle::CEmitter *emitter = getParticleData(m_currentParticleObj)->Groups[0]->getEmitters()[0];
				emitter->setFlow(flow);
				emitter->setTank(tank);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Play Emitter"))
		{
			if (ImGui::Button("Play Particle", btnSize))
			{
				Particle::CEmitter *emitter = getParticleData(m_currentParticleObj)->Groups[0]->getEmitters()[0];
				emitter->setTank(-1);
				emitter->setFlow(2000);
			}

			ImGui::TreePop();
		}

		ImGui::PopItemWidth();

		ImGui::Unindent();
	}
}

void SampleParticles::onGUIZoneNode(int currentZone)
{
	float minSize = 0.1f;
	float maxSize = 5.0f;

	if (currentZone == (int)Particle::Sphere)
	{
		if (ImGui::TreeNode("Sphere (Zone)"))
		{
			static float r = 0.0f;

			if (r == 0.0f)
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CSphere *sphere = dynamic_cast<Particle::CSphere*>(particleData->Groups[0]->getZone());
				r = sphere->getRadius();
			}

			float lastR = r;
			ImGui::SliderFloat("Radius (Zone)", &r, minSize, maxSize, "radius = %.3f");

			if (!core::equals(lastR, r))
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CSphere *sphere = dynamic_cast<Particle::CSphere*>(particleData->Groups[0]->getZone());
				sphere->setRadius(r);
			}

			ImGui::TreePop();
		}
	}
	else if (currentZone == (int)Particle::AABox)
	{
		if (ImGui::TreeNode("AABox (Zone)"))
		{
			static float sizeX = 0.0f;
			static float sizeY = 0.0f;
			static float sizeZ = 0.0f;

			if (sizeX == 0.0f)
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CAABox *aabox = dynamic_cast<Particle::CAABox*>(particleData->Groups[0]->getZone());
				const core::vector3df& s = aabox->getDimension();
				sizeX = s.X;
				sizeY = s.Y;
				sizeZ = s.Z;
			}

			float lastX = sizeX;
			float lastY = sizeY;
			float lastZ = sizeZ;

			ImGui::SliderFloat("X", &sizeX, minSize, maxSize, "x = %.3f");
			ImGui::SliderFloat("Y", &sizeY, minSize, maxSize, "y = %.3f");
			ImGui::SliderFloat("Z", &sizeZ, minSize, maxSize, "z = %.3f");

			if (!core::equals(lastX, sizeX) || !core::equals(lastY, sizeY) || !core::equals(lastZ, sizeZ))
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CAABox *aabox = dynamic_cast<Particle::CAABox*>(particleData->Groups[0]->getZone());
				aabox->setDimension(core::vector3df(sizeX, sizeY, sizeZ));
			}

			ImGui::TreePop();
		}
	}
	else if (currentZone == (int)Particle::Cylinder)
	{
		if (ImGui::TreeNode("Cylinder (Zone)"))
		{
			static float r = 0.0f;
			static float l = 0.0f;
			static float rx = 0.0f;
			static float ry = 0.0f;
			static float rz = 0.0f;

			if (r == 0.0f)
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CCylinder *cylinder = dynamic_cast<Particle::CCylinder*>(particleData->Groups[0]->getZone());
				l = cylinder->getLength();
				r = cylinder->getRadius();
			}

			float lastRadius = r;
			float lastLength = l;
			float lastX = rx;
			float lastY = ry;
			float lastZ = rz;

			ImGui::SliderFloat("Radius", &r, minSize, maxSize, "r = %.3f");
			ImGui::SliderFloat("Length", &l, minSize, maxSize, "l = %.3f");

			core::vector3df direction = imguiDirection(CTransform::s_oy, rx, ry, rz, "Direction", "%.3f");

			if (!core::equals(lastRadius, r) ||
				!core::equals(lastLength, l) ||
				!core::equals(lastX, rx) ||
				!core::equals(lastY, ry) ||
				!core::equals(lastZ, rz))
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CCylinder *cylinder = dynamic_cast<Particle::CCylinder*>(particleData->Groups[0]->getZone());
				cylinder->setRadius(r);
				cylinder->setLength(l);
				cylinder->setDirection(direction);
			}

			ImGui::TreePop();
		}
	}
	else if (currentZone == (int)Particle::Ring)
	{
		if (ImGui::TreeNode("Ring (Zone)"))
		{
			static float r1 = 0.0f;
			static float r2 = 0.0f;

			if (r1 == 0.0f)
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CRing *ring = dynamic_cast<Particle::CRing*>(particleData->Groups[0]->getZone());
				r1 = ring->getMinRadius();
				r2 = ring->getMaxRadius();
			}

			float lastRadius1 = r1;
			float lastRadius2 = r2;

			ImGui::DragFloatRange2("Radius", &r1, &r2, 0.01f, minSize, maxSize, "Min: %.3f", "Max: %.3f");

			if (!core::equals(lastRadius1, r1) || !core::equals(lastRadius2, r2))
			{
				Particle::CParticleBufferData *particleData = getParticleData(m_currentParticleObj);
				Particle::CRing *ring = dynamic_cast<Particle::CRing*>(particleData->Groups[0]->getZone());

				float min = r1;
				float max = r2;
				if (r1 > r2)
				{
					min = r2;
					max = r1;
				}

				ring->setRadius(min, max);
			}

			ImGui::TreePop();
		}
	}
}

void SampleParticles::onGUIEmitterNode(int currentEmitter)
{
	float minForce = 0.0f;
	float maxForce = 0.0f;
	float flow = 0.0f;
	bool fullZone = false;

	float lifeMin = 0.0f;
	float lifeMax = 0.0f;
	float friction = 0.0f;

	Particle::CParticleBufferData* particleData = getParticleData(m_currentParticleObj);
	Particle::CGroup *group = particleData->Groups[0];
	Particle::CEmitter *emitter = group->getEmitters()[0];

	minForce = emitter->getForceMin();
	maxForce = emitter->getForceMax();
	flow = emitter->getFlow();
	fullZone = emitter->isEmitFullZone();

	lifeMin = group->LifeMin;
	lifeMax = group->LifeMax;
	friction = group->Friction;

	if (ImGui::TreeNode("Global variable (Group)"))
	{
		if (ImGui::TreeNode("Life"))
		{
			ImGui::DragFloatRange2("Life", &lifeMin, &lifeMax, 0.01f, 0.01f, 6.0f, "Min: %.3f", "Max: %.3f");
			group->LifeMin = lifeMin;
			group->LifeMax = lifeMax;
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Friction"))
		{
			ImGui::SliderFloat("Friction", &friction, 0.0f, 30.0f, "friction = %.3f");
			group->Friction = friction;
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Gravity"))
		{
			core::vector3df gravity = group->Gravity;
			gravity.normalize();

			float gravityValue = group->Gravity.getLength();

			ImGui::SliderFloat("Gravity", &gravityValue, 0.0f, 30.0f, "gravity = %.3f");

			static float rx = 0.0f;
			static float ry = 0.0f;
			static float rz = 0.0f;
			core::vector3df direction = imguiDirection(-CTransform::s_oy, rx, ry, rz, "Direction", "%.3f");

			group->Gravity = direction * gravityValue;

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Force & Born (Emitter)"))
	{
		ImGui::DragFloatRange2("Force", &minForce, &maxForce, 0.01f, 0.0f, 10.0f, "Min: %.3f", "Max: %.3f");
		emitter->setForce(minForce, maxForce);

		ImGui::SliderFloat("Flow", &flow, 1.0f, 10000.0f, "flow = %.3f");
		emitter->setFlow(flow);

		ImGui::Checkbox("Spawn particle at full zone", &fullZone);
		emitter->setEmitFullZone(fullZone);

		ImGui::TreePop();
	}

	if (currentEmitter == (int)Particle::Straight)
	{
		if (ImGui::TreeNode("Straight (Emitter)"))
		{
			Particle::CStraightEmitter *e = (Particle::CStraightEmitter*)emitter;

			static float rx = 0.0f;
			static float ry = 0.0f;
			static float rz = 0.0f;
			core::vector3df direction = imguiDirection(CTransform::s_oy, rx, ry, rz, "Direction", "%.3f");
			e->setDirection(direction);

			ImGui::TreePop();
		}
	}

	if (currentEmitter == (int)Particle::Spheric)
	{
		if (ImGui::TreeNode("Spheric (Emitter)"))
		{
			Particle::CSphericEmitter *e = (Particle::CSphericEmitter*)emitter;

			static float rx = 0.0f;
			static float ry = 0.0f;
			static float rz = 0.0f;
			core::vector3df direction = imguiDirection(CTransform::s_oy, rx, ry, rz, "Direction", "%.3f");
			e->setDirection(direction);

			float angleMin = core::radToDeg(e->getAngleA());
			float angleMax = core::radToDeg(e->getAngleB());
			ImGui::DragFloatRange2("Angle", &angleMin, &angleMax, 0.1f, 0.0f, 360.0f, "Min: %.3f", "Max: %.3f");
			e->setAngles(core::degToRad(angleMin), core::degToRad(angleMax));

			ImGui::TreePop();
		}
	}
}

void SampleParticles::onGUIRendererNode()
{
	Particle::CParticleBufferData* particleData = getParticleData(m_currentParticleObj);
	Particle::CGroup *group = particleData->Groups[0];

	const char *renderType[] = {
		"Point",
		"Sprite",
		"Line"
	};

	static int currentRenderType = 0;

	if (ImGui::TreeNode("Renderer"))
	{
		if (ImGui::TreeNode("Scale"))
		{
			Particle::CModel *scale = group->getModel(Particle::Scale);
			if (scale != NULL)
			{
				float startS1 = scale->getStartValue1();
				float startS2 = scale->getStartValue2();

				float endS1 = scale->getEndValue1();
				float endS2 = scale->getEndValue2();

				ImGui::DragFloatRange2("Start", &startS1, &startS2, 0.01f, 0.0f, 1.0f, "Start 1: %.3f", "Start 2: %.3f");
				ImGui::DragFloatRange2("End", &endS1, &endS2, 0.01f, 0.0f, 1.0f, "End 1: %.3f", "End 2: %.3f");

				scale->setStart(startS1, startS2)->setEnd(endS1, endS2);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Color"))
		{
			Particle::CModel *modelR = group->getModel(Particle::ColorR);
			Particle::CModel *modelG = group->getModel(Particle::ColorG);
			Particle::CModel *modelB = group->getModel(Particle::ColorB);
			Particle::CModel *modelA = group->getModel(Particle::ColorA);

			float startColor1[4];
			float startColor2[4];
			float endColor1[4];
			float endColor2[4];

			startColor1[0] = modelR->getStartValue1();
			startColor1[1] = modelG->getStartValue1();
			startColor1[2] = modelB->getStartValue1();
			startColor1[3] = modelA->getStartValue1();

			startColor2[0] = modelR->getStartValue2();
			startColor2[1] = modelG->getStartValue2();
			startColor2[2] = modelB->getStartValue2();
			startColor2[3] = modelA->getStartValue2();

			endColor1[0] = modelR->getEndValue1();
			endColor1[1] = modelG->getEndValue1();
			endColor1[2] = modelB->getEndValue1();
			endColor1[3] = modelA->getEndValue1();

			endColor2[0] = modelR->getEndValue2();
			endColor2[1] = modelG->getEndValue2();
			endColor2[2] = modelB->getEndValue2();
			endColor2[3] = modelA->getEndValue2();

			ImGui::ColorEdit4("Start 1", startColor1);
			ImGui::ColorEdit4("Start 2", startColor2);

			ImGui::ColorEdit4("End 1", endColor1);
			ImGui::ColorEdit4("End 2", endColor2);

			modelR->setStart(startColor1[0], startColor2[0])->setEnd(endColor1[0], endColor2[0]);
			modelG->setStart(startColor1[1], startColor2[1])->setEnd(endColor1[1], endColor2[1]);
			modelB->setStart(startColor1[2], startColor2[2])->setEnd(endColor1[2], endColor2[2]);
			modelA->setStart(startColor1[3], startColor2[3])->setEnd(endColor1[3], endColor2[3]);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Rotate speed"))
		{
			Particle::CModel *rotate = group->getModel(Particle::RotateSpeedZ);
			float startR1 = rotate->getStartValue1();
			float startR2 = rotate->getStartValue2();

			ImGui::DragFloatRange2("Rotate", &startR1, &startR2, -4.0, 0.0f, 4.0f, "speed 1: %.3f", "speed: %.3f");

			rotate->setStart(startR1, startR2);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Render type"))
		{
			int lastRenderType = currentRenderType;

			ImGui::Combo("Render type", &currentRenderType, renderType, IM_ARRAYSIZE(renderType));

			if (lastRenderType != currentRenderType)
				updateParticleRendererType(m_currentParticleObj->getComponent<Particle::CParticleComponent>(), currentRenderType);

			float sizeX = group->getRenderer()->SizeX;
			float sizeY = group->getRenderer()->SizeY;
			float sizeZ = group->getRenderer()->SizeZ;

			ImGui::SliderFloat("Size X", &sizeX, 0.0f, 4.0f, "sizeX = %.3f");
			ImGui::SliderFloat("Size Y", &sizeY, 0.0f, 4.0f, "sizeY = %.3f");
			ImGui::SliderFloat("Size Z", &sizeZ, 0.0f, 4.0f, "sizeZ = %.3f");

			group->getRenderer()->SizeX = sizeX;
			group->getRenderer()->SizeY = sizeY;
			group->getRenderer()->SizeZ = sizeZ;

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

core::vector3df SampleParticles::imguiDirection(core::vector3df baseDirection, float &x, float &y, float &z, const char *name, const char *fmt)
{
	float v[] = { x, y, z };
	ImGui::DragFloat3(name, v, 0.1f, -180.0f, 180.0f, fmt);

	x = v[0];
	y = v[1];
	z = v[2];

	core::matrix4 m;
	m.setRotationDegrees(core::vector3df(x, y, z));

	core::vector3df direction = baseDirection;
	m.rotateVect(direction);
	direction.normalize();
	return direction;
}

void SampleParticles::imguiHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
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