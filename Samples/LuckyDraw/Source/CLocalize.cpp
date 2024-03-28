#include "pch.h"
#include "SkylichtEngine.h"
#include "CLocalize.h"

CDataText::CDataText() :
	CObjectSerializable("CDataText"),
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

IMPLEMENT_SINGLETON(CLocalize)

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
	CXMLTableData table(excel->getSheet(0));
	table.addColumn("STRING_ID");
	table.addColumn("LANG_EN");
	table.addColumn("LANG_VN");
	table.fetchData(m_text, 1, -1);

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