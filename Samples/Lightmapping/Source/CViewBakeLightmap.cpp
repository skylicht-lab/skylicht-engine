#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

CViewBakeLightmap::CViewBakeLightmap() :
	m_currentPass(0),
	m_currentMB(0),
	m_currentTris(0),
	m_lastTris(99999),
	m_bakeCameraObject(NULL),
	m_lightBounce(0),
	m_numberRasterize(0),
	m_currentRasterisation(NULL),
#ifdef LIGHTMAP_SPONZA
	m_lightmapSize(1024),
#else
	m_lightmapSize(512),
#endif
	m_timeBeginBake(0),
	m_timeSpentFromLastSave(0),
	m_numRenderers(0),
	m_numIndices(0),
	m_numVertices(0),
	m_guiObject(NULL),
	m_font(NULL)
{
	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
		m_lmRasterize[i] = NULL;
}

CViewBakeLightmap::~CViewBakeLightmap()
{
	if (m_guiObject != NULL)
		m_guiObject->remove();

	if (m_font != NULL)
		delete m_font;

	for (int i = 0; i < MAX_LIGHTMAP_ATLAS; i++)
	{
		if (m_lmRasterize[i] != NULL)
		{
			delete m_lmRasterize[i];
			m_lmRasterize[i] = NULL;
		}
	}
}

Lightmapper::CRasterisation* CViewBakeLightmap::createGetLightmapRasterisation(int index)
{
	if (m_lmRasterize[index] != NULL)
		return m_lmRasterize[index];

	m_lmRasterize[index] = new Lightmapper::CRasterisation(m_lightmapSize, m_lightmapSize);

	if (index + 1 > m_numberRasterize)
		m_numberRasterize = index + 1;

	return m_lmRasterize[index];
}

int CViewBakeLightmap::getRasterisationIndex(Lightmapper::CRasterisation* raster)
{
	for (int i = 0; i < m_numberRasterize; i++)
	{
		if (m_lmRasterize[i] == raster)
			return i;
	}

	return -1;
}

void CViewBakeLightmap::onInit()
{
	// gotoDemoView();
	// return;

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

							// compute for verify load/save progress							
							m_numIndices += mb->getIndexBuffer()->getIndexCount();
							m_numVertices += mb->getVertexBuffer()->getVertexCount();
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

	// load last progress to continue
	loadProgress();
}

void CViewBakeLightmap::onDestroy()
{
	// save progress to continue next time
	saveProgress();

	CBaseRP::setBakeLightmapMode(false);
}

