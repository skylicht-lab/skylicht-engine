#include "pch.h"
#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

CViewBakeLightmap::CViewBakeLightmap() :
#ifdef LIGHTMAP_SPONZA
	m_lightmapSize(1024),
#else
	m_lightmapSize(512),
#endif
	m_numRenderers(0),
	m_guiObject(NULL),
	m_font(NULL)
{

}

CViewBakeLightmap::~CViewBakeLightmap()
{
	if (m_guiObject != NULL)
		m_guiObject->remove();

	if (m_font != NULL)
		delete m_font;
}

void CViewBakeLightmap::onInit()
{
	gotoDemoView();

	/*
	CContext* context = CContext::getInstance();
	CZone* zone = context->getActiveZone();
	CEntityManager* entityMgr = zone->getEntityManager();

	// set default 128px for quality
	CLightmapper::getInstance()->initBaker(128);

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
	*/
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
	CContext* context = CContext::getInstance();
	CCamera* guiCamera = context->getGUICamera();

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
	// enable render indirect
	CDeferredRP::enableRenderIndirect(true);

	// switch to demo view
	CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
}