#include "pch.h"
#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "Lightmap/CLightmapData.h"
#include "Importer/Utils/CMeshUtils.h"

CViewBakeLightmap::CViewBakeLightmap() :
#ifdef LIGHTMAP_SPONZA
	m_lightmapSize(1024),
#else
	m_lightmapSize(512),
#endif
	m_numRenderers(0),
	m_numBakeTexture(0),
	m_currentMesh(0),
	m_guiObject(NULL),
	m_font(NULL)
{
	CBaseApp* app = getApplication();

	// direction
	m_shadowRP = new CShadowMapBakeRP();
	m_shadowRP->initRender(app->getWidth(), app->getHeight());

	m_bakeLightRP = new CDirectionalLightBakeRP();
	m_bakeLightRP->initRender(app->getWidth(), app->getHeight());
	m_shadowRP->setNextPipeLine(m_bakeLightRP);

	// point light
	m_shadowPLRP = new CPointLightShadowBakeRP();
	m_shadowPLRP->initRender(app->getWidth(), app->getHeight());

	m_bakePointLightRP = new CPointLightBakeRP();
	m_bakePointLightRP->initRender(app->getWidth(), app->getHeight());
	m_shadowPLRP->setNextPipeLine(m_bakePointLightRP);

	// texture
	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
	{
		m_directionLightBake[i] = NULL;
		m_pointLightBake[i] = NULL;
		m_subMesh[i] = NULL;
	}
}

CViewBakeLightmap::~CViewBakeLightmap()
{
	if (m_guiObject != NULL)
		m_guiObject->remove();

	if (m_font != NULL)
		delete m_font;

	delete m_shadowRP;
	delete m_bakeLightRP;
	delete m_shadowPLRP;
	delete m_bakePointLightRP;

	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
	{
		if (m_directionLightBake[i])
			getVideoDriver()->removeTexture(m_directionLightBake[i]);

		if (m_pointLightBake[i])
			getVideoDriver()->removeTexture(m_pointLightBake[i]);

		if (m_result[i])
			getVideoDriver()->removeTexture(m_result[i]);
	}
}

void CViewBakeLightmap::onInit()
{
	CContext* context = CContext::getInstance();
	CZone* zone = context->getActiveZone();
	CEntityManager* entityMgr = zone->getEntityManager();

	// force update and render to compute transform (1 frame)
	{
		context->getScene()->update();
		context->getRenderPipeline()->render(
			NULL,
			context->getActiveCamera(),
			context->getScene()->getEntityManager(),
			core::recti(0, 0, 0, 0));
	}

	// get all light
	m_lights = zone->getComponentsInChild<CLight>(false);

	// get all render mesh in zone
	m_renderMesh = zone->getComponentsInChild<CRenderMesh>(false);
	for (CRenderMesh* renderMesh : m_renderMesh)
	{
		if (renderMesh->getGameObject()->isStatic() == true)
		{
			// just list static object
			std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
			for (CRenderMeshData* r : renderers)
			{
				m_numRenderers++;

				if (r->isSkinnedMesh() == false)
				{
					core::matrix4 transform;
					CWorldTransformData* worldTransform = GET_ENTITY_DATA(r->Entity, CWorldTransformData);
					if (worldTransform != NULL)
						transform = worldTransform->World;

					// just list static mesh
					CMesh* mesh = r->getMesh();
					u32 bufferCount = mesh->getMeshBufferCount();
					for (u32 i = 0; i < bufferCount; i++)
					{
						IMeshBuffer* mb = mesh->getMeshBuffer(i);
						if (mb->getVertexBufferCount() > 0)
						{
							// add mesh buffer, that will bake lighting
							m_meshBuffers.push_back(mb);
							m_meshTransforms.push_back(transform);
						}
					}
				}
			}
		}
	}

	// create bake camera
	m_bakeCameraObject = zone->createEmptyObject();
	m_bakeCameraObject->addComponent<CCamera>();

	// create gui object
	m_guiObject = zone->createEmptyObject();
	CCanvas* canvas = m_guiObject->addComponent<CCanvas>();

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
}

void CViewBakeLightmap::onDestroy()
{
	CBaseRP::setBakeLightmapMode(false);
}

void CViewBakeLightmap::onUpdate()
{

}

