#include "pch.h"
#include "CPlayerController.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

CPlayerController::CPlayerController() :
	m_moveSpeed(1.0f),
	m_rotSpeed(50.0f)
{
	m_keyMap.push_back(SKeyMap{ MoveForward , irr::KEY_UP });
	m_keyMap.push_back(SKeyMap{ MoveBackward , irr::KEY_DOWN });
	m_keyMap.push_back(SKeyMap{ TurnLeft , irr::KEY_LEFT });
	m_keyMap.push_back(SKeyMap{ TurnRight , irr::KEY_RIGHT });

	m_keyMap.push_back(SKeyMap{ MoveForward , irr::KEY_KEY_W });
	m_keyMap.push_back(SKeyMap{ MoveBackward , irr::KEY_KEY_S });
	m_keyMap.push_back(SKeyMap{ TurnLeft , irr::KEY_KEY_A });
	m_keyMap.push_back(SKeyMap{ TurnRight , irr::KEY_KEY_D });

	for (u32 i = 0; i < DirectionCount; i++)
		m_input[i] = false;
}

CPlayerController::~CPlayerController()
{
	m_gameObject->getScene()->unRegisterEvent(this);
}

void CPlayerController::initComponent()
{
	m_gameObject->getScene()->registerEvent(getTypeName(), this);
}

void CPlayerController::updateComponent()
{
	f32 timeDiff = getTimeStep();

	// skip lag
	const float delta = 1000.0f / 20.0f;
	if (timeDiff > delta)
		timeDiff = delta;

	// convert to sec
	timeDiff = timeDiff * 0.001f;

	CTransformEuler* transform = m_gameObject->getTransformEuler();

	core::vector3df pos = transform->getPosition();
	core::vector3df lastPos = pos;

	float yaw = transform->getYaw();

	core::vector3df movedir = transform->getFront();
	movedir.normalize();

	if (m_input[MoveForward])
		pos += movedir * timeDiff * m_moveSpeed;

	if (m_input[MoveBackward])
		pos -= movedir * timeDiff * m_moveSpeed;

	if (m_input[TurnLeft])
		yaw = yaw - timeDiff * m_rotSpeed;

	if (m_input[TurnRight])
		yaw = yaw + timeDiff * m_rotSpeed;

	transform->setPosition(pos);
	transform->setYaw(yaw);
}

bool CPlayerController::OnEvent(const SEvent& evt)
{
	if (m_gameObject->isVisible() == false ||
		m_gameObject->isEnable() == false)
	{
		return false;
	}

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