#include "pch.h"
#include "CCamera.h"
#include "CEditorCamera.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	CEditorCamera::CEditorCamera() :
		m_camera(NULL),
		m_moveSpeed(1.0f),
		m_zoomSpeed(1.0f),
		m_rotateSpeed(16.0f),
		m_leftMousePress(false),
		m_rightMousePress(false),
		m_mayaLeftMousePress(false),
		m_mayaRightMousePress(false),
		m_midMousePress(false),
		m_mouseWhell(false),
		m_wheel(0.0f),
		m_altKeyDown(false),
		m_shiftKeyDown(false),
		m_ctrlKeyDown(false),
		m_pivotRotateDistance(5.0f),
		m_controlStyle(CEditorCamera::Default)
	{
		m_cursorControl = getIrrlichtDevice()->getCursorControl();
	}

	CEditorCamera::~CEditorCamera()
	{
		getGameObject()->getScene()->unRegisterEvent(this);
	}

	void CEditorCamera::initComponent()
	{
		getGameObject()->getScene()->registerEvent(getTypeName(), this);
		getGameObject()->setEnableEndUpdate(true);
	}

	void CEditorCamera::updateComponent()
	{
		if (m_camera == NULL)
		{
			m_camera = m_gameObject->getComponent<CCamera>();
		}
	}

	void CEditorCamera::fixVector(core::vector3df& v)
	{
		if (fabsf(v.X) < 0.00001)
			v.X = 0.0f;
		if (fabsf(v.Y) < 0.00001)
			v.Y = 0.0f;
		if (fabsf(v.Z) < 0.00001)
			v.Z = 0.0f;
	}

	void CEditorCamera::setPivotRotate(const core::vector3df& pos)
	{
		CTransformEuler* transform = m_gameObject->getTransformEuler();
		if (m_camera == NULL || transform == NULL)
			return;

		core::plane3df p(pos, -transform->getFront());
		m_pivotRotateDistance = p.getDistanceTo(transform->getPosition());
	}

	void CEditorCamera::endUpdate()
	{
		CTransformEuler* transform = m_gameObject->getTransformEuler();

		if (m_camera == NULL || transform == NULL)
			return;

		if (!m_camera->isInputReceiverEnabled())
			return;

		f32 timeDiff = m_camera->isUseScaledTime() ? getTimeStep() : getNonScaledTimestep();

		// skip lag
		const float delta = 1000.0f / 20.0f;
		if (timeDiff > delta)
			timeDiff = delta;

		core::vector3df pos = transform->getPosition();
		core::vector3df target = transform->getFront();

		// inverse target is used when hold alt + drag left mouse
		core::vector3df rotatePivot = pos + target * m_pivotRotateDistance;
		core::vector3df inverseTarget = pos - rotatePivot;
		inverseTarget.normalize();

		// fix something wrong in math
		fixVector(target);
		fixVector(inverseTarget);

		core::vector3df	up, right;

		core::vector3df relativeRotation = target.getHorizontalAngle();
		core::vector3df inverseRotation = inverseTarget.getHorizontalAngle();

		core::vector3df offsetPosition;

		bool doRotatePivot = false;

		if (m_controlStyle == Maya)
		{
			if (m_altKeyDown)
			{
				if (m_mayaLeftMousePress)
				{
					updateInputRotate(inverseRotation, timeDiff, true);
					doRotatePivot = true;
				}
			}

			if (m_midMousePress)
				updateInputOffset(offsetPosition, timeDiff);
		}
		else if (m_controlStyle == Blender)
		{
			if (!m_ctrlKeyDown)
			{
				if (m_shiftKeyDown)
				{
					if (m_midMousePress)
						updateInputOffset(offsetPosition, timeDiff);
				}
				else
				{
					if (m_midMousePress)
					{
						updateInputRotate(inverseRotation, timeDiff, true);
						doRotatePivot = true;
					}
				}
			}
		}
		else
		{
			if (m_rightMousePress)
				updateInputRotate(relativeRotation, timeDiff);
			else if (m_midMousePress && !m_ctrlKeyDown)
				updateInputOffset(offsetPosition, timeDiff);
			else if (m_leftMousePress)
			{
				if (m_altKeyDown)
				{
					updateInputRotate(inverseRotation, timeDiff, true);
					doRotatePivot = true;
				}
			}
		}

		if (doRotatePivot)
		{
			inverseTarget.set(0.0f, 0.0f, 1.0f);
			core::matrix4 mat;
			mat.setRotationDegrees(core::vector3df(inverseRotation.X, inverseRotation.Y, 0));
			mat.transformVect(inverseTarget);
			inverseTarget.normalize();

			pos = rotatePivot + inverseTarget * m_pivotRotateDistance;

			target = rotatePivot - pos;
			target.normalize();

			core::vector3df up(0.0f, 1.0f, 0.0f);
			right = up.crossProduct(target);
			right.normalize();
		}
		else
		{
			target.set(0.0f, 0.0f, 1.0f);
			right.set(1.0f, 0.0f, 0.0f);

			core::matrix4 mat;
			mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
			mat.transformVect(target);
			mat.transformVect(right);

			target.normalize();
			right.normalize();
		}

		// set position
		core::vector3df movedir = target;

		if (m_mouseWhell)
		{
			pos -= movedir * m_wheel * m_zoomSpeed * timeDiff * 0.1f;
			m_mouseWhell = false;
		}

		if (m_controlStyle == Maya)
		{
			if (m_mayaRightMousePress)
				updateInputZoom(timeDiff, pos, movedir);
		}

		if (m_midMousePress)
		{
			if (m_ctrlKeyDown)
			{
				updateInputZoom(timeDiff, pos, movedir);
			}
			else
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
		}

		// caculate up vector
		up = -right.crossProduct(target);
		up.normalize();

		// write right target
		m_camera->lookAt(pos, pos + target, up);
	}

	void CEditorCamera::updateInputRotate(core::vector3df& relativeRotation, f32 timeDiff, bool useCenterPivot)
	{
		const float MaxVerticalAngle = 88;
		const int MouseYDirection = 1;

		// rotate X
		relativeRotation.Y -= (m_centerCursor.X - m_cursorPos.X) * m_rotateSpeed * MouseYDirection * timeDiff;

		// rotate Y
		if (useCenterPivot)
			relativeRotation.X += (m_centerCursor.Y - m_cursorPos.Y) * m_rotateSpeed * timeDiff;
		else
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

	void CEditorCamera::updateInputZoom(f32 timeDiff, core::vector3df& pos, const core::vector3df& moveDir)
	{
		float zoomX = (m_cursorPos.X - m_centerCursor.X) * m_zoomSpeed * timeDiff;
		float zoomY = (m_cursorPos.Y - m_centerCursor.Y) * m_zoomSpeed * timeDiff;

		core::vector2df delta(zoomX, zoomY);
		float length = delta.getLength();

		int zoomDirection = 0;
		if (fabsf(zoomX) > fabsf(zoomY))
			zoomDirection = 1;
		else
			zoomDirection = 2;

		if (zoomDirection == 1)
		{
			if (zoomX < 0.0f)
				length = -length;
		}
		else if (zoomDirection == 2)
		{
			if (m_controlStyle == EControlStyle::Maya)
			{
				if (zoomY < 0.0f)
					length = -length;
			}
			else
			{
				if (zoomY > 0.0f)
					length = -length;
			}
		}

		pos += moveDir * length;

		m_centerCursor = m_cursorControl->getRelativePosition();
		m_cursorPos = m_centerCursor;
	}

	bool CEditorCamera::OnEvent(const SEvent& evt)
	{
		if (m_camera && !m_camera->isInputReceiverEnabled())
			return false;

		bool acceptMouseEvent = true;
		bool pointerPress = false;

		if (m_controlStyle == Maya && !m_altKeyDown)
			acceptMouseEvent = false;

		if (m_mayaLeftMousePress || m_mayaRightMousePress || m_midMousePress)
		{
			acceptMouseEvent = true;
			pointerPress = true;
		}

		switch (evt.EventType)
		{
		case EET_KEY_INPUT_EVENT:
			if (evt.KeyInput.Key == KEY_MENU ||
				evt.KeyInput.Key == KEY_LMENU ||
				evt.KeyInput.Key == KEY_RMENU)
			{
				if (!m_altKeyDown && pointerPress)
				{
					// when resume alt after release
					m_centerCursor = m_cursorControl->getRelativePosition();
					m_cursorPos = m_centerCursor;
				}

				m_altKeyDown = evt.KeyInput.PressedDown;
			}
			else if (evt.KeyInput.Key == KEY_SHIFT ||
				evt.KeyInput.Key == KEY_LSHIFT ||
				evt.KeyInput.Key == KEY_RSHIFT)
			{
				m_shiftKeyDown = evt.KeyInput.PressedDown;
			}
			else if (evt.KeyInput.Key == KEY_CONTROL)
			{
				m_ctrlKeyDown = evt.KeyInput.PressedDown;
			}
			break;

		case EET_MOUSE_INPUT_EVENT:
			if (evt.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				m_leftMousePress = true;
			if (evt.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				m_leftMousePress = false;
			if (evt.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
				m_rightMousePress = true;
			if (evt.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
				m_rightMousePress = false;

			if (evt.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN && acceptMouseEvent)
			{
				m_mayaLeftMousePress = true;
				m_centerCursor = m_cursorControl->getRelativePosition();
				m_cursorPos = m_centerCursor;
			}
			else if (evt.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN && acceptMouseEvent)
			{
				m_mayaRightMousePress = true;
				m_centerCursor = m_cursorControl->getRelativePosition();
				m_cursorPos = m_centerCursor;
			}
			else if (evt.MouseInput.Event == EMIE_MMOUSE_PRESSED_DOWN)
			{
				m_midMousePress = true;
				m_centerCursor = m_cursorControl->getRelativePosition();
				m_cursorPos = m_centerCursor;
			}
			else if (evt.MouseInput.Event == EMIE_LMOUSE_LEFT_UP && acceptMouseEvent)
			{
				m_mayaLeftMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_RMOUSE_LEFT_UP && acceptMouseEvent)
			{
				m_mayaRightMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_MMOUSE_LEFT_UP)
			{
				m_midMousePress = false;
			}
			else if (evt.MouseInput.Event == EMIE_MOUSE_MOVED)
			{
				if (m_controlStyle == Maya)
				{
					if (m_mayaLeftMousePress || m_mayaRightMousePress || m_midMousePress)
					{
						m_cursorPos = m_cursorControl->getRelativePosition();
						return true;
					}
				}
				else
				{
					if (m_leftMousePress || m_rightMousePress || m_midMousePress)
					{
						m_cursorPos = m_cursorControl->getRelativePosition();
						return true;
					}
				}
			}
			else if (evt.MouseInput.Event == EMIE_MOUSE_WHEEL)
			{
				m_wheel = evt.MouseInput.Wheel;
				m_mouseWhell = true;
			}
			break;
		case EET_GAME_RESUME:
		{
			m_leftMousePress = false;
			m_rightMousePress = false;
			m_mayaLeftMousePress = false;
			m_mayaRightMousePress = false;
			m_midMousePress = false;
			m_mouseWhell = false;
			m_altKeyDown = false;
			m_shiftKeyDown = false;
			m_ctrlKeyDown = false;
		}
		break;
		default:
			break;
		}

		return false;
	}
}
