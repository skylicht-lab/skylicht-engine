#include "pch.h"
#include "IClipboard.h"

#include "CDesktopClipboard.h"
#include "CAndroidClipboard.h"

namespace Skylicht
{
	IClipboard::IClipboard()
	{
	}

	IClipboard::~IClipboard()
	{
	}

	IClipboard* getOSClipboard()
	{
#if defined(ANDROID)
		return CAndroidClipboard::getInstance();
#else
		return CDesktopClipboard::getInstance();
#endif
	}
}
