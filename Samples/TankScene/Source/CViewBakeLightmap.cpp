#include "pch.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"

CViewBakeLightmap::CViewBakeLightmap() :
	m_guiObject(NULL),
	m_bakeCameraObject(NULL),
	m_textInfo(NULL),
	m_font(NULL),
	m_currentMeshBuffer(0),
	m_currentVertex(0)
{

}

CViewBakeLightmap::~CViewBakeLightmap()
{
	// delete gui object
	m_guiObject->remove();

	// delete font
	delete m_font;
}

void CViewBakeLightmap::onInit()
{
	CContext *context = CContext::getInstance();
	CCamera *camera = context->getActiveCamera();

	CZone *zone = context->getActiveZone();

	// get all render mesh in zone
	m_renderMesh = zone->getComponentsInChild<CRenderMesh>(false);
	for (CRenderMesh *renderMesh : m_renderMesh)
	{
		if (renderMesh->getGameObject()->isStatic() == true)
		{
			// just list static object
			std::vector<CRenderMeshData*>& renderers = renderMesh->getRenderers();
			for (CRenderMeshData *r : renderers)
			{
				if (r->isSkinnedMesh() == false)
				{
					// just list static mesh
					CMesh *mesh = r->getMesh();
					u32 bufferCount = mesh->getMeshBufferCount();
					for (u32 i = 0; i < bufferCount; i++)
						m_allMeshBuffer.push_back(mesh->getMeshBuffer(i));
				}
			}
		}
	}

	// create bake camera
	m_bakeCameraObject = zone->createEmptyObject();
	m_bakeCameraObject->addComponent<CCamera>();

	// create gui object
	m_guiObject = zone->createEmptyObject();
	CCanvas *canvas = m_guiObject->addComponent<CCanvas>();

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
}

void CViewBakeLightmap::onDestroy()
{

}

void CViewBakeLightmap::onUpdate()
{
	CContext *context = CContext::getInstance();

	// update direction light
	context->updateDirectionLight();

	// update scene
	CScene *scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// bake lightmap
	u32 numMB = m_allMeshBuffer.size();
	if (m_currentMeshBuffer < numMB)
	{
		IMeshBuffer *mb = m_allMeshBuffer[m_currentMeshBuffer];
		u32 numVtx = mb->getVertexBuffer(0)->getVertexCount();

		char status[512];
		sprintf(status, "MeshBuffer: %d/%d\nVertex: %d/%d",
			m_currentMeshBuffer + 1, numMB,
			m_currentVertex, numVtx);
		m_textInfo->setText(status);

		// lightmaper bake meshbuffer
		CCamera *camera = m_bakeCameraObject->getComponent<CCamera>();

		// bake
		int bakeCount = CLightmapper::getInstance()->bakeMeshBuffer(
			mb,
			camera, context->getRenderPipeline(), scene->getEntityManager(),
			m_currentVertex, NUM_MTBAKER,
			m_color);

		m_currentVertex += bakeCount;

		// next mesh
		if (m_currentVertex >= numVtx || bakeCount == 0)
		{
			m_currentMeshBuffer++;
			m_currentVertex = 0;
		}
	}
	else
	{
		// next to demo scene
		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
}

void CViewBakeLightmap::onRender()
{
	CContext *context = CContext::getInstance();
	CCamera *guiCamera = context->getGUICamera();

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewBakeLightmap::onPostRender()
{

}
