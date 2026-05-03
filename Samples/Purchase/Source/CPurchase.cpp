#include "pch.h"
#include "CPurchase.h"
#include "StorePurchase/CTestStoreController.h"

#include "CShopConfig.h"
#include "CShopData.h"
#include "CProfileData.h"
#include "ViewManager/CViewManager.h"
#include "CViewHeader.h"

#if defined(SKYLICHT_ANDROID)
// #include "CAndroidStoreController.h"
#elif defined(SKYLICHT_IOS)
// #include "CIOSStoreController.h"
#endif

IMPLEMENT_SINGLETON(CPurchase);

CPurchase::CPurchase() :
	m_controller(NULL)
{

}

CPurchase::~CPurchase()
{
	if (m_controller != NULL)
		delete m_controller;
}

void CPurchase::init()
{
	std::vector<std::string> productIds;

	const std::vector<CObjectSerializable*>& datas = CShopConfig::getInstance()->getData();
	for (CObjectSerializable* data : datas)
	{
		CShopData* shopData = dynamic_cast<CShopData*>(data);
		productIds.push_back(shopData->Iap.get());
	}

	init(productIds);
}

void CPurchase::init(const std::vector<std::string>& productIds)
{
	if (m_controller != NULL)
	{
		delete m_controller;
		m_controller = NULL;
	}

	// Platform specific controller initialization will go here
#if defined(SKYLICHT_ANDROID)
	// m_controller = new CAndroidStoreController(this, productIds);
#elif defined(SKYLICHT_IOS)
	// m_controller = new CIOSStoreController(this, productIds);
#else
	m_controller = new CTestStoreController(this, productIds);
#endif
}

void CPurchase::purchase(const char* id)
{
	if (m_controller != NULL)
	{
		m_controller->initiatePurchase(id);
	}
	else
	{
		onPurchaseFailed(id, -1);
	}
}

const SIAPProduct* CPurchase::getProduct(const char* id)
{
	auto it = m_products.find(id);
	if (it != m_products.end())
		return &it->second;
	return NULL;
}

void CPurchase::onInitialized(IStoreController* controller, const std::vector<SIAPProduct>& products)
{
	m_controller = controller;

	for (const SIAPProduct& p : products)
	{
		m_products[p.ProductId] = p;
	}
}

void CPurchase::onInitializeFailed(int error)
{
	// Handle initialization failure
}

void CPurchase::onPurchaseSucceeded(const char* productId, const char* receipt)
{
	CShopConfig* config = CShopConfig::getInstance();
	CShopData* shopData = config->getDataByProductId(productId);

	if (shopData != NULL)
	{
		CProfileData* profile = CProfileData::getInstance();
		if (shopData->Item.get() == "Gold")
		{
			profile->Gold += shopData->Value.get();
		}
		else if (shopData->Item.get() == "Diamond")
		{
			profile->Diamond += shopData->Value.get();
		}

		CViewHeader* header = dynamic_cast<CViewHeader*>(CViewManager::getInstance()->getLayer(1)->getCurrentView());
		if (header != NULL)
		{
			header->updateConsumer(true);
		}
	}

	if (OnPurchaseSuccess != nullptr)
		OnPurchaseSuccess(std::string(productId), std::string(receipt));
}

void CPurchase::onPurchaseFailed(const char* productId, int error)
{
	if (OnPurchaseFailed != nullptr)
		OnPurchaseFailed(std::string(productId), error);
}