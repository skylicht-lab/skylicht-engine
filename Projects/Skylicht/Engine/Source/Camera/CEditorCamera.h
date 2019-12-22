#ifndef _EDITOR_CAMERA_H_
#define _EDITOR_CAMERA_H_

#include "Components/CComponentSystem.h"

namespace Skylicht
{
	class CEditorCamera : 
		public CComponentSystem,
		public IEventReceiver
	{
	protected:
		f32	m_moveSpeed;
		f32 m_rotateSpeed;
		gui::ICursorControl *m_cursorControl;

		core::position2df m_centerCursor;
		core::position2df m_cursorPos;

		bool	m_leftMousePress;
		bool	m_rightMousePress;
		bool	m_midMousePress;

		bool	m_mouseWhell;
		float	m_wheel;

	public:
		CEditorCamera();

		virtual ~CEditorCamera();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void endUpdate();

		virtual bool OnEvent(const SEvent& event);

	public:
		inline void setRotateSpeed(float speed)
		{
			m_rotateSpeed = speed;
		}

		inline void setMoveSpeed(float speed)
		{
			m_moveSpeed = speed;
		}

		void updateInputRotate(core::vector3df &relativeRotation, f32 timeDiff);

		void updateInputOffset(core::vector3df &offsetPosition, f32 timeDiff);
	};
}

#endif