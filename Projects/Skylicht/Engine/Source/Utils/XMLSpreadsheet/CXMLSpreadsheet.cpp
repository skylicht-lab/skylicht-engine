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

#include "pch.h"
#include "CXMLSpreadsheet.h"
#include "Utils/CStringImp.h"

#include <time.h> 

namespace Skylicht
{
	CXMLSpreadsheet::CXMLSpreadsheet()
	{

	}

	CXMLSpreadsheet::~CXMLSpreadsheet()
	{
		for (SSheet* s : m_sheets)
		{
			delete s;
		}
		m_sheets.clear();
	}

	bool CXMLSpreadsheet::open(const char *file)
	{
		io::IXMLReader *xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (xmlReader == NULL)
			return false;

		bool result = open(xmlReader);

		xmlReader->drop();
		return result;
	}

	bool CXMLSpreadsheet::open(io::IXMLReader *xmlReader)
	{
		wchar_t textw[1024 * 4];
		char text[1024 * 4];

		int readState = -1;
		int row = -1;
		int col = -1;

		SSheet *currentSheet = NULL;
		SRow* currentRow = NULL;
		SCell* currentCell = NULL;

		while (xmlReader->read())
		{
			switch (xmlReader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"Worksheet")
				{
					const wchar_t *ssName = xmlReader->getAttributeValue(L"ss:Name");

					currentSheet = new SSheet();
					m_sheets.push_back(currentSheet);

					if (ssName != NULL)
					{
						CStringImp::copy(text, ssName);
						currentSheet->Name = text;

						CStringImp::convertUTF8ToUnicode(text, textw);
						currentSheet->NameUnicode = textw;
					}
				}
				else if (nodeName == L"Table")
				{
					if (currentSheet == NULL)
						return false;

					const wchar_t *ssColCount = xmlReader->getAttributeValue(L"ss:ExpandedColumnCount");
					const wchar_t *ssRowCount = xmlReader->getAttributeValue(L"ss:ExpandedRowCount");

					if (ssColCount != NULL)
					{
						CStringImp::copy(text, ssColCount);
						currentSheet->NumCol = atoi(text);
					}

					if (ssRowCount != NULL)
					{
						CStringImp::copy(text, ssRowCount);
						currentSheet->NumRow = atoi(text);
					}
				}
				else if (nodeName == L"Row")
				{
					if (currentSheet == NULL)
						return false;

					const wchar_t *ssIndex = xmlReader->getAttributeValue(L"ss:Index");
					if (ssIndex == NULL)
					{
						row++;
					}
					else
					{
						CStringImp::copy(text, ssIndex);
						row = atoi(text) - 1;
					}

					currentRow = new SRow();
					currentRow->Index = row;
					currentSheet->Rows.push_back(currentRow);
					col = -1;
				}
				else if (nodeName == L"Cell")
				{
					if (currentRow == NULL)
						return false;

					const wchar_t *ssIndex = xmlReader->getAttributeValue(L"ss:Index");
					if (ssIndex == NULL)
					{
						col++;
					}
					else
					{
						CStringImp::copy(text, ssIndex);
						col = atoi(text) - 1;
					}

					currentCell = new SCell();
					currentCell->Row = row;
					currentCell->Col = col;

					currentRow->Cells.push_back(currentCell);
					readState = 0;
				}
				else if (nodeName == L"Data")
				{
					if (currentCell == NULL)
						return false;

					std::wstring ssType = xmlReader->getAttributeValue(L"ss:Type");
					if (ssType == L"Number")
						currentCell->Type = Decimal;
					else if (ssType == L"DateTime")
						currentCell->Type = DateTime;
					else
						currentCell->Type = String;

					readState = 1;
				}
			}
			break;
			case io::EXN_TEXT:
			{
				if (readState == 1)
				{
					if (currentCell == NULL)
						return false;

					const wchar_t *data = xmlReader->getNodeData();

					CStringImp::copy(text, data);
					currentCell->Value = text;

					CStringImp::convertUTF8ToUnicode(text, textw);
					currentCell->UnicodeValue = textw;

					if (currentCell->Type == Decimal)
					{
						currentCell->NumberInt = atoi(text);
						currentCell->NumberFloat = (float)atof(text);
					}
					else if (currentCell->Type == DateTime)
					{
						int y, mm, d, h, m;
						float s;
						if (sscanf(text, "%d-%d-%dT%d:%d:%f", &y, &mm, &d, &h, &m, &s) != -1)
						{
							currentCell->TimeDay = d;
							currentCell->TimeMonth = mm;
							currentCell->TimeYear = y;
							currentCell->TimeHour = h;
							currentCell->TimeMin = m;
							currentCell->TimeSec = (int)s;

							tm t;
							t.tm_year = y - 1900;
							t.tm_mon = mm - 1;
							t.tm_mday = d;
							t.tm_hour = h;
							t.tm_min = m;
							t.tm_sec = (int)s;
							currentCell->Time = (long)mktime(&t);
						}
					}

					readState = -1;
				}
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"Row")
				{
					currentRow = NULL;
				}
				else if (nodeName == L"Cell")
				{
					currentCell = NULL;
				}
				else if (nodeName == L"Worksheet")
				{
					currentSheet = NULL;
					row = -1;
					col = -1;
				}
			}
			break;
			default:
			{

			}
			break;
			}
		}

		return true;
	}

	CXMLSpreadsheet::SCell* CXMLSpreadsheet::getCell(SSheet* sheet, u32 row, u32 col)
	{
		for (SRow* r : sheet->Rows)
		{
			if (r->Index == row)
			{
				for (SCell* c : r->Cells)
				{
					if (c->Col == col)
					{
						return c;
					}
				}
			}
			else if (r->Index > row)
			{
				return NULL;
			}
		}

		return NULL;
	}

	CXMLSpreadsheet::SCell* CXMLSpreadsheet::getCell(SSheet* sheet, const char *cellName)
	{
		u32 row, col;
		if (convertCellName(cellName, row, col))
		{
			return getCell(sheet, row, col);
		}
		return NULL;
	}

	std::list<CXMLSpreadsheet::SCell*> CXMLSpreadsheet::getRange(SSheet* sheet, u32 fromRow, u32 fromCol, u32 toRow, u32 toCol)
	{
		if (fromRow > toRow)
		{
			u32 t = fromRow;
			fromRow = toRow;
			toRow = t;
		}

		if (fromCol > toCol)
		{
			u32 t = fromCol;
			fromCol = toCol;
			toCol = t;
		}

		std::list<CXMLSpreadsheet::SCell*> result;

		for (SRow* r : sheet->Rows)
		{
			if (r->Index > toRow)
			{
				break;
			}
			else if (r->Index >= fromRow)
			{
				for (SCell* c : r->Cells)
				{
					if (c->Col >= fromCol && c->Col <= toCol)
					{
						result.push_back(c);
					}
				}
			}
		}

		return result;
	}

	std::list<CXMLSpreadsheet::SCell*> CXMLSpreadsheet::getRange(SSheet* sheet, const char *from, const char *to)
	{
		u32 fromRow;
		u32 fromCol;
		u32 toRow;
		u32 toCol;

		convertCellName(from, fromRow, fromCol);
		convertCellName(to, toRow, toCol);

		return getRange(sheet, fromRow, fromCol, toRow, toCol);
	}

	bool CXMLSpreadsheet::convertCellName(const char *cellName, u32& row, u32 &col)
	{
		row = 0;
		col = 0;

		int n = strlen(cellName);
		if (n > 64)
			return false;

		char cellUpper[64];
		CStringImp::copy(cellUpper, cellName);
		CStringImp::toUpper(cellUpper);

		char colName[64];
		char rowName[64];
		int iCol = 0;
		int iRow = 0;
		colName[iCol] = 0;
		rowName[iRow] = 0;

		for (int i = 0; i < n; i++)
		{
			if (cellUpper[i] >= 'A' && cellUpper[i] <= 'Z')
			{
				colName[iCol++] = cellUpper[i];
				colName[iCol] = 0;
			}

			if (cellUpper[i] >= '0' && cellUpper[i] <= '9')
			{
				rowName[iRow++] = cellUpper[i];
				rowName[iRow] = 0;
			}
		}

		int rangeValue = 25; // 'Z' - 'A';
		int base = 1;

		for (int i = strlen(colName) - 1; i >= 0; i--)
		{
			col += (colName[i] - 'A') * base;
			base = base * rangeValue;
		}

		row = atoi(rowName) - 1;
		return true;
	}
}