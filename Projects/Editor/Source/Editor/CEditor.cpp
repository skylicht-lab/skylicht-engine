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
#include "CEditor.h"
#include "CWindowConfig.h"
#include "CEditorSetting.h"
#include "Utils/CStringImp.h"

#include "Space/ApplyTemplate/CSpaceApplyTemplate.h"
#include "Space/Import/CSpaceImport.h"
#include "Space/Scene/CSpaceScene.h"
#include "Space/Assets/CSpaceAssets.h"
#include "Space/Console/CSpaceConsole.h"
#include "Space/Property/CSpaceProperty.h"
#include "Space/Hierarchy/CSpaceHierarchy.h"
#include "Space/LoadScene/CSpaceLoadScene.h"
#include "Space/ProjectSettings/CSpaceProjectSettings.h"
#include "Space/GUIDesign/CSpaceGUIDesign.h"
#include "Space/GUIHierarchy/CSpaceGUIHierarchy.h"
#include "Space/GoogleMap/CSpaceGMap.h"
#include "Space/GoogleMap/CSpaceExportGMap.h"
#include "Space/Sprite/CSpaceExportSprite.h"
#include "Space/Sprite/CSpaceSprite.h"
#include "Space/Particle/CSpaceParticle.h"
#include "Space/InterpolateCurves/CSpaceInterpolateCurves.h"

#include "SpaceController/CSceneController.h"
#include "SpaceController/CPropertyController.h"
#include "SpaceController/CAssetPropertyController.h"
#include "SpaceController/CAssetCreateController.h"
#include "SpaceController/CSpriteController.h"
#include "SpaceController/CGUIDesignController.h"
#include "SpaceController/CParticleController.h"

#include "Handles/CHandles.h"

#include "AssetManager/CAssetManager.h"
#include "Selection/CSelection.h"
#include "Activator/CEditorActivator.h"
#include "ProjectSettings/CProjectSettings.h"

#ifdef BUILD_SKYLICHT_GRAPH
#include "Space/BuildWalkingMap/CSpaceBuildWalkingMap.h"
#endif

#ifdef BUILD_SKYLICHT_LIGHMAPPER
#include "Space/BakeDirectional/CSpaceBakeDirectional.h"
#endif

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CEditor);

		CEditor::CEditor() :
			m_menuBar(NULL),
			m_canvas(NULL),
			m_dockPanel(NULL),
			m_statusInfo(NULL),
			m_status(NULL),
			m_importDialog(NULL),
			m_waitingDialog(NULL),
			m_uiInitiate(false),
			m_confirmQuit(false),
			m_assetWatcher(NULL)
		{
			// init canvas
			m_canvas = GUI::Context::getRoot();

			// file watcher
			m_assetWatcher = new CAssetWatcher();

			// scene icon
			m_spriteIcon = new CSpriteAtlas(video::ECF_A8R8G8B8, 256, 256);
			m_spriteIcon->addFrame("light", "Editor/Icon/Objects/Light.png");
			m_spriteIcon->addFrame("camera", "Editor/Icon/Objects/Camera.png");
			m_spriteIcon->updateTexture();

			// init controller
			CSceneController::createGetInstance()->initContextMenu(m_canvas);
			CSpriteController::createGetInstance()->initContextMenu(m_canvas);
			CGUIDesignController::createGetInstance()->initContextMenu(m_canvas);
			CParticleController::createGetInstance()->initContextMenu(m_canvas);

			CPropertyController::createGetInstance();
			CAssetPropertyController::createGetInstance();
			CAssetCreateController::createGetInstance();

			CSelection::createGetInstance();

			CEditorActivator::createGetInstance();
			CProjectSettings::createGetInstance();
		}

		CEditor::~CEditor()
		{
			getSubjectTransformGizmos().removeAllObserver();

			CSceneController::getInstance()->deleteScene();
			CGUIDesignController::getInstance()->deleteScene();

			CEditorActivator::releaseInstance();
			CAssetPropertyController::releaseInstance();
			CAssetCreateController::releaseInstance();
			CPropertyController::releaseInstance();

			CSelection::releaseInstance();

			CParticleController::releaseInstance();
			CSceneController::releaseInstance();
			CSpriteController::releaseInstance();
			CGUIDesignController::releaseInstance();

			CProjectSettings::releaseInstance();
			CEditorSetting::releaseInstance();

			delete m_spriteIcon;
			delete m_assetWatcher;
		}

		void CEditor::update()
		{
			m_assetWatcher->update();

			Editor::CAssetManager::getInstance()->update();

			for (CSpace* s : m_workspaces)
				s->update();

			CSpace* space = getWorkspace(m_waitingDialog);
			if (space != NULL)
			{
				CSpaceLoading* spaceLoadScene = dynamic_cast<CSpaceLoading*>(space);
				if (spaceLoadScene != NULL && spaceLoadScene->isFinished())
				{
					m_waitingDialog->remove();
					m_waitingDialog = NULL;
				}
			}
		}

		void CEditor::pause()
		{
			if (m_assetWatcher)
				m_assetWatcher->beginWatch();

			for (CSpace* s : m_workspaces)
			{
				s->pause();
			}
		}

		void CEditor::resume()
		{
			if (m_assetWatcher)
			{
				m_assetWatcher->endWatch();
				if (m_assetWatcher->needReImport())
				{
					m_assetWatcher->lock();
					initImportGUI(true);
				}
			}

			for (CSpace* s : m_workspaces)
			{
				s->resume();
			}
		}

		void CEditor::refresh()
		{
			for (CSpace* s : m_workspaces)
			{
				s->refresh();
			}

			m_canvas->invalidate();
			m_canvas->recurseLayout();
		}

		bool CEditor::needReImport()
		{
			return m_assetWatcher->needReImport();
		}

		bool CEditor::isImportFinish()
		{
			CSpace* space = getWorkspace(m_importDialog);
			if (space != NULL)
			{
				CSpaceImport* spaceImport = dynamic_cast<CSpaceImport*>(space);
				if (spaceImport != NULL)
					return spaceImport->isFinished();
			}

			return false;
		}

		void CEditor::closeImportDialog()
		{
			m_importDialog->remove();
			m_importDialog = NULL;

			if (m_assetWatcher->needReImport())
				m_assetWatcher->unlock();
		}

		void CEditor::closeProperty()
		{
			CSpaceProperty* spaceProperty = (CSpaceProperty*)getWorkspaceByName(L"Property");

			if (spaceProperty == NULL)
				return;

			// Clear old ui
			spaceProperty->clearAllGroup();

			// Tabable
			spaceProperty->getWindow()->getCanvas()->TabableGroup.clear();
			spaceProperty->getWindow()->forceUpdateLayout();
		}

		void CEditor::initImportGUI(bool fromWatcher)
		{
			m_importDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_importDialog->setCaption(L"Import Assets");
			m_importDialog->showCloseButton(false);
			m_importDialog->setCenterPosition();
			m_importDialog->bringToFront();

			initWorkspace(m_importDialog, m_importDialog->getCaption());

			CSpace* space = getWorkspace(m_importDialog);
			CSpaceImport* spaceImport = dynamic_cast<CSpaceImport*>(space);

			if (fromWatcher == true)
			{
				spaceImport->initImportFiles(m_assetWatcher->getFiles(), m_assetWatcher->getDeletedFiles());
				spaceImport->setLoadStepCount(1);
			}
			else
				spaceImport->initImportAll();
		}

		void CEditor::initImportGUI(std::list<SFileNode*> files)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Import Assets");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);
			CSpaceImport* spaceImport = dynamic_cast<CSpaceImport*>(space);

			std::list<std::string> deletes;

			spaceImport->initImportFiles(files, deletes);
			spaceImport->setLoadStepCount(1);
		}

		void CEditor::initLoadSceneGUI(const char* path)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Load Scene");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);

			CSpaceLoadScene* spaceLoadScene = dynamic_cast<CSpaceLoadScene*>(space);
			spaceLoadScene->loadScene(path);
		}

		void CEditor::initApplyTemplateGUI(const char* path)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Apply Template");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);

			CSpaceApplyTemplate* spaceApplyTemplate = dynamic_cast<CSpaceApplyTemplate*>(space);
			spaceApplyTemplate->applyTemplate(path);
		}

		void CEditor::initReplaceTemplateGUI(const char* templateId, const char* path)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Apply Template");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);

			CSpaceApplyTemplate* spaceApplyTemplate = dynamic_cast<CSpaceApplyTemplate*>(space);
			spaceApplyTemplate->replaceTemplate(templateId, path);
		}

