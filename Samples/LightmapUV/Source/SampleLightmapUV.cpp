#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleLightmapUV.h"

#include "GridPlane/CGridPlane.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleLightmapUV* app = new SampleLightmapUV();
	getApplication()->registerAppEvent("SampleLightmapUV", app);
}

SampleLightmapUV::SampleLightmapUV() :
	m_scene(NULL),
	m_camera(NULL),
	m_model(NULL),
	m_renderMesh(NULL),
	m_threadFinish(false),
	m_initMeshUV(false),
	m_thread(NULL),
	m_UVChartsTexture(NULL),
	m_font(NULL),
	m_guiObject(NULL),
	m_textInfo(NULL)
{

}

SampleLightmapUV::~SampleLightmapUV()
{
	if (m_thread != NULL)
		delete m_thread;

	getVideoDriver()->removeTexture(m_UVChartsTexture);

	delete m_font;
	delete m_forwardRP;
	delete m_scene;
}

void SampleLightmapUV::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Show console
	app->showDebugConsole();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SampleModels.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SampleModelsResource.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath(app->getTexturePackageName("SampleModels").c_str()), false, false);

	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Sponza.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SponzaResource.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath(app->getTexturePackageName("Sponza").c_str()), false, false);

	// Load Font
	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// Load basic shader
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone* zone = m_scene->createZone();

	// 3D grid
	CGameObject* grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// Camera
	CGameObject* cameraObj = zone->createEmptyObject();
	cameraObj->addComponent<CCamera>();
	cameraObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = cameraObj->getComponent<CCamera>();
	m_camera->setProjectionType(CCamera::Perspective);
	m_camera->setPosition(core::vector3df(-10.0f, 5.0f, 10.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// Direction lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, Transform::Oy);

	// 3D model	
#ifdef LIGHTMAP_SPONZA
	m_model = CMeshManager::getInstance()->loadModel("Sponza/Sponza.dae", "Sponza/Textures");
#else
	m_model = CMeshManager::getInstance()->loadModel("SampleModels/Gazebo/gazebo.obj", "");
#endif

	if (m_model != NULL)
	{
		CGameObject* renderObj = zone->createEmptyObject();

		m_renderMesh = renderObj->addComponent<CRenderMesh>();
		m_renderMesh->initFromPrefab(m_model);

		std::vector<CRenderMeshData*>& renderers = m_renderMesh->getRenderers();
		std::vector<CWorldTransformData*>& transforms = m_renderMesh->getRenderTransforms();

		// hack for sponza mesh
		int meshID = 0;
		for (CRenderMeshData* renderData : renderers)
		{
#ifdef LIGHTMAP_SPONZA
			std::string name = transforms[meshID]->Name;
			if (name == "celling_sponza")
			{
				m_unwrap.addMesh(renderData->getMesh(), 0.5f);
			}
			else if (name == "floor_sponza")
			{
				m_unwrap.addMesh(renderData->getMesh(), 0.2f);
			}
			else if (name == "wall_sponza")
			{
				m_unwrap.addMesh(renderData->getMesh(), 0.2f);
			}
			else if (name == "smallwall_sponza")
			{
				m_unwrap.addMesh(renderData->getMesh(), 0.5f);
			}
			else if (name == "top_sponza")
			{
				m_unwrap.addMesh(renderData->getMesh(), 0.1f);
			}
			else
			{
				// default mesh
				m_unwrap.addMesh(renderData->getMesh(), 1.0f);
			}
#else
			m_unwrap.addMesh(renderData->getMesh(), 1.0f);
#endif
			meshID++;
		}

		// save list renderer
		m_allRenderers = renderers;

		// run thread
		m_thread = Skylicht::System::IThread::createThread(this);
	}

	// Render pipeline
	m_forwardRP = new CForwardRP(false);
	m_forwardRP->initRender(app->getWidth(), app->getHeight());

	// Create 2D camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// create gui object
	m_guiObject = zone->createEmptyObject();
	CCanvas* canvas = m_guiObject->addComponent<CCanvas>();

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
}

void SampleLightmapUV::runThread()
{
	// generate uv in thread
#ifdef LIGHTMAP_SPONZA
	m_unwrap.generate(1024, 0.3f);
#else
	// auto texel in 512
	m_unwrap.generate(512, 0.0f);
#endif

	m_unwrap.generateUVImage();

	// write to bin folder output layout uv
	char name[512];
	sprintf(name, "mesh");
	m_unwrap.writeUVToImage(name);

	// call init mesh uv
	m_initMeshUV = true;
	m_threadFinish = true;
}

void SampleLightmapUV::updateMeshUV()
{
	scene::IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();

	// Update lightmap uv to renderer
	for (CRenderMeshData* renderData : m_allRenderers)
	{
		CMesh* mesh = renderData->getMesh();
		for (u32 i = 0; i < mesh->getMeshBufferCount(); i++)
		{
			IMeshBuffer* mb = mesh->getMeshBuffer(i);

			IVertexBuffer* vb = mb->getVertexBuffer(0);
			IVertexDescriptor* vd = mb->getVertexDescriptor();

			// alloc new vtx buffer (because current vtx buffer is on GPU Memory, that can't change)
			CVertexBuffer<video::S3DVertex2TCoordsTangents>* vertexBuffer = new CVertexBuffer<video::S3DVertex2TCoordsTangents>();

			// copy vertex data
			mh->copyVertices(
				vb,
				0,
				getVideoDriver()->getVertexDescriptor(EVT_TANGENTS),
				vertexBuffer,
				0,
				getVideoDriver()->getVertexDescriptor(EVT_2TCOORDS_TANGENTS),
				false
			);

			// notify update vertex to GPU Memory
			vertexBuffer->setHardwareMappingHint(EHM_STATIC);

			// hold before replace vb
			vb->grab();

			// replace current vertex buffer
			mb->setVertexBuffer(vertexBuffer, 0);
			mb->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(EVT_2TCOORDS_TANGENTS));

			// write uv lightmap
			m_unwrap.writeUVToMeshBuffer(mb, mb, Lightmapper::CUnwrapUV::LIGHTMAP);

			// notify change
			mb->setDirty(scene::EBT_VERTEX);

			// drop
			vb->drop();
			vertexBuffer->drop();
		}
	}

	// Exporter result
#ifdef LIGHTMAP_SPONZA
	CMeshManager::getInstance()->exportModel(
		m_renderMesh->getEntities().pointer(),
		m_renderMesh->getEntities().size(),
		"../Assets/Sponza/Sponza.smesh");
#else	
	CMeshManager::getInstance()->exportModel(
		m_renderMesh->getEntities().pointer(),
		m_renderMesh->getEntities().size(),
		"../Assets/SampleModels/Gazebo/gazebo.smesh");
#endif

	// Update material
	core::array<IImage*> arrayTexture;
	for (int i = 0, n = m_unwrap.getAtlasCount(); i < n; i++)
	{
		IImage* img = m_unwrap.getChartsImage(i);
		arrayTexture.push_back(img);
	}

	// Texture array
	m_UVChartsTexture = getVideoDriver()->getTextureArray(arrayTexture.pointer(), arrayTexture.size());

	// Get list default material
	ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(m_model);

	for (CMaterial* m : materials)
	{
		m->changeShader("BuiltIn/Shader/Lightmap/LightmapUV.xml");
		m->setUniformTexture("uTexLightmap", m_UVChartsTexture);
	}

	m_renderMesh->initMaterial(materials);
}

void SampleLightmapUV::updateThread()
{

}

void SampleLightmapUV::onUpdate()
{
	// this gpu task should in main thread
	if (m_initMeshUV == true)
	{
		updateMeshUV();
		m_initMeshUV = false;
	}

	// update application
	m_scene->update();
}

void SampleLightmapUV::onRender()
{
	// render 3D
	if (m_threadFinish == true)
	{
		m_forwardRP->render(NULL,
			m_camera,
			m_scene->getZone(0)->getEntityManager(),
			core::recti());

		m_guiObject->setVisible(false);
	}
	else
	{
		m_textInfo->setText(m_unwrap.getLog());
	}

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

void SampleLightmapUV::onResize(int w, int h)
{
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
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
