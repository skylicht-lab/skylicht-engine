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

		virtual void restorePurchase();

		virtual void initiatePurchase(const char* productId);

		virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds);
	};
}
