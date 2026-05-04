#pragma once

#include <vector>
#include <string>

class IStoreController
{
public:
	virtual ~IStoreController() {}

	virtual void restorePurchase() = 0;

	virtual void initiatePurchase(const char* productId) = 0;

	virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds) = 0;
};
