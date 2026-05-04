#include "pch.h"
#include "CTestStoreController.h"

CTestStoreController::CTestStoreController(IStoreListener* listener, const std::vector<std::string>& productIds) :
	m_listener(listener)
{
	std::vector<SIAPProduct> products;
	m_listener->onInitialized(this, products);
}

CTestStoreController::~CTestStoreController()
{
}

void CTestStoreController::restorePurchase()
{

}

void CTestStoreController::initiatePurchase(const char* productId)
{
	// Always return success for test
	m_listener->onPurchaseSucceeded(productId, "mock_receipt_data");
}

void CTestStoreController::fetchAdditionalProducts(const std::vector<std::string>& productIds)
{
	// Not implemented for test
}
