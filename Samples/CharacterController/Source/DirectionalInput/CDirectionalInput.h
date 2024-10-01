#pragma once

#include "CharacterController/CCharacterController.h"

class CDirectionalInput :
	public CComponentSystem,
	public IEventReceiver
{
protected:
	Physics::CCharacterController* m_controller;

	enum EMoveDirection
	{
		MoveForward = 0,
		MoveBackward,
		StrafeLeft,
		StrafeRight,
		DirectionCount
	};

	struct SKeyMap
	{
		EMoveDirection Direction;
		EKEY_CODE KeyCode;
	};

	CCamera* m_camera;

	std::vector<SKeyMap> m_keyMap;

	bool m_input[DirectionCount];

	f32 m_moveSpeed;
public:
	CDirectionalInput();

	virtual ~CDirectionalInput();

	virtual void initComponent();

	virtual void updateComponent();

	virtual bool OnEvent(const SEvent& event);
};