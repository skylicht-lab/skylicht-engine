#pragma once

#include "Utils/CSingleton.h"
#include "StorePurchase/IStoreListener.h"

class CPurchase : public IStoreListener
{
public:
	std::function<void(std::string, std::string)> OnPurchaseSuccess;

	std::function<void(std::string, int)> OnPurchaseFailed;

protected:
	IStoreController* m_controller;

	std::map<std::string, SIAPProduct> m_products;

public:
	CPurchase();

	virtual ~CPurchase();

	DECLARE_SINGLETON(CPurchase)

	void init();

	void init(const std::vector<std::string>& productIds);

	void purchase(const char* id);

	const SIAPProduct* getProduct(const char* id);

	// IStoreListener implementation
	virtual void onInitialized(IStoreController* controller, const std::vector<SIAPProduct>& products);

	virtual void onInitializeFailed(int error);

	virtual void onPurchaseSucceeded(const char* productId, const char* receipt);

	virtual void onPurchaseFailed(const char* productId, int error);
};