#pragma once

#include <functional>

namespace Skylicht
{
	class IInAppReview
	{
	public:
		IInAppReview();

		virtual ~IInAppReview();

		virtual void showInAppReview(bool isTesting = false) = 0;
	};

	IInAppReview* getInAppReview();
}
