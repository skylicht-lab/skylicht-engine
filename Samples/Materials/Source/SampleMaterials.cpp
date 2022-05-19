#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleMaterials.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "CSphereComponent.h"
#include "Lightmapper/CLightmapper.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleMaterials *app = new SampleMaterials();
	getApplication()->registerAppEvent("SampleMaterials", app);
}

SampleMaterials::SampleMaterials() :
	m_scene(NULL),
	m_bakeSHLighting(true),
	m_reflectionProbe(NULL),
	m_forwardRP(NULL)
{
	Lightmapper::CLightmapper::createGetInstance();
}

SampleMaterials::~SampleMaterials()
{
	delete m_scene;

	delete m_forwardRP;

	Lightmapper::CLightmapper::releaseInstance();
}

void SampleMaterials::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	io::IFileSystem* fs = app->getFileSystem();
	fs->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("SampleMaterials.zip"), false, false);

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
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

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
	// CGameObject *grid = zone->createEmptyObject();
	// grid->addComponent<CGridPlane>();

	// Create sphere 1
	CGameObject *sphereObj = zone->createEmptyObject();
	CSphereComponent *sphere = sphereObj->addComponent<CSphereComponent>();

	// SH ambient lighting
	CIndirectLighting *indirect = sphereObj->addComponent<CIndirectLighting>();
	indirect->setIndirectLightingType(CIndirectLighting::SH9);

	// Reflection probe
	CGameObject *reflectionProbeObj = zone->createEmptyObject();
	m_reflectionProbe = reflectionProbeObj->addComponent<CReflectionProbe>();

	// Load texture
	CTextureManager *textureManager = CTextureManager::getInstance();
	ITexture *brickDiffuse = textureManager->getTexture("SampleMaterials/Textures/brick_diff.png");
	ITexture *brickNormal = textureManager->getTexture("SampleMaterials/Textures/brick_norm.png");
	ITexture *brickSpec = textureManager->getTexture("SampleMaterials/Textures/brick_spec.png");

	// Apply shader & uniform texture
	// Uniform name: uTexDiffuse, uTexNormal, uTexSpecular declare in SG.xml
	CMaterial *material = sphere->getMaterial();
	material->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SG.xml");
	material->setUniformTexture("uTexDiffuse", brickDiffuse);
	material->setUniformTexture("uTexNormal", brickNormal);
	material->setUniformTexture("uTexSpecular", brickSpec);
	material->applyMaterial();

	m_spheres.push_back(sphereObj);

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleMaterials::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleMaterials::onRender()
{
	if (m_bakeSHLighting == true)
	{
		m_bakeSHLighting = false;

		for (CGameObject *sphere : m_spheres)
			sphere->setVisible(false);

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

		for (CGameObject *sphere : m_spheres)
			sphere->setVisible(true);

		// bake environment
		m_reflectionProbe->bakeProbe(bakeCamera, m_forwardRP, m_scene->getEntityManager());
	}

	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleMaterials::onPostRender()
{
	// post render application
}

bool SampleMaterials::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleMaterials::onResize(int w, int h)
{
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleMaterials::onResume()
{
	// resume application
}

void SampleMaterials::onPause()
{
	// pause application
}

void SampleMaterials::onQuitApp()
{
	// end application
	delete this;
}