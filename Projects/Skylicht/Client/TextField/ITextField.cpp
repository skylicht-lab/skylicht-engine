#include "pch.h"
#include "ITextField.h"

#include "CIOSTextField.h"

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
#else
		return NULL;
#endif
	}
}
