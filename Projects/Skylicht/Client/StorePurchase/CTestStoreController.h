#pragma once

#include "Utils/CSingleton.h"
#include "IStoreController.h"

namespace Skylicht
{
	class CTestStoreController : public IStoreController
	{
	public:
		CTestStoreController();

		virtual ~CTestStoreController();

		DECLARE_SINGLETON(CTestStoreController)

		virtual void init();

		virtual void restorePurchase();

		virtual void restart();

		virtual void initiatePurchase(const char* productId);

		virtual void fetchAdditionalProducts(const std::vector<std::string>& productIds);
	};
}
