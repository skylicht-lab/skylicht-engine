#pragma once

#include "Utils/CSingleton.h"
#include "IInAppReview.h"

namespace Skylicht
{
	class CIOSInAppReview : public IInAppReview
	{
	public:
		CIOSInAppReview();

		virtual ~CIOSInAppReview();

		DECLARE_SINGLETON(CIOSInAppReview)

		virtual void showInAppReview(bool isTesting = false);
	};
}
