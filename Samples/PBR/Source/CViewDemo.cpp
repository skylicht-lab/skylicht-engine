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
	CCamera* camera = context->getActiveCamera();

	CEventManager::getInstance()->registerEvent("ViewDemo", this);
}

void CViewDemo::onDestroy()
{
	CEventManager::getInstance()->unRegisterEvent(this);
}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
	{
		// update scene
		scene->update();
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

bool CViewDemo::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			CContext* context = CContext::getInstance();

			m_mouseX = event.MouseInput.X;
			m_mouseY = event.MouseInput.Y;

			// get view ray
			const core::recti& vp = getVideoDriver()->getViewPort();
			core::line3df ray = CProjective::getViewRay(
				context->getActiveCamera(),
				(float)m_mouseX,
				(float)m_mouseY,
				vp.getWidth(), vp.getHeight()
			);
		}
	}

	return false;
}
