#pragma once

#include <functional>

namespace Skylicht
{
	class IPostNotification
	{
	public:
		enum EPermission
		{
			NotDetermined = 0,
			Denied,
			Allowed
		};

		std::function<void(EPermission)> OnPermissionUpdate;

	public:
		IPostNotification();

		virtual ~IPostNotification();

		virtual void checkPermissionAsync(std::function<void(EPermission)> callback) = 0;

		virtual void requestPermission() = 0;

		virtual void scheduleNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow) = 0;

		virtual void removeNotification(const char* id) = 0;

		virtual void clear() = 0;

	};

	IPostNotification* getOSPostNotification();
}
