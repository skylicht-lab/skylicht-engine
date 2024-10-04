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

	// full rect grey background
	canvas->createRect(SColor(0xff3b3432));

	// this is the gui element, that will render spine inside
	CGUIElement* element = canvas->createElement();
	element->setWidth(400.0f);
	element->setHeight(400.0f);
	element->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	// element->setDrawBorder(true);

	// init spine2d
	m_spineResource = new spine::CSpineResource();
	if (m_spineResource->loadAtlas("SampleSpine2D/spineboy-pma.atlas", "SampleSpine2D"))
	{
		if (m_spineResource->loadSkeletonJson("SampleSpine2D/spineboy-pro.json", 0.5f))
		{
			// see document: https://en.esotericsoftware.com/spine-cpp
			spine::CSkeletonDrawable* drawable = m_spineResource->getDrawable();
			spine::AnimationState* animationState = drawable->getAnimationState();

			animationState->getData()->setDefaultMix(0.2f);

			spine::Skeleton* skeleton = drawable->getSkeleton();
			skeleton->setScaleY(-1.0f);
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

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();
	CCamera* guiCamera = context->getGUICamera();
	CScene* scene = context->getScene();

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::renderSpine(CGUIElement* element)
{
	spine::CSkeletonDrawable* drawable = m_spineResource->getDrawable();
	spine::Skeleton* skeleton = drawable->getSkeleton();

	// setup for aim IK
	if (skeleton)
	{
		spine::Bone* crosshair = skeleton->findBone("crosshair");
		if (crosshair)
		{
			float x = 200.0f, y = 200.0f;
			crosshair->setX(x);
			crosshair->setY(y);
		}
	}

	drawable->setDrawOffset(core::vector2df(element->getWidth() * 0.5f, element->getHeight()));
	drawable->update(getTimeStep(), spine::Physics_Update);
	drawable->render(element);
}

void CViewDemo::onPostRender()
{

}

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(850, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(280, 270), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Spine 2D", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		const char* animationName[] = {
			"aim",
			"death",
			"hoverboard",
			"idle",
			"idle-turn",
			"jump",
			"portal",
			"run",
			"run-to-idle",
			"shoot",
			"walk",
		};

		static int animId = 7;

		spine::CSkeletonDrawable* drawable = m_spineResource->getDrawable();
		spine::Skeleton* skeleton = drawable->getSkeleton();
		spine::AnimationState* animationState = drawable->getAnimationState();

		spine::TrackEntry* track = animationState->getCurrent(0);
		spine::Animation* anim = track->getAnimation();

		if (anim)
		{
			float time = track->getAnimationTime();
			float duration = anim->getDuration();
			ImGui::LabelText("Time ", "%f/%f", time, duration);
		}

		if (ImGui::Combo("Animation", &animId, animationName, IM_ARRAYSIZE(animationName)))
		{
			animationState->setAnimation(0, animationName[animId], true);
		}

		ImGui::End();
	}
}
