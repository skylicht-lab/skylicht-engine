#pragma once

#include "Utils/CSingleton.h"
#include "IStoreController.h"

namespace Skylicht
{
	class CPlayStoreController : public IStoreController
	{
	public:
		CPlayStoreController();

		virtual ~CPlayStoreController();

		DECLARE_SINGLETON(CPlayStoreController)

		using IStoreController::fetchAdditionalProducts;

		virtual void init();

		virtual void restorePurchase();

		virtual void restart();

		virtual void initiatePurchase(const char* productId);

		virtual void setProductType(const char* productId, EIAPProductType type);

		virtual void setProductTypes(const std::vector<SIAPProductConfig>& products);

		virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds);
	};
}
