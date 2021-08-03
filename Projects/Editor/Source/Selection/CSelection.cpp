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

namespace Skylicht
{
	namespace Editor
	{
		CSelection::CSelection()
		{

		}

		CSelection::~CSelection()
		{

		}

		void CSelection::clear()
		{
			m_selected.clear();
		}

		std::vector<CSelectObject> CSelection::getSelectedByType(CSelectObject::ESelectType type)
		{
			std::vector<CSelectObject> result;

			for (CSelectObject selected : m_selected)
			{
				if (selected.getType() == type)
				{
					result.push_back(selected);
				}
			}

			return result;
		}

		CSelectObject CSelection::getLastSelected()
		{
			CSelectObject ret;
			if (m_selected.size() > 0)
			{
				ret = m_selected.back();
			}
			return ret;
		}

		void CSelection::addSelect(const CSelectObject& obj)
		{
			if (std::find(m_selected.begin(), m_selected.end(), obj) != m_selected.end())
				return;

			m_selected.push_back(obj);
		}

		void CSelection::addSelect(const std::vector<CSelectObject>& obj)
		{
			for (CSelectObject s : obj)
			{
				addSelect(s);
			}
		}

		void CSelection::unSelect(const CSelectObject& obj)
		{
			std::vector<CSelectObject>::iterator i = m_selected.begin(), end = m_selected.end();
			while (i != end)
			{
				if ((*i) == obj)
				{
					m_selected.erase(i);
					return;
				}
				++i;
			}
		}

		void CSelection::unSelect(const std::vector<CSelectObject>& obj)
		{
			for (CSelectObject s : obj)
			{
				unSelect(s);
			}
		}
	}
}