#include "pch.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Importer/Utils/CMeshUtils.h"

CViewBakeLightmap::CViewBakeLightmap() :
	m_guiObject(NULL),
	m_bakeCameraObject(NULL),
	m_textInfo(NULL),
	m_font(NULL),
	m_currentMeshBuffer(0),
	m_currentVertex(0),
	m_totalVertexBaked(0),
	m_lightBounce(0),
	m_timeBeginBake(0)
{

}

CViewBakeLightmap::~CViewBakeLightmap()
{
	// delete gui object
	m_guiObject->remove();

	// delete font
	delete m_font;

	// delete baked color buffer
	for (u32 i = 0, n = m_colorBuffers.size(); i < n; i++)
		delete m_colorBuffers[i];
	m_colorBuffers.clear();
}

void CViewBakeLightmap::onInit()
{
	CContext* context = CContext::getInstance();
	CZone* zone = context->getActiveZone();
	CEntityManager* entityMgr = zone->getEntityManager();

	// set 32px for optimize speed
	CLightmapper::getInstance()->initBaker(32);

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
							m_neshBuffers.push_back(mb);
							m_meshTransforms.push_back(transform);

							int vtxCount = mb->getVertexBuffer(0)->getVertexCount();

							// alloc color buffer, that is result of indirect lighting baked
							SColorBuffer* cb = new SColorBuffer();
							cb->Color.set_used(vtxCount);
							cb->SH.set_used(vtxCount);

							for (int i = 0; i < vtxCount; i++)
							{
								cb->Color[i].set(255, 0, 0, 0);
								cb->SH[i].zero();
							}

							m_colorBuffers.push_back(cb);
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

	m_timeBeginBake = os::Timer::getRealTime();
}

void CViewBakeLightmap::onDestroy()
{
	CBaseRP::setBakeLightmapMode(false);
}

void CViewBakeLightmap::onUpdate()
{
	CContext* context = CContext::getInstance();

	// update scene
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// get total light bounce
	u32 numLightBounce = 1;
	if (CDirectionalLight::getCurrentDirectionLight() != NULL)
		numLightBounce = CDirectionalLight::getCurrentDirectionLight()->getBounce();

	// bake lightmap
	u32 numMB = (u32)m_neshBuffers.size();
	if (m_currentMeshBuffer < numMB && numLightBounce > 0)
	{
		if (m_lightBounce == 0)
			CDeferredRP::enableRenderIndirect(false);
		else
			CDeferredRP::enableRenderIndirect(true);

		CBaseRP::setBakeLightingMapBounce(m_lightBounce);
		CBaseRP::setBakeLightmapMode(true);

		IMeshBuffer* mb = m_neshBuffers[m_currentMeshBuffer];
		SColorBuffer* cb = m_colorBuffers[m_currentMeshBuffer];
		const core::matrix4& transform = m_meshTransforms[m_currentMeshBuffer];

		u32 numVtx = mb->getVertexBuffer(0)->getVertexCount();

		// compute wait percent
		float percentPerBuffer = 1.0f / (float)numMB;
		float percent = (percentPerBuffer * m_currentMeshBuffer + percentPerBuffer * (m_currentVertex / (float)numVtx)) * 100.0f;

		u32 deltaTime = os::Timer::getRealTime() - m_timeBeginBake;

		char status[512];
		sprintf(status, "LIGHTMAPPING (%d/%d): %d%%\n\n- MeshBuffer: %d/%d\n- Vertex: %d/%d\n\n - Total: %d\n-Time: %d seconds",
			m_lightBounce + 1,
			numLightBounce,
			(int)percent,
			m_currentMeshBuffer + 1, numMB,
			m_currentVertex, numVtx,
			m_totalVertexBaked,
			deltaTime / 1000);
		m_textInfo->setText(status);

		// lightmaper bake meshbuffer
		CCamera* camera = m_bakeCameraObject->getComponent<CCamera>();

		// bake light to color & sh
		int bakeCount = CLightmapper::getInstance()->bakeMeshBuffer(
			mb,
			transform,
			camera, context->getRenderPipeline(), scene->getEntityManager(),
			m_currentVertex, MAX_NUM_THREAD,
			cb->Color,
			cb->SH);

		m_currentVertex += bakeCount;
		m_totalVertexBaked += bakeCount;

		// next mesh
		if (m_currentVertex >= numVtx || bakeCount == 0)
		{
			m_currentMeshBuffer++;
			m_currentVertex = 0;
		}
	}
	else
	{
		copyColorBufferToMeshBuffer();

		CDeferredRP::enableRenderIndirect(true);
		// CDeferredRP::enableTestIndirect(true);

		m_lightBounce++;
		m_currentMeshBuffer = 0;
		m_currentVertex = 0;

		if (m_lightBounce >= (int)numLightBounce)
		{
			// test exporter
			if (m_renderMesh.size() > 0)
			{
				CMeshManager::getInstance()->exportModel(
					m_renderMesh[0]->getEntities().pointer(),
					m_renderMesh[0]->getEntities().size(),
					"../Assets/TankScene/TankScene.smesh");
			}

			CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
			return;
		}
	}
}

void CViewBakeLightmap::copyColorBufferToMeshBuffer()
{
	// copy baked color buffer to mesh buffer
	for (u32 i = 0, n = (u32)m_neshBuffers.size(); i < n; i++)
	{
		IMeshBuffer* mb = m_neshBuffers[i];
		SColorBuffer* cb = m_colorBuffers[i];

		if (mb->getVertexType() != EVT_TANGENTS || mb->getVertexBufferCount() == 0)
			continue;

		IVertexBuffer* vb = mb->getVertexBuffer(0);
		u32 vtxCount = vb->getVertexCount();

		// alloc new vtx buffer (because current vtx buffer is on GPU Memory, that can't change)
		CVertexBuffer<video::S3DVertexTangents>* vertexBuffer = new CVertexBuffer<video::S3DVertexTangents>();

		// copy vertex data
		CMeshUtils::copyVertices(vb, vertexBuffer);

		// copy baked color
		video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)vertexBuffer->getVertices();
		for (u32 i = 0; i < vtxCount; i++)
			vtx[i].Color = cb->Color[i];

		// notify update vertex to GPU Memory
		vertexBuffer->setHardwareMappingHint(EHM_STATIC);

		// replace current vertex buffer
		mb->setVertexBuffer(vertexBuffer, 0);
		mb->setDirty(EBT_VERTEX);

		// drop
		vertexBuffer->drop();
	}
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
