#include "pch.h"
#include "CCamera.h"
#include "C3rdCamera.h"
#include "Transform/CWorldTransformSystem.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	C3rdCamera::C3rdCamera() :
		m_camera(NULL),
		m_rotateSpeed(16.0f),
		m_followEntity(NULL),
		m_isFollowPosition(true),
		m_camPan(0.001f),
		m_camTilt(-20.0f),
		m_targetDistance(3.0f),
		m_leftMousePress(false)
	{
		m_followPosition.set(0.0f, 1.8f, 0.0f);
		m_cursorControl = getIrrlichtDevice()->getCursorControl();
	}

	C3rdCamera::~C3rdCamera()
	{
		m_gameObject->getScene()->unRegisterEvent(this);

		CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
		transformSystem->unRegisterLateUpdate(this);
	}

	void C3rdCamera::initComponent()
	{
		m_gameObject->getScene()->registerEvent(getTypeName(), this);

		CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
		transformSystem->registerLateUpdate(this);
	}

	void C3rdCamera::updateComponent()
	{
		if (m_camera == NULL)
			m_camera = m_gameObject->getComponent<CCamera>();
	}

	void C3rdCamera::lateUpdate()
	{
		CTransformEuler* transform = m_gameObject->getTransformEuler();
		if (m_camera == NULL || transform == NULL)
			return;

		f32 timeDiff = getTimeStep();

		// skip lag
		const float delta = 1000.0f / 20.0f;
		if (timeDiff > delta)
			timeDiff = delta;

		if (m_camera->isInputReceiverEnabled())
		{
			if (m_leftMousePress)
				updateInputRotate(timeDiff);
		}

		// thanks for 
		// https://github.com/netpipe/IrrlichtDemos/blob/master/Camera/3rdPersonCam/I3rdPersonCam.h

		core::vector3df posVector;

		posVector.X = cosf((m_camPan + 180.0f) * core::DEGTORAD) * sinf((m_camTilt + 90.0f) * core::DEGTORAD);
		posVector.Y = cosf((m_camTilt + 90.0f) * core::DEGTORAD);
		posVector.Z = sinf((m_camPan + 180.0f) * core::DEGTORAD) * sinf((m_camTilt + 90.0f) * core::DEGTORAD);

		core::vector3df target = m_followPosition;
		if (m_followEntity && !m_isFollowPosition)
		{
			CWorldTransformData* world = GET_ENTITY_DATA(m_followEntity, CWorldTransformData);
			target = world->World.getTranslation();
		}

		core::vector3df camLocation;
		camLocation.X = target.X + posVector.X * m_targetDistance;
		camLocation.Y = target.Y + posVector.Y * m_targetDistance;
		camLocation.Z = target.Z + posVector.Z * m_targetDistance;

		// need call recalculateViewMatrix because we update camera in LateUpdate
		transform->setPosition(camLocation);
		m_camera->lookAt(camLocation, target, Transform::Oy);
		m_camera->recalculateViewMatrix();
	}

	void C3rdCamera::updateInputRotate(float timeDiff)
	{
		const float MaxVerticalAngle = 88;
		const int MouseYDirection = 1;

		float dx = m_centerCursor.X - m_cursorPos.X;
		float dy = m_centerCursor.Y - m_cursorPos.Y;

		m_camPan += dx * m_rotateSpeed * timeDiff;
		m_camTilt += dy * m_rotateSpeed * MouseYDirection * timeDiff;

		m_camPan = fmodf(m_camPan, 360.0f);

		// fix bug when m_camPan == 0.0f
		if (m_camPan < 0.001f && m_camPan > -0.001f)
			m_camPan = 0.001f;

		m_camTilt = core::clamp(m_camTilt, -MaxVerticalAngle, MaxVerticalAngle);

		m_centerCursor = m_cursorControl->getRelativePosition();
		m_cursorPos = m_centerCursor;
	}

	bool C3rdCamera::OnEvent(const SEvent& evt)
	{
		switch (evt.EventType)
		{
		case EET_KEY_INPUT_EVENT:
			break;

		case EET_MOUSE_INPUT_EVENT:
			if (evt.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			{
				m_leftMousePress = true;

				m_centerCursor = m_cursorControl->getRelativePosition();
				m_cursorPos = m_centerCursor;
			}
			else if (evt.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
			{
				m_leftMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_MOUSE_MOVED)
			{
				if (m_leftMousePress)
				{
					m_cursorPos = m_cursorControl->getRelativePosition();
					return true;
				}
			}
			else if (evt.MouseInput.Event == EMIE_MOUSE_WHEEL)
			{

			}
			break;

		default:
			break;
		}

		return false;
	}
}