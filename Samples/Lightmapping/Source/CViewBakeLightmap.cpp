#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

CViewBakeLightmap::CViewBakeLightmap() :
	m_currentMB(0),
	m_currentTris(0),
	m_lastTris(99999),
	m_bakeCameraObject(NULL)
{
	m_lmRasterize = new Lightmapper::CRasterisation(512, 512);
}

CViewBakeLightmap::~CViewBakeLightmap()
{
	delete m_lmRasterize;
}

void CViewBakeLightmap::onInit()
{
	CContext *context = CContext::getInstance();
	CZone *zone = context->getActiveZone();
	CEntityManager *entityMgr = zone->getEntityManager();

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
}

void CViewBakeLightmap::onDestroy()
{

}

void CViewBakeLightmap::onUpdate()
{
	if (m_currentMB < m_meshBuffers.size())
	{
		IMeshBuffer *mb = m_meshBuffers[m_currentMB];
		const core::matrix4& transform = m_meshTransforms[m_currentMB];

		IIndexBuffer *idx = mb->getIndexBuffer();
		u32 numTris = idx->getIndexCount() / 3;

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

			core::vector3df binormals[] = {
				vertices[v1].Binormal,
				vertices[v2].Binormal,
				vertices[v3].Binormal
			};

			m_pixel = m_lmRasterize->setTriangle(positions, uvs, normals, tangents, binormals);
			m_lastTris = m_currentTris;
		}

		core::vector3df outPos;
		core::vector3df outNormal;
		core::vector3df outTangent;
		core::vector3df outBinormal;

		// sampling pixel
		m_lmRasterize->samplingTrianglePosition(outPos, outNormal, outTangent, outBinormal, m_pixel);

		// next pixel
		m_lmRasterize->moveNextPixel(m_pixel, 4);

		// finish
		if (m_lmRasterize->isFinished(m_pixel))
			m_currentTris++;

		// go next buffer
		if (m_currentTris >= numTris)
		{
			m_currentMB++;
			m_currentTris = 0;
			m_lastTris = 9999;
		}

		printf("%d/%d - %d %d\n", m_currentTris, numTris, m_pixel.X, m_pixel.Y);
	}
	else
	{
		unsigned char *data = m_lmRasterize->getImageData();

		core::dimension2du size(m_lmRasterize->getWidth(), m_lmRasterize->getHeight());
		IImage *img = getVideoDriver()->createImageFromData(video::ECF_R8G8B8, size, data);
		getVideoDriver()->writeImageToFile(img, "C:\\SVN\\m_lmRasterize.png");
		img->drop();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
}

void CViewBakeLightmap::onRender()
{

}

void CViewBakeLightmap::onPostRender()
{

}