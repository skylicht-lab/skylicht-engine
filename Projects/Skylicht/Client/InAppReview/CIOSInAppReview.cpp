#include "pch.h"
#include "CIOSInAppReview.h"

#ifdef IOS
void ios_showInAppReview(bool isTesting);
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CIOSInAppReview);

	CIOSInAppReview::CIOSInAppReview()
	{

	}

	CIOSInAppReview::~CIOSInAppReview()
	{

	}

	void CIOSInAppReview::showInAppReview(bool isTesting)
	{
#ifdef IOS
		ios_showInAppReview(isTesting);
#endif
	}
}
