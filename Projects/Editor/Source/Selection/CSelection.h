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
#include "CSelectObject.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSelection
		{
		public:
			DECLARE_SINGLETON(CSelection)

		protected:
			std::vector<CSelectObject*> m_selected;

		public:
			CSelection();

			virtual ~CSelection();

			std::vector<CSelectObject*>& getSelected()
			{
				return m_selected;
			}

			bool isSelected()
			{
				return m_selected.size() > 0;
			}

			std::vector<CSelectObject*> getSelectedByType(CSelectObject::ESelectType type);

			CSelectObject* getSelectedById(CSelectObject::ESelectType type, const char* id);

			void clear();

			std::vector<CSelectObject*>& getAllSelected()
			{
				return m_selected;
			}

			CSelectObject* getLastSelected();

			CSelectObject* getSelected(CGameObject* obj);

			CSelectObject* getSelected(CEntity* entity);

			CSelectObject* getSelected(CGUIElement* gui);

			CSelectObject* addSelect(CGameObject* obj);

			CSelectObject* addSelect(CEntity* entity);

			CSelectObject* addSelect(CGUIElement* gui);

			void notify(CGameObject* obj, IObserver* from);

			void notify(CGUIElement* gui, IObserver* from);

			void addSelect(const std::vector<CGameObject*>& obj);

			void addSelect(const std::vector<CGUIElement*>& ui);
			
			bool unSelect(CGameObject* obj);

			bool unSelect(const std::vector<CGameObject*>& obj);

			void unSelect(CEntity* entity);

			void unSelect(const std::vector<CEntity*>& entities);

			void unSelect(CGUIElement* gui);
		};
	}
}
