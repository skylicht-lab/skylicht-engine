#include "pch.h"
#include "CShopConfig.h"

IMPLEMENT_SINGLETON(CShopConfig);

CShopConfig::CShopConfig()
{

}

CShopConfig::~CShopConfig()
{

}

void CShopConfig::init(const char* cfgFile)
{
	CBaseConfig::init(cfgFile);

	for (CObjectSerializable* data : m_data)
	{
		CShopData* shopData = dynamic_cast<CShopData*>(data);
		if (shopData)
		{
			char text[64];
			sprintf(text, "$%0.2f", shopData->Price.get());
			shopData->PriceLocalize = text;
		}
	}
}

CShopData* CShopConfig::getDataByProductId(const char* productId)
{
	CShopData* shopData = NULL;

	for (CObjectSerializable* obj : m_data)
	{
		CShopData* d = dynamic_cast<CShopData*>(obj);
		if (d && d->Iap.get() == productId)
		{
			shopData = d;
			break;
		}
	}

	return shopData;
}