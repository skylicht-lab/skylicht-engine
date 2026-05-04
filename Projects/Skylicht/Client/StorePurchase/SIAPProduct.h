#pragma once

#include <string>

struct SIAPProduct
{
	std::string ProductId;
	std::string LocalizedTitle;
	std::string LocalizedDescription;
	std::string LocalizedPrice;
	double PriceValue;
	std::string CurrencyCode;

	SIAPProduct() :
		PriceValue(0.0)
	{
	}
};
