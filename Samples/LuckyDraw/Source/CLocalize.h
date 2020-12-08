#pragma once

enum class ELanguage
{
	EN,
	VN
};

class CDataText : public CObjectSerizable
{
public:
	CStringProperty SRING_ID;
	CStringProperty LANG_EN;
	CStringProperty LANG_VN;

	CDataText();

	virtual ~CDataText();

	const char* getString(ELanguage lang);
};

class CLocalize : public CGameSingleton<CLocalize>
{
protected:
	std::vector<CDataText*> m_text;
	std::map<std::string, CDataText*> m_map;

	ELanguage m_language;

public:
	CLocalize();

	virtual ~CLocalize();

	void init(CXMLSpreadsheet* excel);

	const char* getString(const char* id);

	void setLanguage(ELanguage lang)
	{
		m_language = lang;
	}

	static const char* get(const char* id)
	{
		return CLocalize::getInstance()->getString(id);
	}
};