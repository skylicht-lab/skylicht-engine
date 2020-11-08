/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

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
#include "CSpaceScene.h"
#include "GridPlane/CGridPlane.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceScene::CSpaceScene(GUI::CDockableWindow *window, CEditor *editor) :
			CSpace(window, editor),
			m_scene(NULL),
			m_renderRP(NULL),
			m_editorCamera(NULL),
			m_gridPlane(NULL)
		{
			initDefaultScene();

			m_window->getInnerPanel()->OnRender = BIND_LISTENER(&CSpaceScene::onRender, this);
		}

		CSpaceScene::~CSpaceScene()
		{
			if (m_scene != NULL)
				delete m_scene;

			if (m_renderRP != NULL)
				delete m_renderRP;
		}

		void CSpaceScene::initDefaultScene()
		{
			// create a scene
			m_scene = new CScene();

			// create a zone in Scene
			CZone *zone = m_scene->createZone();

			// create editor camera
			CGameObject *camObj = zone->createEmptyObject();
			camObj->addComponent<CCamera>();
			camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

			m_editorCamera = camObj->getComponent<CCamera>();
			m_editorCamera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
			m_editorCamera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

			// grid
			m_gridPlane = zone->createEmptyObject();
			m_gridPlane->addComponent<CGridPlane>();

			// lighting
			CGameObject *lightObj = zone->createEmptyObject();
			CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
			SColor c(255, 255, 244, 214);
			directionalLight->setColor(SColorf(c));

			CTransformEuler *lightTransform = lightObj->getTransformEuler();
			lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

			core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
			lightTransform->setOrientation(direction, CTransform::s_oy);
		}

		void CSpaceScene::onResize(float w, float h)
		{
			if (w >= 32.0f && h >= 32.0f)
			{
				if (m_renderRP == NULL)
				{
					// rendering pipe line
					m_renderRP = new CForwardRP();
					m_renderRP->initRender((int)w, (int)h);
				}
				else
				{
					// resize
					m_renderRP->resize((int)w, (int)h);
				}
			}
		}

		void CSpaceScene::update()
		{
			m_scene->update();
		}

		void CSpaceScene::onRender(GUI::CBase *base)
		{
			if (m_renderRP != NULL)
			{
				GUI::CGUIContext::getRenderer()->flush();
				core::recti vp = getVideoDriver()->getViewPort();
				getVideoDriver()->enableScissor(false);
				getVideoDriver()->clearZBuffer();

				GUI::SPoint position = base->localPosToCanvas();
				core::recti viewport;
				viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
				viewport.LowerRightCorner.set((int)(position.X + base->width()), (int)(position.Y + base->height()));

				m_renderRP->render(NULL, m_editorCamera, m_scene->getEntityManager(), viewport);

				getVideoDriver()->enableScissor(true);
				getVideoDriver()->setViewPort(vp);
				GUI::CGUIContext::getRenderer()->setProjection();
			}
		}

		bool CSpaceScene::isEditorObject(CGameObject *object)
		{
			if (m_editorCamera->getGameObject() == object ||
				m_gridPlane == object)
			{
				return true;
			}

			return false;
		}
	}
}