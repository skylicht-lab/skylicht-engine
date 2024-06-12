#include "pch.h"
#include "Context/CContext.h"
#include "CViewDemo.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "Boids/CBoldData.h"
#include "Boids/CBoldSystem.h"
#include "Boids/CBoldAnimationSystem.h"

#include "SkinnedInstancing/CSkinnedInstanceData.h"

#include "Debug/CSceneDebug.h"

CViewDemo::CViewDemo() :
	m_followEntity(NULL),
	m_followRotateCam(NULL),
	m_followTopCam(NULL),
	m_cameraBrain(NULL),
	m_cameraBlendTween(NULL),
	m_debugNeighbor(false)
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

	core::vector3df targetOffset(0.0f, 0.5f, 0.0f);

	// create follow 3rd camera
	CGameObject* camera3rdObj = zone->createEmptyObject();
	CCamera* camera3rd = camera3rdObj->addComponent<CCamera>();
	camera3rd->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	camera3rd->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));
	m_followRotateCam = camera3rdObj->addComponent<C3rdCamera>();
	m_followRotateCam->setTargetOffset(targetOffset);

	// create follow 3rd top camera
	CGameObject* camera3rdTopObj = zone->createEmptyObject();
	CCamera* camera3rdTop = camera3rdTopObj->addComponent<CCamera>();
	camera3rdTop->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	camera3rdTop->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));
	camera3rdTop->setInputReceiver(false);
	m_followTopCam = camera3rdTopObj->addComponent<C3rdCamera>();
	m_followTopCam->setOrientation(45.0f, -45.0f);
	m_followTopCam->setTargetDistance(10.0f);
	m_followTopCam->setTargetOffset(targetOffset);

	// create editor camera
	CGameObject* cameraEditorObj = zone->createEmptyObject();
	CCamera* cameraEditor = cameraEditorObj->addComponent<CCamera>();
	cameraEditor->setPosition(core::vector3df(10.0f, 20.0f, 10.0f));
	cameraEditor->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));
	m_editorCam = cameraEditorObj->addComponent<CEditorCamera>();
	cameraEditorObj->addComponent<CFpsMoveCamera>();

	// create main camera
	CGameObject* cameraMainObj = zone->createEmptyObject();
	CCamera* cameraMain = cameraMainObj->addComponent<CCamera>();
	m_cameraBrain = cameraMainObj->addComponent<CCameraBrain>();
	m_cameraBrain->setTargetCamera(camera3rd);

	// activate camera
	context->setActiveCamera(cameraMain);

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

			m_followEntity = instancing->getEntities()[randomId];

			if (m_followRotateCam)
				m_followRotateCam->setFollowTarget(m_followEntity);

			if (m_followTopCam)
				m_followTopCam->setFollowTarget(m_followEntity);

			m_cameraBrain->lateUpdate();
			break;
		}
	}
}

