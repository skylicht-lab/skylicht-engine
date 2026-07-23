#pragma once

#include "IPostNotification.h"
#include "Utils/CSingleton.h"

namespace Skylicht
{
	class CIOSPostNotification : public IPostNotification
	{
	public:
		DECLARE_SINGLETON(CIOSPostNotification)

	public:
		CIOSPostNotification();

		virtual ~CIOSPostNotification();

		virtual void checkPermissionAsync(std::function<void(EPermission)> callback);

		virtual void requestPermission();

		virtual void scheduleNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow);

		virtual void removeNotification(const char* id);

		virtual void clear();

		void onPermissionChecked(EPermission permission);

		void onPermissionUpdate(EPermission permission);

	protected:
		std::function<void(EPermission)> m_checkPermissionCallback;
	};
}
