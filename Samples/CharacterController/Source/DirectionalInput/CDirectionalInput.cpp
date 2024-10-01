#include "pch.h"
#include "SkylichtEngine.h"
#include "CDirectionalInput.h"
#include "GameObject/CGameObject.h"
#include "Context/CContext.h"
#include "EventManager/CEventManager.h"

CDirectionalInput::CDirectionalInput() :
	m_controller(NULL),
	m_moveSpeed(5.0f),
	m_jumpSpeed(15.0f)
{
	m_keyMap.push_back(SKeyMap{ MoveForward , irr::KEY_UP });
	m_keyMap.push_back(SKeyMap{ MoveBackward , irr::KEY_DOWN });
	m_keyMap.push_back(SKeyMap{ StrafeLeft , irr::KEY_LEFT });
	m_keyMap.push_back(SKeyMap{ StrafeRight , irr::KEY_RIGHT });

	m_keyMap.push_back(SKeyMap{ MoveForward , irr::KEY_KEY_W });
	m_keyMap.push_back(SKeyMap{ MoveBackward , irr::KEY_KEY_S });
	m_keyMap.push_back(SKeyMap{ StrafeLeft , irr::KEY_KEY_A });
	m_keyMap.push_back(SKeyMap{ StrafeRight , irr::KEY_KEY_D });

	m_keyMap.push_back(SKeyMap{ Jump , irr::KEY_SPACE });

	for (u32 i = 0; i < DirectionCount; i++)
		m_input[i] = false;
}

CDirectionalInput::~CDirectionalInput()
{
	m_gameObject->getScene()->unRegisterEvent(this);
}

void CDirectionalInput::initComponent()
{
	m_controller = m_gameObject->getComponent<Physics::CCharacterController>();
	m_gameObject->getScene()->registerEvent("CDirectionalInput", this);
}

void CDirectionalInput::updateComponent()
{
	if (!m_controller)
		return;

	f32 timeDiff = getTimeStep();

	// skip lag
	const float delta = 1000.0f / 20.0f;
	if (timeDiff > delta)
		timeDiff = delta;

	// convert to sec
	timeDiff = timeDiff * 0.001f;

	CCamera* camera = CContext::getInstance()->getActiveCamera();

	core::vector3df movedir = camera->getGameObject()->getFront();
	movedir.Y = 0.0f;
	movedir.normalize();

	core::vector3df walkDirection;

	if (m_input[MoveForward])
		walkDirection += movedir * timeDiff * m_moveSpeed;

	if (m_input[MoveBackward])
		walkDirection -= movedir * timeDiff * m_moveSpeed;

	core::vector3df strafevect = movedir;
	strafevect = strafevect.crossProduct(Transform::Oy);

	if (m_input[StrafeLeft])
		walkDirection += strafevect * timeDiff * m_moveSpeed;

	if (m_input[StrafeRight])
		walkDirection -= strafevect * timeDiff * m_moveSpeed;

	if (m_input[Jump])
	{
		if (m_controller->canJump())
		{
			m_controller->setJumpSpeed(m_jumpSpeed);
			m_controller->jump(core::vector3df());
		}
	}

	m_controller->setWalkDirection(walkDirection);
}

bool CDirectionalInput::OnEvent(const SEvent& evt)
{
	switch (evt.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		for (u32 i = 0, n = (u32)m_keyMap.size(); i < n; ++i)
		{
			if (m_keyMap[i].KeyCode == evt.KeyInput.Key)
			{
				m_input[m_keyMap[i].Direction] = evt.KeyInput.PressedDown;
				return true;
			}
		}
		break;
	default:
		break;
	}

	return false;
}