#pragma once

#include "IPostNotification.h"
#include "Utils/CSingleton.h"

namespace Skylicht
{
	class CAndroidPostNotification : public IPostNotification
	{
	public:
		DECLARE_SINGLETON(CAndroidPostNotification)

	public:
		CAndroidPostNotification();

		virtual ~CAndroidPostNotification();

		virtual void checkPermissionAsync(std::function<void(EPermission)> callback);

		virtual void requestPermission();

		virtual void scheduleNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow);

		virtual void removeNotification(const char* id);

		virtual void clear();

		void onPermissionUpdate(EPermission permission);
	};
}
