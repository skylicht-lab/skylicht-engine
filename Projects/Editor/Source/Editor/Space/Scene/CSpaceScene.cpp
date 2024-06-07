/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CSpaceScene.h"
#include "GUI/Input/CInput.h"
#include "GridPlane/CGridPlane.h"
#include "SpriteDraw/CSprite.h"
#include "SkySun/CSkySun.h"
#include "LightProbes/CLightProbes.h"
#include "ReflectionProbe/CReflectionProbe.h"

#include "Editor/CEditor.h"
#include "Handles/CHandles.h"
#include "Selection/CSelection.h"
#include "Selection/CSelecting.h"
#include "Projective/CProjective.h"

#include "Editor/SpaceController/CSceneController.h"
#include "Editor/SpaceController/CPropertyController.h"

#include "Editor/Gizmos/Transform/CTransformGizmos.h"

using namespace std::placeholders;

namespace Skylicht
{
	namespace Editor
	{
		CSpaceScene::CSpaceScene(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_scene(NULL),
			m_renderRP(NULL),
			m_shadowMapRendering(NULL),
			m_forwardRP(NULL),
			m_postProcessor(NULL),
			m_viewpointRP(NULL),
			m_editorCamera(NULL),
			m_gridPlane(NULL),
			m_leftMouseDown(false),
			m_rightMouseDown(false),
			m_middleMouseDown(false),
			m_mouseX(0.0f),
			m_mouseY(0.0f),
			m_viewpointZone(NULL),
			m_viewpoint(NULL),
			m_viewpointCamera(NULL),
			m_viewpointController(NULL),
			m_view(NULL),
			m_handlesRenderer(NULL),
			m_gizmosRenderer(NULL),
			m_selectObjectSystem(NULL),
			m_enableRender(true)
		{
			CScene* currentScene = CSceneController::getInstance()->getScene();
			if (currentScene == NULL)
				initDefaultScene();
			else
			{
				m_scene = currentScene;
				reinitCurrentScene();
			}

			GUI::CToolbar* toolbar = new GUI::CToolbar(window);
			m_groupEditor = new GUI::CToggleGroup();
			m_groupTransform = new GUI::CToggleGroup();
			m_groupCameraView = new GUI::CToggleGroup();

			GUI::CButton* button = NULL;

			// select & hand
			button = toolbar->addButton(L"Select", GUI::ESystemIcon::ViewSelect);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onEditorSelect, this);
			m_toolbarButton[ESceneToolBar::Select] = button;
			m_groupEditor->addButton(button);

			button = toolbar->addButton(L"Hand", GUI::ESystemIcon::ViewHand);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onEditorHand, this);
			m_toolbarButton[ESceneToolBar::Hand] = button;
			m_groupEditor->addButton(button);

			toolbar->addSpace();

			// transform			
			button = toolbar->addButton(L"Move", GUI::ESystemIcon::Move);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onToolbarTransform, this);
			m_toolbarButton[ESceneToolBar::Move] = button;
			m_groupTransform->addButton(button);

