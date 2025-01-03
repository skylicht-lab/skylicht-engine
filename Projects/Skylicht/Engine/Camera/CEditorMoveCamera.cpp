#include "pch.h"
#include "CCamera.h"
#include "CEditorMoveCamera.h"
#include "CEditorCamera.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	CEditorMoveCamera::CEditorMoveCamera()
	{

	}

	CEditorMoveCamera::~CEditorMoveCamera()
	{

	}

	void CEditorMoveCamera::initComponent()
	{
		CFpsMoveCamera::initComponent();
	}

	void CEditorMoveCamera::updateComponent()
	{
		CFpsMoveCamera::updateComponent();
	}

	bool CEditorMoveCamera::OnEvent(const SEvent& evt)
	{
		if (m_camera && !m_camera->isInputReceiverEnabled())
			return false;

		CEditorCamera* editorCamera = m_camera->getGameObject()->getComponent<CEditorCamera>();
		bool rightMousePress = editorCamera ? editorCamera->isRightMousePressed() : false;

		switch (evt.EventType)
		{
		case EET_KEY_INPUT_EVENT:
			for (u32 i = 0, n = (u32)m_keyMap.size(); i < n; ++i)
			{
				bool arrowKey = false;

				if (evt.KeyInput.Key == irr::KEY_UP ||
					evt.KeyInput.Key == irr::KEY_DOWN ||
					evt.KeyInput.Key == irr::KEY_LEFT ||
					evt.KeyInput.Key == irr::KEY_RIGHT)
				{
					arrowKey = true;
				}

				if (m_keyMap[i].KeyCode == evt.KeyInput.Key)
				{
					if (arrowKey || rightMousePress)
						m_input[m_keyMap[i].Direction] = evt.KeyInput.PressedDown;
					else
						m_input[m_keyMap[i].Direction] = false;

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