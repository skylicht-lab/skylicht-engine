/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CHandles.h"
#include "Scene/CScene.h"
#include "EditorComponents/Handles/CHandlesRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		CHandles::CHandles() :
			m_handlePosition(false),
			m_handleRotation(false),
			m_handleScale(false),
			m_mouseState(0),
			m_endCheck(false)
		{

		}

		CHandles::~CHandles()
		{

		}

		void CHandles::end()
		{
			m_handlePosition = false;
			m_handleRotation = false;
			m_handleScale = false;
		}

		bool CHandles::endCheck()
		{
			bool ret = m_endCheck;
			if (m_endCheck == true)
				m_endCheck = false;
			return ret;
		}

		core::vector3df CHandles::positionHandle(const core::vector3df& position, const core::vector3df& localRotation)
		{
			if (!m_handlePosition)
				m_targetPosition = position;

			m_handlePosition = true;
			m_handleRotation = false;
			m_handleScale = false;

			m_position = position;
			m_rotation = localRotation;

			return m_targetPosition;
		}

		core::vector3df CHandles::scaleHandle(const core::vector3df& scale, const core::vector3df& origin, const core::vector3df& localRotation)
		{
			if (!m_handleScale)
				m_targetScale = scale;

			m_handleRotation = false;
			m_handlePosition = false;
			m_handleScale = true;

			m_rotation = localRotation;
			m_position = origin;
			m_scale = scale;
			return m_targetScale;
		}

		core::quaternion CHandles::rotateHandle(const core::quaternion& rotate, const core::vector3df& origin)
		{
			if (!m_handleRotation)
				m_targetRotation = rotate;

			m_handleRotation = true;
			m_handlePosition = false;
			m_handleScale = false;

			m_position = origin;
			m_rotation = rotate;
			return m_targetRotation;
		}

		bool CHandles::OnEvent(const SEvent& event)
		{
			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				int mouseX = event.MouseInput.X;
				int mouseY = event.MouseInput.Y;

				CScene* scene = (CScene*)event.GameEvent.Sender;
				CHandlesRenderer* handleRenderer = scene->getEntityManager()->getSystem<CHandlesRenderer>();

				if (handleRenderer != NULL)
				{
					if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
					{
						if (m_mouseState == 2)
							m_mouseState = 0;
						handleRenderer->onMouseEvent(mouseX, mouseY, m_mouseState);
					}
					else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
					{
						m_mouseState = 1;
						handleRenderer->onMouseEvent(mouseX, mouseY, m_mouseState);
					}
					else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
					{
						m_mouseState = 2;
						handleRenderer->onMouseEvent(mouseX, mouseY, m_mouseState);
					}

					return true;
				}
			}
			else if (event.EventType == EET_KEY_INPUT_EVENT)
			{
				CScene* scene = (CScene*)event.GameEvent.Sender;
				CHandlesRenderer* handleRenderer = scene->getEntityManager()->getSystem<CHandlesRenderer>();

				if (event.KeyInput.PressedDown && event.KeyInput.Key == irr::KEY_ESCAPE)
				{
					if (m_mouseState == 1)
					{
						handleRenderer->cancel();
					}
					return true;
				}
			}

			return false;
		}
	}
}