			button = toolbar->addButton(L"Rotate", GUI::ESystemIcon::Rotate);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onToolbarTransform, this);
			m_toolbarButton[ESceneToolBar::Rotate] = button;
			m_groupTransform->addButton(button);

			button = toolbar->addButton(L"Scale", GUI::ESystemIcon::Scale);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onToolbarTransform, this);
			m_toolbarButton[ESceneToolBar::Scale] = button;
			m_groupTransform->addButton(button);

			toolbar->addSpace();

			GUI::CComboBox* worldSpaceDropDown = (GUI::CComboBox*)toolbar->addControl(new GUI::CComboBox(toolbar));
			worldSpaceDropDown->setWidth(120.0f);
			worldSpaceDropDown->addItem(L"Local space");
			worldSpaceDropDown->addItem(L"World space");
			worldSpaceDropDown->setSelectIndex(0, false);
			worldSpaceDropDown->setDisabled(true);
			worldSpaceDropDown->OnChanged = BIND_LISTENER(&CSpaceScene::onToolbarWorldSpace, this);
			m_toolbarButton[World] = worldSpaceDropDown;

			toolbar->addSpace();

			button = toolbar->addButton(L"Snap", GUI::ESystemIcon::Snap);
			m_snapSettingMenu = new GUI::CMenu(window->getCanvas());
			m_snapSettingController = new CSnapSettingController(editor, m_snapSettingMenu);
			button->setToggle(true);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onSnapSetting, this);
			m_toolbarButton[Snap] = button;

			CHandles* handles = CHandles::getInstance();
			if (handles->isSnapRotate() ||
				handles->isSnapXZ() ||
				handles->isSnapY())
			{
				button->setIsToggle(true);
			}
			else
			{
				button->setIsToggle(false);
			}

			getSubjectTransformGizmos().addObserver(this);

			// camera			
			button = toolbar->addButton(L"Ortho", GUI::ESystemIcon::Ortho, true);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onCameraOrtho, this);
			m_toolbarButton[ESceneToolBar::Ortho] = button;
			m_groupCameraView->addButton(button);

			button = toolbar->addButton(L"Perspective", GUI::ESystemIcon::Perspective, true);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onCameraPerspective, this);
			m_toolbarButton[ESceneToolBar::Perspective] = button;
			m_groupCameraView->addButton(button);

			button = toolbar->addButton(L"Camera", GUI::ESystemIcon::Camera, true);
			m_cameraSettingMenu = new GUI::CMenu(window->getCanvas());
			m_cameraSettingController = new CCameraSettingController(editor, m_cameraSettingMenu);
			button->OnPress = BIND_LISTENER(&CSpaceScene::onCameraSetting, this);

			// init group
			m_groupEditor->enable(true);
			m_groupEditor->selectButton(m_toolbarButton[ESceneToolBar::Select]);

			m_groupCameraView->enable(true);
			m_groupCameraView->selectButton(m_toolbarButton[ESceneToolBar::Perspective]);

			m_groupTransform->selectButton(m_toolbarButton[ESceneToolBar::Move]);
			m_groupTransform->enable(false);

			// 3d view
			m_view = new GUI::CBase(window);
			m_view->dock(GUI::EPosition::Fill);

			m_view->OnRender = BIND_LISTENER(&CSpaceScene::onRender, this);
			m_view->OnMouseMoved = std::bind(&CSpaceScene::onMouseMoved, this, _1, _2, _3, _4, _5);
			m_view->OnLeftMouseClick = std::bind(&CSpaceScene::onLeftMouseClick, this, _1, _2, _3, _4);
			m_view->OnRightMouseClick = std::bind(&CSpaceScene::onRightMouseClick, this, _1, _2, _3, _4);
			m_view->OnMiddleMouseClick = std::bind(&CSpaceScene::onMiddleMouseClick, this, _1, _2, _3, _4);
			m_view->OnMouseWheeled = std::bind(&CSpaceScene::onMouseWheeled, this, _1, _2);

			// KEYBOARD
			m_view->setKeyboardInputEnabled(true);

			m_view->OnKeyPress = std::bind(&CSpaceScene::onKeyPressed, this, _1, _2, _3);

			m_view->addAccelerator("G", [&](GUI::CBase* base) {this->onHotkey(base, "G"); });
			m_view->addAccelerator("R", [&](GUI::CBase* base) {this->onHotkey(base, "R"); });
			m_view->addAccelerator("S", [&](GUI::CBase* base) {this->onHotkey(base, "S"); });
			m_view->addAccelerator("Ctrl + C", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + C"); });
			m_view->addAccelerator("Ctrl + V", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + V"); });
			m_view->addAccelerator("Ctrl + D", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + D"); });
			m_view->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + Z"); });
			m_view->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->onHotkey(base, "Ctrl + Y"); });

			m_view->OnResize = BIND_LISTENER(&CSpaceScene::onRenderResize, this);

			m_view->OnAcceptDragDrop = [](GUI::SDragDropPackage* data)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
						bool isFolder = rowItem->getTagBool();
						if (isFolder)
							return false;

						std::string path = rowItem->getTagString();
						std::string fileExt = CPath::getFileNameExt(path);
						fileExt = CStringImp::toLower(fileExt);
						if (fileExt == "dae" ||
							fileExt == "obj" ||
							fileExt == "fbx" ||
							fileExt == "smesh")
						{
							return true;
						}
					}
					return false;
				};

			m_view->OnDrop = [&](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::SPoint viewPoint = m_view->canvasPosToLocal(GUI::SPoint(mouseX, mouseY));

						core::line3df ray = CProjective::getViewRay(
							m_editorCamera,
							(float)viewPoint.X, (float)viewPoint.Y,
							m_sceneRect.getWidth(),
							m_sceneRect.getHeight()
						);

						core::vector3df out;
						core::plane3df p;
						p.setPlane(core::vector3df(0.0f, 1.0f, 0.0f), 0.0f);
						if (p.getIntersectionWithLimitedLine(ray.start, ray.end, out))
						{
							CSceneController* sceneController = CSceneController::getInstance();

							CGameObject* targetObject = sceneController->createEmptyObject(NULL);
							targetObject->getTransformEuler()->setPosition(out);

							if (targetObject != NULL)
							{
								GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
								std::string path = rowItem->getTagString();

								sceneController->createResourceComponent(path, targetObject);
								CHierachyNode* node = sceneController->selectOnHierachy(targetObject);
								sceneController->onSelectNode(node, true);
								sceneController->updateTreeNode(targetObject);
							}
						}
					}
				};

			GUI::SDimension size = window->getSize();
			initRenderPipeline(size.Width, size.Height);

			CSceneController::getInstance()->setSpaceScene(this);
		}

		CSpaceScene::~CSpaceScene()
		{
			if (m_viewpointRP != NULL)
			{
				delete m_viewpointRP;
				m_viewpointRP = NULL;
			}

			if (m_rendering != NULL)
			{
				delete m_rendering;
				m_rendering = NULL;
			}

			if (m_shadowMapRendering != NULL)
			{
				delete m_shadowMapRendering;
				m_shadowMapRendering = NULL;
			}

			if (m_forwardRP != NULL)
			{
				delete m_forwardRP;
				m_forwardRP = NULL;
			}

			if (m_postProcessor != NULL)
			{
				delete m_postProcessor;
				m_postProcessor = NULL;
			}

			if (m_viewpointController != NULL)
				delete m_viewpointController;

			delete m_groupEditor;
			delete m_groupTransform;
			delete m_groupCameraView;

			delete m_snapSettingController;
			delete m_cameraSettingController;

			getSubjectTransformGizmos().removeObserver(this);

			CSceneController* sceneController = CSceneController::getInstance();
			sceneController->setSpaceScene(NULL);
		}

		void CSpaceScene::initEditorObject(CZone* zone)
		{
			// create editor camera
			CGameObject* camObj = zone->createEmptyObject();
			camObj->setName(L"EditorCamera");
			camObj->setEditorObject(true);
			camObj->addComponent<CCamera>();
			camObj->addComponent<CEditorCamera>()->setMoveSpeed(1.0f);

			m_editorCamera = camObj->getComponent<CCamera>();
			m_editorCamera->setPosition(core::vector3df(-2.0f, 1.5f, -2.0f));
			m_editorCamera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

			if (m_renderSize.Height > 0)
				m_editorCamera->setAspect(m_renderSize.Width / m_renderSize.Height);

			// grid
			m_gridPlane = zone->createEmptyObject();
			m_gridPlane->setName(L"Grid3D");
			m_gridPlane->setEditorObject(true);
			m_gridPlane->addComponent<CGridPlane>();


			// viewpoint zone
			m_viewpointZone = m_scene->createZone();
			m_viewpointZone->setName(L"ViewpointZone");
			m_viewpointZone->setEditorObject(true);

			// gui canvas
			CGameObject* guiCanvas = zone->createEmptyObject();
			guiCanvas->setName(L"GUICanvas");
			guiCanvas->setEditorObject(true);

			CCanvas* canvas = guiCanvas->addComponent<CCanvas>();
			canvas->IsInEditor = true;

			/*
			CGUIRect* rect = canvas->createRect(SColor(255, 0, 0, 0));
			rect->setDock(EGUIDock::DockFill);
			rect->setName("Canvas");
			*/

			CGUIElement* root = canvas->getRootElement();
			root->setDock(EGUIDock::NoDock);
			root->setRect(core::rectf(0.0f, 0.0f, 1920.0f, 1080.0f));

			// camera			
			camObj = zone->createEmptyObject();
			camObj->setName(L"GUICamera");
			camObj->setEditorObject(true);
			camObj->addComponent<CCamera>()->setProjectionType(CCamera::OrthoUI);

			camObj = m_viewpointZone->createEmptyObject();
			camObj->setName(L"ViewpointCamera");
			camObj->setEditorObject(true);
			camObj->addComponent<CCamera>();

			m_viewpointCamera = camObj->getComponent<CCamera>();
			m_viewpointCamera->setAspect(1.0f);

			// viewpoint
			CGameObject* viewpoint = m_viewpointZone->createEmptyObject();
			viewpoint->setName(L"Viewpoint");
			m_viewpoint = viewpoint->addComponent<CViewpoint>();

			// delete old viewpoint
			if (m_viewpointController != NULL)
				delete m_viewpointController;

			m_viewpointController = new CViewpointController();
			m_viewpointController->setCamera(m_editorCamera, m_viewpointCamera);

			// add handle renderer
			CEntityManager* entityMgr = m_scene->getEntityManager();

			m_selectObjectSystem = entityMgr->addSystem<CSelectObjectSystem>();

			m_handlesRenderer = entityMgr->addRenderSystem<CHandlesRenderer>();
			m_gizmosRenderer = entityMgr->addRenderSystem<CGizmosRenderer>();

			// register event
			m_scene->registerEvent("Handles", CHandles::getInstance());
			m_scene->registerEvent("Selecting", CSelecting::getInstance());

			m_scene->updateIndexSearchObject();
		}

		CScene* CSpaceScene::initNullScene()
		{
			if (m_scene != NULL)
				delete m_scene;

			m_scene = new CScene();

			m_viewpointController = NULL;

			return m_scene;
		}

		void CSpaceScene::reinitCurrentScene()
		{
			m_editorCamera = m_scene->searchObjectInChild(L"EditorCamera")->getComponent<CCamera>();
			m_gridPlane = m_scene->searchObjectInChild(L"Grid3D");
			m_viewpointZone = (CZone*)m_scene->searchObjectInChild(L"ViewpointZone");
			m_viewpoint = m_scene->searchObjectInChild(L"Viewpoint")->getComponent<CViewpoint>();

			if (m_viewpointController != NULL)
				delete m_viewpointController;

			m_viewpointController = new CViewpointController();
			m_viewpointController->setCamera(m_editorCamera, m_viewpointCamera);

			CEntityManager* entityMgr = m_scene->getEntityManager();
			m_selectObjectSystem = entityMgr->getSystem<CSelectObjectSystem>();
			m_handlesRenderer = entityMgr->getSystem<CHandlesRenderer>();
			m_gizmosRenderer = entityMgr->getSystem<CGizmosRenderer>();
		}

		void CSpaceScene::initDefaultScene()
		{
			// create a scene
			m_scene = new CScene();

			// create a zone in scene
			CZone* zone = m_scene->createZone();

			initEditorObject(zone);

			// lighting
			CGameObject* lightObj = zone->createEmptyObject();
			lightObj->setName(L"Direction Light");

			CTransformEuler* lightTransform = lightObj->getTransformEuler();
			lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

			core::vector3df direction = core::vector3df(0.0f, -1.5f, -2.0f);
			lightTransform->setOrientation(direction, Transform::Oy);

			CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
			SColor c(255, 255, 244, 214);
			directionalLight->setColor(SColorf(c));

			// sky & sun
			CGameObject* skySunObj = zone->createEmptyObject();
			skySunObj->setName(L"Sky");
			skySunObj->addComponent<CSkySun>();

			// lightprobres
			CGameObject* lightProbesObj = zone->createEmptyObject();
			lightProbesObj->setName(L"Light Probes");
			lightProbesObj->addComponent<CLightProbes>();

			// reflection probe
			CGameObject* reflectionProbesObj = zone->createEmptyObject();
			reflectionProbesObj->setName(L"Reflection Probe");
			reflectionProbesObj->addComponent<CReflectionProbe>();
			reflectionProbesObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 2.0f, 0.0f));

			// update search index
			m_scene->updateAddRemoveObject();
			m_scene->updateIndexSearchObject();

			// set scene to controller
			CSceneController::getInstance()->setScene(m_scene);
		}

		void CSpaceScene::onResize(float w, float h)
		{
			initRenderPipeline(w, h);
		}

		void CSpaceScene::onRenderResize(GUI::CBase* base)
		{
			initRenderPipeline(base->getSize().Width, base->getSize().Height);
		}

		// toolbar
		void CSpaceScene::onSnapSetting(GUI::CBase* base)
		{
			m_snapSettingMenu->open(base);
			m_snapSettingController->onShow();
		}

		void CSpaceScene::updateSnapButton()
		{
			CHandles* handles = CHandles::getInstance();
			if (handles->isSnapRotate() ||
				handles->isSnapXZ() ||
				handles->isSnapY())
			{
				m_toolbarButton[Snap]->setIsToggle(true);
			}
			else
			{
				m_toolbarButton[Snap]->setIsToggle(false);
			}
		}

		void CSpaceScene::onCameraSetting(GUI::CBase* base)
		{
			m_cameraSettingMenu->open(base);
			m_cameraSettingController->onShow();
		}

		void CSpaceScene::onCameraPerspective(GUI::CBase* base)
		{
			m_groupCameraView->selectButton(m_toolbarButton[ESceneToolBar::Perspective]);
			if (m_editorCamera != NULL)
				m_editorCamera->setProjectionType(CCamera::Perspective);
		}

		void CSpaceScene::onCameraOrtho(GUI::CBase* base)
		{
			m_groupCameraView->selectButton(m_toolbarButton[ESceneToolBar::Ortho]);
			if (m_editorCamera != NULL)
				m_editorCamera->setProjectionType(CCamera::Ortho);
		}

		void CSpaceScene::onEditorSelect(GUI::CBase* base)
		{
			if (CSelection::getInstance()->getSelected().size() == 0)
			{
				m_groupTransform->enable(false);
				m_toolbarButton[World]->setDisabled(true);
			}
			else
			{
				m_groupTransform->enable(true);
				m_toolbarButton[World]->setDisabled(false);

				GUI::CButton* button = m_groupTransform->getSelectButton();
				if (button != NULL)
				{
					CSubject<ETransformGizmo>& gizmos = getSubjectTransformGizmos();
					if (button == m_toolbarButton[ESceneToolBar::Move])
						gizmos.set(ETransformGizmo::Translate);
					else if (button == m_toolbarButton[ESceneToolBar::Rotate])
						gizmos.set(ETransformGizmo::Rotate);
					else if (button == m_toolbarButton[ESceneToolBar::Scale])
						gizmos.set(ETransformGizmo::Scale);
					gizmos.notify(this);
				}
			}

			m_groupEditor->selectButton(m_toolbarButton[ESceneToolBar::Select]);
			m_view->setCursor(GUI::ECursorType::Normal);
		}

		void CSpaceScene::onEditorHand(GUI::CBase* base)
		{
			m_groupTransform->enable(false);
			m_toolbarButton[World]->setDisabled(true);

			CSubject<ETransformGizmo>& gizmos = getSubjectTransformGizmos();
			gizmos.set(ETransformGizmo::None);
			gizmos.notify(this);

			m_groupEditor->selectButton(m_toolbarButton[ESceneToolBar::Hand]);
			m_view->setCursor(GUI::ECursorType::Finger);
		}

		void CSpaceScene::onToolbarTransform(GUI::CBase* base)
		{
			CSubject<ETransformGizmo>& gizmos = getSubjectTransformGizmos();
			ETransformGizmo type = gizmos.get();

			if (base == m_toolbarButton[ESceneToolBar::Move] && type != ETransformGizmo::Translate)
			{
				gizmos.set(ETransformGizmo::Translate);
				gizmos.notify(this);
			}
			else if (base == m_toolbarButton[ESceneToolBar::Rotate] && type != ETransformGizmo::Rotate)
			{
				gizmos.set(ETransformGizmo::Rotate);
				gizmos.notify(this);
			}
			else if (base == m_toolbarButton[ESceneToolBar::Scale] && type != ETransformGizmo::Scale)
			{
				gizmos.set(ETransformGizmo::Scale);
				gizmos.notify(this);
			}

			m_groupTransform->selectButton(dynamic_cast<GUI::CButton*>(base));
		}

		void CSpaceScene::onToolbarWorldSpace(GUI::CBase* base)
		{
			CHandles* handles = CHandles::getInstance();
			GUI::CComboBox* btn = (GUI::CComboBox*)base;
			if (btn->getSelectIndex() == 0)
				handles->setUseLocalSpace(true);
			else
				handles->setUseLocalSpace(false);
			m_editor->refresh();
		}

		void CSpaceScene::onNotify(ISubject* subject, IObserver* from)
		{
			if (from == this)
				return;

			CSubject<ETransformGizmo>& gizmos = getSubjectTransformGizmos();
			if (subject == &gizmos)
			{
				ETransformGizmo type = gizmos.get();
				switch (type)
				{
				case ETransformGizmo::None:
					m_groupTransform->enable(false);
					m_toolbarButton[World]->setDisabled(true);
					break;
				case ETransformGizmo::Translate:
					m_groupTransform->selectButton(m_toolbarButton[ESceneToolBar::Move]);
					m_groupTransform->enable(true);
					m_toolbarButton[World]->setDisabled(false);
					break;
				case ETransformGizmo::Rotate:
					m_groupTransform->selectButton(m_toolbarButton[ESceneToolBar::Rotate]);
					m_groupTransform->enable(true);
					m_toolbarButton[World]->setDisabled(false);
					break;
				case ETransformGizmo::Scale:
					m_groupTransform->selectButton(m_toolbarButton[ESceneToolBar::Scale]);
					m_groupTransform->enable(true);
					m_toolbarButton[World]->setDisabled(false);
					break;
				default:
					break;
				}
			}
		}

		// end toolbar

		void CSpaceScene::initRenderPipeline(float fw, float fh)
		{
			int w = (int)fw;
			int h = (int)fh;

			if (w >= 32.0f && h >= 32.0f)
			{
				if (m_renderRP == NULL)
				{
					// 1st
					m_shadowMapRendering = new CShadowMapRP();
					m_shadowMapRendering->initRender(w, h);

					// 2nd
					m_rendering = new CDeferredRP();
					m_rendering->initRender(w, h);
					m_rendering->enableUpdateEntity(false);

					// 3rd
					m_forwardRP = new CForwardRP(false);
					m_forwardRP->initRender(w, h);
					m_forwardRP->enableUpdateEntity(false);

					// link rp
					m_shadowMapRendering->setNextPipeLine(m_rendering);
					m_rendering->setNextPipeLine(m_forwardRP);

					// post processor
					/*
					m_postProcessor = new CPostProcessorRP();
					m_postProcessor->enableAutoExposure(false);
					m_postProcessor->enableBloomEffect(false);
					m_postProcessor->enableFXAA(false);
					m_postProcessor->enableScreenSpaceReflection(true);
					m_postProcessor->initRender(w, h);

					// apply post processor
					m_rendering->setPostProcessor(m_postProcessor);
					*/

					m_renderRP = m_shadowMapRendering;
				}
				else
				{
					// resize
					if (m_shadowMapRendering != NULL)
						m_shadowMapRendering->resize((int)w, h);

					if (m_rendering != NULL)
						m_rendering->resize(w, h);

					if (m_forwardRP != NULL)
						m_forwardRP->resize(w, h);

					if (m_postProcessor != NULL)
						m_postProcessor->resize(w, h);
				}

				if (m_viewpointRP == NULL)
				{
					CForwardRP* vpRP = new CForwardRP(false);
					vpRP->enableUpdateEntity(true);
					vpRP->initRender(w, h);
					m_viewpointRP = vpRP;
				}
				else
				{
					m_viewpointRP->resize(w, h);
				}

				// update camera aspect
				m_editorCamera->setAspect(fw / fh);

				m_renderSize.Width = fw;
				m_renderSize.Height = fh;
			}
		}

		void CSpaceScene::update()
		{
			// update controller
			CSceneController::getInstance()->update();

			// update viewpoint
			if (m_viewpointController != NULL)
				m_viewpointController->update();

			// update camera setting
			if (!m_cameraSettingMenu->isHidden())
				m_cameraSettingController->update();

			// update scene
			m_scene->update();
		}

		void CSpaceScene::refresh()
		{
			CSceneController::getInstance()->refresh();
		}

		void CSpaceScene::onRender(GUI::CBase* base)
		{
			if (m_renderRP != NULL)
			{
				// flush 2d gui
				GUI::CGUIContext::getRenderer()->flush();
				core::recti vp = getVideoDriver()->getViewPort();
				getVideoDriver()->enableScissor(false);
				getVideoDriver()->clearZBuffer();

				if (m_enableRender)
				{
					// setup scene viewport
					GUI::SPoint position = base->localPosToCanvas();
					core::recti viewport;
					viewport.UpperLeftCorner.set((int)position.X, (int)position.Y);
					viewport.LowerRightCorner.set((int)(position.X + base->width()), (int)(position.Y + base->height()));
					m_sceneRect = viewport;

					// draw scene
					m_scene->setVisibleAllZone(true);
					m_viewpointZone->setVisible(false);

					// render handles
					m_handlesRenderer->setEnable(true);
					m_handlesRenderer->setCameraAndViewport(m_editorCamera, viewport);

					// update select object system
					m_selectObjectSystem->setCameraAndViewport(m_editorCamera, viewport);

					// render gizmos
					m_gizmosRenderer->setEnable(true);

					// render scene
					m_renderRP->render(NULL, m_editorCamera, m_scene->getEntityManager(), viewport);

					// setup viewpoint viewport
					int paddingTop = 10;
					int paddingLeft = 10;
					int viewpointSize = 125;
					viewport.UpperLeftCorner.set((int)(position.X + base->width()) - viewpointSize - paddingLeft, (int)position.Y + paddingTop);
					viewport.LowerRightCorner = viewport.UpperLeftCorner + core::vector2di(viewpointSize, viewpointSize);

					// viewpoint rect (local)
					GUI::SPoint upleft = m_view->canvasPosToLocal(GUI::SPoint((float)viewport.UpperLeftCorner.X, (float)viewport.UpperLeftCorner.Y));
					GUI::SPoint lowerRight = m_view->canvasPosToLocal(GUI::SPoint((float)viewport.LowerRightCorner.X, (float)viewport.LowerRightCorner.Y));
					m_viewpointRect.UpperLeftCorner.set(upleft.X, upleft.Y);
					m_viewpointRect.LowerRightCorner.set(lowerRight.X, lowerRight.Y);
					m_viewpointRect.repair();

					getVideoDriver()->clearZBuffer();

					// draw viewpoint
					m_scene->setVisibleAllZone(false);
					m_viewpointZone->setVisible(true);
					m_handlesRenderer->setEnable(false);
					m_gizmosRenderer->setEnable(false);

					m_viewpointRP->render(NULL, m_viewpointCamera, m_scene->getEntityManager(), viewport);

					// disable viewpoint
					m_scene->setVisibleAllZone(true);
					m_viewpointZone->setVisible(false);
					m_handlesRenderer->setEnable(true);
				}
				else
				{

				}

				// resume gui render
				getVideoDriver()->enableScissor(true);
				getVideoDriver()->setViewPort(vp);
				GUI::CGUIContext::getRenderer()->setProjection();
			}
		}

		void CSpaceScene::onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY)
		{
			GUI::SPoint local = m_view->canvasPosToLocal(GUI::SPoint(x, y));
			postMouseEventToScene(EMIE_MOUSE_MOVED, local.X, local.Y);
		}

		void CSpaceScene::onLeftMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_leftMouseDown = down;
			GUI::SPoint local = m_view->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_viewpointRect.isPointInside(core::vector2df(local.X, local.Y)))
			{
				// viewpoint
				if (down)
				{
					float x = local.X - m_viewpointRect.UpperLeftCorner.X;
					float y = local.Y - m_viewpointRect.UpperLeftCorner.Y;
					CViewpointData::EAxis axis = m_viewpoint->getViewpointData()->hit(m_viewpointCamera, x, y, (int)m_viewpointRect.getWidth(), (int)m_viewpointRect.getHeight());

					if (axis != CViewpointData::None)
						m_viewpointController->setCameraLook(axis);
				}
			}
			else
			{
				// scene
				if (down)
				{
					GUI::CInput::getInput()->setCapture(m_view);
					postMouseEventToScene(EMIE_LMOUSE_PRESSED_DOWN, local.X, local.Y);
				}
				else
				{
					GUI::CInput::getInput()->setCapture(NULL);
					postMouseEventToScene(EMIE_LMOUSE_LEFT_UP, local.X, local.Y);

					// try test selection
					// core::line3df ray = CProjective::getViewRay(m_editorCamera, local.X, local.Y, m_sceneRect.getWidth(), m_sceneRect.getHeight());
				}
			}
		}

		void CSpaceScene::onRightMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_rightMouseDown = down;
			GUI::SPoint local = m_view->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_viewpointRect.isPointInside(core::vector2df(local.X, local.Y)))
			{
				// viewpoint
			}
			else
			{
				// scene
				if (down)
				{
					GUI::CInput::getInput()->setCapture(m_view);
					postMouseEventToScene(EMIE_RMOUSE_PRESSED_DOWN, local.X, local.Y);
				}
				else
				{
					GUI::CInput::getInput()->setCapture(NULL);
					postMouseEventToScene(EMIE_RMOUSE_LEFT_UP, local.X, local.Y);
				}
			}
		}

		void CSpaceScene::onMiddleMouseClick(GUI::CBase* base, float x, float y, bool down)
		{
			m_middleMouseDown = down;
			GUI::SPoint local = m_view->canvasPosToLocal(GUI::SPoint(x, y));

			if (m_viewpointRect.isPointInside(core::vector2df(local.X, local.Y)))
			{
				// viewpoint
			}
			else
			{
				// scene
				if (down)
				{
					GUI::CInput::getInput()->setCapture(m_view);
					postMouseEventToScene(EMIE_MMOUSE_PRESSED_DOWN, local.X, local.Y);
				}
				else
				{
					GUI::CInput::getInput()->setCapture(NULL);
					postMouseEventToScene(EMIE_MMOUSE_LEFT_UP, local.X, local.Y);
				}
			}
		}

		void CSpaceScene::onMouseWheeled(GUI::CBase* base, int wheel)
		{
			if (m_viewpointRect.isPointInside(core::vector2df(m_mouseX, m_mouseY)))
			{
				// viewpoint
			}
			else
			{
				// scene
				SEvent event;
				event.EventType = EET_MOUSE_INPUT_EVENT;
				event.MouseInput.Event = EMIE_MOUSE_WHEEL;
				event.MouseInput.X = (int)m_mouseX;
				event.MouseInput.Y = (int)m_mouseY;
				event.MouseInput.Wheel = (float)wheel;
				event.MouseInput.ButtonStates = 0;

				if (m_leftMouseDown)
					event.MouseInput.ButtonStates |= EMBSM_LEFT;

				if (m_rightMouseDown)
					event.MouseInput.ButtonStates |= EMBSM_RIGHT;

				event.GameEvent.Sender = m_scene;

				m_scene->OnEvent(event);
			}
		}

		void CSpaceScene::postMouseEventToScene(EMOUSE_INPUT_EVENT eventType, float x, float y)
		{
			m_mouseX = x;
			m_mouseY = y;

			SEvent event;
			event.EventType = EET_MOUSE_INPUT_EVENT;
			event.MouseInput.Event = eventType;
			event.MouseInput.X = (int)m_mouseX;
			event.MouseInput.Y = (int)m_mouseY;
			event.MouseInput.Wheel = 0.0f;
			event.MouseInput.ButtonStates = 0;

			if (m_leftMouseDown)
				event.MouseInput.ButtonStates |= EMBSM_LEFT;

			if (m_rightMouseDown)
				event.MouseInput.ButtonStates |= EMBSM_RIGHT;

			event.GameEvent.Sender = m_scene;

			m_scene->OnEvent(event);
		}

		void CSpaceScene::onHotkey(GUI::CBase* base, const std::string& hotkey)
		{
			if (hotkey == "G")
			{
				onToolbarTransform(m_toolbarButton[ESceneToolBar::Move]);
				onEditorSelect(m_toolbarButton[ESceneToolBar::Scale]);
			}
			else if (hotkey == "R")
			{
				onToolbarTransform(m_toolbarButton[ESceneToolBar::Rotate]);
				onEditorSelect(m_toolbarButton[ESceneToolBar::Scale]);
			}
			else if (hotkey == "S")
			{
				onToolbarTransform(m_toolbarButton[ESceneToolBar::Scale]);
				onEditorSelect(m_toolbarButton[ESceneToolBar::Scale]);
			}
			else if (hotkey == "Ctrl + C")
			{
				CSceneController::getInstance()->onCopy();
			}
			else if (hotkey == "Ctrl + V")
			{
				CSceneController::getInstance()->onPaste();
			}
			else if (hotkey == "Ctrl + D")
			{
				CSceneController::getInstance()->onDuplicate();
			}
			else if (hotkey == "Ctrl + Z")
			{
				CSceneController::getInstance()->onUndo();
			}
			else if (hotkey == "Ctrl + Y")
			{
				CSceneController::getInstance()->onRedo();
			}
		}

		void CSpaceScene::onKeyPressed(GUI::CBase* base, int key, bool down)
		{
			GUI::CInput* input = GUI::CInput::getInput();

			SEvent event;
			event.EventType = EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)key;
			event.KeyInput.PressedDown = down;
			event.KeyInput.Control = input->IsControlDown();
			event.KeyInput.Shift = input->IsShiftDown();

			event.GameEvent.Sender = m_scene;

			m_scene->OnEvent(event);

			if (down == true)
			{
				if (key == GUI::KEY_DELETE)
				{
					CSceneController::getInstance()->onDelete();
				}
			}
		}

		bool CSpaceScene::isEditorObject(CGameObject* object)
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