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
	CScene *scene = context->getScene();
	if (scene != NULL)
		scene->update();
}

void CViewDemo::onRender()
{
	CContext *context = CContext::getInstance();

	CCamera *camera = context->getActiveCamera();
	CScene *scene = context->getScene();

	if (camera != NULL && scene != NULL)
	{
		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager());
	}
}