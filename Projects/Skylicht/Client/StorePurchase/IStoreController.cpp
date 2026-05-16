#include "pch.h"
#include "IStoreController.h"
#include "IStoreListener.h"

#include "CPlayStoreController.h"
#include "CAppStoreController.h"
#include "CTestStoreController.h"

namespace Skylicht
{
	IStoreController::IStoreController() :
		m_isInitialized(false)
	{

	}

	IStoreController::~IStoreController()
	{

	}

	void IStoreController::addListener(IStoreListener* listener)
	{
		for (IStoreListener* l : m_listener)
		{
			if (l == listener)
				return;
		}
		m_listener.push_back(listener);
	}

	void IStoreController::removeListener(IStoreListener* listener)
	{
		for (auto it = m_listener.begin(); it != m_listener.end(); ++it)
		{
			if (*it == listener)
			{
				m_listener.erase(it);
				return;
			}
		}
	}

	void IStoreController::notifyInitialized()
	{
		m_isInitialized = true;
	}

	void IStoreController::notifyProductReceived(const std::vector<SIAPProduct>& products)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onProductReceived(this, products);
	}

	void IStoreController::notifyFetchProductFailed(int error, const char* message)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onFetchProductFailed(this, error, message);
	}

	void IStoreController::notifyInitializeFailed(int error, const char* message)
	{
		m_isInitialized = false;

		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onInitializeFailed(error, message);
	}

	void IStoreController::notifyRestorePurchaseFailed(int error, const char* message)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onRestorePurchaseFailed(error, message);
	}

	void IStoreController::notifyPurchaseSucceeded(const char* productId, const char* receipt)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onPurchaseSucceeded(productId, receipt);
	}

	void IStoreController::notifyPurchaseFailed(const char* productId, int error, const char* message)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onPurchaseFailed(productId, error, message);
	}

	IStoreController* getStoreController(bool isTesting)
	{
		if (isTesting)
			return CTestStoreController::getInstance();

#if defined(ANDROID)
		return CPlayStoreController::getInstance();
#elif defined(IOS)
		return CAppStoreController::getInstance();
#else
		return CTestStoreController::getInstance();
#endif
	}
}