void CViewDemo::testSpawnEntity(int count)
{
	// remove all entity
	if (m_followRotateCam)
		m_followRotateCam->setFollowTarget((CEntity*)NULL);

	if (m_followTopCam)
		m_followTopCam->setFollowTarget((CEntity*)NULL);

	for (CRenderSkinnedInstancing* instancing : m_instancings)
	{
		if (instancing)
			instancing->removeAllEntities();
	}

	// get bound random
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	CBoldSystem* boldSystem = scene->getEntityManager()->getSystem<CBoldSystem>();
	CBoldAnimationSystem* boldAnimationSystem = scene->getEntityManager()->getSystem<CBoldAnimationSystem>();

	// spawn new entity (count)
	CEntity* entity;
	CBoldData* bold;
	CWorldTransformData* transform;
	CSkinnedInstanceData* skinnedInstance;
	core::vector3df position;
	float x, z, time;

	if (boldSystem && boldAnimationSystem)
	{
		float minX, maxX, minZ, maxZ;
		boldSystem->getBounds(minX, maxX, minZ, maxZ);

		float dx = maxX - minX;
		float dz = maxZ - minZ;

		core::array<SMovingAnimation>& clips = boldAnimationSystem->getClips();

		for (int i = 0; i < count / 2; i++)
		{
			// ENTITY MODEL 0
			x = minX + os::Randomizer::frand() * dx;
			z = minZ + os::Randomizer::frand() * dz;

			entity = m_instancings[0]->spawn();

			// set animation (random time)
			skinnedInstance = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
			time = os::Randomizer::frand();
			skinnedInstance->setAnimation(0, clips[0].Clip, time * clips[0].Clip->Duration, clips[0].FPS, 0);
			skinnedInstance->setAnimation(1, clips[1].Clip, time * clips[1].Clip->Duration, clips[1].FPS, 1);
			skinnedInstance->setAnimationWeight(0, 1.0f);
			skinnedInstance->setAnimationWeight(1, 0.0f);

			// set position
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			position.set(x, 0.0f, z);

			transform->Relative.setTranslation(position);

			// add bold data
			bold = entity->addData<CBoldData>();
			bold->Location = position;


			// ENTITY MODEL 1
			x = minX + os::Randomizer::frand() * dx;
			z = minZ + os::Randomizer::frand() * dz;

			entity = m_instancings[1]->spawn();

			// set animation (random time)
			skinnedInstance = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
			time = os::Randomizer::frand();
			skinnedInstance->setAnimation(0, clips[0].Clip, time * clips[0].Clip->Duration, clips[0].FPS, 0);
			skinnedInstance->setAnimation(1, clips[1].Clip, time * clips[1].Clip->Duration, clips[1].FPS, 1);
			skinnedInstance->setAnimationWeight(0, 1.0f);
			skinnedInstance->setAnimationWeight(1, 0.0f);

			// set position
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			position.set(x, 0.0f, z);

			transform->Relative.setTranslation(position);

			// add bold data
			bold = entity->addData<CBoldData>();
			bold->Location = position;
		}
	}

	followRandomEntity();
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

	CImguiManager::getInstance()->onNewFrame();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

	if (m_debugNeighbor)
	{
		CBoldData* bold = GET_ENTITY_DATA(m_followEntity, CBoldData);

		float radius = 0.25f;

		// draw green circle in follow entity
		CSceneDebug* noZDebug = CSceneDebug::getInstance()->getNoZDebug();
		noZDebug->addCircle(bold->Location, radius, Transform::Oy, SColor(255, 0, 255, 0));

		// draw red circle in neighbor
		CBoldData** neighbor = bold->Neighbor.pointer();
		int numNeighbor = bold->Neighbor.count();
		for (int i = 0; i < numNeighbor; i++)
		{
			CBoldData* b = neighbor[i];
			if (b != bold)
			{
				noZDebug->addCircle(b->Location, radius, Transform::Oy, SColor(255, 255, 0, 0));
			}
		}
	}

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

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::setTargetCamera(CCamera* cam, float blendDuration)
{
	// enable input in new camera
	if (cam != m_followTopCam->getCamera())
		cam->setInputReceiver(true);

	// focus new camera
	m_cameraBrain->setTargetCamera(cam, 0.0f);

	// animation blend change camera
	if (m_cameraBlendTween)
	{
		m_cameraBlendTween->stop();
		m_cameraBlendTween = NULL;
	}

	m_cameraBlendTween = new CTweenFloat(0.0f, 1.0f, blendDuration);
	m_cameraBlendTween->setEase(EaseOutCubic);
	m_cameraBlendTween->OnUpdate = [&](CTween* tween) {	m_cameraBrain->setBlendValue(m_cameraBlendTween->getValue()); };
	m_cameraBlendTween->OnFinish = [&](CTween* tween) {	m_cameraBlendTween = NULL; };

	CTweenManager::getInstance()->addTween(m_cameraBlendTween);
}

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(350, 290), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Boid Camera", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		// Show FPS
		int fps = getIrrlichtDevice()->getVideoDriver()->getFPS();
		ImGui::Text("FPS: %d", fps);

		if (ImGui::CollapsingHeader("Camera Type", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const char* items[] = { "Follow Entity", "Top View", "Editor" };
			static int cameraType = 0;
			static int currentCameraType = cameraType;
			ImGui::Combo("Camera", &cameraType, items, IM_ARRAYSIZE(items));
			if (currentCameraType != cameraType)
			{
				m_followRotateCam->getCamera()->setInputReceiver(false);
				m_followTopCam->getCamera()->setInputReceiver(false);
				m_editorCam->getCamera()->setInputReceiver(false);

				currentCameraType = cameraType;
				float blendDuration = 500.0f;

				if (currentCameraType == 0)
				{
					setTargetCamera(m_followRotateCam->getCamera(), blendDuration);
				}
				else if (currentCameraType == 1)
				{
					setTargetCamera(m_followTopCam->getCamera(), blendDuration);
				}
				else
				{
					CCamera* cam = m_editorCam->getCamera();

					core::vector3df pos = m_cameraBrain->getPosition();
					const core::vector3df& lookAt = m_cameraBrain->getLookAt();
					const core::vector3df& up = m_cameraBrain->getUp();

					pos = pos - lookAt * 10.0f;

					cam->setPosition(pos);
					cam->lookAt(pos + lookAt, up);

					setTargetCamera(m_editorCam->getCamera(), blendDuration);
				}
			}

			ImGui::Spacing();

			float minDistance = 0.5f;
			float farDistance = 20.0f;

			if (currentCameraType == 0)
			{
				float distance = m_followRotateCam->getTargetDistance();
				ImGui::SliderFloat("Target distance", &distance, minDistance, farDistance, "%.3f");
				m_followRotateCam->setTargetDistance(distance);
			}
			else if (currentCameraType == 1)
			{
				float distance = m_followTopCam->getTargetDistance();
				ImGui::SliderFloat("Target distance", &distance, minDistance, farDistance, "%.3f");
				m_followTopCam->setTargetDistance(distance);
			}
			else
			{
				ImGui::Text("Press ASDW to move camera");
				ImGui::Text("Drag Right Mouse to rotate camera");
				ImGui::Text("Scroll Middle Mouse to go near/far");
			}

			if (currentCameraType == 0 || currentCameraType == 1)
			{
				if (ImGui::Button("Change another entity"))
				{
					followRandomEntity();
				}
			}
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Show neighbor", &m_debugNeighbor);
		}

		if (ImGui::CollapsingHeader("Entites", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static int entityCount = 20;
			int minEntity = 10;
			int maxEntity = 1000;
			ImGui::SliderInt("Num entity", &entityCount, minEntity, maxEntity);

			char buttonLabel[512];
			sprintf(buttonLabel, "Test spawn %d entity", entityCount);

			if (ImGui::Button(buttonLabel))
			{
				testSpawnEntity(entityCount);
			}
		}

		ImGui::End();
	}
}

void CViewDemo::onPostRender()
{

}
