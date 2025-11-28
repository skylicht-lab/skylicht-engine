#include "pch.h"
#include "ITextField.h"

#include "CIOSTextField.h"
#include "CAndroidTextField.h"

namespace Skylicht
{
	ITextField::ITextField()
	{
	}

	ITextField::~ITextField()
	{
	}

	ITextField *getOSTextField()
	{
#if defined(IOS)
		return CIOSTextField::getInstance();
#elif defined(ANDROID)
		return CAndroidTextField::getInstance();
#else
		return NULL;
#endif
	}
}
