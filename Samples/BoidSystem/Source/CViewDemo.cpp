#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

CViewDemo::CViewDemo()
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();

	CZone* zone = scene->getZone(0);
	CGameObject* cameraObj = zone->createEmptyObject();

	// create camera
	CCamera* camera = cameraObj->addComponent<CCamera>();
	camera->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	cameraObj->addComponent<C3rdCamera>();
	context->setActiveCamera(camera);


	// random a following entity
	zone->updateIndexSearchObject();

	CGameObject* crowd1 = zone->searchObjectInChild(L"Crowd1");
	if (crowd1)
		m_instancings.push_back(crowd1->getComponent<CRenderSkinnedInstancing>());

	CGameObject* crowd2 = zone->searchObjectInChild(L"Crowd2");
	if (crowd2)
		m_instancings.push_back(crowd2->getComponent<CRenderSkinnedInstancing>());

	followRandomEntity();
}

void CViewDemo::followRandomEntity()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	if (camera == NULL)
		return;

	C3rdCamera* followingCamera = camera->getGameObject()->getComponent<C3rdCamera>();
	if (followingCamera == NULL)
		return;

	int totalEntity = 0;
	for (CRenderSkinnedInstancing* instancing : m_instancings)
	{
		if (instancing)
			totalEntity += instancing->getEntityCount();
	}

	if (totalEntity == 0)
		return;

	int begin = 0;
	int end = 0;

	int r = os::Randomizer::rand() % totalEntity;
	for (CRenderSkinnedInstancing* instancing : m_instancings)
	{
		if (instancing)
		{
			begin = end;
			end += instancing->getEntityCount();
		}

		if (r < end)
		{
			int randomId = r - begin;
			CEntity* entity = instancing->getEntities()[randomId];
			followingCamera->setFollowTarget(entity);
			break;
		}
	}
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}
}

void CViewDemo::onPostRender()
{

}
