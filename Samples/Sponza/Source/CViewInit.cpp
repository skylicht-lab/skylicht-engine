#include "pch.h"

#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "CFireLight.h"
#include "LightProbes/CLightProbe.h"
#include "LightProbes/CLightProbeRender.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char* name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	CBaseApp* app = getApplication();
	app->showDebugConsole();
	app->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->initSGDeferredShader();
}

void CViewInit::initScene()
{
	CBaseApp* app = getApplication();

	CScene* scene = CContext::getInstance()->initScene();
	CZone* zone = scene->createZone();

	// camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(1.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	CCamera* camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(1.0f, 2.0f, 1.0f));
	camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject* guiCameraObj = zone->createEmptyObject();
	guiCameraObj->addComponent<CCamera>();
	CCamera* guiCamera = guiCameraObj->getComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/Helipad.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome* skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();

	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	directionalLight->setIntensity(1.3f);

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(2.0f, -5.0f, 1.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	core::vector3df pointLightPosition[] = {
		{-11.19f, 2.4f, 4.01f},
		{-11.2f, 2.4f, -4.5f},
		{12.03f, 2.4f, 4.04f},
		{12.01f, 2.4f, -4.47f},
	};

	std::vector<CPointLight*> pointLights;

	for (int i = 0; i < 4; i++)
	{
		CGameObject* pointLightObj = zone->createEmptyObject();

		CPointLight* pointLight = pointLightObj->addComponent<CPointLight>();
		pointLight->setShadow(true);
		pointLight->setColor(SColor(255, 221, 123, 34));
		pointLight->setRadius(4.0f);

		CTransformEuler* pointLightTransform = pointLightObj->getTransformEuler();
		pointLightTransform->setPosition(pointLightPosition[i]);

		pointLightObj->addComponent<CFireLight>();

		pointLights.push_back(pointLight);
	}

	// sponza
	CMeshManager* meshManager = CMeshManager::getInstance();
	CEntityPrefab* prefab = NULL;

	std::vector<std::string> textureFolders;
	textureFolders.push_back("Sponza/Textures");

	// Load model from Sponza.smesh
	// How to export "Sponza.smesh" see SampleLightmapUV
	prefab = meshManager->loadModel("Sponza/Sponza.smesh", NULL, true);
	if (prefab != NULL)
	{
		// load material
		ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial("Sponza/Sponza.mat", true, textureFolders);

		// create render mesh object
		CGameObject* sponza = zone->createEmptyObject();
		sponza->setStatic(true);

		// renderer
		CRenderMesh* renderer = sponza->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);

		// indirect indirect lighting
		CIndirectLighting* indirectLighting = sponza->addComponent<CIndirectLighting>();

		// init lightmap texture array
		// See SampleLightmapping, that how to render this
		std::vector<std::string> textures;
		textures.push_back("Sponza/LightMapRasterize_bounce_3_0.png");
		textures.push_back("Sponza/LightMapRasterize_bounce_3_1.png");
		textures.push_back("Sponza/LightMapRasterize_bounce_3_2.png");

		ITexture* lightmapTexture = CTextureManager::getInstance()->getTextureArray(textures);
		if (lightmapTexture != NULL)
		{
			indirectLighting->setLightmap(lightmapTexture);
			indirectLighting->setIndirectLightingType(CIndirectLighting::LightmapArray);
		}
	}

	// init fire
	for (int i = 0; i < 4; i++)
	{
		CGameObject* fire = zone->createEmptyObject();
		Particle::CParticleComponent* fireParticle = fire->addComponent<Particle::CParticleComponent>();
		initFireParticle(fireParticle);

		fire->getTransformEuler()->setPosition(pointLightPosition[i] - core::vector3df(0.0f, 0.8f, 0.0f));
		fire->getTransformEuler()->setScale(core::vector3df(0.4f, 0.4f, 0.4f));
	}

	// save to context
	CContext* context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);

	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
	context->setPointLight(pointLights);

	// context->getDefferredRP()->enableTestIndirect(true);

	initProbes();
}

