#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

u32 CViewBakeLightmap::s_numLightBounce = 2;

CViewBakeLightmap::CViewBakeLightmap() :
	m_currentPass(0),
	m_currentMB(0),
	m_currentTris(0),
	m_lastTris(99999),
	m_bakeCameraObject(NULL),
	m_lightBounce(0)
{
	m_lmRasterize = new Lightmapper::CRasterisation(512, 512);
}

CViewBakeLightmap::~CViewBakeLightmap()
{
	m_guiObject->remove();

	delete m_font;
	delete m_lmRasterize;
}

void CViewBakeLightmap::onInit()
{
	CContext *context = CContext::getInstance();
	CZone *zone = context->getActiveZone();
	CEntityManager *entityMgr = zone->getEntityManager();

	// set default 128px for quality
	CLightmapper::getInstance()->initBaker(128);

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
					core::matrix4 transform;
					CEntity *entity = entityMgr->getEntity(r->EntityIndex);
					CWorldTransformData *worldTransform = entity->getData<CWorldTransformData>();
					if (worldTransform != NULL)
						transform = worldTransform->World;

					// just list static mesh
					CMesh *mesh = r->getMesh();
					u32 bufferCount = mesh->getMeshBufferCount();
					for (u32 i = 0; i < bufferCount; i++)
					{
						IMeshBuffer *mb = mesh->getMeshBuffer(i);
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
	CCanvas *canvas = m_guiObject->addComponent<CCanvas>();

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(CGUIElement::Center, CGUIElement::Middle);

	m_timeBeginBake = os::Timer::getRealTime();
}

void CViewBakeLightmap::onDestroy()
{

}

void CViewBakeLightmap::onUpdate()
{
	u32 deltaTime = os::Timer::getRealTime() - m_timeBeginBake;

	if (m_lightBounce == 0)
		CDeferredRP::enableRenderIndirect(false);
	else
		CDeferredRP::enableRenderIndirect(true);

	for (int loopCount = 0; loopCount < 64; loopCount++)
	{
		if (m_currentMB < m_meshBuffers.size())
		{
			IMeshBuffer *mb = m_meshBuffers[m_currentMB];
			const core::matrix4& transform = m_meshTransforms[m_currentMB];

			IIndexBuffer *idx = mb->getIndexBuffer();
			u32 numTris = idx->getIndexCount() / 3;

			char status[512];
			sprintf(status, "LIGHTMAPPING (%d/%d):\n\n- Bake step: %d/%d\n- Triangle: %d/%d\n- Time: %d seconds",
				m_lightBounce + 1, s_numLightBounce,
				m_currentPass + 1, 7,
				m_currentTris, numTris,
				deltaTime / 1000);
			m_textInfo->setText(status);

			CRasterisation::ERasterPass pass = (CRasterisation::ERasterPass)m_currentPass;

			if (m_lastTris != m_currentTris)
			{
				u32 v1 = 0;
				u32 v2 = 0;
				u32 v3 = 0;

				if (idx->getIndexSize() == 2)
				{
					u16* indexbuffer = (u16*)idx->getIndices();
					v1 = (u32)indexbuffer[m_currentTris * 3];
					v2 = (u32)indexbuffer[m_currentTris * 3 + 1];
					v3 = (u32)indexbuffer[m_currentTris * 3 + 2];
				}
				else if (idx->getIndexSize() == 4)
				{
					u32* indexbuffer = (u32*)idx->getIndices();
					v1 = (u32)indexbuffer[m_currentTris * 3];
					v2 = (u32)indexbuffer[m_currentTris * 3 + 1];
					v3 = (u32)indexbuffer[m_currentTris * 3 + 2];
				}

				IVertexBuffer *vtx = mb->getVertexBuffer();

				S3DVertexTangents *vertices = (S3DVertexTangents*)vtx->getVertices();

				core::vector3df positions[] = {
					vertices[v1].Pos,
					vertices[v2].Pos,
					vertices[v3].Pos
				};

				core::vector2df uvs[] = {
					vertices[v1].TCoords,
					vertices[v2].TCoords,
					vertices[v3].TCoords
				};

				core::vector3df normals[] = {
					vertices[v1].Normal,
					vertices[v2].Normal,
					vertices[v3].Normal
				};

				core::vector3df tangents[] = {
					vertices[v1].Tangent,
					vertices[v2].Tangent,
					vertices[v3].Tangent
				};

				m_pixel = m_lmRasterize->setTriangle(positions, uvs, normals, tangents, pass);
				m_lastTris = m_currentTris;
			}

			core::vector3df outPos;
			core::vector3df outNormal;
			core::vector3df outTangent;
			core::vector3df outBinormal;

			bool forceBake = false;

			// sampling pixel
			m_lmRasterize->samplingTrianglePosition(outPos, outNormal, outTangent, outBinormal, m_pixel);

			// next pixel
			m_lmRasterize->moveNextPixel(m_pixel);

			// finish
			if (m_lmRasterize->isFinished(m_pixel))
				m_currentTris++;

			// go next buffer
			if (m_currentTris >= numTris)
			{
				m_currentMB++;

				m_currentTris = 0;
				m_lastTris = 9999;

				forceBake = true;
			}

			// bake indirect pixels
			core::array<SBakePixel>& listPixels = m_lmRasterize->getBakePixelQueue();
			if (listPixels.size() == CLightmapper::getNumThread() || forceBake == true)
			{
				int n = (int)listPixels.size();
				m_out.clear();

				for (int i = 0; i < n; i++)
				{
					m_bakePositions[i] = listPixels[i].Position;
					m_bakeNormals[i] = listPixels[i].Normal;
					m_bakeTangents[i] = listPixels[i].Tangent;
					m_bakeBinormals[i] = listPixels[i].Binormal;
				}

				CContext *context = CContext::getInstance();
				CScene *scene = context->getScene();

				CLightmapper::getInstance()->bakeAtPosition(
					m_bakeCameraObject->getComponent<CCamera>(),
					context->getRenderPipeline(),
					scene->getEntityManager(),
					m_bakePositions,
					m_bakeNormals,
					m_bakeTangents,
					m_bakeBinormals,
					m_out,
					n,
					5
				);

				// bake
				m_lmRasterize->flushPixel(m_out);
			}
		}
		else
		{
			// next pass
			m_currentPass++;

			m_currentMB = 0;
			m_currentTris = 0;
			m_lastTris = 9999;

			if (m_currentPass >= (int)CRasterisation::PassCount)
			{
				m_lightBounce++;

				// todo fix seam
				m_lmRasterize->fixSeamPixel();

				IVideoDriver *driver = getVideoDriver();

				unsigned char *data = m_lmRasterize->getLightmapData();

				core::dimension2du size(m_lmRasterize->getWidth(), m_lmRasterize->getHeight());
				IImage *img = driver->createImageFromData(video::ECF_R8G8B8, size, data);

				ITexture *lightmapTexture = driver->addTexture("lightmap_bake", img);
				if (lightmapTexture != NULL)
				{
					for (CRenderMesh *renderMesh : m_renderMesh)
					{
						if (renderMesh->getGameObject()->isStatic() == true)
						{
							CIndirectLighting *indirect = renderMesh->getGameObject()->getComponent<CIndirectLighting>();
							if (indirect == NULL)
								indirect = renderMesh->getGameObject()->addComponent<CIndirectLighting>();

							indirect->setLightmap(lightmapTexture);
							indirect->setIndirectLightingType(CIndirectLighting::Lightmap);
						}
					}
				}

				bool testWriteFile = true;

				if (testWriteFile)
				{
					char outFileName[512];
					sprintf(outFileName, "LightMapRasterize_%d.png", m_lightBounce);
					driver->writeImageToFile(img, outFileName);
				}

				img->drop();

				if (testWriteFile)
				{
					data = m_lmRasterize->getTestBakeImage();
					img = driver->createImageFromData(video::ECF_R8G8B8, size, data);

					char outFileName[512];
					sprintf(outFileName, "LightMapRasterize_debug_%d.png", m_lightBounce);
					driver->writeImageToFile(img, outFileName);
					img->drop();
				}

				m_lmRasterize->resetBake();

				if (m_lightBounce >= s_numLightBounce)
				{
					CDeferredRP::enableRenderIndirect(true);

					CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
				}
				else
				{
					// reset next light bounce
					m_currentPass = 0;
					m_currentMB = 0;
					m_currentTris = 0;
					m_lastTris = 9999;
				}
			}

			return;
		}
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