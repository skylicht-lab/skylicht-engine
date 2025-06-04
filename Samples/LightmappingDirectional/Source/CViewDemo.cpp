#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"

#include "Debug/CSceneDebug.h"

CViewDemo::CViewDemo()
{

}

CViewDemo::~CViewDemo()
{
}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CZone* zone = context->getActiveZone();

	m_lights = zone->getComponentsInChild<CLight>(false);
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();

	// update application
	context->getScene()->update();

	// debug light position
	CSceneDebug* debug = CSceneDebug::getInstance();
	for (CLight* light : m_lights)
	{
		SColor lightColor = light->getColor().toSColor();
		core::vector3df pos = light->getGameObject()->getPosition();
		debug->addPosition(pos, 0.2f, lightColor);
	}
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