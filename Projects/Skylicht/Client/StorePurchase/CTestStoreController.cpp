#include "pch.h"
#include "CTestStoreController.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CTestStoreController);

	CTestStoreController::CTestStoreController()
	{
		notifyInitialized();
	}

	CTestStoreController::~CTestStoreController()
	{
	}

	void CTestStoreController::restorePurchase()
	{

	}

	void CTestStoreController::restart()
	{
		notifyInitialized();
	}

	void CTestStoreController::initiatePurchase(const char* productId)
	{
		// Always return success for test
		notifyPurchaseSucceeded(productId, "mock_receipt_data");
	}

	void CTestStoreController::fetchAdditionalProducts(const std::vector<std::string>& productIds)
	{
		std::vector<SIAPProduct> products;
		for (const std::string& id : productIds)
		{
			SIAPProduct p;
			p.ProductId = id;
			p.LocalizedTitle = id;
			p.LocalizedDescription = id;
			p.LocalizedPrice = "$0.00";
			p.PriceValue = 0.0f;
			p.CurrencyCode = "USD";
			products.push_back(p);
		}
		notifyProductReceived(products);
	}
}
