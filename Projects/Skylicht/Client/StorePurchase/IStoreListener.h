#pragma once

#include "IStoreController.h"
#include "SIAPProduct.h"
#include <vector>

class IStoreListener
{
public:
	virtual ~IStoreListener() {}

	virtual void onInitialized(IStoreController* controller) = 0;

	virtual void onProductReceived(IStoreController* controller, const std::vector<SIAPProduct>& products) = 0;

	virtual void onInitializeFailed(int error, const char* message) = 0;

	virtual void onRestorePurchaseFailed(int error, const char* message) = 0;

	virtual void onPurchaseSucceeded(const char* productId, const char* receipt) = 0;

	virtual void onPurchaseFailed(const char* productId, int error, const char* message) = 0;
};
