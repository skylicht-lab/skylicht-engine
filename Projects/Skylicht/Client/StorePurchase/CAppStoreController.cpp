#include "pch.h"
#include "CAppStoreController.h"

#ifdef IOS
void appstore_restorePurchase();
void appstore_initiatePurchase(const char* productId);
void appstore_fetchAdditionalProducts(const char** productIds, int count);

void appstore_onProductsReceived(const char** productIds,
								 const char** titles,
								 const char** descriptions,
								 const char** localizedPrices,
								 double* prices,
								 const char** currencyCodes,
								 int count)
{
	std::vector<SIAPProduct> products;
	for (int i = 0; i < count; i++)
	{
		SIAPProduct p;
		p.ProductId = productIds[i];
		p.LocalizedTitle = titles[i];
		p.LocalizedDescription = descriptions[i];
		p.LocalizedPrice = localizedPrices[i];
		p.PriceValue = prices[i];
		p.CurrencyCode = currencyCodes[i];
		products.push_back(p);
	}

		if (count > 0)
			CAppStoreController::getInstance()->notifyInitialized(products);
	}

	void appstore_onInitialized()
	{
		std::vector<SIAPProduct> products;
		CAppStoreController::getInstance()->notifyInitialized(products);
	}

	void appstore_onInitializeFailed(int error, const char* message)
	{
		CAppStoreController::getInstance()->notifyInitializeFailed(error);
	}

	void appstore_onPurchaseSucceeded(const char* productId, const char* receipt)
	{
		CAppStoreController::getInstance()->notifyPurchaseSucceeded(productId, receipt);
	}

	void appstore_onPurchaseFailed(const char* productId, int error)
	{
		CAppStoreController::getInstance()->notifyPurchaseFailed(productId, error);
	}
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CAppStoreController);

	CAppStoreController::CAppStoreController()
	{

	}

	CAppStoreController::~CAppStoreController()
	{

	}

	void CAppStoreController::restorePurchase()
	{
#ifdef IOS
		appstore_restorePurchase();
#endif
	}

	void CAppStoreController::initiatePurchase(const char* productId)
	{
#ifdef IOS
		appstore_initiatePurchase(productId);
#endif
	}

	void CAppStoreController::fetchAdditionalProducts(const std::vector<std::string>& productIds)
	{
#ifdef IOS
		std::vector<const char*> ids;
		for (size_t i = 0, n = productIds.size(); i < n; i++)
			ids.push_back(productIds[i].c_str());
		appstore_fetchAdditionalProducts(ids.data(), (int)ids.size());
#endif
	}
}
