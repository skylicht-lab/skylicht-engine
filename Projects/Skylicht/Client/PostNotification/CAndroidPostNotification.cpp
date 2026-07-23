#include "pch.h"
#include "CAndroidPostNotification.h"

#if defined(ANDROID)
#include "ViewManager/CViewManager.h"

extern "C" {
	int android_checkPostNotificationPermission();
	void android_requestPostNotificationPermission();
	void android_schedulePostNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow);
	void android_removePostNotification(const char* id);
	void android_clearPostNotification();
}

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CAndroidPostNotification);

	CAndroidPostNotification::CAndroidPostNotification()
	{

	}

	CAndroidPostNotification::~CAndroidPostNotification()
	{

	}

	void CAndroidPostNotification::checkPermissionAsync(std::function<void(EPermission)> callback)
	{
		CViewManager::getInstance()->runInUI([callback]() {
			EPermission permission = (EPermission)android_checkPostNotificationPermission();
			if (callback)
				callback(permission);
			});
	}

	void CAndroidPostNotification::requestPermission()
	{
		android_requestPostNotificationPermission();
	}

	void CAndroidPostNotification::scheduleNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow)
	{
		android_schedulePostNotification(id, title, content, delayFromNow);
	}

	void CAndroidPostNotification::removeNotification(const char* id)
	{
		android_removePostNotification(id);
	}

	void CAndroidPostNotification::clear()
	{
		android_clearPostNotification();
	}

	void CAndroidPostNotification::onPermissionUpdate(EPermission permission)
	{
		CViewManager::getInstance()->runInUI([this, permission]() {
			if (OnPermissionUpdate)
				OnPermissionUpdate(permission);
			});
	}
}

extern "C" void android_onPostNotificationPermissionUpdate(int permission)
{
	Skylicht::CAndroidPostNotification::getInstance()->onPermissionUpdate((Skylicht::IPostNotification::EPermission)permission);
}
#endif
