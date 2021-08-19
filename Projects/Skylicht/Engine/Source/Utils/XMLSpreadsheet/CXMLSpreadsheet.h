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

#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	class CXMLSpreadsheet
	{
	public:
		struct SCell
		{
			u32 Row;
			u32 Col;
			EPropertyDataType Type;
			std::string Value;
			std::wstring UnicodeValue;
			int NumberInt;
			float NumberFloat;
			long Time;
			int TimeYear;
			int TimeMonth;
			int TimeDay;
			int TimeHour;
			int TimeMin;
			int TimeSec;

			SCell()
			{
				Row = 0;
				Col = 0;
				Type = String;
				NumberInt = 0;
				NumberFloat = 0.0f;
				Time = 0;
				TimeYear = 0;
				TimeMonth = 0;
				TimeDay = 0;
				TimeHour = 0;
				TimeMin = 0;
				TimeSec = 0;
			}
		};

		struct SRow
		{
			u32 Index;
			std::list<SCell*> Cells;

			~SRow()
			{
				for (SCell* c : Cells)
				{
					delete c;
				}
				Cells.clear();
			}
		};

		struct SSheet
		{
			int NumRow;
			int NumCol;
			std::string Name;
			std::wstring NameUnicode;
			std::list<SRow*> Rows;

			~SSheet()
			{
				for (SRow* r : Rows)
				{
					delete r;
				}
				Rows.clear();
			}
		};

	protected:
		std::vector<SSheet*> m_sheets;

	public:
		CXMLSpreadsheet();

		virtual ~CXMLSpreadsheet();

		bool open(const char* file);

		bool open(io::IXMLReader* xmlReader);

		inline u32 getSheetCount()
		{
			return (u32)m_sheets.size();
		}

		SSheet* getSheet(int i)
		{
			return m_sheets[i];
		}

		SSheet* operator[](int i)
		{
			return m_sheets[i];
		}

		SCell* getCell(SSheet* sheet, u32 row, u32 col);

		SCell* getCell(SSheet* sheet, const char* cellName);

		std::list<SCell*> getRange(SSheet* sheet, u32 fromRow, u32 fromCol, u32 toRow, u32 toCol);

		std::list<SCell*> getRange(SSheet* sheet, const char* from, const char* to);

		bool convertCellName(const char* cellName, u32& row, u32& col);
	};
}