#pragma once

#include "Serializable/CObjectSerializable.h"
#include "Utils/CSingleton.h"

class CBaseConfig
{
protected:
	std::vector<CObjectSerializable*> m_data;

public:
	CBaseConfig();

	virtual ~CBaseConfig();

	virtual void init(const char* cfgFile);

	virtual CObjectSerializable* createEmptyData();

	inline const std::vector<CObjectSerializable*>& getData()
	{
		return m_data;
	}
};
