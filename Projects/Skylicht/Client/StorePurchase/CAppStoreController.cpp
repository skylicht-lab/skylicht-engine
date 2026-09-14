#include "pch.h"
#include "CAppStoreController.h"

#ifdef IOS
#include "AppStoreController.h"

extern "C"
{
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
			p.Type = Skylicht::CAppStoreController::getInstance()->getProductType(productIds[i]);
			products.push_back(p);
		}
		Skylicht::CAppStoreController::getInstance()->notifyProductReceived(products);
	}

	void appstore_onInitialized()
	{
		Skylicht::CAppStoreController::getInstance()->notifyInitialized();
	}

	void appstore_onInitializeFailed(int error, const char* message)
	{
		Skylicht::CAppStoreController::getInstance()->notifyInitializeFailed(error, message);
	}

	void appstore_onFetchProductFailed(int error, const char* message)
	{
		Skylicht::CAppStoreController::getInstance()->notifyFetchProductFailed(error, message);
	}

	void appstore_onRestorePurchaseFailed(int error, const char* message)
	{
		Skylicht::CAppStoreController::getInstance()->notifyRestorePurchaseFailed(error, message);
	}

	void appstore_onRestorePurchaseCompleted()
	{
		Skylicht::CAppStoreController::getInstance()->notifyRestorePurchaseCompleted();
	}

	void appstore_onPurchaseSucceeded(const char* productId, const char* receipt)
	{
		Skylicht::CAppStoreController::getInstance()->notifyPurchaseSucceeded(productId, receipt);
	}

	void appstore_onPurchaseRestored(const char* productId, const char* receipt)
	{
		Skylicht::CAppStoreController::getInstance()->notifyPurchaseRestored(productId, receipt);
	}

	void appstore_onPurchaseFailed(const char* productId, int error, const char* message)
	{
		Skylicht::CAppStoreController::getInstance()->notifyPurchaseFailed(productId, error, message);
	}
};
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

	void CAppStoreController::init()
	{
#ifdef IOS
		appstore_init();
#endif
	}

	void CAppStoreController::restorePurchase()
	{
#ifdef IOS
		appstore_restorePurchase();
#endif
	}

	void CAppStoreController::restart()
	{
#ifdef IOS
		// iOS StoreKit is usually automatic, but we can re-request if needed
#endif
	}

	void CAppStoreController::initiatePurchase(const char* productId)
	{
#ifdef IOS
		appstore_initiatePurchase(productId);
#endif
	}

	void CAppStoreController::setProductType(const char* productId, EIAPProductType type)
	{
		SIAPProductConfig product(productId ? productId : "", type);
		std::vector<SIAPProductConfig> products;
		products.push_back(product);
		setProductTypes(products);
	}

	void CAppStoreController::setProductTypes(const std::vector<SIAPProductConfig>& products)
	{
		IStoreController::setProductTypes(products);

#ifdef IOS
		std::vector<const char*> ids;
		std::vector<int> types;
		for (size_t i = 0, n = products.size(); i < n; i++)
		{
			ids.push_back(products[i].ProductId.c_str());
			types.push_back((int)products[i].Type);
		}
		appstore_setProductTypes(ids.data(), types.data(), (int)ids.size());
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
