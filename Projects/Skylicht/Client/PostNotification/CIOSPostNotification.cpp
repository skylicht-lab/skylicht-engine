#include "pch.h"
#include "CIOSPostNotification.h"

#if defined(IOS)
#include "ViewManager/CViewManager.h"

extern "C" {
	void ios_checkPostNotificationPermissionAsync();
	void ios_requestPostNotificationPermission();
	void ios_schedulePostNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow);
	void ios_removePostNotification(const char* id);
	void ios_clearPostNotification();
}

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CIOSPostNotification);

	CIOSPostNotification::CIOSPostNotification()
	{

	}

	CIOSPostNotification::~CIOSPostNotification()
	{

	}

	void CIOSPostNotification::checkPermissionAsync(std::function<void(EPermission)> callback)
	{
		m_checkPermissionCallback = callback;
		ios_checkPostNotificationPermissionAsync();
	}

	void CIOSPostNotification::requestPermission()
	{
		ios_requestPostNotificationPermission();
	}

	void CIOSPostNotification::scheduleNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow)
	{
		ios_schedulePostNotification(id, title, content, delayFromNow);
	}

	void CIOSPostNotification::removeNotification(const char* id)
	{
		ios_removePostNotification(id);
	}

	void CIOSPostNotification::clear()
	{
		ios_clearPostNotification();
	}

	void CIOSPostNotification::onPermissionChecked(EPermission permission)
	{
		CViewManager::getInstance()->runInUI([this, permission]() {
			if (m_checkPermissionCallback)
			{
				std::function<void(EPermission)> callback = m_checkPermissionCallback;
				m_checkPermissionCallback = nullptr;
				callback(permission);
			}
			});
	}

	void CIOSPostNotification::onPermissionUpdate(EPermission permission)
	{
		CViewManager::getInstance()->runInUI([this, permission]() {
			if (OnPermissionUpdate)
				OnPermissionUpdate(permission);
			});
	}
}

extern "C" void ios_onPostNotificationPermissionCheck(int permission)
{
	Skylicht::CIOSPostNotification::getInstance()->onPermissionChecked((Skylicht::IPostNotification::EPermission)permission);
}

extern "C" void ios_onPostNotificationPermissionUpdate(int permission)
{
	Skylicht::CIOSPostNotification::getInstance()->onPermissionUpdate((Skylicht::IPostNotification::EPermission)permission);
}
#endif
