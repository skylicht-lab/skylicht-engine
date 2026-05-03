#pragma once

#include "CBaseConfig.h"

enum class ELanguage
{
	EN,
	VN
};

class CDataText : public CObjectSerializable
{
public:
	CStringProperty STRING_ID;
	CStringProperty LANG_EN;
	CStringProperty LANG_VN;

	CDataText();

	virtual ~CDataText();

	const char* getString(ELanguage lang);
};


class CLocalize : public CBaseConfig
{
protected:
	std::vector<CDataText*> m_text;
	std::map<std::string, CDataText*> m_map;

	ELanguage m_language;

public:
	CLocalize();

	virtual ~CLocalize();

	virtual void init(const char* cfgFile);

	virtual CObjectSerializable* createEmptyData()
	{
		return new CDataText();
	}

	inline void setLanguage(ELanguage lang)
	{
		m_language = lang;
	}

	inline ELanguage getLanguage()
	{
		return m_language;
	}

	const char* getString(const char* id);

	const char* getString(ELanguage lang, const char* id);

	static const char* get(const char* id);

	static const char* get(ELanguage lang, const char* id);

	DECLARE_SINGLETON(CLocalize)
};
