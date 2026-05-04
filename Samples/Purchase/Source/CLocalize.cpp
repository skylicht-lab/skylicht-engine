#include "pch.h"
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

IMPLEMENT_SINGLETON(CLocalize);

CLocalize::CLocalize() :
	m_language(ELanguage::EN)
{

}

CLocalize::~CLocalize()
{
	m_map.clear();
}

void CLocalize::init(const char* cfgFile)
{
	CBaseConfig::init(cfgFile);

	for (CObjectSerializable* data : m_data)
	{
		CDataText* t = dynamic_cast<CDataText*>(data);
		if (t)
			m_map[t->SRING_ID.get()] = t;
	}
}

const char* CLocalize::getString(const char* id)
{
	std::map<std::string, CDataText*>::iterator i = m_map.find(id);
	if (i == m_map.end())
		return "";

	return (*i).second->getString(m_language);
}

const char* CLocalize::getString(ELanguage lang, const char* id)
{
	std::map<std::string, CDataText*>::iterator i = m_map.find(id);
	if (i == m_map.end())
		return "";

	return (*i).second->getString(lang);
}

const char* CLocalize::get(const char* id)
{
	return CLocalize::getInstance()->getString(id);
}

const char* CLocalize::get(ELanguage lang, const char* id)
{
	return CLocalize::getInstance()->getString(lang, id);
}