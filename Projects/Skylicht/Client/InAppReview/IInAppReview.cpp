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
		return CIOSInAppReview::getInstance();
#elif defined(ANDROID)
		return CAndroidInAppReview::getInstance();
#else
		return NULL;
#endif
	}
}
