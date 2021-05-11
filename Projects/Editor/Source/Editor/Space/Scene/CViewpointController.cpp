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
#include "CViewpointController.h"

namespace Skylicht
{
	namespace Editor
	{
		CViewpointController::CViewpointController() :
			m_viewpointCamera(NULL),
			m_editorCamera(NULL)
		{

		}

		CViewpointController::~CViewpointController()
		{
		}

		void CViewpointController::update()
		{
			updateViewpoint();
		}

		void CViewpointController::updateViewpoint()
		{
			if (m_editorCamera == NULL || m_viewpointCamera == NULL)
				return;

			core::vector3df look = m_editorCamera->getLookVector();
			core::vector3df up = m_editorCamera->getUpVector();

			float distance = 2.2f;
			core::vector3df pos = -look * distance;

			m_viewpointCamera->setPosition(pos);
			m_viewpointCamera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), up);
		}

		void CViewpointController::setupCameraTween(CTweenVector3df* look, CTweenVector3df* up)
		{
			look->OnUpdate = [camera = m_editorCamera, up, look](CTween* t)
			{
				camera->setUpVector(up->getValue());
				camera->setLookVector(look->getValue());
			};

			up->setEase(EEasingFunctions::EaseLinear);
			look->setEase(EEasingFunctions::EaseLinear);

			CTweenManager::getInstance()->addTween(up);
			CTweenManager::getInstance()->addTween(look);
		}

		void CViewpointController::setCameraLook(CViewpointData::EAxis axis)
		{
			if (m_editorCamera == NULL)
				return;

			float time = 350.0f;

			core::vector3df look = m_editorCamera->getLookVector();
			core::vector3df up = m_editorCamera->getUpVector();

			if (axis == CViewpointData::X)
			{
				CTweenVector3df* tweenLook = new CTweenVector3df(look, core::vector3df(-1.0f, 0.0f, 0.0f), time);
				CTweenVector3df* tweenUp = new CTweenVector3df(up, core::vector3df(0.0f, 1.0f, 0.0f), time);
				setupCameraTween(tweenLook, tweenUp);
			}
			else if (axis == CViewpointData::XNeg)
			{
				CTweenVector3df* tweenLook = new CTweenVector3df(look, core::vector3df(1.0f, 0.0f, 0.0f), time);
				CTweenVector3df* tweenUp = new CTweenVector3df(up, core::vector3df(0.0f, 1.0f, 0.0f), time);
				setupCameraTween(tweenLook, tweenUp);
			}
			else if (axis == CViewpointData::Z)
			{
				CTweenVector3df* tweenLook = new CTweenVector3df(look, core::vector3df(0.0f, 0.0f, -1.0f), time);
				CTweenVector3df* tweenUp = new CTweenVector3df(up, core::vector3df(0.0f, 1.0f, 0.0f), time);
				setupCameraTween(tweenLook, tweenUp);
			}
			else if (axis == CViewpointData::ZNeg)
			{
				CTweenVector3df* tweenLook = new CTweenVector3df(look, core::vector3df(0.0f, 0.0f, 1.0f), time);
				CTweenVector3df* tweenUp = new CTweenVector3df(up, core::vector3df(0.0f, 1.0f, 0.0f), time);
				setupCameraTween(tweenLook, tweenUp);
			}
			else if (axis == CViewpointData::Y)
			{
				CTweenVector3df* tweenLook = new CTweenVector3df(look, core::vector3df(0.0f, -1.0f, 0.0f), time);
				CTweenVector3df* tweenUp = new CTweenVector3df(up, core::vector3df(0.0f, 0.0f, 1.0f), time);
				setupCameraTween(tweenLook, tweenUp);
			}
			else if (axis == CViewpointData::YNeg)
			{
				CTweenVector3df* tweenLook = new CTweenVector3df(look, core::vector3df(0.0f, 1.0f, 0.0f), time);
				CTweenVector3df* tweenUp = new CTweenVector3df(up, core::vector3df(0.0f, 0.0f, 1.0f), time);
				setupCameraTween(tweenLook, tweenUp);
			}
		}
	}
}