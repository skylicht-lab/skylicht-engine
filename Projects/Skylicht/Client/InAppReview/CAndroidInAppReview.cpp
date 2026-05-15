#include "pch.h"
#include "CAndroidInAppReview.h"

#ifdef ANDROID
extern "C"
{
	void androidInAppReview_show(int isTesting);
};
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CAndroidInAppReview);

	CAndroidInAppReview::CAndroidInAppReview()
	{

	}

	CAndroidInAppReview::~CAndroidInAppReview()
	{

	}

	void CAndroidInAppReview::showInAppReview(bool isTesting)
	{
#ifdef ANDROID
		androidInAppReview_show(isTesting);
#endif
	}
}
