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

}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext *context = CContext::getInstance();
	CScene *scene =	context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CZone *zone = context->getActiveZone();

	if (camera != NULL && zone != NULL)
	{
		context->getRenderPipeline()->render(camera, zone->getEntityManager());
	}
}