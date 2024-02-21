#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "PhysicsEngine/CPhysicsEngine.h"

CViewDemo::CViewDemo()
{
	
}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext *context = CContext::getInstance();
	CCamera *camera = context->getActiveCamera();	
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext *context = CContext::getInstance();
	CScene *scene = context->getScene();
	if (scene != NULL)
	{
		// update physics
		float timestepSec = getTimeStep() / 1000.0f;
		Physics::CPhysicsEngine::getInstance()->updatePhysics(timestepSec);

		// update scene
		scene->update();
	}
}

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CCamera *guiCamera = context->getGUICamera();

	CScene *scene = context->getScene();

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
