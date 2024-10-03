#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "CImguiManager.h"
#include "imgui.h"

CViewDemo::CViewDemo() :
	m_spineResource(NULL)
{
	spine::CSpineResource::initRenderer();
}

CViewDemo::~CViewDemo()
{
	if (m_spineResource)
		delete m_spineResource;

	spine::CSpineResource::releaseRenderer();
}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CScene* scene = context->getScene();
	CZone* zone = scene->getZone(0);

	CGameObject* canvasObj = zone->createEmptyObject();
	CCanvas* canvas = canvasObj->addComponent<CCanvas>();

	// this is the gui element, that will render spine inside
	CGUIElement* element = canvas->createElement();
	element->setWidth(400.0f);
	element->setHeight(400.0f);
	element->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	element->setDrawBorder(true);

	// init spine2d
	m_spineResource = new spine::CSpineResource();
	if (m_spineResource->loadAtlas("SampleSpine2D/spineboy-pma.atlas", "SampleSpine2D"))
	{
		if (m_spineResource->loadSkeletonJson("SampleSpine2D/spineboy-pro.json", 0.5f))
		{
			spine::CSkeletonDrawable* drawable = m_spineResource->getDrawable();
			spine::Skeleton* skeleton = drawable->getSkeleton();
			spine::AnimationState* animationState = drawable->getAnimationState();

			animationState->getData()->setDefaultMix(0.2f);
			skeleton->setToSetupPose();

			animationState->setAnimation(0, "portal", true);
			animationState->addAnimation(0, "run", true, 0);

			// callback for draw spine
			element->OnRender = std::bind(&CViewDemo::renderSpine, this, std::placeholders::_1);
		}
	}

	scene->updateIndexSearchObject();
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

void CViewDemo::renderSpine(CGUIElement* element)
{
	if (m_spineResource)
	{
		spine::CSkeletonDrawable* drawable = m_spineResource->getDrawable();

		drawable->setDrawOffset(core::vector2df(0.0f, element->getHeight() * 0.5f));
		drawable->update(getTimeStep(), spine::Physics_Update);
		drawable->render(element);
	}
}

void CViewDemo::onPostRender()
{

}
