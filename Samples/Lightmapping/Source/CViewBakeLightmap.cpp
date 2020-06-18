#include "pch.h"
#include "CViewBakeLightmap.h"
#include "CViewDemo.h"
#include "Context/CContext.h"
#include "ViewManager/CViewManager.h"

CViewBakeLightmap::CViewBakeLightmap()
{

}

CViewBakeLightmap::~CViewBakeLightmap()
{

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

	CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
}

void CViewBakeLightmap::onDestroy()
{

}

void CViewBakeLightmap::onUpdate()
{

}

void CViewBakeLightmap::onRender()
{

}

void CViewBakeLightmap::onPostRender()
{

}