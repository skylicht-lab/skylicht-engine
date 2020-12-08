#include "pch.h"
#include "SkylichtEngine.h"
#include "CLocalize.h"

CDataText::CDataText() :
	SRING_ID(this, "STRING_ID"),
	LANG_EN(this, "LANG_EN"),
	LANG_VN(this, "LANG_VN")
{

}

CDataText::~CDataText()
{

}

const char* CDataText::getString(ELanguage lang)
{
	const char* result = "";
	switch (lang)
	{
	case ELanguage::EN:
		result = LANG_EN.getString();
		break;
	case ELanguage::VN:
		result = LANG_VN.getString();
		break;
	default:
		break;
	}
	return result;
}


CLocalize::CLocalize() :
	m_language(ELanguage::EN)
{

}

CLocalize::~CLocalize()
{
	for (CDataText* t : m_text)
		delete t;
	m_text.clear();
	m_map.clear();
}

void CLocalize::init(CXMLSpreadsheet* excel)
{
	CXMLTableData peopleTable(excel->getSheet(0));
	peopleTable.addColumn("STRING_ID");
	peopleTable.addColumn("LANG_EN");
	peopleTable.addColumn("LANG_VN");
	peopleTable.fetchData(m_text, 1, -1);

	for (CDataText* t : m_text)
		m_map[t->SRING_ID.get()] = t;
}

const char* CLocalize::getString(const char* id)
{
	std::map<std::string, CDataText*>::iterator i = m_map.find(id);
	if (i == m_map.end())
		return "";

	return (*i).second->getString(m_language);
}