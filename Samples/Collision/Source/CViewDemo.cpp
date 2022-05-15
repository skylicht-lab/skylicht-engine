#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "Projective/CProjective.h"
#include "Debug/CSceneDebug.h"

CViewDemo::CViewDemo() :
	m_mouseX(0.0f),
	m_mouseY(0.0f)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CEventManager::getInstance()->registerEvent("ViewDemo", this);
}

void CViewDemo::onDestroy()
{
	CEventManager::getInstance()->unRegisterEvent(this);
}

bool CViewDemo::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		m_mouseX = (float)event.MouseInput.X;
		m_mouseY = (float)event.MouseInput.Y;
	}
	return true;
}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// get view ray from camera to mouse position
	const core::recti& vp = getVideoDriver()->getViewPort();
	core::line3df ray = CProjective::getViewRay(
		context->getActiveCamera(),
		m_mouseX, m_mouseY,
		vp.getWidth(), vp.getHeight()
	);

	float outBestDistanceSquared = 100 * 100; // hit in 100m
	core::vector3df intersection;
	core::triangle3df triangle;
	CCollisionNode* node = NULL;

	if (context->getCollisionManager()->getCollisionPoint(ray, outBestDistanceSquared, intersection, triangle, node))
	{
		// draw the hit triangle
		CSceneDebug* sceneDebug = CSceneDebug::getInstance();
		sceneDebug->addTri(triangle, SColor(255, 255, 0, 0));

		// draw intersection point
		core::line3df line;
		line.start = intersection;
		line.end = intersection + triangle.getNormal().normalize() * 1.0f;
		sceneDebug->addLine(line, SColor(255, 255, 0, 255));
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
		context->updateDirectionLight();

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
