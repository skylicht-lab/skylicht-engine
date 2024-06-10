#pragma once

#include "Components/CComponentSystem.h"
#include "IEventReceiver.h"

class CPlayerController :
	public Skylicht::CComponentSystem,
	public IEventReceiver
{
protected:
	enum EMoveDirection
	{
		MoveForward = 0,
		MoveBackward,
		TurnLeft,
		TurnRight,
		DirectionCount
	};

	struct SKeyMap
	{
		EMoveDirection Direction;
		EKEY_CODE KeyCode;
	};

	std::vector<SKeyMap> m_keyMap;

	bool m_input[DirectionCount];

	f32 m_moveSpeed;
	f32 m_rotSpeed;

public:
	CPlayerController();

	virtual ~CPlayerController();

	virtual void initComponent();

	virtual void updateComponent();

	virtual bool OnEvent(const SEvent& event);

	inline void setMoveSpeed(float speed)
	{
		m_moveSpeed = speed;
	}
};