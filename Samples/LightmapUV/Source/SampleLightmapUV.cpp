#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleLightmapUV.h"

#include "GridPlane/CGridPlane.h"
#include "UnwrapUV/CUnwrapUV.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmapUV *app = new SampleLightmapUV();
	getApplication()->registerAppEvent("SampleLightmapUV", app);
}

SampleLightmapUV::SampleLightmapUV() :
	m_scene(NULL),
	m_camera(NULL)
{

}

SampleLightmapUV::~SampleLightmapUV()
{
	delete m_forwardRP;
	delete m_scene;
}

void SampleLightmapUV::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("LightmapUV.zip"), false, false);

	// Load basic shader
	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// 3D grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// Camera
	CGameObject *cameraObj = zone->createEmptyObject();
	cameraObj->addComponent<CCamera>();
	cameraObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = cameraObj->getComponent<CCamera>();
	m_camera->setProjectionType(CCamera::Perspective);
	m_camera->setPosition(core::vector3df(-10.0f, 5.0f, 10.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// 3D model
	CEntityPrefab *model = CMeshManager::getInstance()->loadModel("LightmapUV/gazebo.obj", "LightmapUV");
	if (model != NULL)
	{
		CGameObject *gazeboObj = zone->createEmptyObject();
		CRenderMesh *renderMesh = gazeboObj->addComponent<CRenderMesh>();
		renderMesh->initFromPrefab(model);

		// CMaterialManager::getInstance()->exportMaterial(model, "../Assets/LightmapUV/gazebo.xml");

		// Unwrap lightmap uv
		Lightmapper::CUnwrapUV unwrap;

		std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
		for (CRenderMeshData* renderData : renderers)
		{
			CMesh *mesh = renderData->getMesh();
			unwrap.addMesh(mesh);
		}

		unwrap.generate();
		unwrap.writeUVToImage();
	}

	// Render pipeline
	m_forwardRP = new CForwardRP();

	// Create 2D camera
	CGameObject *guiCameraObject = (CGameObject*)zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);
}

void SampleLightmapUV::onUpdate()
{
	// update application
	m_scene->update();
}

void SampleLightmapUV::onRender()
{
	// render 3D
	m_forwardRP->render(NULL,
		m_camera,
		m_scene->getZone(0)->getEntityManager(),
		core::recti());

	// render 2D
	CGraphics2D::getInstance()->render(m_guiCamera);
}

void SampleLightmapUV::onPostRender()
{
	// post render application
}

bool SampleLightmapUV::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleLightmapUV::onResume()
{
	// resume application
}

void SampleLightmapUV::onPause()
{
	// pause application
}

void SampleLightmapUV::onQuitApp()
{
	// end application
	delete this;
}