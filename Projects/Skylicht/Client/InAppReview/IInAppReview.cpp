#include "pch.h"
#include "IInAppReview.h"

#include "CAndroidInAppReview.h"
#include "CIOSInAppReview.h"

namespace Skylicht
{
	IInAppReview::IInAppReview()
	{
	}

	IInAppReview::~IInAppReview()
	{
	}

	IInAppReview* getInAppReview()
	{
#if defined(IOS)
		return CIOSInAppReview::createGetInstance();
#elif defined(ANDROID)
		return CAndroidInAppReview::createGetInstance();
#else
		return NULL;
#endif
	}
}
