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
		// update physics
		float timestepSec = getTimeStep() / 1000.0f;
		Physics::CPhysicsEngine::getInstance()->updatePhysics(timestepSec);

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

			// physics raycast
			Physics::CPhysicsEngine* physicsEngine = Physics::CPhysicsEngine::getInstance();

			Physics::SClosestRaycastResult result;
			if (physicsEngine->rayTest(ray.start, ray.end, result))
			{
				if (result.Collider)
				{
					CGameObject* gameObject = result.Collider->getGameObject();

					char log[512];
					sprintf(log, "Closest rayTest: %s", gameObject->getNameA());
					os::Printer::log(log);

					if (result.Body && result.Body->isDynamic())
					{
						core::vector3df worldPosition = result.Body->getPosition();
						core::vector3df localHit = result.HitPointWorld - worldPosition;

						// add a force to rigidbody
						core::vector3df force = ray.getVector();
						force.normalize();
						force *= 100.0f;

						result.Body->setState(Physics::CRigidbody::Activate);
						result.Body->applyForce(force, localHit);
					}
				}
			}
		}
	}

	return false;
}
