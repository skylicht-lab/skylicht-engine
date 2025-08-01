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

#pragma once

#include "Utils/CSingleton.h"

#include "Scene/CScene.h"

#include "Editor/Space/Scene/CSpaceScene.h"
#include "Editor/Space/Hierarchy/CSpaceHierarchy.h"
#include "Editor/Space/Hierarchy/CHierachyNode.h"
#include "Editor/Space/GUIDesign/CSpaceGUIDesign.h"

#include "Editor/Gizmos/CGizmos.h"
#include "GizmosComponents/SelectObject/CSelectObjectSystem.h"
#include "Selection/CSelectObject.h"

#include "CContextMenuScene.h"

#include "Reactive/CObserver.h"
#include "Reactive/CSubject.h"

#include "Editor/Gizmos/Transform/CTransformGizmos.h"
#include "Editor/Gizmos/Transform/CWorldTransformDataGizmos.h"
#include "Editor/Gizmos/Particle/CParticleGizmos.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSceneController :
			public IObserver,
			public IFileLoader
		{
		public:
			DECLARE_SINGLETON(CSceneController)

		protected:
			CSpaceScene* m_spaceScene;

			CSpaceHierarchy* m_spaceHierarchy;

			CHierachyNode* m_hierachyNode;

			CHierachyNode* m_contextNode;

			CHierachyNode* m_focusNode;

			CScene* m_scene;

			CZone* m_zone;

			CSceneHistory* m_history;

			GUI::CCanvas* m_canvas;

			CContextMenuScene* m_contextMenuScene;

			CGizmos* m_gizmos;

			CTransformGizmos* m_transformGizmos;

			CWorldTransformDataGizmos* m_worldTransformDataGizmos;

			CParticleGizmos* m_particleGizmos;

			std::string m_scenePath;

		public:
			CSceneController();

			virtual ~CSceneController();

			void update();

			void updateSelectedRigidBody();

			void refresh();

			inline CTransformGizmos* getTransformGizmos()
			{
				return m_transformGizmos;
			}

			inline CWorldTransformDataGizmos* getWorldTransformDataGizmos()
			{
				return m_worldTransformDataGizmos;
			}

			inline CParticleGizmos* getParticleGizmos()
			{
				return m_particleGizmos;
			}

			inline CGizmos* getGizmos()
			{
				return m_gizmos;
			}

			void setGizmos(CGizmos* gizmos);

			void initContextMenu(GUI::CCanvas* canvas);

			void setSpaceScene(CSpaceScene* scene);

			void removeAllHierarchyNodes();

			inline CSpaceScene* getSpaceScene()
			{
				return m_spaceScene;
			}

			inline IRenderPipeline* getRenderPipeLine()
			{
				if (m_spaceScene)
					return m_spaceScene->getRenderPipeline();
				return NULL;
			}

			inline void enablePostProcessing(bool b)
			{
				if (m_spaceScene)
					return m_spaceScene->enablePostProcessing(b);
			}

			CSelectObjectSystem* getSelectObjectSystem()
			{
				return m_spaceScene->getSelectObjectSystem();
			}

			void setSpaceHierarchy(CSpaceHierarchy* hierarchy);

			inline CSpaceHierarchy* getSpaceHierarchy()
			{
				return m_spaceHierarchy;
			}

			bool needSave();

			void save(const char* path);

			virtual void loadFile(const std::string& path);

			void newScene();

			void doNewScene();

			void doLoadScene(const std::string& path);

			void doFinishLoadScene();

			void doMeshChange(const char* resource);

			void doMaterialChange(const char* resource);

			const std::string& getScenePath()
			{
				return m_scenePath;
			}

			void deleteScene();

			void setScene(CScene* scene);

			void reinitHierachyData();

			inline CScene* getScene()
			{
				return m_scene;
			}

			void setZone(CZone* zone);

			CZone* getZone()
			{
				return m_zone;
			}

			CSceneHistory* getHistory()
			{
				return m_history;
			}

			void buildHierarchyData();

			void onCommand(const std::wstring& objectType);

			void onContextMenu(CHierachyNode* node);

			void onUpdateNode(CHierachyNode* node);

			void onSelectNode(CHierachyNode* node, bool selected);

			void onDoubleClickNode(CHierachyNode* node);

			void focusCameraToSelectObject();

			void onObjectChange(CGameObject* object);

			void onHistoryModifyObject(CGameObject* object);

			void onMoveStructure(CGameObject* object, CContainerObject* toContainer, CGameObject* before);

			void onMoveStructure(CZone* object, CZone* before);

			void onDeleteObject(CGameObject* object);

			void onDelete();

			void onCopy();

			void onPaste();

			void onDuplicate();

			void onCut();

			void onUndo();

			void onRedo();

			virtual void onNotify(ISubject* subject, IObserver* from);

			void deselectAllOnHierachy(bool callEvent = true);

			CHierachyNode* deselectOnHierachy(CGameObject* gameObject, bool callEvent = true);

			CHierachyNode* deselectOnHierachy(CEntity* entity, bool callEvent = true);

			CHierachyNode* selectOnHierachy(CGameObject* gameObject, bool callEvent = true);

			CHierachyNode* selectOnHierachy(CEntity* entity, bool callEvent = true);

			inline CHierachyNode* getContextNode()
			{
				return m_contextNode;
			}

			inline void clearContextNode()
			{
				m_contextNode = NULL;
			}

			void createZone(bool saveHistory = true);

			void removeObject(CGameObject* gameObject);

			CZone* createZoneObject(CObjectSerializable* data, CGameObject* before, bool saveHistory = true);

			CGameObject* createGameObject(CContainerObject* parent, CGameObject* before, CObjectSerializable* data, bool saveHistory = true);

			CGameObject* createEmptyObject(CContainerObject* parent, bool saveHistory = true);

			CGameObject* createContainerObject(CContainerObject* parent, bool saveHistory = true);

			CGameObject* createComponentObject(const char* name, std::vector<std::string>& components, CContainerObject* parent, bool saveHistory = true);

			void createResourceComponent(const std::string& path, CGameObject* gameObject);

			CGameObject* createTemplateObject(const std::string& path, CContainerObject* container, bool saveHistory = true);

			CHierachyNode* buildHierarchyData(CGameObject* object, CHierachyNode* parentNode);

			void rebuildHierarchyData(CGameObject* object);

			void rebuildHierarchyEntityData(CGameObject* object, CHierachyNode* parentNode);

			void updateTreeNode(CGameObject* object);

			void onCreateTemplate(CGameObject* object);

			void onCreateTemplate(CGameObject* object, const char* folder);

			void onApplyTemplate(CGameObject* object);

			void doFinishApplyTemplate(std::list<CGameObject*>& objects);

			void onRevertTemplate(CGameObject* object);

			void onUnpackTemplate(CGameObject* object);

			void focusCameraToEntity(CEntity* entity);
			
			void focusCameraToObject(CGameObject* obj);

			void applySelected(std::vector<CSelectObject*> ids);

		protected:

			void setNodeEvent(CHierachyNode* node);
		};
	}
}
