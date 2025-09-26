#include "pch.h"
#include "CViewDemo.h"

#include "PhysicsEngine/CPhysicsEngine.h"
#include "Collider/CBoxCollider.h"
#include "Collider/CCapsuleCollider.h"

#include "DirectionalInput/CDirectionalInput.h"
#include "Camera/C3rdCamera.h"
#include "Primitive/CCapsule.h"
#include "Primitive/CCube.h"

#include "Context/CContext.h"
#include "CImguiManager.h"
#include "imgui.h"

#include "Debug/CSceneDebug.h"

CViewDemo::CViewDemo() :
	m_player(NULL)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	initObjects();
}

void CViewDemo::initObjects()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CScene* scene = context->getScene();
	CZone* zone = scene->getZone(0);
	scene->updateIndexSearchObject();

	// Capsule
	float capsuleRadius = 0.5f;
	float capsuleHeight = 1.2f;

	CGameObject* capsuleObj = zone->createEmptyObject();

	// renderer
	CCapsule* capsule = capsuleObj->addComponent<CCapsule>();
	capsule->setRadius(capsuleRadius);
	capsule->setHeight(capsuleHeight);
	capsule->init();

	// indirect lighting
	capsuleObj->addComponent<CIndirectLighting>();

	// collider & character
	Physics::CCapsuleCollider* capsuleCollider = capsuleObj->addComponent<Physics::CCapsuleCollider>();
	capsuleCollider->setCapsule(capsuleRadius, capsuleHeight);
	m_player = capsuleObj->addComponent<Physics::CCharacterController>();
	m_player->initCharacter(capsuleRadius);
	m_player->setPosition(core::vector3df(0.0f, 5.0f, 0.0f));
	m_player->setRotation(core::vector3df(0.0f, 0.0f, 0.0f));
	m_player->syncTransform();

	// if character hit a collision object
	m_player->OnCollision = [&](
		Physics::ICollisionObject* a,
		Physics::ICollisionObject* b,
		Physics::SCollisionContactPoint* point,
		int numContact) {
			Physics::ICollisionObject* collide = a == m_player ? b : a;
			if (collide->getCollisionType() == Physics::ICollisionObject::RigidBody)
			{
				Physics::CRigidbody* body = (Physics::CRigidbody*)collide;

				core::vector3df myPosition = m_player->getPosition();
				core::vector3df test(0.0f, -1.4f, 0.0f);

				// Check if character is on box
				bool isOnBody = false;
				Physics::SAllRaycastResult result;
				if (Physics::CPhysicsEngine::getInstance()->rayTest(myPosition, myPosition + test, result))
				{
					for (int i = 0, n = result.Bodies.size(); i < n; i++)
					{
						if (result.Bodies[i] == body)
						{
							isOnBody = true;
							break;
						}
					}
				}

				if (body->getTag() == 1)
				{
					if (isOnBody)
					{
						body->setDynamic(false);
					}
					else
					{

						body->setDynamic(true);

						core::vector3df force = m_player->getLinearVelocity();
						force.normalize();

						// add a force to rigidbody
						force *= 20.0f;

						body->setState(Physics::CRigidbody::Activate);
						body->applyForce(force, core::vector3df());
					}
				}
			}
		};

	// input to control capsule
	capsuleObj->addComponent<CDirectionalInput>();

	// follow camera
	C3rdCamera* followCamera = camera->getGameObject()->getComponent<C3rdCamera>();
	if (followCamera)
	{
		followCamera->setFollowTarget(capsuleObj);
		followCamera->setTargetDistance(5.0f);
	}


	// Cube
	for (int i = 0; i < 40; i++)
	{
		CGameObject* cubeObj = zone->createEmptyObject();

		// renderer
		CCube* cube = cubeObj->addComponent<CCube>();

		CMaterial* material = cube->getMaterial();
		material->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");

		// enable instancing
		cube->setInstancing(true);

		// indirect lighting
		cubeObj->addComponent<CIndirectLighting>();

		// collider & rigidbody
		cubeObj->addComponent<Physics::CBoxCollider>();

		float randomX = -30.0f + os::Randomizer::frand() * 60.0f;
		float randomZ = -30.0f + os::Randomizer::frand() * 60.0f;
		float ramdonRot = os::Randomizer::frand() * 90.0f;

		float randomScale = 1.0f + os::Randomizer::frand() * 2.0f;

		Physics::CRigidbody* body = cubeObj->addComponent<Physics::CRigidbody>();
		body->initRigidbody();
		body->setPosition(core::vector3df(randomX, 20.0f, randomZ));
		body->setRotation(core::vector3df(ramdonRot, ramdonRot, ramdonRot));
		body->setLocalScale(core::vector3df(randomScale, randomScale, randomScale));
		body->setSleepingThresholds(2.0f, 2.0f);
		// body->setCcdMotionThreshold(1.0f);
		// body->setCcdSweptSphereRadius(randomScale + 1.0f);
		body->syncTransform();
		body->setTag(1);

		body->OnCollision = [body](
			Physics::ICollisionObject* a,
			Physics::ICollisionObject* b,
			Physics::SCollisionContactPoint* point,
			int numContact) {
				Physics::CRigidbody::EActivationState state = body->getState();

				// Change box state so rigid body doesn't vibrate when player stands up
				if (body->isDynamic() && state == Physics::CRigidbody::Sleep)
					body->setDynamic(false);
			};

		m_boxs.push_back(body);
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
	{
		// update physics
		float ts = getTimeStep();

		Physics::CPhysicsEngine* physicsEngine = Physics::CPhysicsEngine::getInstance();
		int physicsStep = 2;
		float f = 1.0f / (float)physicsStep;
		for (int i = 0; i < physicsStep; i++)
			physicsEngine->updatePhysics(ts * 0.001f * f, 2);

		scene->update();

		// imgui update
		CImguiManager::getInstance()->onNewFrame();
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

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::onPostRender()
{

}

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(835, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(260, 120), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Character controller", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		ImGui::Text("Press ASDW to move capsule");
		ImGui::Text("Drag left Mouse to rotate camera");
		ImGui::Separator();

		if (ImGui::Button("Reset"))
		{
			// reset player position
			m_player->setPosition(core::vector3df(0.0f, 5.0f, 0.0f));
			m_player->setRotation(core::vector3df(0.0f, 0.0f, 0.0f));
			m_player->syncTransform();

			// reset box position
			for (Physics::CRigidbody* body : m_boxs)
			{
				float randomX = -30.0f + os::Randomizer::frand() * 60.0f;
				float randomZ = -30.0f + os::Randomizer::frand() * 60.0f;
				float ramdonRot = os::Randomizer::frand() * 90.0f;

				body->setPosition(core::vector3df(randomX, 20.0f, randomZ));
				body->setRotation(core::vector3df(ramdonRot, ramdonRot, ramdonRot));
				body->activate();
				body->setDynamic(true);
				body->syncTransform();
			}
		}

		ImGui::End();
	}
}
