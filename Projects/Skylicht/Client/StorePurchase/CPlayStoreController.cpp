#include "pch.h"
#include "CPlayStoreController.h"

#ifdef ANDROID
extern "C"
{
	void playStore_restorePurchase();
	void playStore_initiatePurchase(const char* productId);
	void playStore_fetchAdditionalProducts(const char** productIds, int count);

	void playStore_onProductsReceived(const char** productIds,
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
			CPlayStoreController::getInstance()->notifyInitialized(products);
	}

	void playStore_onInitialized()
	{
		std::vector<SIAPProduct> products;
		CPlayStoreController::getInstance()->notifyInitialized(products);
	}
	void playStore_onInitializeFailed(int error, const char* message)
	{
		CPlayStoreController::getInstance()->notifyInitializeFailed(error);
	}
	void playStore_onPurchaseSucceeded(const char* productId, const char* receipt)
	{
		CPlayStoreController::getInstance()->notifyPurchaseSucceeded(productId, receipt);
	}

	void playStore_onPurchaseFailed(const char* productId, int error)
	{
		CPlayStoreController::getInstance()->notifyPurchaseFailed(productId, error);
	}
};
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CPlayStoreController);

	CPlayStoreController::CPlayStoreController()
	{

	}

	CPlayStoreController::~CPlayStoreController()
	{

	}

	void CPlayStoreController::restorePurchase()
	{
#ifdef ANDROID
		playStore_restorePurchase();
#endif
	}

	void CPlayStoreController::initiatePurchase(const char* productId)
	{
#ifdef ANDROID
		playStore_initiatePurchase(productId);
#endif
	}

	void CPlayStoreController::fetchAdditionalProducts(const std::vector<std::string>& productIds)
	{
#ifdef ANDROID
		std::vector<const char*> ids;
		for (size_t i = 0, n = productIds.size(); i < n; i++)
			ids.push_back(productIds[i].c_str());
		playStore_fetchAdditionalProducts(ids.data(), (int)ids.size());
#endif
	}
}
