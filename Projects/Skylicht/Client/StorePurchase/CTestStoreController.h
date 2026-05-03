#pragma once

#include "IStoreController.h"
#include "IStoreListener.h"

class CTestStoreController : public IStoreController
{
protected:
	IStoreListener* m_listener;

public:
	CTestStoreController(IStoreListener* listener, const std::vector<std::string>& productIds);

	virtual ~CTestStoreController();

	virtual void restorePurchase();

	virtual void initiatePurchase(const char* productId);

	virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds);
};
