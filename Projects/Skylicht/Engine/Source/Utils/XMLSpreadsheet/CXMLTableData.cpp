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
#include "CXMLTableData.h"

namespace Skylicht
{
	CXMLTableData::CXMLTableData(CXMLSpreadsheet::SSheet *sheet) :
		m_sheet(sheet)
	{
	}

	CXMLTableData::~CXMLTableData()
	{

	}

	void CXMLTableData::addColumn(EPropertyDataType type, const char *name)
	{
		m_column.push_back(new CXMLColumn(type, name));
	}

	void CXMLTableData::insertColumn(EPropertyDataType type, const char *name, u32 position)
	{
		std::vector<CXMLColumn*>::iterator i = m_column.begin();
		i += position;
		m_column.insert(i, new CXMLColumn(type, name));
	}

	void CXMLTableData::removeColumn(CXMLColumn *col)
	{
		std::vector<CXMLColumn*>::iterator i = m_column.begin(), end = m_column.end();
		while (i != end)
		{
			if ((*i) == col)
			{
				m_column.erase(i);
				return;
			}
			++i;
		}
	}

	void CXMLTableData::removeColumn(u32 index)
	{
		std::vector<CXMLColumn*>::iterator i = m_column.begin();
		i += index;
		m_column.erase(i);
	}
}