void CViewInit::initProbes()
{
	CContext* context = CContext::getInstance();
	CZone* zone = context->getActiveZone();

	std::vector<core::vector3df> probesPosition;

	for (int i = 0; i < 7; i++)
	{
		float dis = 4.0f;
		float x = i * dis - 3.0f * dis;

		// row 0
		probesPosition.push_back(core::vector3df(x, 2.0f, -0.4f));

		probesPosition.push_back(core::vector3df(x, 2.0f, 4.0f));

		probesPosition.push_back(core::vector3df(x, 2.0f, -4.6f));

		// row 1

		probesPosition.push_back(core::vector3df(x, 7.0f, -0.4f));

		probesPosition.push_back(core::vector3df(x, 7.0f, 4.0f));

		probesPosition.push_back(core::vector3df(x, 7.0f, -4.36));

		// row 2

		probesPosition.push_back(core::vector3df(x, 16.0f, -0.4f));
	}

	std::vector<CLightProbe*> probes;

	for (u32 i = 0, n = (int)probesPosition.size(); i < n; i++)
	{
		// probe
		CGameObject* probeObj = zone->createEmptyObject();
		CLightProbe* probe = probeObj->addComponent<CLightProbe>();

		CTransformEuler* probeTransform = probeObj->getTransformEuler();
		probeTransform->setPosition(probesPosition[i]);

		probes.push_back(probe);
	}

	CLightProbeRender::showProbe(false);

	context->setProbes(probes);
}

