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
#include "CSelection.h"
#include "Entity/CEntityHandleData.h"

#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CSelection);

		CSelection::CSelection()
		{

		}

		CSelection::~CSelection()
		{
			clear();
		}

		void CSelection::clear()
		{
			for (CSelectObject* selectObject : m_selected)
				delete selectObject;

			m_selected.clear();

			CSceneHistory* history = CSceneController::getInstance()->getHistory();
			if (history)
				history->endSaveHistory();
		}

		std::vector<CSelectObject*> CSelection::getSelectedByType(CSelectObject::ESelectType type)
		{
			std::vector<CSelectObject*> result;

			for (CSelectObject* selected : m_selected)
			{
				if (selected->getType() == type)
				{
					result.push_back(selected);
				}
			}

			return result;
		}

		CSelectObject* CSelection::getSelectedById(CSelectObject::ESelectType type, const char* id)
		{
			for (CSelectObject* selected : m_selected)
			{
				if (selected->getType() == type &&
					selected->getID() == id)
				{
					return selected;
				}
			}
			return NULL;
		}

		CSelectObject* CSelection::getLastSelected()
		{
			CSelectObject* ret = NULL;
			if (m_selected.size() > 0)
			{
				ret = m_selected.back();
			}
			return ret;
		}

		void CSelection::notify(CGameObject* obj, IObserver* from)
		{
			CSelectObject* select = getSelected(obj);
			if (select != NULL)
				select->notify(from);
		}

		void CSelection::notify(CGUIElement* gui, IObserver* from)
		{
			CSelectObject* select = getSelected(gui);
			if (select != NULL)
				select->notify(from);
		}

		CSelectObject* CSelection::getSelected(CGameObject* obj)
		{
			return getSelectedById(CSelectObject::GameObject, obj->getID().c_str());
		}

		CSelectObject* CSelection::getSelected(CEntity* entity)
		{
			std::string id = entity->getID();
			if (id.empty())
				return NULL;

			return getSelectedById(CSelectObject::Entity, id.c_str());
		}

		CSelectObject* CSelection::getSelected(CGUIElement* gui)
		{
			std::string id = gui->getID();
			if (id.empty())
				return NULL;

			return getSelectedById(CSelectObject::GUIElement, id.c_str());
		}

		CSelectObject* CSelection::addSelect(CGameObject* obj)
		{
			CSelectObject* selected = getSelected(obj);
			if (selected != NULL)
				return selected;

			selected = new CSelectObject(obj);
			m_selected.push_back(selected);

			// notify history
			CSceneHistory* history = CSceneController::getInstance()->getHistory();
			history->beginSaveHistory(obj);

			return selected;
		}

		CSelectObject* CSelection::addSelect(CEntity* entity)
		{
			CSelectObject* selected = getSelected(entity);
			if (selected != NULL)
				return selected;

			selected = new CSelectObject(entity);
			m_selected.push_back(selected);

			// notify history
			CEntityHandleData* hander = GET_ENTITY_DATA(entity, CEntityHandleData);
			if (hander && hander->Handler)
			{
				CSceneHistory* history = CSceneController::getInstance()->getHistory();
				history->beginSaveHistory(hander->Handler->getGameObject());
			}

			return selected;
		}

		CSelectObject* CSelection::addSelect(CGUIElement* gui)
		{
			CSelectObject* selected = getSelected(gui);
			if (selected != NULL)
				return selected;

			gui->setDrawBorder(true);

			selected = new CSelectObject(gui);
			m_selected.push_back(selected);

			return selected;
		}

		void CSelection::addSelect(const std::vector<CGameObject*>& obj)
		{
			for (CGameObject* s : obj)
			{
				addSelect(s);
			}
		}
	
		void CSelection::addSelect(const std::vector<CGUIElement*>& ui)
		{
			for (CGUIElement* s : ui)
			{
				addSelect(s);
			}
		}

		bool CSelection::unSelect(CGameObject* obj)
		{
			std::vector<CSelectObject*>::iterator i = m_selected.begin(), end = m_selected.end();
			while (i != end)
			{
				CSelectObject* sel = (*i);

				if (sel->getType() == CSelectObject::GameObject && sel->getID() == obj->getID())
				{
					// notify history
					CSceneHistory* history = CSceneController::getInstance()->getHistory();
					history->removeSaveHistory(obj);

					delete sel;
					m_selected.erase(i);
					return true;
				}
				++i;
			}

			return false;
		}

		bool CSelection::unSelect(const std::vector<CGameObject*>& obj)
		{
			bool ret = false;
			for (CGameObject* gameObject : obj)
			{
				if (unSelect(gameObject))
					ret = true;
			}
			return ret;
		}

		void CSelection::unSelect(CEntity* entity)
		{
			std::string id = entity->getID();
			if (id.empty())
				return;

			std::vector<CSelectObject*>::iterator i = m_selected.begin(), end = m_selected.end();
			while (i != end)
			{
				CSelectObject* sel = (*i);

				if (sel->getType() == CSelectObject::Entity && sel->getID() == id)
				{
					// notify history
					CEntityHandleData* hander = GET_ENTITY_DATA(entity, CEntityHandleData);
					if (hander && hander->Handler)
					{
						CSceneHistory* history = CSceneController::getInstance()->getHistory();
						history->removeSaveHistory(hander->Handler->getGameObject());
					}

					delete sel;
					m_selected.erase(i);
					return;
				}
				++i;
			}
		}

		void CSelection::unSelect(const std::vector<CEntity*>& entities)
		{
			for (CEntity* entity : entities)
			{
				unSelect(entity);
			}
		}

		void CSelection::unSelect(CGUIElement* gui)
		{
			std::string id = gui->getID();
			if (id.empty())
				return;

			gui->setDrawBorder(false);

			std::vector<CSelectObject*>::iterator i = m_selected.begin(), end = m_selected.end();
			while (i != end)
			{
				CSelectObject* sel = (*i);
				if (sel->getType() == CSelectObject::GUIElement && sel->getID() == id)
				{
					delete sel;
					m_selected.erase(i);
					return;
				}
				++i;
			}
		}
	}
}
