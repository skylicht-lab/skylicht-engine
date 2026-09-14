#pragma once

#include <vector>
#include <string>
#include <map>
#include "SIAPProduct.h"

class IStoreListener;

namespace Skylicht
{
	class IStoreController
	{
	protected:
		std::vector<IStoreListener*> m_listener;

		bool m_isInitialized;

		std::map<std::string, EIAPProductType> m_productTypes;

	public:
		IStoreController();

		virtual ~IStoreController();

		void addListener(IStoreListener* listener);

		void removeListener(IStoreListener* listener);

		virtual void init() = 0;

		virtual void restorePurchase() = 0;

		virtual void restart() = 0;

		virtual void initiatePurchase(const char* productId) = 0;

		virtual void setProductType(const char* productId, EIAPProductType type);

		virtual void setProductTypes(const std::vector<SIAPProductConfig>& products);

		EIAPProductType getProductType(const char* productId) const;

		virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds) = 0;

		virtual void fetchAdditionalProducts(const std::vector<SIAPProductConfig>& products);

		inline bool isInitialized() { return m_isInitialized; }

	public:
		void notifyInitialized();

		void notifyProductReceived(const std::vector<SIAPProduct>& products);

		void notifyFetchProductFailed(int error, const char* message);

		void notifyInitializeFailed(int error, const char* message);

		void notifyRestorePurchaseFailed(int error, const char* message);

		void notifyRestorePurchaseCompleted();

		void notifyPurchaseSucceeded(const char* productId, const char* receipt);

		void notifyPurchaseRestored(const char* productId, const char* receipt);

		void notifyPurchaseFailed(const char* productId, int error, const char* message);
	};

	IStoreController* getStoreController(bool isTesting = false);
}
