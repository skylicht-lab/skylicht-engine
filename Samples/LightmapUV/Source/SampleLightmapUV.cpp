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
	m_camera(NULL),
	m_UVChartsTexture(NULL)
{

}

SampleLightmapUV::~SampleLightmapUV()
{
	getVideoDriver()->removeTexture(m_UVChartsTexture);

	delete m_forwardRP;
	delete m_scene;
}

void SampleLightmapUV::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("SampleModels.zip"), false, false);
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("Sponza.zip"), false, false);

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

	// Direction lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// 3D model
	// CEntityPrefab *model = CMeshManager::getInstance()->loadModel("SampleModels/Gazebo/gazebo.obj", "");
	CEntityPrefab *model = CMeshManager::getInstance()->loadModel("Sponza/Sponza.dae", "Sponza/Textures");

	scene::IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();

	if (model != NULL)
	{
		CGameObject *renderObj = zone->createEmptyObject();
		CRenderMesh *renderMesh = renderObj->addComponent<CRenderMesh>();
		renderMesh->initFromPrefab(model);

		// Get list default material
		ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(model);

		// Unwrap lightmap uv		
		Lightmapper::CUnwrapUV unwrap;

		std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
		std::vector<CWorldTransformData*>& transforms = renderMesh->getRenderTransforms();
		
		// hack for sponza mesh
		int meshID = 0;
		for (CRenderMeshData* renderData : renderers)
		{
			std::string name = transforms[meshID]->Name;
			if (name == "celling_sponza")
			{
				unwrap.addMesh(renderData->getMesh(), 0.02f);
			}
			else if (name == "floor_sponza")
			{
				unwrap.addMesh(renderData->getMesh(), 0.02f);
			}				
			else if (name == "wall_sponza")
			{
				unwrap.addMesh(renderData->getMesh(), 0.02f);
			}			
			else if (name == "smallwall_sponza")
			{
				unwrap.addMesh(renderData->getMesh(), 0.01f);
			}
			else if (name == "top_sponza")
			{
				unwrap.addMesh(renderData->getMesh(), 0.01f);
			}
			// if (name == "object_sponza")
			else
			{
				unwrap.addMesh(renderData->getMesh(), 1.0f);
			}						

			meshID++;
		}

		// for (CRenderMeshData* renderData : renderers)		
		//	unwrap.addMesh(renderData->getMesh(), 1.0f);

		unwrap.generate(4096, 2.0f);
		unwrap.generateUVImage();

		// Write to bin folder output layout uv
		char name[512];
		sprintf(name, "mesh");
		unwrap.writeUVToImage(name);

		// Update lightmap uv to renderer
		for (CRenderMeshData* renderData : renderers)
		{
			CMesh *mesh = renderData->getMesh();
			for (u32 i = 0; i < mesh->getMeshBufferCount(); i++)
			{
				IMeshBuffer *mb = mesh->getMeshBuffer(i);
				
				IVertexBuffer *vb = mb->getVertexBuffer(0);
				IVertexDescriptor *vd = mb->getVertexDescriptor();

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
				unwrap.writeUVToMeshBuffer(mb, mb, Lightmapper::CUnwrapUV::LIGHTMAP);

				// notify change
				mb->setDirty(scene::EBT_VERTEX);

				// drop
				vb->drop();
				vertexBuffer->drop();
			}
		}

		// test exporter
		CMeshManager::getInstance()->exportModel(
			renderMesh->getEntities().pointer(),
			renderMesh->getEntities().size(),
			"../Assets/Sponza/Sponza.smesh");
		
		// Update material
		/*
		for (int i = 0, n = unwrap.getAtlasCount(); i < n; i++)
		{
			IImage *img = unwrap.getChartsImage(i);
			m_UVChartsTexture = getVideoDriver()->addTexture("ChartsTexture", img);
		}

		for (CMaterial *m : materials)
		{
			m->changeShader("BuiltIn/Shader/Basic/TextureColor.xml");
			m->setTexture(&m_UVChartsTexture, 1);
		}

		renderMesh->initMaterial(materials);
		*/
	}

	// Render pipeline
	m_forwardRP = new CForwardRP();

	// Create 2D camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
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