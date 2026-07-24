#include "pch.h"
#include "IPostNotification.h"

#if defined(IOS)
#include "CIOSPostNotification.h"
#elif defined(ANDROID)
#include "CAndroidPostNotification.h"
#endif

namespace Skylicht
{
	IPostNotification::IPostNotification()
	{

	}

	IPostNotification::~IPostNotification()
	{

	};

	IPostNotification* getOSPostNotification()
	{
#if defined(IOS)
		return CIOSPostNotification::createGetInstance();
#elif defined(ANDROID)
		return CAndroidPostNotification::createGetInstance();
#else
		return NULL;
#endif
	}

}
