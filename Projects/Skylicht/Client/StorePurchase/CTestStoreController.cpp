#include "pch.h"
#include "CTestStoreController.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CTestStoreController);

	CTestStoreController::CTestStoreController()
	{
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
			p.LocalizedPrice = "$0.99";
			p.PriceValue = 0.99;
			p.CurrencyCode = "USD";
			products.push_back(p);
		}
		notifyProductReceived(products);
	}
}
