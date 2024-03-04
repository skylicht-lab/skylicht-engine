#include "pch.h"
#include "CCamera.h"
#include "CEditorCamera.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	CEditorCamera::CEditorCamera() :
		m_moveSpeed(1.0f),
		m_rotateSpeed(16.0f),
		m_leftMousePress(false),
		m_rightMousePress(false),
		m_midMousePress(false),
		m_mouseWhell(false),
		m_wheel(0.0f)
	{
		m_cursorControl = getIrrlichtDevice()->getCursorControl();
	}

	CEditorCamera::~CEditorCamera()
	{
		getGameObject()->getScene()->unRegisterEvent(this);
	}

	void CEditorCamera::initComponent()
	{
		getGameObject()->getScene()->registerEvent("CEditorCamera", this);
		getGameObject()->setEnableEndUpdate(true);
	}

	void CEditorCamera::updateComponent()
	{

	}

	void CEditorCamera::endUpdate()
	{
		CTransformEuler* transform = m_gameObject->getTransformEuler();
		CCamera* camera = m_gameObject->getComponent<CCamera>();

		if (camera == NULL || transform == NULL)
			return;

		if (!camera->isInputReceiverEnabled())
			return;

		f32 timeDiff = getTimeStep();

		// skip lag
		const float delta = 1000.0f / 20.0f;
		if (timeDiff > delta)
			timeDiff = delta;

		core::vector3df pos = transform->getPosition();

		core::vector3df target = transform->getFront();

		// fix something wrong in math
		if (fabsf(target.X) < 0.00001)
			target.X = 0.0f;
		if (fabsf(target.Y) < 0.00001)
			target.Y = 0.0f;
		if (fabsf(target.Z) < 0.00001)
			target.Z = 0.0f;

		core::vector3df	up, right;

		core::vector3df relativeRotation = target.getHorizontalAngle();
		core::vector3df offsetPosition;

		if (m_rightMousePress)
			updateInputRotate(relativeRotation, timeDiff);
		else if (m_midMousePress)
			updateInputOffset(offsetPosition, timeDiff);

		// calc target after rotation
		target.set(0.0f, 0.0f, 1.0f);
		right.set(1.0f, 0.0f, 0.0f);

		core::matrix4 mat;
		mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
		mat.transformVect(target);
		mat.transformVect(right);

		target.normalize();
		right.normalize();

		// set position
		core::vector3df movedir = target;

		if (m_mouseWhell)
		{
			pos -= movedir * m_wheel * m_moveSpeed * timeDiff * 0.1f;
			m_mouseWhell = false;
		}

		if (m_midMousePress)
		{
			// move left, right
			core::vector3df strafevect = target;
			strafevect = strafevect.crossProduct(transform->getUp());
			strafevect.normalize();
			pos += strafevect * offsetPosition.X;

			// move up, down
			up = strafevect.crossProduct(target);
			up.normalize();
			pos += up * offsetPosition.Y;
		}

		// caculate up vector
		up = -right.crossProduct(target);
		up.normalize();

		// write right target
		camera->lookAt(pos, pos + target, up);
	}

	void CEditorCamera::updateInputRotate(core::vector3df& relativeRotation, f32 timeDiff)
	{
		const float MaxVerticalAngle = 88;
		const int MouseYDirection = 1;

		// rotate X
		relativeRotation.Y -= (m_centerCursor.X - m_cursorPos.X) * m_rotateSpeed * MouseYDirection * timeDiff;

		// rotate Y
		relativeRotation.X -= (m_centerCursor.Y - m_cursorPos.Y) * m_rotateSpeed * timeDiff;

		if (relativeRotation.X > MaxVerticalAngle * 2 && relativeRotation.X < 360.0f - MaxVerticalAngle)
		{
			relativeRotation.X = 360.0f - MaxVerticalAngle;
		}
		else if (relativeRotation.X > MaxVerticalAngle && relativeRotation.X < 360.0f - MaxVerticalAngle)
		{
			relativeRotation.X = MaxVerticalAngle;
		}

		m_centerCursor = m_cursorControl->getRelativePosition();
		m_cursorPos = m_centerCursor;
	}

	void CEditorCamera::updateInputOffset(core::vector3df& offsetPosition, f32 timeDiff)
	{
		offsetPosition.X = (m_cursorPos.X - m_centerCursor.X);
		offsetPosition.Y = (m_cursorPos.Y - m_centerCursor.Y);

		offsetPosition = offsetPosition * m_moveSpeed * timeDiff;

		m_centerCursor = m_cursorControl->getRelativePosition();
		m_cursorPos = m_centerCursor;
	}

	bool CEditorCamera::OnEvent(const SEvent& evt)
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
			else if (evt.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
			{
				m_rightMousePress = true;
				m_centerCursor = m_cursorControl->getRelativePosition();
				m_cursorPos = m_centerCursor;
			}
			else if (evt.MouseInput.Event == EMIE_MMOUSE_PRESSED_DOWN)
			{
				m_midMousePress = true;
				m_centerCursor = m_cursorControl->getRelativePosition();
				m_cursorPos = m_centerCursor;
			}
			else if (evt.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
			{
				m_leftMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
			{
				m_rightMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_MMOUSE_LEFT_UP)
			{
				m_midMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_MOUSE_MOVED)
			{
				if (m_leftMousePress || m_rightMousePress || m_midMousePress)
				{
					m_cursorPos = m_cursorControl->getRelativePosition();
					return true;
				}
			}
			else if (evt.MouseInput.Event == EMIE_MOUSE_WHEEL)
			{
				m_wheel = evt.MouseInput.Wheel;
				m_mouseWhell = true;
			}
			break;

		default:
			break;
		}

		return false;
	}
}