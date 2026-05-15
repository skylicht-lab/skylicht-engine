#pragma once

#include "Utils/CSingleton.h"
#include "IStoreController.h"

namespace Skylicht
{
	class CAppStoreController : public IStoreController
	{
	public:
		CAppStoreController();

		virtual ~CAppStoreController();

		DECLARE_SINGLETON(CAppStoreController)

		virtual void restorePurchase();

		virtual void restart();

		virtual void initiatePurchase(const char* productId);

		virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds);
	};
}