void CViewInit::initFireParticle(Particle::CParticleComponent* ps)
{
	ITexture* texture = NULL;
	Particle::CFactory* factory = ps->getParticleFactory();

	// GROUP: FIRE
	Particle::CGroup* fireGroup = ps->createParticleGroup();

	// Billboard render use is slower but render additive look better
	Particle::CBillboardAdditiveRenderer* fire = factory->createBillboardAdditiveRenderer();

	// Particle::CQuadRenderer *fire = factory->createQuadRenderer();
	// fire->setMaterialType(Particle::Additive, Particle::Camera);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/fire2.png");
	fire->setAtlas(2, 2);
	fire->SizeX = 0.4f;
	fire->SizeY = 0.4f;
	fire->getMaterial()->setTexture(0, texture);
	fire->getMaterial()->applyMaterial();

	// enable render emission buffer
	fire->setEmission(true);

	fireGroup->setRenderer(fire);
	fireGroup->createModel(Particle::ColorR)->setStart(0.9f, 1.0f);
	fireGroup->createModel(Particle::ColorG)->setStart(0.5f, 0.6f);
	fireGroup->createModel(Particle::ColorB)->setStart(0.3f);
	fireGroup->createModel(Particle::ColorA)->setStart(0.8)->setEnd(0.0f);
	fireGroup->createModel(Particle::RotateZ)->setStart(0.0f, 2.0f * core::PI);
	fireGroup->createModel(Particle::RotateSpeedZ)->setStart(-0.5f, 0.5f);
	fireGroup->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);
	fireGroup->LifeMin = 1.0f;
	fireGroup->LifeMax = 1.5f;
	fireGroup->Gravity.set(0.0f, 1.0f, 0.0f);

	Particle::CInterpolator* fireSizeInterpolate = fireGroup->createInterpolator();
	fireSizeInterpolate->addEntry(0.0f, 0.0f);
	fireSizeInterpolate->addEntry(0.5f, 2.0f);
	fireSizeInterpolate->addEntry(1.0f, 0.0f);
	fireGroup->createModel(Particle::Scale)->setInterpolator(fireSizeInterpolate);

	// Emitters
	// The emitters are arranged so that the fire looks realistic
	Particle::CStraightEmitter* fireEmitter1 = factory->createStraightEmitter(core::vector3df(0.0f, 1.0f, 0.0f));
	fireEmitter1->setZone(factory->createSphereZone(core::vector3df(0.0f, -1.0f, 0.0f), 0.5f));
	fireEmitter1->setFlow(15);
	fireEmitter1->setForce(1.0f, 2.5f);

	core::vector3df position(0.15f, -1.2f, 0.075f);
	core::vector3df direction(0.6f, 0.5f, 0.0f);
	core::quaternion q;
	q.fromAngleAxis(72.0f * core::DEGTORAD, CTransform::s_oy);
	float radius = 0.2f;

	Particle::CStraightEmitter* fireEmitter2 = factory->createStraightEmitter(core::vector3df(direction));
	fireEmitter2->setZone(factory->createSphereZone(position, radius));
	fireEmitter2->setFlow(10);
	fireEmitter2->setForce(0.5f, 1.5f);

	direction = q * direction;
	q.getMatrix().transformVect(position);
	Particle::CStraightEmitter* fireEmitter3 = factory->createStraightEmitter(core::vector3df(direction));
	fireEmitter3->setZone(factory->createSphereZone(position, radius));
	fireEmitter3->setFlow(10);
	fireEmitter3->setForce(0.5f, 1.5f);

	direction = q * direction;
	q.getMatrix().transformVect(position);
	Particle::CStraightEmitter* fireEmitter4 = factory->createStraightEmitter(core::vector3df(direction));
	fireEmitter4->setZone(factory->createSphereZone(position, radius));
	fireEmitter4->setFlow(10);
	fireEmitter4->setForce(0.5f, 1.5f);

	direction = q * direction;
	q.getMatrix().transformVect(position);
	Particle::CStraightEmitter* fireEmitter5 = factory->createStraightEmitter(core::vector3df(direction));
	fireEmitter5->setZone(factory->createSphereZone(position, radius));
	fireEmitter5->setFlow(10);
	fireEmitter5->setForce(0.5f, 1.5f);

	direction = q * direction;
	q.getMatrix().transformVect(position);
	Particle::CStraightEmitter* fireEmitter6 = factory->createStraightEmitter(core::vector3df(direction));
	fireEmitter6->setZone(factory->createSphereZone(position, radius));
	fireEmitter6->setFlow(10);
	fireEmitter6->setForce(0.5f, 1.5f);

	fireGroup->addEmitter(fireEmitter1);
	fireGroup->addEmitter(fireEmitter2);
	fireGroup->addEmitter(fireEmitter3);
	fireGroup->addEmitter(fireEmitter4);
	fireGroup->addEmitter(fireEmitter5);
	fireGroup->addEmitter(fireEmitter6);

	// GROUP: SMOKE
	Particle::CGroup* smokeGroup = ps->createParticleGroup();

	Particle::CBillboardAdditiveRenderer* smoke = factory->createBillboardAdditiveRenderer();

	// Particle::CQuadRenderer *smoke = factory->createQuadRenderer();
	// smoke->setMaterialType(Particle::Additive, Particle::Camera);

	smokeGroup->setRenderer(smoke);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/explosion.png");
	smoke->setAtlas(2, 2);
	smoke->SizeX = 0.3f;
	smoke->SizeY = 0.3f;
	smoke->getMaterial()->setTexture(0, texture);
	smoke->getMaterial()->applyMaterial();

	smokeGroup->createModel(Particle::ColorR)->setStart(0.3f)->setEnd(0.2f);
	smokeGroup->createModel(Particle::ColorG)->setStart(0.25f)->setEnd(0.2f);
	smokeGroup->createModel(Particle::ColorB)->setStart(0.2f);
	smokeGroup->createModel(Particle::Scale)->setStart(3.0f)->setEnd(6.0f);
	smokeGroup->createModel(Particle::RotateZ)->setStart(0.0f, 2.0f * core::PI);
	smokeGroup->createModel(Particle::RotateSpeedZ)->setStart(-0.5f, .5f);
	smokeGroup->createModel(Particle::FrameIndex)->setStart(0.0f, 3.0f);
	smokeGroup->LifeMin = 3.0f;
	smokeGroup->LifeMax = 4.0f;
	smokeGroup->Gravity.set(0.0f, 0.4f, 0.0f);

	Particle::CInterpolator* smokeAlphaInterpolator = smokeGroup->createInterpolator();
	smokeAlphaInterpolator->addEntry(0.0f, 0.0f);
	smokeAlphaInterpolator->addEntry(0.2f, 0.7f);
	smokeAlphaInterpolator->addEntry(1.0f, 0.0f);
	smokeGroup->createModel(Particle::ColorA)->setInterpolator(smokeAlphaInterpolator);

	Particle::CEmitter* smokeEmitter = factory->createSphericEmitter(core::vector3df(0.0f, 1.0f, 0.0f), 0.0f, 0.5f * core::PI);
	smokeEmitter->setZone(factory->createSphereZone(core::vector3df(), 1.2f));
	smokeEmitter->setFlow(25);
	smokeEmitter->setForce(0.5f, 1.0f);

	smokeGroup->addEmitter(smokeEmitter);

	// GROUP: POINT SPARK
	Particle::CGroup* pointSparkGroup = ps->createParticleGroup();

	Particle::CQuadRenderer* pointSpark = factory->createQuadRenderer();
	pointSparkGroup->setRenderer(pointSpark);

	texture = CTextureManager::getInstance()->getTexture("Particles/Textures/point.png");
	pointSpark->setMaterialType(Particle::Additive, Particle::Camera);
	pointSpark->getMaterial()->setTexture(0, texture);
	pointSpark->getMaterial()->applyMaterial();
	pointSpark->SizeX = 0.15f;
	pointSpark->SizeY = 0.15f;
	pointSpark->setEmission(true);

	Particle::CInterpolator* sparkInterpolator = pointSparkGroup->createInterpolator();
	sparkInterpolator->addEntry(0.0f, 0.0f);
	sparkInterpolator->addEntry(0.2f, 0.7f);
	sparkInterpolator->addEntry(1.0f, 0.0f);
	pointSparkGroup->createModel(Particle::ColorA)->setInterpolator(sparkInterpolator);

	pointSparkGroup->createModel(Particle::ColorR)->setStart(1.0f);
	pointSparkGroup->createModel(Particle::ColorG)->setStart(0.4f)->setEnd(0.3f, 0.1f);
	pointSparkGroup->createModel(Particle::ColorB)->setStart(0.0f)->setEnd(0.3f);
	pointSparkGroup->createModel(Particle::Scale)->setStart(1.0f)->setEnd(0.0f);
	pointSparkGroup->LifeMin = 2.0f;
	pointSparkGroup->LifeMax = 4.0f;
	pointSparkGroup->Gravity.set(0.0f, 1.0f, 0.0f);
	pointSparkGroup->Friction = 0.4f;

	Particle::CEmitter* pointSparkEmitter = factory->createSphericEmitter(core::vector3df(0.0f, 1.0f, 0.0f), 0.0f, 0.3f * core::PI);
	pointSparkEmitter->setFlow(10);
	pointSparkEmitter->setForce(1.5f, 2.0f);
	pointSparkEmitter->setZone(factory->createSphereZone(core::vector3df(0.0f, -0.5f, 0.0f), 0.5f));
	pointSparkGroup->addEmitter(pointSparkEmitter);

}

