#pragma once

#include <string>

enum EIAPProductType
{
	IAP_CONSUMABLE = 0,
	IAP_NON_CONSUMABLE = 1
};

struct SIAPProductConfig
{
	std::string ProductId;
	EIAPProductType Type;

	SIAPProductConfig() :
		Type(IAP_CONSUMABLE)
	{
	}

	SIAPProductConfig(const std::string& productId, EIAPProductType type) :
		ProductId(productId),
		Type(type)
	{
	}
};

struct SIAPProduct
{
	std::string ProductId;
	std::string LocalizedTitle;
	std::string LocalizedDescription;
	std::string LocalizedPrice;
	double PriceValue;
	std::string CurrencyCode;
	EIAPProductType Type;

	SIAPProduct() :
		PriceValue(0.0),
		Type(IAP_CONSUMABLE)
	{
	}
};
