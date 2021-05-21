/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
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
			m_mouseState(0)
		{

		}

		CHandles::~CHandles()
		{

		}

		void CHandles::beginCheck()
		{

		}

		bool CHandles::endCheck()
		{
			return false;
		}

		core::vector3df CHandles::positionHandle(const core::vector3df& position)
		{
			m_position = position;
			m_handlePosition = true;
			return position;
		}

		core::vector3df CHandles::scaleHandle(const core::vector3df& scale, const core::vector3df& origin)
		{
			return scale;
		}

		core::quaternion CHandles::rotateHandle(const core::quaternion& rotate, const core::vector3df& origin)
		{
			return rotate;
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

			return false;
		}
	}
}