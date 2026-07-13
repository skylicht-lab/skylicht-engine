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
	/**
	 * @brief Table adapter that maps spreadsheet rows to serializable objects.
	 * @ingroup Utilities
	 *
	 * Columns define the property names used when copying row values into
	 * `CObjectSerializable` instances.
	 */
	class SKYLICHT_API CXMLTableData
	{
	protected:
		std::vector<std::string> m_column;

		CXMLSpreadsheet::SSheet* m_sheet;

	public:
		/**
		 * @brief Construct a table adapter for a sheet.
		 * @param sheet Source sheet. The adapter does not own this pointer.
		 */
		CXMLTableData(CXMLSpreadsheet::SSheet* sheet);

		/**
		 * @brief Destroy the table adapter.
		 */
		virtual ~CXMLTableData();

		/**
		 * @brief Append a column mapping.
		 * @param name Serializable property name for the column.
		 */
		void addColumn(const char* name);

		/**
		 * @brief Insert a column mapping at a position.
		 * @param name Serializable property name for the column.
		 * @param position Zero-based insertion position.
		 */
		void insertColumn(const char* name, u32 position);

		/**
		 * @brief Remove a column mapping.
		 * @param index Zero-based column index.
		 */
		void removeColumn(u32 index);

		/**
		 * @brief Get a column mapping name.
		 * @param index Zero-based column index.
		 * @return Column name.
		 */
		inline const char* getColumn(u32 index)
		{
			return m_column[index].c_str();
		}

		/**
		 * @brief Get the number of mapped columns.
		 * @return Column count.
		 */
		u32 getNumColumn()
		{
			return (u32)m_column.size();
		}

		/**
		 * @brief Get the number of rows in the source sheet.
		 * @return Row count.
		 */
		u32 getNumRow()
		{
			return (u32)m_sheet->Rows.size();
		}

		/**
		 * @brief Create and fill serializable objects from sheet rows.
		 * @param creatorFunc Function that creates a new serializable object.
		 * @param data Receives created objects. Ownership is transferred to the caller.
		 * @param fromRow First row index to fetch.
		 * @param count Maximum number of rows to fetch, or a non-positive value for all rows.
		 * @return Number of objects appended to `data`.
		 */
		u32 fetchData(std::function<CObjectSerializable* ()> creatorFunc, std::vector<CObjectSerializable*>& data, u32 fromRow, int count);

		/**
		 * @brief Copy one spreadsheet row into a serializable object.
		 * @param row Source row.
		 * @param data Destination serializable object.
		 */
		void copyRowToObject(CXMLSpreadsheet::SRow* row, CObjectSerializable* data);

		/**
		 * @brief Delete all pointers in a fetched data vector and clear it.
		 * @tparam T Object type stored in the vector.
		 * @param data Vector of owned pointers.
		 */
		template<class T>
		void freeData(std::vector<T*>& data)
		{
			for (T* i : data)
				delete i;
			data.clear();
		}

		/**
		 * @brief Create and fill typed serializable objects from sheet rows.
		 *
		 * `T` must derive from `CObjectSerializable`.
		 *
		 * @tparam T Object type to allocate.
		 * @param data Receives created objects. Ownership is transferred to the caller.
		 * @param fromRow First row index to fetch.
		 * @param count Maximum number of rows to fetch, or a non-positive value for all rows.
		 * @return Number of objects appended to `data`, or 0 if `T` is not serializable.
		 */
		template<class T>
		u32 fetchData(std::vector<T*>& data, u32 fromRow, int count)
		{
			std::vector<CXMLSpreadsheet::SRow*>::iterator i = m_sheet->Rows.begin(), end = m_sheet->Rows.end();
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

				copyRowToObject(row, objectSerizable);

				data.push_back(obj);

				++i;
			}

			return (u32)data.size();
		}
	};
}
