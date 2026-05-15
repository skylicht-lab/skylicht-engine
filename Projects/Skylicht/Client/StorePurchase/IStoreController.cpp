#include "pch.h"
#include "IStoreController.h"
#include "IStoreListener.h"

#include "CPlayStoreController.h"
#include "CAppStoreController.h"
#include "CTestStoreController.h"

namespace Skylicht
{
	IStoreController::IStoreController()
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

	void IStoreController::notifyInitialized(const std::vector<SIAPProduct>& products)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onInitialized(this, products);
	}

	void IStoreController::notifyInitializeFailed(int error)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onInitializeFailed(error);
	}

	void IStoreController::notifyPurchaseSucceeded(const char* productId, const char* receipt)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onPurchaseSucceeded(productId, receipt);
	}

	void IStoreController::notifyPurchaseFailed(const char* productId, int error)
	{
		std::vector<IStoreListener*> listeners = m_listener;
		for (IStoreListener* l : listeners)
			l->onPurchaseFailed(productId, error);
	}

	IStoreController* getStoreController()
	{
#if defined(ANDROID)
		return CPlayStoreController::getInstance();
#elif defined(IOS)
		return CAppStoreController::getInstance();
#else
		return CTestStoreController::getInstance();
#endif
	}
}