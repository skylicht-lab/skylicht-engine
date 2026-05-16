#include "pch.h"
#include "CPurchase.h"

#include "CShopConfig.h"
#include "CShopData.h"
#include "CProfileData.h"
#include "ViewManager/CViewManager.h"
#include "CViewHeader.h"

IMPLEMENT_SINGLETON(CPurchase);

CPurchase::CPurchase() :
	m_controller(NULL)
{

}

CPurchase::~CPurchase()
{
	if (m_controller != NULL)
	{
		m_controller->removeListener(this);
		m_controller = NULL;
	}
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
		m_controller->removeListener(this);

	m_controller = getStoreController();
	if (m_controller != NULL)
	{
		m_controller->addListener(this);
		m_controller->fetchAdditionalProducts(productIds);
	}
}

void CPurchase::purchase(const char* id)
{
	if (m_controller != NULL)
	{
		m_controller->initiatePurchase(id);
	}
	else
	{
		onPurchaseFailed(id, -1, "No store controller");
	}
}

const SIAPProduct* CPurchase::getProduct(const char* id)
{
	auto it = m_products.find(id);
	if (it != m_products.end())
		return &it->second;
	return NULL;
}

void CPurchase::onProductReceived(IStoreController* controller, const std::vector<SIAPProduct>& products)
{
	for (const SIAPProduct& p : products)
		m_products[p.ProductId] = p;
}

void CPurchase::onFetchProductFailed(IStoreController* controller, int error, const char* message)
{
	// Handle fetch product failure
}

void CPurchase::onInitializeFailed(int error, const char* message)
{
	// Handle initialization failure
}

void CPurchase::onRestorePurchaseFailed(int error, const char* message)
{
	// Handle restore purchase failure
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

void CPurchase::onPurchaseFailed(const char* productId, int error, const char* message)
{
	if (OnPurchaseFailed != nullptr)
		OnPurchaseFailed(std::string(productId), error);
}
