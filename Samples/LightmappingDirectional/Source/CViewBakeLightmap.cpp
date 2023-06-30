#include "pch.h"
#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "Lightmap/CLightmapData.h"

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

	m_shadowRP = new CShadowMapBakeRP();
	m_shadowRP->initRender(app->getWidth(), app->getHeight());

	m_bakeLightRP = new CDirectionalLightBakeRP();
	m_bakeLightRP->initRender(app->getWidth(), app->getHeight());

	m_shadowRP->setNextPipeLine(m_bakeLightRP);

	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
	{
		m_bakeTexture[i] = NULL;
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

	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
	{
		if (m_bakeTexture[i])
			getVideoDriver()->removeTexture(m_bakeTexture[i]);
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
					CEntity* entity = entityMgr->getEntity(r->EntityIndex);
					CWorldTransformData* worldTransform = entity->getData<CWorldTransformData>();
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
	S3DVertex2TCoordsTangents* vertices = (S3DVertex2TCoordsTangents*)mb->getVertexBuffer()->getVertices();
	int lightmapIndex = (int)vertices[0].Lightmap.Z;

	// calc max textures
	if (lightmapIndex + 1 > m_numBakeTexture)
		m_numBakeTexture = lightmapIndex + 1;

	if (m_bakeTexture[lightmapIndex] == NULL)
	{
		core::dimension2du size((u32)m_lightmapSize, (u32)m_lightmapSize);
		m_bakeTexture[lightmapIndex] = getVideoDriver()->addRenderTargetTexture(size, "bakeLM", video::ECF_A8R8G8B8);

		// clear black
		getVideoDriver()->setRenderTarget(m_bakeTexture[lightmapIndex], true, true, SColor(0, 0, 0, 0));
	}

	// calc bbox of mesh
	const core::matrix4& transform = m_meshTransforms[m_currentMesh];
	core::aabbox3df box = mb->getBoundingBox();
	transform.transformBoxEx(box);

	// set box & mesh that render light
	m_shadowRP->setBound(box);
	m_bakeLightRP->setRenderMesh(mb);

	// set camera view
	core::vector3df center = box.getCenter();
	core::vector3df v = box.MaxEdge - center;

	// set camera, that make sure the meshbuffer always render, that is view
	bakeCamera->setPosition(center + v * 1.5f);
	bakeCamera->lookAt(center, core::vector3df(0.0f, 1.0f, 0.0f));

	// update scene first
	context->getScene()->update();

	// render uv
	m_shadowRP->render(m_bakeTexture[lightmapIndex], bakeCamera, entityMgr, core::recti());

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
	for (int i = 0; i < m_numBakeTexture; i++)
	{
		if (m_bakeTexture[i])
		{
			char outFileName[512];
			sprintf(outFileName, "LightMapDirectional_%d.png", i);
			CBaseRP::saveFBOToFile(m_bakeTexture[i], outFileName);
		}
	}

	// enable render indirect
	CDeferredRP::enableRenderIndirect(true);

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

	// switch to demo view
	CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
}