#ifdef BUILD_SKYLICHT_GRAPH
		void CEditor::initBuildWalkMap(Graph::CGraphComponent* component)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Build walking map");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);

			CSpaceBuildWalkingMap* spaceBuild = dynamic_cast<CSpaceBuildWalkingMap*>(space);
			spaceBuild->build(component);
		}
#endif

		void CEditor::initEditorGUI()
		{
			// bind save layout function
			m_canvas->OnSaveDockLayout = std::bind(&CEditor::saveLayout, this, std::placeholders::_1);

			// init menu bar
			m_menuBar = new GUI::CMenuBar(m_canvas);
			initMenuBar();

			// init status bar
			GUI::CToolbar* statusBar = new GUI::CToolbar(m_canvas);
			statusBar->dock(GUI::EPosition::Bottom);

			m_statusInfo = new GUI::CTableRow(statusBar);
			m_statusInfo->setWidth(450.0f);
			m_statusInfo->setColumnCount(4);

			float colWidth = round(m_statusInfo->width() / 4.0f);
			for (int i = 0; i < 4; i++)
				m_statusInfo->setColumnWidth(i, colWidth);

			m_statusInfo->setCellText(1, L"");
			m_statusInfo->setCellText(2, L"");
			m_statusInfo->setCellText(3, L"");
			m_statusInfo->dock(GUI::EPosition::Right);

			m_status = new GUI::CIconTextItem(statusBar);
			m_status->setLabel(L"");
			m_status->setIcon(GUI::ESystemIcon::None);
			m_status->dock(GUI::EPosition::Fill);

			// init dock panel
			m_dockPanel = new GUI::CDockPanel(m_canvas);
			m_dockPanel->dock(GUI::EPosition::Fill);

			// calculate layout
			m_canvas->recurseLayout();

			// init setting
			CEditorSetting::createGetInstance()->load();

			u32 x, y, w, h;
			bool maximize = false;
			std::string data;
			CWindowConfig::loadConfigAndExtraData(x, y, w, h, maximize, data);

			if (data.length() > 0)
			{
				initSessionLayout(data);
			}
			else
			{
				initDefaultLayout();
			}

			m_uiInitiate = true;
		}

		void CEditor::initMenuBar()
		{
			GUI::CMenu* submenu, * temp;

			GUI::CMenuItem* logo = m_menuBar->addItem(GUI::ESystemIcon::AppIcon);
			submenu = logo->getMenu();
			submenu->addItem(L"About");
			submenu->addItem(L"Development funding", GUI::ESystemIcon::Web);
			submenu->addSeparator();
			submenu->addItem(L"Close", GUI::ESystemIcon::Quit, L"Ctrl + Q");
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandLogo, this);

			GUI::CMenuItem* file = m_menuBar->addItem(L"File");
			submenu = file->getMenu();
			submenu->addItem(L"New scene");
			submenu->addSeparator();
			submenu->addItem(L"Save", GUI::ESystemIcon::Save, L"Ctrl + S");
			submenu->addItem(L"Save As");
			submenu->OnOpen = BIND_LISTENER(&CEditor::OnOpenMenuFile, this);
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandFile, this);

			GUI::CMenuItem* edit = m_menuBar->addItem(L"Edit");
			submenu = edit->getMenu();
			submenu->addItem(L"Undo", L"Ctrl + Z");
			submenu->addItem(L"Redo", L"Ctrl + Y");
			submenu->addSeparator();
			submenu->addItem(L"Copy", GUI::ESystemIcon::Copy, L"Ctrl + C");
			submenu->addItem(L"Paste", GUI::ESystemIcon::Paste, L"Ctrl + V");
			submenu->addItem(L"Duplicate", GUI::ESystemIcon::Duplicate, L"Ctrl + D");
			submenu->addItem(L"Cut", GUI::ESystemIcon::None, L"Ctrl + X");
			submenu->addSeparator();
			submenu->addItem(L"Delete");
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandEdit, this);

			GUI::CMenuItem* asset = m_menuBar->addItem(L"Assets");
			submenu = asset->getMenu();
			temp = submenu;

			GUI::CMenuItem* create = submenu->addItem(L"Create");
			submenu = create->getMenu();
			submenu->addItem(L"Scene");
			submenu->addItem(L"Material");
			submenu->addItem(L"Font");
			submenu->addItem(L"GUI");
			submenu->addItem(L"Sprite");
			submenu->addItem(L"File array");
			submenu->addItem(L"Texture array");
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandAssetCreate, this);

			GUI::CMenuItem* gameObject = m_menuBar->addItem(L"GameObject");
			submenu = gameObject->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);
			submenu->addItem(L"Zone", GUI::ESystemIcon::Collection);
			submenu->addItem(L"Container Object", GUI::ESystemIcon::Folder);
			submenu->addItem(L"Empty Object", GUI::ESystemIcon::Res3D);
			submenu->addSeparator();
			temp = submenu;

			GUI::CMenuItem* object = submenu->addItem(L"Object");
			submenu = object->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);

			submenu->addItem(L"Cube");
			submenu->addItem(L"Sphere");
			submenu->addItem(L"Plane");
			submenu->addItem(L"Capsule");
			submenu->addItem(L"Cylinder");

			submenu->addSeparator();

			submenu->addItem(L"Skydome");
			submenu->addItem(L"Skybox");
			submenu->addItem(L"Sky");
			submenu->addSeparator();

			submenu->addItem(L"Mesh");
			submenu->addItem(L"Mesh Instancing");

			submenu->addSeparator();
			submenu->addItem(L"Particle");

			/*
			submenu->addSeparator();

			submenu->addItem(L"Terrain");

			submenu = temp;
			GUI::CMenuItem* effect = submenu->addItem(L"Effect");
			submenu = effect->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);

			submenu->addItem(L"Particle System");
			submenu->addItem(L"Line");
			submenu->addItem(L"Trail");
			*/

			submenu = temp;
			GUI::CMenuItem* lighting = submenu->addItem(L"Lighting");
			submenu = lighting->getMenu();
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandGameObject, this);

			submenu->addItem(L"Direction Light");
			submenu->addItem(L"Point Light");
			submenu->addItem(L"Spot Light");
			submenu->addSeparator();
			submenu->addItem(L"Reflection Probe");
			submenu->addItem(L"Light Probes");

			/*
			submenu = temp;
			submenu->addSeparator();
			submenu->addItem(L"Camera");

			submenu->addSeparator();
			submenu->addItem(L"Trigger");
			*/

			GUI::CMenuItem* window = m_menuBar->addItem(L"Window");
			submenu = window->getMenu();
			submenu->addItem(L"Project Setting", GUI::ESystemIcon::Setting);
			submenu->addSeparator();
			m_menuWindowItems.clear();
			m_menuWindowItems.push_back(submenu->addItem(L"Assets"));
			m_menuWindowItems.push_back(submenu->addItem(L"Property"));
			m_menuWindowItems.push_back(submenu->addItem(L"Scene"));
			m_menuWindowItems.push_back(submenu->addItem(L"Hierarchy"));
			m_menuWindowItems.push_back(submenu->addItem(L"Console"));
			m_menuWindowItems.push_back(submenu->addItem(L"GUI Design"));
			m_menuWindowItems.push_back(submenu->addItem(L"GUI Hierarchy"));
			m_menuWindowItems.push_back(submenu->addItem(L"Particle"));
			submenu->addSeparator();
			m_menuWindowItems.push_back(submenu->addItem(L"Google Map"));
			submenu->addSeparator();
			submenu->addItem(L"Reset layout");
			submenu->OnOpen = BIND_LISTENER(&CEditor::OnCommandWindowOpen, this);
			submenu->OnCommand = BIND_LISTENER(&CEditor::OnCommandWindow, this);

			GUI::CMenuItem* help = m_menuBar->addItem(L"Help");
			submenu = help->getMenu();
			submenu->addItem(L"Tutorial", GUI::ESystemIcon::Guide);
			submenu->addItem(L"Report a bug", GUI::ESystemIcon::Web);
			submenu->addSeparator();
			submenu->addItem(L"Development funding", GUI::ESystemIcon::Web);

			// add hot key
			m_canvas->addAccelerator("Ctrl + Q", BIND_LISTENER(&CEditor::OnMenuQuit, this));
			m_canvas->addAccelerator("Ctrl + S", BIND_LISTENER(&CEditor::OnMenuSave, this));
		}

		void CEditor::saveLayout(const std::string& data)
		{
			CWindowConfig::saveExtraData(data.c_str());
		}

		void CEditor::initDefaultLayout()
		{
			float width = m_dockPanel->width();
			float height = m_dockPanel->height();
			float w, h;

			w = width * 0.2f;
			h = round(height * 0.8f);
			GUI::CDockableWindow* hierarchy = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			hierarchy->setCaption(L"Hierarchy");
			m_dockPanel->dockChildWindow(hierarchy, NULL, GUI::CDockPanel::DockLeft);
			m_dockPanel->recurseLayout();

			GUI::CDockableWindow* guiHierarchy = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			guiHierarchy->setCaption(L"GUI Hierarchy");
			hierarchy->getCurrentDockTab()->dockWindow(guiHierarchy);
			hierarchy->getCurrentDockTab()->setCurrentWindow(hierarchy);
			m_dockPanel->recurseLayout();

			w = width;
			h = round(height * 0.8f);
			GUI::CDockableWindow* scene = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			scene->setCaption(L"Scene");
			m_dockPanel->dockChildWindow(scene, NULL, GUI::CDockPanel::DockCenter);
			m_dockPanel->recurseLayout();

			GUI::CDockableWindow* guiDesign = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			guiDesign->setCaption(L"GUI Design");
			scene->getCurrentDockTab()->dockWindow(guiDesign);
			scene->getCurrentDockTab()->setCurrentWindow(scene);
			m_dockPanel->recurseLayout();

			w = round(width * 0.7f);
			h = round(height * 0.3f);
			GUI::CDockableWindow* asset = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			asset->setCaption(L"Assets");
			m_dockPanel->dockChildWindow(asset, NULL, GUI::CDockPanel::DockBottom);
			m_dockPanel->recurseLayout();

			w = round(width * 0.35f);
			h = round(height * 0.3f);
			GUI::CDockableWindow* console = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			console->setCaption(L"Console");
			m_dockPanel->dockChildWindow(console, asset->getCurrentDockTab(), GUI::CDockPanel::DockTargetRight);
			m_dockPanel->recurseLayout();

			w = width * 0.3f;
			h = height;
			GUI::CDockableWindow* property = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			property->setCaption(L"Property");
			m_dockPanel->dockChildWindow(property, NULL, GUI::CDockPanel::DockRight);
			m_dockPanel->recurseLayout();

			initWorkspace(scene, scene->getCaption());
			initWorkspace(asset, asset->getCaption());
			initWorkspace(console, console->getCaption());
			initWorkspace(property, property->getCaption());
			initWorkspace(hierarchy, hierarchy->getCaption());
			initWorkspace(guiHierarchy, guiHierarchy->getCaption());
			initWorkspace(guiDesign, guiDesign->getCaption());
		}

		CSpace* CEditor::initWorkspace(GUI::CWindow* window, const std::wstring& workspace)
		{
			// note:
			// Space will delete when window closed see CSpace::onDestroy
			CSpace* ret = NULL;

			if (workspace == L"Scene")
			{
				ret = new CSpaceScene(window, this);
			}
			else if (workspace == L"Import Assets")
			{
				ret = new CSpaceImport(window, this);
			}
			else if (workspace == L"GUI Design")
			{
				ret = new CSpaceGUIDesign(window, this);
			}
			else if (workspace == L"GUI Hierarchy")
			{
				ret = new CSpaceGUIHierarchy(window, this);
			}
			else if (workspace == L"Apply Template")
			{
				ret = new CSpaceApplyTemplate(window, this);
			}
			else if (workspace == L"Sprite")
			{
				ret = new CSpaceSprite(window, this);
			}
			else if (workspace == L"Assets")
			{
				ret = new CSpaceAssets(window, this);
			}
			else if (workspace == L"Console")
			{
				ret = new CSpaceConsole(window, this);
			}
			else if (workspace == L"Property")
			{
				ret = new CSpaceProperty(window, this);
			}
			else if (workspace == L"Google Map")
			{
				ret = new CSpaceGMap(window, this);
			}
			else if (workspace == L"Export GMap")
			{
				ret = new CSpaceExportGMap(window, this);
			}
			else if (workspace == L"Export Sprite")
			{
				ret = new CSpaceExportSprite(window, this);
			}
			else if (workspace == L"Hierarchy")
			{
				ret = new CSpaceHierarchy(window, this);
			}
			else if (workspace == L"Project Setting")
			{
				ret = new CSpaceProjectSettings(window, this);
			}
			else if (workspace == L"Load Scene")
			{
				ret = new CSpaceLoadScene(window, this);
			}
#ifdef BUILD_SKYLICHT_GRAPH
			else if (workspace == L"Build walking map")
			{
				ret = new CSpaceBuildWalkingMap(window, this);
			}
#endif
			else if (workspace == L"Particle")
			{
				ret = new CSpaceParticle(window, this);
			}
			else if (workspace == L"Interpolate Curves")
			{
				ret = new CSpaceInterpolateCurves(window, this);
			}
#ifdef BUILD_SKYLICHT_LIGHMAPPER
			else if (workspace == L"Bake Directional")
			{
				ret = new CSpaceBakeDirectional(window, this);
			}
#endif

			if (ret)
				m_workspaces.push_back(ret);

			return ret;
		}

		void CEditor::removeWorkspace(CSpace* space)
		{
			std::list<CSpace*>::iterator i = std::find(m_workspaces.begin(), m_workspaces.end(), space);
			if (i != m_workspaces.end())
			{
				delete (*i);
				m_workspaces.erase(i);
			}
		}

		CSpace* CEditor::getWorkspace(GUI::CWindow* window)
		{
			for (CSpace* s : m_workspaces)
			{
				if (s->getWindow() == window)
					return s;
			}
			return NULL;
		}

		CSpace* CEditor::getWorkspaceByName(const std::wstring& name)
		{
			for (CSpace* s : m_workspaces)
			{
				if (s->getWindow()->getCaption() == name)
					return s;
			}
			return NULL;
		}

		std::vector<CSpace*> CEditor::getAllWorkspaceByName(const std::wstring& name)
		{
			std::vector<CSpace*> results;

			for (CSpace* s : m_workspaces)
			{
				if (s->getWindow()->getCaption() == name)
					results.push_back(s);
			}

			return results;
		}

		void CEditor::refreshAssetSpace(CSpace* skip)
		{
			for (CSpace* s : m_workspaces)
			{
				if (s == skip)
					continue;

				CSpaceAssets* asset = dynamic_cast<CSpaceAssets*>(s);
				if (asset)
					asset->refresh();
			}
		}

		void CEditor::initSessionLayout(const std::string& data)
		{
			io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
			io::IReadFile* file = fs->createMemoryReadFile(data.c_str(), (s32)data.length(), "data");
			io::IXMLReader* xmlRead = fs->createXMLReader(file);
			if (xmlRead == NULL)
				return;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"canvas") == nodeName)
					{
						readBound(xmlRead, m_canvas);
					}
					else if (core::stringw(L"dock-panel") == nodeName)
					{
						// dock layout
						readBound(xmlRead, m_dockPanel);
						readDockLayout(xmlRead, m_dockPanel);
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
				}
				break;
				case io::EXN_TEXT:
				{
					// std::wstring text = xmlRead->getNodeData();
				}
				break;
				default:
					break;
				}
			}

			xmlRead->drop();
			file->drop();
		}

		void CEditor::readBound(io::IXMLReader* xml, GUI::CBase* base)
		{
			char ansiData[64];
			const wchar_t* valueX = xml->getAttributeValue(L"x");
			const wchar_t* valueY = xml->getAttributeValue(L"y");
			const wchar_t* valueW = xml->getAttributeValue(L"w");
			const wchar_t* valueH = xml->getAttributeValue(L"h");

			if (valueX == NULL || valueY == NULL || valueW == NULL || valueH == NULL)
				return;

			float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

			if (valueX != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueX, ansiData);
				x = (float)atof(ansiData);
			}

			if (valueY != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueY, ansiData);
				y = (float)atof(ansiData);
			}

			if (valueW != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueW, ansiData);
				w = (float)atof(ansiData);
			}

			if (valueH != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueH, ansiData);
				h = (float)atof(ansiData);
			}

			base->setBounds(x, y, w, h);
		}

		void CEditor::readDockLayout(io::IXMLReader* xmlRead, GUI::CDockPanel* panel)
		{
			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (nodeName == L"horizontal")
					{
						readSpliterLayout(xmlRead, panel, panel->getMainSpliter(), true);
					}
					else if (nodeName == L"vertical")
					{
						readSpliterLayout(xmlRead, panel, panel->getMainSpliter(), false);
					}
					else if (nodeName == L"window")
					{
						const wchar_t* name;
						name = xmlRead->getAttributeValue(L"name");
						if (name != NULL)
						{
							GUI::CDockableWindow* win = new GUI::CDockableWindow(panel, 0, 0, 1, 1);
							readBound(xmlRead, win);
							win->setCaption(name);

							initWorkspace(win, name);
						}
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"dock-panel") == nodeName)
					{
						return;
					}
				}
				break;
				default:
					break;
				}
			}
		}

		void CEditor::readSpliterLayout(io::IXMLReader* xmlRead, GUI::CDockPanel* panel, GUI::CSplitter* spliter, bool isHorizontal)
		{
			u32 count = 0;
			u32 weakRow = 0;
			u32 weakCol = 0;

			char ansiData[64];
			const wchar_t* valueCount;

			readBound(xmlRead, spliter);

			valueCount = xmlRead->getAttributeValue(L"count");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				count = (u32)atoi(ansiData);
			}

			valueCount = xmlRead->getAttributeValue(L"weak-row");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				weakRow = (u32)atoi(ansiData);
			}

			valueCount = xmlRead->getAttributeValue(L"weak-col");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				weakCol = (u32)atoi(ansiData);
			}

			if (isHorizontal)
				spliter->setNumberRowCol(1, count);
			else
				spliter->setNumberRowCol(count, 1);

			spliter->setWeakRow(weakRow);
			spliter->setWeakCol(weakCol);

			u32 col = 0;
			u32 row = 0;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (nodeName == L"col")
					{
						float width = 0;
						valueCount = xmlRead->getAttributeValue(L"width");
						if (valueCount != NULL)
						{
							CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
							width = (float)atof(ansiData);
						}

						spliter->setColWidth(col, width);
					}
					if (nodeName == L"row")
					{
						float height = 0;
						valueCount = xmlRead->getAttributeValue(L"height");
						if (valueCount != NULL)
						{
							CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
							height = (float)atof(ansiData);
						}

						spliter->setRowHeight(row, height);
					}
					if (nodeName == L"horizontal")
					{
						GUI::CSplitter* newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, panel, newSpliter, true);
					}
					else if (nodeName == L"vertical")
					{
						GUI::CSplitter* newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, panel, newSpliter, false);
					}
					if (nodeName == L"docktab")
					{
						GUI::CDockTabControl* docktab = new GUI::CDockTabControl(spliter, panel);
						readBound(xmlRead, docktab);

						spliter->setControl(docktab, row, col);
						readDockTab(xmlRead, docktab);
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"horizontal") == nodeName && isHorizontal)
					{
						spliter->saveUserExpectedSize();
						return;
					}
					else if (core::stringw(L"vertical") == nodeName && !isHorizontal)
					{
						spliter->saveUserExpectedSize();
						return;
					}
					else if (nodeName == L"col" && isHorizontal)
					{
						col++;
					}
					else if (nodeName == L"row" && !isHorizontal)
					{
						row++;
					}
				}
				break;
				default:
					break;
				}
			}
		}

		void CEditor::readDockTab(io::IXMLReader* xmlRead, GUI::CDockTabControl* tabcontrol)
		{
			const wchar_t* currentTab = xmlRead->getAttributeValue(L"current");

			std::wstring current;
			GUI::CDockableWindow* currentWin = NULL;
			if (currentTab != NULL)
				current = currentTab;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (nodeName == L"window")
					{
						const wchar_t* name;
						name = xmlRead->getAttributeValue(L"name");
						if (name != NULL)
						{
							GUI::CDockableWindow* win = new GUI::CDockableWindow(m_dockPanel, 0, 0, 1, 1);
							win->setCaption(name);
							tabcontrol->dockWindow(win);

							initWorkspace(win, name);

							if (CStringImp::comp(name, current.c_str()) == 0)
								currentWin = win;
						}
					}
				}
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (core::stringw(L"docktab") == nodeName)
					{
						if (currentWin != NULL)
							tabcontrol->setCurrentWindow(currentWin);
						return;
					}
				}
				break;
				default:
					break;
				}
			}
		}

		bool CEditor::onClose()
		{
			if (m_confirmQuit == false)
			{
				CSceneController* sceneController = CSceneController::getInstance();
				if (sceneController->needSave())
				{
					const std::string& path = sceneController->getScenePath();
					std::string name = CPath::getFileName(path);
					if (name.empty())
						name = "UntitledScene.scene";

					GUI::CMessageBox* msgBox = new GUI::CMessageBox(m_canvas, GUI::CMessageBox::YesNoCancel);
					msgBox->setMessage("Save changes before close the Editor?", name);
					msgBox->OnYes = [&, &p = path, controller = sceneController](GUI::CBase* button) {
						if (p.empty())
						{
							std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
							GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas, GUI::COpenSaveDialog::Save, assetFolder.c_str(), assetFolder.c_str(), "scene;*");
							dialog->OnSave = [&, controller = controller](std::string path)
								{
									controller->save(path.c_str());
								};
						}
						else
						{
							controller->save(path.c_str());
							m_confirmQuit = true;
							getIrrlichtDevice()->closeDevice();
						}
						};
					msgBox->OnNo = [&](GUI::CBase* button) {
						m_confirmQuit = true;
						getIrrlichtDevice()->closeDevice();
						};
					return false;
				}
				else
				{
					m_confirmQuit = true;
					return true;
				}
			}
			else
			{
				m_confirmQuit = true;
				return true;
			}
		}

		void CEditor::OnMenuQuit(GUI::CBase* item)
		{
			if (onClose() == true)
			{
				getIrrlichtDevice()->closeDevice();
			}
		}

		void CEditor::OnMenuNewScene(GUI::CBase* item)
		{
			CSceneController::getInstance()->newScene();
		}

		void CEditor::OnMenuSave(GUI::CBase* item)
		{
			bool saveScene = true;
			bool saveCanvas = false;

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject != NULL)
			{
				if (selectObject->getType() == CSelectObject::GUIElement)
				{
					saveCanvas = true;
					saveScene = false;
				}
			}

			if (saveScene)
			{
				onSaveScene();
			}
			else if (saveCanvas)
			{
				onSaveGUICanvas();
			}
		}

		void CEditor::OnMenuSaveAs(GUI::CBase* item)
		{
			bool saveScene = true;
			bool saveCanvas = false;

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject != NULL)
			{
				if (selectObject->getType() == CSelectObject::GUIElement)
				{
					saveCanvas = true;
					saveScene = false;
				}
			}

			std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();

			if (saveScene)
			{
				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas,
					GUI::COpenSaveDialog::SaveAs,
					assetFolder.c_str(),
					assetFolder.c_str(),
					"scene;*");
				dialog->OnSave = [&](std::string path)
					{
						CSceneController::getInstance()->save(path.c_str());
					};
			}
			else if (saveCanvas)
			{
				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas,
					GUI::COpenSaveDialog::Save,
					assetFolder.c_str(),
					assetFolder.c_str(),
					"gui;*"
				);
				dialog->OnSave = [&](std::string path)
					{
						CGUIDesignController::getInstance()->save(path.c_str());
					};
			}
		}

		void CEditor::onSaveScene()
		{
			std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
			CSceneController* sceneController = CSceneController::getInstance();
			const std::string& fileName = sceneController->getScenePath();

			if (fileName.empty())
			{
				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas,
					GUI::COpenSaveDialog::Save,
					assetFolder.c_str(),
					assetFolder.c_str(),
					"scene;*"
				);

				dialog->OnSave = [&, controller = sceneController](std::string path)
					{
						controller->save(path.c_str());
					};
			}
			else
			{
				sceneController->save(fileName.c_str());
			}
		}

		void CEditor::onSaveGUICanvas()
		{
			std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
			CGUIDesignController* guiDesignController = CGUIDesignController::getInstance();

			const std::string& fileName = guiDesignController->getSaveGUIPath();

			if (fileName.empty())
			{
				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas,
					GUI::COpenSaveDialog::Save,
					assetFolder.c_str(),
					assetFolder.c_str(),
					"gui;*"
				);

				dialog->OnSave = [&](std::string path)
					{
						CGUIDesignController::getInstance()->save(path.c_str());
					};
			}
			else
			{
				guiDesignController->save(fileName.c_str());
			}
		}

		void CEditor::OnCommandLogo(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Close")
			{
				OnMenuQuit(item);
			}
		}

		void CEditor::OnOpenMenuFile(GUI::CBase* item)
		{
			GUI::CMenu* file = dynamic_cast<GUI::CMenu*>(item);
			GUI::CMenuItem* save = file->getItemByLabel(L"Save");
			GUI::CMenuItem* saveAs = file->getItemByLabel(L"Save As");

			CSceneController* sceneController = CSceneController::getInstance();
			if (sceneController->needSave())
				save->setDisabled(false);
			else
				save->setDisabled(true);

			saveAs->setDisabled(false);
		}

		void CEditor::OnCommandFile(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"New scene")
			{
				OnMenuNewScene(item);
			}
			else if (label == L"Save")
			{
				OnMenuSave(item);
			}
			else if (label == L"Save As")
			{
				OnMenuSaveAs(item);
			}
		}

		void CEditor::OnCommandEdit(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			CSceneController* sceneController = CSceneController::getInstance();

			if (label == L"Undo")
				sceneController->onUndo();
			else if (label == L"Redo")
				sceneController->onRedo();
			else if (label == L"Copy")
				sceneController->onCopy();
			else if (label == L"Paste")
				sceneController->onPaste();
			else if (label == L"Duplicate")
				sceneController->onDuplicate();
			else if (label == L"Cut")
				sceneController->onCut();
			else if (label == L"Delete")
				sceneController->onDelete();
		}

		void CEditor::OnCommandAssetCreate(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			CAssetCreateController* assetCreater = CAssetCreateController::getInstance();

			if (label == L"Scene")
			{
				assetCreater->createEmptyScene();
			}
			else if (label == L"Material")
			{
				assetCreater->createEmptyMaterial();
			}
			else if (label == L"Font")
			{
				assetCreater->createEmptyFont();
			}
			else if (label == L"Sprite")
			{
				assetCreater->createEmptySprite();
			}
			else if (label == L"GUI")
			{
				assetCreater->createEmptyGUI();
			}
			else if (label == L"File array")
			{
				assetCreater->createEmptyFileArray();
			}
			else if (label == L"Texture array")
			{
				assetCreater->createEmptyTextureArray();
			}
		}

		void CEditor::OnCommandWindowOpen(GUI::CBase* item)
		{
			GUI::CMenu* menu = dynamic_cast<GUI::CMenu*>(item);
			if (menu == NULL)
				return;

			for (GUI::CMenuItem* item : m_menuWindowItems)
			{
				setCheckIcon(item);
			}
		}

		void CEditor::setCheckIcon(GUI::CMenuItem* item)
		{
			if (item == NULL)
				return;

			item->showIcon(true);

			CSpace* space = getWorkspaceByName(item->getLabel());
			if (space != NULL)
				item->setIcon(GUI::ESystemIcon::Check);
			else
				item->setIcon(GUI::ESystemIcon::None);
		}

		void CEditor::closeOpenWorkspace(GUI::CMenuItem* item)
		{
			CSpace* space = getWorkspaceByName(item->getLabel());
			if (space != NULL)
			{
				// Space is open -> close
				GUI::CDockableWindow* dockWindow = dynamic_cast<GUI::CDockableWindow*>(space->getWindow());
				if (dockWindow != NULL)
				{
					GUI::CDockTabControl* dockTab = dockWindow->getCurrentDockTab();
					if (dockTab != NULL)
					{
						// if window is docked on TabControl
						GUI::CTabButton* tabButton = dockTab->getTabButtonByPage(dockWindow);
						if (tabButton != NULL)
							dockTab->doTabClose(tabButton);
					}
					else
					{
						// call close on float window
						space->getWindow()->onCloseWindow();
					}
				}
				else
				{
					// call close non DockableWindow
					space->getWindow()->onCloseWindow();
				}
			}
			else
			{
				// Space is hide -> open
				float w = 680.0f;
				float h = 480.0f;

				const std::wstring& label = item->getLabel();
				getWorkspaceSize(label, w, h);

				GUI::CDockableWindow* window = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
				window->setCaption(label);
				window->setCenterPosition();
				initWorkspace(window, window->getCaption());
			}
		}

		CSpace* CEditor::openWorkspace(const std::wstring& name)
		{
			// Space is hide -> open
			float w = 680.0f;
			float h = 480.0f;
			getWorkspaceSize(name, w, h);

			GUI::CDockableWindow* window = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			window->setCaption(name);
			window->setCenterPosition();

			CSpace* ret = initWorkspace(window, window->getCaption());
			if (!ret)
				window->onCloseWindow();

			return ret;
		}

		void CEditor::getWorkspaceSize(const std::wstring& name, float& w, float& h)
		{
			w = 680.0f;
			h = 480.0f;

			if (name == L"GUI Hierarchy" ||
				name == L"Hierarchy" ||
				name == L"Property" ||
				name == L"Particle")
			{
				w = 300.0f;
			}
		}

		void CEditor::OnCommandWindow(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Reset layout")
			{
				// delete old control
				m_dockPanel->remove();
				m_canvas->processDelayedDeletes();

				// init dock panel
				m_dockPanel = new GUI::CDockPanel(m_canvas);
				m_dockPanel->dock(GUI::EPosition::Fill);

				// calculate layout
				m_canvas->recurseLayout();

				// reset layout
				initDefaultLayout();

				m_canvas->invalidate();

				refresh();
			}
			else if (label == L"Project Setting")
			{
				showProjectSetting();
			}
			else if (label == L"Google Map")
			{
				showGoogleMap();
			}
			else
			{
				// do close/open the window by item label
				for (GUI::CMenuItem* item : m_menuWindowItems)
				{
					if (item == menuItem)
					{
						closeOpenWorkspace(menuItem);
						break;
					}
				}
			}
		}

		void CEditor::showProjectSetting()
		{
			float w = 680.0f;
			float h = 480.0f;
			GUI::CDialogWindow* window = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, w, h);
			window->setCaption(L"Project Setting");
			window->setCenterPosition();
			window->setResizable(true);
			initWorkspace(window, window->getCaption());
		}

		void CEditor::showGoogleMap()
		{
			float w = 1024.0f;
			float h = 768.0f;
			GUI::CWindow* window = new GUI::CWindow(m_canvas, 0.0f, 0.0f, w, h);
			window->setCaption(L"Google Map");
			window->setCenterPosition();
			window->setResizable(true);
			initWorkspace(window, window->getCaption());
		}

		void CEditor::showInterpolateCurves()
		{
			float w = 680.0f;
			float h = 480.0f;
			GUI::CWindow* window = new GUI::CWindow(m_canvas, 0.0f, 0.0f, w, h);
			window->setCaption(L"Interpolate Curves");
			window->setCenterPosition();
			window->setResizable(true);
			initWorkspace(window, window->getCaption());
		}

		void CEditor::exportGMap(const char* path, long x1, long y1, long x2, long y2, int zoom, int type, int gridSize)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Export GMap");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);
			CSpaceExportGMap* spaceExport = dynamic_cast<CSpaceExportGMap*>(space);
			spaceExport->exportMap(path, x1, y1, x2, y2, zoom, type, gridSize);
		}

		void CEditor::exportSprite(const char* id, const char* path, const std::vector<std::string>& pngs, int width, int height, bool alpha)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Export Sprite");
			m_waitingDialog->showCloseButton(false);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);
			CSpaceExportSprite* spaceExport = dynamic_cast<CSpaceExportSprite*>(space);
			spaceExport->exportSprite(id, path, pngs, width, height, alpha);
		}

#ifdef BUILD_SKYLICHT_LIGHMAPPER
		void CEditor::bakeDirectional(Lightmapper::CBakeLightComponent* component)
		{
			m_waitingDialog = new GUI::CDialogWindow(m_canvas, 0.0f, 0.0f, 600.0f, 120.0f);
			m_waitingDialog->setCaption(L"Bake Directional");
			m_waitingDialog->showCloseButton(true);
			m_waitingDialog->setCenterPosition();
			m_waitingDialog->bringToFront();

			initWorkspace(m_waitingDialog, m_waitingDialog->getCaption());

			CSpace* space = getWorkspace(m_waitingDialog);
			CSpaceBakeDirectional* spaceExport = dynamic_cast<CSpaceBakeDirectional*>(space);
			spaceExport->bake(component);
		}
#endif

		void CEditor::OnCommandGameObject(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();

			CSceneController::getInstance()->onCommand(label);
		}
	}
}