void CViewInit::onDestroy()
{

}

void CViewInit::onUpdate()
{
	CContext* context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("Common.zip");
		listBundles.push_back("Sponza.zip");
		listBundles.push_back(getApplication()->getTexturePackageName("Sponza"));
		listBundles.push_back("Particles.zip");

#ifdef __EMSCRIPTEN__
		std::vector<std::string> listFile;
		listFile.insert(listFile.end(), listBundles.begin(), listBundles.end());

		const char* filename = listFile[m_downloaded].c_str();

		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL(filename, filename);
			m_getFile->download(CGetFileURL::Get);

			char log[512];
			sprintf(log, "Download asset: %s", filename);
			os::Printer::log(log);
		}
		else
		{
			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// [bundles].zip
				fileSystem->addFileArchive(filename, false, false);

				if (++m_downloaded >= listFile.size())
					m_initState = CViewInit::InitScene;
				else
				{
					delete m_getFile;
					m_getFile = NULL;
				}
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
		}
#else

		for (std::string& bundle : listBundles)
		{
			const char* r = bundle.c_str();
#if defined(WINDOWS_STORE)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#elif defined(MACOS)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#else
			fileSystem->addFileArchive(r, false, false);
#endif
		}

		m_initState = CViewInit::InitScene;
#endif
	}
	break;
	case CViewInit::InitScene:
	{
		initScene();
		m_initState = CViewInit::Finished;
	}
	break;
	case CViewInit::Error:
	{
		// todo nothing with black screen
	}
	break;
	default:
	{
		CScene* scene = context->getScene();
		if (scene != NULL)
			scene->update();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
	}
}

void CViewInit::onRender()
{

}
