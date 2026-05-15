#pragma once

#include <vector>
#include <string>
#include "SIAPProduct.h"

class IStoreListener;

namespace Skylicht
{
	class IStoreController
	{
	protected:
		std::vector<IStoreListener*> m_listener;

	public:
		IStoreController();
		virtual ~IStoreController();

		void addListener(IStoreListener* listener);

		void removeListener(IStoreListener* listener);

		virtual void restorePurchase() = 0;

		virtual void restart() = 0;

		virtual void initiatePurchase(const char* productId) = 0;

		virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds) = 0;

	public:
		void notifyInitialized();

		void notifyProductReceived(const std::vector<SIAPProduct>& products);

		void notifyInitializeFailed(int error, const char* message);

		void notifyRestorePurchaseFailed(int error, const char* message);

		void notifyPurchaseSucceeded(const char* productId, const char* receipt);

		void notifyPurchaseFailed(const char* productId, int error, const char* message);
	};

	IStoreController* getStoreController(bool isTesting = false);
}
