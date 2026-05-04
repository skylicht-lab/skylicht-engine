#pragma once

#include "IStoreController.h"
#include "SIAPProduct.h"
#include <vector>

class IStoreListener
{
public:
	virtual ~IStoreListener() {}

	virtual void onInitialized(IStoreController* controller, const std::vector<SIAPProduct>& products) = 0;

	virtual void onInitializeFailed(int error) = 0;

	virtual void onPurchaseSucceeded(const char* productId, const char* receipt) = 0;

	virtual void onPurchaseFailed(const char* productId, int error) = 0;
};
