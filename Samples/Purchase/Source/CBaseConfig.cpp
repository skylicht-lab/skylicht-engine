#include "pch.h"
#include "CBaseConfig.h"
#include "Utils/XMLSpreadsheet/CXMLSpreadsheet.h"
#include "Utils/XMLSpreadsheet/CXMLTableData.h"

CBaseConfig::CBaseConfig()
{

}

CBaseConfig::~CBaseConfig()
{
	for (CObjectSerializable* data : m_data)
		delete data;
	m_data.clear();
}

void CBaseConfig::init(const char* cfgFile)
{
	CXMLSpreadsheet sheets;
	if (sheets.openCSV(cfgFile))
	{
		CXMLTableData table(sheets.getSheet(0));

		CObjectSerializable* header = createEmptyData();
		if (!header)
			return;

		for (u32 i = 0, n = header->getNumProperty(); i < n; i++)
		{
			CValueProperty* value = header->getPropertyID(i);
			table.addColumn(value->Name.c_str());
		}

		table.fetchData([&] {
			return this->createEmptyData();
			},
			m_data,
			1, -1);

		delete header;
	}
}

CObjectSerializable* CBaseConfig::createEmptyData()
{
	return NULL;
}