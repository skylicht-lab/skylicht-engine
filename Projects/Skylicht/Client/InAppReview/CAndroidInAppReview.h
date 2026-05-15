#pragma once

#include "Utils/CSingleton.h"
#include "IInAppReview.h"

namespace Skylicht
{
	class CAndroidInAppReview : public IInAppReview
	{
	public:
		CAndroidInAppReview();

		virtual ~CAndroidInAppReview();

		DECLARE_SINGLETON(CAndroidInAppReview)

		virtual void showInAppReview(bool isTesting);
	};
}