void CViewBakeLightmap::onRender()
{
	// remove submesh
	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
	{
		if (m_subMesh[i])
		{
			m_subMesh[i]->drop();
			m_subMesh[i] = NULL;
		}
	}

	if (m_currentMesh >= m_meshBuffers.size())
	{
		// finish
		gotoDemoView();
		return;
	}

	CContext* context = CContext::getInstance();
	CZone* zone = context->getActiveZone();
	CEntityManager* entityMgr = zone->getEntityManager();

	CCamera* guiCamera = context->getGUICamera();
	CCamera* bakeCamera = m_bakeCameraObject->getComponent<CCamera>();

	// lightmap texture
	IMeshBuffer* mb = m_meshBuffers[m_currentMesh];

	IVertexBuffer* srcVtx = mb->getVertexBuffer();
	IIndexBuffer* srcIdx = mb->getIndexBuffer();

	S3DVertex2TCoordsTangents* vertices = (S3DVertex2TCoordsTangents*)srcVtx->getVertices();
	void* indices = srcIdx->getIndices();

	E_INDEX_TYPE type = srcIdx->getType();

	// Collect the lightmap index to many submesh
	int count = srcIdx->getIndexCount();
	for (int i = 0; i < count; i += 3)
	{
		u32 vertexId0 = 0;
		u32 vertexId1 = 0;
		u32 vertexId2 = 0;

		if (type == EIT_16BIT)
		{
			u16* data = (u16*)indices;
			vertexId0 = (u32)data[i];
			vertexId1 = (u32)data[i + 1];
			vertexId2 = (u32)data[i + 2];
		}
		else
		{
			u32* data = (u32*)indices;
			vertexId0 = data[i];
			vertexId1 = data[i + 1];
			vertexId2 = data[i + 2];
		}

		// query triangle lightmap index
		int lightmapIndex = (int)vertices[vertexId0].Lightmap.Z;
		if (lightmapIndex < 0 || lightmapIndex > 32)
			continue;

		if (m_subMesh[lightmapIndex] == NULL)
		{
			m_subMesh[lightmapIndex] = new CMeshBuffer<S3DVertex2TCoordsTangents>(getVideoDriver()->getVertexDescriptor(EVT_2TCOORDS_TANGENTS), type);
			CMeshUtils::copyVertices(srcVtx, m_subMesh[lightmapIndex]->getVertexBuffer());
		}

		IIndexBuffer* idx = m_subMesh[lightmapIndex]->getIndexBuffer();
		idx->addIndex(vertexId0);
		idx->addIndex(vertexId1);
		idx->addIndex(vertexId2);

		// calc max textures
		if (lightmapIndex + 1 > m_numBakeTexture)
			m_numBakeTexture = lightmapIndex + 1;
	}

	// init render target
	for (int i = 0; i < m_numBakeTexture; i++)
	{
		if (m_directionLightBake[i] == NULL)
		{
			core::dimension2du size((u32)m_lightmapSize, (u32)m_lightmapSize);

			m_directionLightBake[i] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);
			getVideoDriver()->setRenderTarget(m_directionLightBake[i], true, true, SColor(0, 0, 0, 0));

			m_pointLightBake[i] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);
			getVideoDriver()->setRenderTarget(m_pointLightBake[i], true, true, SColor(0, 0, 0, 0));
		}
	}

	// calc bbox of mesh
	const core::matrix4& transform = m_meshTransforms[m_currentMesh];
	core::aabbox3df box = mb->getBoundingBox();
	transform.transformBoxEx(box);

	// set box & mesh that render light
	m_shadowRP->setBound(box);

	// setup target for pipeline
	m_bakeLightRP->setRenderMesh(mb, m_subMesh, m_directionLightBake, m_numBakeTexture);
	m_bakePointLightRP->setRenderMesh(mb, m_subMesh, m_pointLightBake, m_numBakeTexture);

	// set camera view
	core::vector3df center = box.getCenter();
	core::vector3df v = box.MaxEdge - center;

	// set camera, that make sure the meshbuffer always render, that is view
	bakeCamera->setPosition(center + v * 1.5f);
	bakeCamera->lookAt(center, core::vector3df(0.0f, 1.0f, 0.0f));

	// update scene first
	context->getScene()->update();

	// bake direction light
	m_shadowRP->render(NULL, bakeCamera, entityMgr, core::recti());

	// bake all point light
	for (CLight* light : m_lights)
	{
		m_shadowPLRP->setCurrentLight(light);
		m_shadowPLRP->render(NULL, bakeCamera, entityMgr, core::recti());
	}

	// next mesh
	m_currentMesh++;

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewBakeLightmap::onPostRender()
{

}

void CViewBakeLightmap::gotoDemoView()
{
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->loadShader("BuiltIn/Shader/BakeDirectional/BakeFinal.xml");

	for (int i = 0; i < m_numBakeTexture; i++)
	{
		// bake final, that merge direction light & point light & shadow mask in alpha channel
		core::dimension2du size((u32)m_lightmapSize, (u32)m_lightmapSize);
		float lmSize = (float)m_lightmapSize;

		m_result[i] = getVideoDriver()->addRenderTargetTexture(size, "resultLM", video::ECF_A8R8G8B8);

		getVideoDriver()->setRenderTarget(m_result[i]);
		m_bakeLightRP->beginRender2D(lmSize, lmSize);
		SMaterial material;
		material.setTexture(0, m_directionLightBake[i]);
		material.setTexture(1, m_pointLightBake[i]);
		material.MaterialType = shaderMgr->getShaderIDByName("BakeFinal");
		m_bakeLightRP->renderBufferToTarget(0.0f, 0.0f, lmSize, lmSize, material, false);

		if (m_directionLightBake[i])
		{
			char outFileName[512];
			sprintf(outFileName, "LightMapDirectional_%d.png", i);
			CBaseRP::saveFBOToFile(m_directionLightBake[i], outFileName);
		}

		if (m_pointLightBake[i])
		{
			char outFileName[512];
			sprintf(outFileName, "LightMapPLight_%d.png", i);
			CBaseRP::saveFBOToFile(m_pointLightBake[i], outFileName);
		}

		getVideoDriver()->setRenderTarget(NULL);

		// final texture
		{
			char outFileName[512];
			sprintf(outFileName, "LightMap_%d.png", i);
			CBaseRP::saveFBOToFile(m_result[i], outFileName);
		}
	}

	// enable render indirect
	CDeferredRP::enableRenderIndirect(true);

	/*
	std::vector<std::string> listTextures;

	for (int i = 0; i < m_numBakeTexture; i++)
	{
		char lightmapName[512];
		sprintf(lightmapName, "LightMapDirectional_%d.png", i);
		listTextures.push_back(lightmapName);
	}

	ITexture* lightmapTexture = CTextureManager::getInstance()->getTextureArray(listTextures);
	if (lightmapTexture != NULL)
	{
		// bind lightmap texture as indirect lighting
		for (CRenderMesh* renderMesh : m_renderMesh)
		{
			if (renderMesh->getGameObject()->isStatic() == true)
			{

			}
		}
	}
	*/

	// switch to demo view
	CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
}