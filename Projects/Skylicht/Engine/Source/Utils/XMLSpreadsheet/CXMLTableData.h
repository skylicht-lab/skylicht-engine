/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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

#include "CXMLSpreadsheet.h"
#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	class SKYLICHT_API CXMLTableData
	{
	protected:
		std::vector<std::string> m_column;

		CXMLSpreadsheet::SSheet* m_sheet;

	public:
		CXMLTableData(CXMLSpreadsheet::SSheet* sheet);

		virtual ~CXMLTableData();

		void addColumn(const char* name);

		void insertColumn(const char* name, u32 position);

		void removeColumn(u32 index);

		inline const char* getColumn(u32 index)
		{
			return m_column[index].c_str();
		}

		u32 getNumColumn()
		{
			return (u32)m_column.size();
		}

		u32 getNumRow()
		{
			return (u32)m_sheet->Rows.size();
		}

		template<class T>
		void freeData(std::vector<T*>& data)
		{
			for (T* i : data)
				delete i;
			data.clear();
		}

		template<class T>
		u32 fetchData(std::vector<T*>& data, u32 fromRow, int count)
		{
			std::list<CXMLSpreadsheet::SRow*>::iterator i = m_sheet->Rows.begin(), end = m_sheet->Rows.end();
			while (i != end)
			{
				CXMLSpreadsheet::SRow* row = (*i);
				if (row->Index < fromRow)
				{
					++i;
					continue;
				}

				if (count > 0)
				{
					if (row->Index >= fromRow + (u32)count)
						break;
				}

				T* obj = new T();
				CObjectSerializable* objectSerizable = dynamic_cast<CObjectSerializable*>(obj);
				if (objectSerizable == NULL)
				{
					delete obj;
					return 0;
				}

				std::list<CXMLSpreadsheet::SCell*>::iterator j = row->Cells.begin(), e = row->Cells.end();
				while (j != e)
				{
					CXMLSpreadsheet::SCell* cell = (*j);
					if (cell->Col < m_column.size())
					{
						const std::string& colName = m_column[cell->Col];
						{
							CValueProperty* value = objectSerizable->getProperty(colName.c_str());
							if (value == NULL)
								continue;

							switch (value->getType())
							{
							case String:
								(dynamic_cast<CStringProperty*>(value))->set(cell->Value.c_str());
								break;
							case Integer:
								(dynamic_cast<CIntProperty*>(value))->set(cell->NumberInt);
								break;
							case Float:
								(dynamic_cast<CFloatProperty*>(value))->set(cell->NumberFloat);
								break;
							case DateTime:
								(dynamic_cast<CDateTimeProperty*>(value))->set(cell->Time);
								break;
							default:
								break;
							}
						}
					}

					++j;
				}

				data.push_back(obj);

				++i;
			}

			return (u32)data.size();
		}
	};
}