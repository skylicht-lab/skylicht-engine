#pragma once

#include "CShopData.h"
#include "CBaseConfig.h"
#include "Utils/CSingleton.h"

class CShopConfig : public CBaseConfig
{
protected:

public:
	CShopConfig();

	virtual ~CShopConfig();

	virtual void init(const char* cfgFile);

	CShopData* getDataByProductId(const char* productId);

	virtual CObjectSerializable* createEmptyData()
	{
		return new CShopData();
	}

	DECLARE_SINGLETON(CShopConfig)
};