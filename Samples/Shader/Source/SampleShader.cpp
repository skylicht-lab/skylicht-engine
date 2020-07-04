#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleShader.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "Lightmapper/CLightmapper.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleShader *app = new SampleShader();
	getApplication()->registerAppEvent("SampleShader", app);
}

SampleShader::SampleShader() :
	m_scene(NULL),
	m_bakeSHLighting(true)
{
	Lightmapper::CLightmapper::createGetInstance();
}

SampleShader::~SampleShader()
{
	delete m_scene;

	Lightmapper::CLightmapper::releaseInstance();
}

void SampleShader::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	io::IFileSystem* fs = app->getFileSystem();
	fs->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("PBR.zip"), false, false);

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

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/MonValley.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// 3D grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	CMeshManager *meshManager = CMeshManager::getInstance();
	CEntityPrefab *prefab = NULL;

	std::vector<std::string> textureFolders;
	textureFolders.push_back("Sponza/Textures");

	// Load wheel model
	prefab = meshManager->loadModel("PBR/Models/wheel_backleft.dae", NULL, true);
	if (prefab != NULL)
	{
		// load normal map
		ITexture *normalMap = CTextureManager::getInstance()->getTexture("PBR/Models/wheels_norm.tga");

		// init material
		ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(prefab);
		for (CMaterial *material : materials)
		{
			material->changeShader("PBR/Shader/Normal.xml");
			material->setUniformTexture("uTexNormal", normalMap);
		}

		// create render mesh object
		CGameObject *wheel = zone->createEmptyObject();
		wheel->setStatic(true);

		// render mesh & init material
		CRenderMesh *renderer = wheel->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);

		// set indirect lighting by VertexColor
		CIndirectLighting *indirectLighting = wheel->addComponent<CIndirectLighting>();
		indirectLighting->setIndirectLightingType(CIndirectLighting::SH4);

		m_objects.push_back(wheel);
	}

	// Rendering
	m_forwardRP = new CForwardRP();
}

void SampleShader::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleShader::onRender()
{
	if (m_bakeSHLighting == true)
	{
		m_bakeSHLighting = false;

		for (CGameObject *obj : m_objects)
			obj->setVisible(false);

		CGameObject *bakeCameraObj = m_scene->getZone(0)->createEmptyObject();
		CCamera *bakeCamera = bakeCameraObj->addComponent<CCamera>();
		m_scene->updateAddRemoveObject();

		core::vector3df pos(0.0f, 0.0f, 0.0f);

		core::vector3df normal = CTransform::s_oy;
		core::vector3df tangent = CTransform::s_ox;
		core::vector3df binormal = normal.crossProduct(tangent);
		binormal.normalize();

		Lightmapper::CLightmapper *lm = Lightmapper::CLightmapper::getInstance();
		lm->initBaker(64);
		Lightmapper::CSH9 sh = lm->bakeAtPosition(
			bakeCamera,
			m_forwardRP,
			m_scene->getEntityManager(),
			pos,
			normal, tangent, binormal);

		// apply indirect lighting
		std::vector<CIndirectLighting*> lightings = m_scene->getZone(0)->getComponentsInChild<CIndirectLighting>(false);
		for (CIndirectLighting *indirect : lightings)
			indirect->setSH(sh.getValue());

		for (CGameObject *obj : m_objects)
			obj->setVisible(true);
	}

	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleShader::onPostRender()
{
	// post render application
}

bool SampleShader::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleShader::onResume()
{
	// resume application
}

void SampleShader::onPause()
{
	// pause application
}

void SampleShader::onQuitApp()
{
	// end application
	delete this;
}