void CViewBakeLightmap::onUpdate()
{
	u32 deltaTime = (os::Timer::getRealTime() - m_timeBeginBake) + m_timeSpentFromLastSave;

	if (m_lightBounce == 0)
		CDeferredRP::enableRenderIndirect(false);
	else
		CDeferredRP::enableRenderIndirect(true);

	CBaseRP::setBakeLightingMapBounce(m_lightBounce);
	CBaseRP::setBakeLightmapMode(true);

	u32 numLightBounce = 1;

	if (CDirectionalLight::getCurrentDirectionLight() != NULL)
		numLightBounce = CDirectionalLight::getCurrentDirectionLight()->getBounce();

	for (int loopCount = 0; loopCount < 64; loopCount++)
	{
		if (m_currentMB < m_meshBuffers.size())
		{
			IMeshBuffer* mb = m_meshBuffers[m_currentMB];
			const core::matrix4& transform = m_meshTransforms[m_currentMB];

			IIndexBuffer* idx = mb->getIndexBuffer();
			u32 numTris = idx->getIndexCount() / 3;

			int secs = deltaTime / 1000;
			int mins = secs / 60;
			int hours = mins / 60;

			mins = mins - hours * 60;

			char status[512];
			sprintf(status, "LIGHTMAPPING (%d/%d):\n\n- MeshBuffer: %d/%d\n- Bake step: %d/%d\n- Triangle: %d/%d\n- Time: %d seconds - (%02d:%02d) hm",
				m_lightBounce + 1, numLightBounce,
				m_currentMB + 1, (int)m_meshBuffers.size(),
				m_currentPass + 1, 7,
				m_currentTris, numTris,
				secs,
				hours, mins);
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

				IVertexBuffer* vtx = mb->getVertexBuffer();

				S3DVertex2TCoordsTangents* vertices = (S3DVertex2TCoordsTangents*)vtx->getVertices();

				core::vector3df positions[] = {
					vertices[v1].Pos,
					vertices[v2].Pos,
					vertices[v3].Pos
				};

				core::vector2df uvs[] = {
					core::vector2df(vertices[v1].Lightmap.X, vertices[v1].Lightmap.Y),
					core::vector2df(vertices[v2].Lightmap.X, vertices[v2].Lightmap.Y),
					core::vector2df(vertices[v3].Lightmap.X, vertices[v3].Lightmap.Y),
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

				for (int i = 0; i < 3; i++)
				{
					transform.transformVect(positions[i]);
					transform.rotateVect(normals[i]);
					transform.rotateVect(tangents[i]);

					normals[i].normalize();
					tangents[i].normalize();
				}

				int lmIndex = (int)vertices[v1].Lightmap.Z;
				if (lmIndex >= 0)
				{
					m_currentRasterisation = createGetLightmapRasterisation(lmIndex);
					m_pixel = m_currentRasterisation->setTriangle(positions, uvs, normals, tangents, pass);
					m_lastTris = m_currentTris;
				}
				else
				{
					// skip this triangle
					m_currentTris++;
					continue;
				}
			}

			core::vector3df outPos;
			core::vector3df outNormal;
			core::vector3df outTangent;
			core::vector3df outBinormal;

			bool forceBake = false;

			// sampling pixel
			m_currentRasterisation->samplingTrianglePosition(outPos, outNormal, outTangent, outBinormal, m_pixel);

			// next pixel
			m_currentRasterisation->moveNextPixel(m_pixel);

			// finish
			if (m_currentRasterisation->isFinished(m_pixel))
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
			core::array<SBakePixel>& listPixels = m_currentRasterisation->getBakePixelQueue();
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

				CContext* context = CContext::getInstance();
				CScene* scene = context->getScene();

				// bake lighting color
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

				// write result to image
				m_currentRasterisation->flushPixel(m_out);
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

				IVideoDriver* driver = getVideoDriver();
				core::array<IImage*> lightmapImages;

				core::dimension2du size(m_lightmapSize, m_lightmapSize);

				for (int i = 0; i < m_numberRasterize; i++)
				{
					// todo fix seam
					m_lmRasterize[i]->imageDilate();

					// lighting data
					unsigned char* data = m_lmRasterize[i]->getLightmapData();

					// create lightmap image					
					IImage* img = driver->createImageFromData(video::ECF_R8G8B8, size, data);
					lightmapImages.push_back(img);
				}

				// init lightmap texture array
				ITexture* lightmapTexture = driver->getTextureArray(lightmapImages.pointer(), lightmapImages.size());
				if (lightmapTexture != NULL)
				{
					// bind lightmap texture as indirect lighting
					for (CRenderMesh* renderMesh : m_renderMesh)
					{
						if (renderMesh->getGameObject()->isStatic() == true)
						{
							CIndirectLighting* indirect = renderMesh->getGameObject()->getComponent<CIndirectLighting>();
							if (indirect == NULL)
								indirect = renderMesh->getGameObject()->addComponent<CIndirectLighting>();

							indirect->setIndirectLightmap(lightmapTexture);
							indirect->setIndirectLightingType(CIndirectLighting::LightmapArray);
						}
					}
				}

				// write output to png
				bool testWriteFile = true;
				if (testWriteFile)
				{
					for (int i = 0; i < m_numberRasterize; i++)
					{
						char outFileName[512];
						sprintf(outFileName, "LightMapRasterize_bounce_%d_%d.png", m_lightBounce, i);
						driver->writeImageToFile(lightmapImages[i], outFileName);
					}
				}

				for (int i = 0; i < m_numberRasterize; i++)
				{
					lightmapImages[i]->drop();
					lightmapImages[i] = NULL;
				}

				// write debug bake to png
				if (testWriteFile)
				{
					for (int i = 0; i < m_numberRasterize; i++)
					{
						// debug data
						unsigned char* data = m_lmRasterize[i]->getTestBakeImage();
						IImage* img = driver->createImageFromData(video::ECF_R8G8B8, size, data);

						char outFileName[512];
						sprintf(outFileName, "LightMapRasterize_debug_bounce_%d_%d.png", m_lightBounce, i);
						driver->writeImageToFile(img, outFileName);
						img->drop();
					}
				}

				// clear reset data for next bounce bake
				for (int i = 0; i < m_numberRasterize; i++)
					m_lmRasterize[i]->resetBake();

				if (m_lightBounce >= numLightBounce)
				{
					gotoDemoView();
				}
				else
				{
					// reset next light bounce
					m_currentPass = 0;
					m_currentMB = 0;
					m_currentTris = 0;
					m_lastTris = 9999;
					m_currentRasterisation = NULL;
				}
			}

			return;
		}
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

void CViewBakeLightmap::saveProgress()
{
	// time baked
	u32 deltaTime = (os::Timer::getRealTime() - m_timeBeginBake) + m_timeSpentFromLastSave;

	// init 10mb (auto grow later)
	CMemoryStream* stream = new CMemoryStream(10 * 1024 * 1024);

	stream->writeUInt((u32)getVideoDriver()->getDriverType());

	stream->writeUInt(m_numRenderers);
	stream->writeUInt(m_numIndices);
	stream->writeUInt(m_numVertices);
	stream->writeUInt(m_lightmapSize);
	stream->writeUInt((u32)m_meshBuffers.size());

	stream->writeInt(m_lightBounce);
	stream->writeInt(m_currentPass);
	stream->writeInt(m_currentMB);
	stream->writeInt(m_currentTris);
	stream->writeInt(m_lastTris);
	stream->writeUInt(deltaTime);

	stream->writeInt(m_numberRasterize);
	for (int i = 0; i < m_numberRasterize; i++)
		m_lmRasterize[i]->save(stream);

	stream->writeInt(getRasterisationIndex(m_currentRasterisation));

	io::IWriteFile* file = getIrrlichtDevice()->getFileSystem()->createAndWriteFile("LightmapProgress.dat");
	file->write(stream->getData(), stream->getSize());
	file->drop();

	if (m_currentPass != (int)CRasterisation::PassCount)
	{
		// write current output to review
		core::dimension2du size(m_lightmapSize, m_lightmapSize);
		char outFileName[512];
		IVideoDriver* driver = getVideoDriver();

		for (int i = 0; i < m_numberRasterize; i++)
		{
			unsigned char* data = m_lmRasterize[i]->getLightmapData();

			IImage* img = driver->createImageFromData(video::ECF_R8G8B8, size, data);
			sprintf(outFileName, "LightMapRasterize_review_%d.png", i);
			driver->writeImageToFile(img, outFileName);
			img->drop();
		}
	}
}

void CViewBakeLightmap::loadProgress()
{
	io::IReadFile* file = getIrrlichtDevice()->getFileSystem()->createAndOpenFile("LightmapProgress.dat");
	if (file != NULL)
	{
		u32 fileSize = file->getSize();
		unsigned char* data = new unsigned char[fileSize];
		file->read(data, fileSize);

		CMemoryStream* stream = new CMemoryStream(data, fileSize);

		u32 driverType = (u32)getVideoDriver()->getDriverType();

		u32 readDriverType = stream->readUInt();
		u32 numRenderers = stream->readUInt();
		u32 numIndices = stream->readUInt();
		u32 numVertices = stream->readUInt();
		u32 lmSize = stream->readUInt();
		u32 numMeshBuffer = stream->readUInt();

		if (driverType != driverType ||
			numRenderers != m_numRenderers ||
			numIndices != m_numIndices ||
			numVertices != m_numVertices ||
			numMeshBuffer != m_meshBuffers.size() ||
			lmSize != m_lightmapSize)
		{
			// broken progress
			file->drop();
			return;
		}

		m_lightBounce = stream->readInt();
		m_currentPass = stream->readInt();
		m_currentMB = stream->readInt();
		m_currentTris = stream->readInt();
		m_lastTris = stream->readInt();
		m_timeSpentFromLastSave = stream->readUInt();

		int m_numberRasterize = stream->readInt();
		for (int i = 0; i < m_numberRasterize; i++)
		{
			Lightmapper::CRasterisation* raster = createGetLightmapRasterisation(i);
			raster->load(stream);
		}

		int rasterIndex = stream->readInt();
		if (rasterIndex == -1)
			m_currentRasterisation = NULL;
		m_currentRasterisation = m_lmRasterize[rasterIndex];

		delete stream;
		delete data;

		file->drop();

		// need load current lightmap
		if (m_lightBounce >= 1)
		{
			std::vector<std::string> listTextures;

			for (int i = 0; i < m_numberRasterize; i++)
			{
				char lightmapName[512];
				sprintf(lightmapName, "LightMapRasterize_bounce_%d_%d.png", m_lightBounce, i);
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
						CIndirectLighting* indirect = renderMesh->getGameObject()->getComponent<CIndirectLighting>();
						if (indirect == NULL)
							indirect = renderMesh->getGameObject()->addComponent<CIndirectLighting>();

						indirect->setIndirectLightmap(lightmapTexture);
						indirect->setIndirectLightingType(CIndirectLighting::LightmapArray);
					}
				}
			}

			if (m_currentPass >= (int)CRasterisation::PassCount)
			{
				u32 numLightBounce = 1;

				if (CDirectionalLight::getCurrentDirectionLight() != NULL)
					numLightBounce = CDirectionalLight::getCurrentDirectionLight()->getBounce();

				// finish
				if (m_lightBounce >= numLightBounce)
				{
					gotoDemoView();
				}
				else
				{
					m_currentPass = 0;
				}
			}
		}
	}
}

void CViewBakeLightmap::gotoDemoView()
{
	// enable render indirect
	CDeferredRP::enableRenderIndirect(true);

	// switch to demo view
	CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
}