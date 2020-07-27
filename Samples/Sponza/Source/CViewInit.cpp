#include "pch.h"

#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "Lightmapper/Components/Probe/CLightProbe.h"
#include "Lightmapper/Components/Probe/CLightProbeRender.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char *name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	getApplication()->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Specular.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Diffuse.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessMask.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLightShadow.xml");

	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SH.xml");
}

void CViewInit::initScene()
{
	CBaseApp *app = getApplication();

	CScene *scene = CContext::getInstance()->initScene();
	CZone *zone = scene->createZone();

	// camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(1.0f);

	CCamera *camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(1.0f, 2.0f, 1.0f));
	camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject *guiCameraObj = zone->createEmptyObject();
	guiCameraObj->addComponent<CCamera>();
	CCamera *guiCamera = guiCameraObj->getComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	core::vector3df pointLightPosition[] = {
		{-11.19f, 2.4f, 4.01f},
		{-11.2f, 2.4f, -4.5f},
		{12.03f, 2.4f, 4.04f},
		{12.01f, 2.4f, -4.47f},
		{6.18f, 1.6f, -2.2f},
		{6.18f, 1.6f, 1.43f},
		{-4.89f, 1.6f, -2.17f},
		{-4.89f, 1.6f, 1.42f},
	};

	for (int i = 0; i < 8; i++)
	{
		CGameObject *pointLightObj = zone->createEmptyObject();

		CPointLight *pointLight = pointLightObj->addComponent<CPointLight>();
		pointLight->setShadow(true);

		if (i >= 4)
			pointLight->setRadius(3.0f);
		else
			pointLight->setRadius(6.0f);

		CTransformEuler *pointLightTransform = pointLightObj->getTransformEuler();
		pointLightTransform->setPosition(pointLightPosition[i]);
	}

	// sponza
	CMeshManager *meshManager = CMeshManager::getInstance();
	CEntityPrefab *prefab = NULL;

	std::vector<std::string> textureFolders;
	textureFolders.push_back("Sponza/Textures");

	// load model
	prefab = meshManager->loadModel("Sponza/Sponza.smesh", NULL, true);
	if (prefab != NULL)
	{
		// load material
		ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial("Sponza/Sponza.xml", true, textureFolders);

		// create render mesh object
		CGameObject *sponza = zone->createEmptyObject();
		sponza->setStatic(true);

		// renderer
		CRenderMesh *renderer = sponza->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);

		// indirect indirect lighting
		// CIndirectLighting *indirectLighting = sponza->addComponent<CIndirectLighting>();
		// indirectLighting->setIndirectLightingType(CIndirectLighting::VertexColor);
	}

	// save to context
	CContext *context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);

	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);

	initProbes();
}

void CViewInit::initProbes()
{
	CContext *context = CContext::getInstance();
	CZone *zone = context->getActiveZone();

	std::vector<core::vector3df> probesPosition;

	for (int i = 0; i < 7; i++)
	{
		float x = i * 5.6f - 3.0f * 5.6f;

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
		CGameObject *probeObj = zone->createEmptyObject();
		CLightProbe *probe = probeObj->addComponent<Lightmapper::CLightProbe>();

		CTransformEuler *probeTransform = probeObj->getTransformEuler();
		probeTransform->setPosition(probesPosition[i]);

		probes.push_back(probe);
	}

	CLightProbeRender::showProbe(true);

	context->setProbes(probes);
}

void CViewInit::onDestroy()
{

}

void CViewInit::onUpdate()
{
	CContext *context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("Common.Zip");
		listBundles.push_back("Sponza.Zip");

#ifdef __EMSCRIPTEN__
		std::vector<std::string> listFile;
		listFile.insert(listFile.end(), listBundles.begin(), listBundles.end());

		const char *filename = listFile[m_downloaded].c_str();

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
			const char *r = bundle.c_str();
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
		CScene *scene = context->getScene();
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
