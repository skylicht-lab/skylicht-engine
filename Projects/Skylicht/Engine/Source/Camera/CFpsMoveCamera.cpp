#include "pch.h"
#include "CCamera.h"
#include "CFpsMoveCamera.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	CFpsMoveCamera::CFpsMoveCamera() :
		m_moveSpeed(1.0f)
	{
		m_keyMap.push_back(SKeyMap{ MoveForward , irr::KEY_UP });
		m_keyMap.push_back(SKeyMap{ MoveBackward , irr::KEY_DOWN });
		m_keyMap.push_back(SKeyMap{ StrafeLeft , irr::KEY_LEFT });
		m_keyMap.push_back(SKeyMap{ StrafeRight , irr::KEY_RIGHT });

		m_keyMap.push_back(SKeyMap{ MoveForward , irr::KEY_KEY_W });
		m_keyMap.push_back(SKeyMap{ MoveBackward , irr::KEY_KEY_S });
		m_keyMap.push_back(SKeyMap{ StrafeLeft , irr::KEY_KEY_A });
		m_keyMap.push_back(SKeyMap{ StrafeRight , irr::KEY_KEY_D });

		for (u32 i = 0; i < DirectionCount; i++)
			m_input[i] = false;
	}

	CFpsMoveCamera::~CFpsMoveCamera()
	{
		getGameObject()->getScene()->unRegisterEvent(this);
	}

	void CFpsMoveCamera::initComponent()
	{
		getGameObject()->getScene()->registerEvent("CFpsMoveCamera", this);
	}

	void CFpsMoveCamera::updateComponent()
	{

	}

	void CFpsMoveCamera::endUpdate()
	{
		CTransformEuler *transform = m_gameObject->getTransformEuler();
		CCamera *camera = m_gameObject->getComponent<CCamera>();

		if (camera == NULL || transform == NULL)
			return;

		if (!camera->isInputReceiverEnabled())
			return;

		f32 timeDiff = getTimeStep();

		// skip lag
		const float delta = 1000.0f / 20.0f;
		if (timeDiff > delta)
			timeDiff = delta;

		// convert to sec
		timeDiff = timeDiff * 0.001f;

		core::vector3df pos = transform->getPosition();

		core::vector3df movedir = transform->getFront();
		movedir.normalize();

		if (m_input[MoveForward])
			pos += movedir * timeDiff * m_moveSpeed;

		if (m_input[MoveBackward])
			pos -= movedir * timeDiff * m_moveSpeed;

		core::vector3df strafevect = movedir;
		strafevect = strafevect.crossProduct(camera->getUpVector());

		if (m_input[StrafeLeft])
			pos += strafevect * timeDiff * m_moveSpeed;

		if (m_input[StrafeRight])
			pos -= strafevect * timeDiff * m_moveSpeed;

		camera->setPosition(pos);
	}

	bool CFpsMoveCamera::OnEvent(const SEvent& evt)
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
}