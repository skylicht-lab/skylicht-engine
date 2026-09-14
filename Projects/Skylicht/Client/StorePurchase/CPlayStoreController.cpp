#include "pch.h"
#include "CPlayStoreController.h"

#ifdef ANDROID
extern "C"
{
	void playStore_restorePurchase();
	void playStore_initiatePurchase(const char* productId);
	void playStore_setProductTypes(const char** productIds, const int* types, int count);
	void playStore_fetchAdditionalProducts(const char** productIds, int count);
	void playStore_restart();

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
			p.Type = Skylicht::CPlayStoreController::getInstance()->getProductType(productIds[i]);
			products.push_back(p);
		}
		Skylicht::CPlayStoreController::getInstance()->notifyProductReceived(products);
	}

	void playStore_onInitialized()
	{
		Skylicht::CPlayStoreController::createGetInstance()->notifyInitialized();
	}
	void playStore_onInitializeFailed(int error, const char* message)
	{
		Skylicht::CPlayStoreController::createGetInstance()->notifyInitializeFailed(error, message);
	}
	void playStore_onFetchProductFailed(int error, const char* message)
	{
		Skylicht::CPlayStoreController::getInstance()->notifyFetchProductFailed(error, message);
	}
	void playStore_onRestorePurchaseFailed(int error, const char* message)
	{
		Skylicht::CPlayStoreController::getInstance()->notifyRestorePurchaseFailed(error, message);
	}
	void playStore_onRestorePurchaseCompleted()
	{
		Skylicht::CPlayStoreController::getInstance()->notifyRestorePurchaseCompleted();
	}
	void playStore_onPurchaseSucceeded(const char* productId, const char* receipt)
	{
		Skylicht::CPlayStoreController::getInstance()->notifyPurchaseSucceeded(productId, receipt);
	}
	void playStore_onPurchaseRestored(const char* productId, const char* receipt)
	{
		Skylicht::CPlayStoreController::getInstance()->notifyPurchaseRestored(productId, receipt);
	}

	void playStore_onPurchaseFailed(const char* productId, int error, const char* message)
	{
		Skylicht::CPlayStoreController::getInstance()->notifyPurchaseFailed(productId, error, message);
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

	void CPlayStoreController::init()
	{

	}

	void CPlayStoreController::restorePurchase()
	{
#ifdef ANDROID
		playStore_restorePurchase();
#endif
	}

	void CPlayStoreController::restart()
	{
#ifdef ANDROID
		playStore_restart();
#endif
	}

	void CPlayStoreController::initiatePurchase(const char* productId)
	{
#ifdef ANDROID
		playStore_initiatePurchase(productId);
#endif
	}

	void CPlayStoreController::setProductType(const char* productId, EIAPProductType type)
	{
		SIAPProductConfig product(productId ? productId : "", type);
		std::vector<SIAPProductConfig> products;
		products.push_back(product);
		setProductTypes(products);
	}

	void CPlayStoreController::setProductTypes(const std::vector<SIAPProductConfig>& products)
	{
		IStoreController::setProductTypes(products);

#ifdef ANDROID
		std::vector<const char*> ids;
		std::vector<int> types;
		for (size_t i = 0, n = products.size(); i < n; i++)
		{
			ids.push_back(products[i].ProductId.c_str());
			types.push_back((int)products[i].Type);
		}
		playStore_setProductTypes(ids.data(), types.data(), (int)ids.size());
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
