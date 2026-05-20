#include "pch.h"
#include "CDesktopClipboard.h"
#include "CBaseApp.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CDesktopClipboard);

	CDesktopClipboard::CDesktopClipboard()
	{
	}

	CDesktopClipboard::~CDesktopClipboard()
	{
	}

	void CDesktopClipboard::copyToClipboard(const char* text)
	{
		IrrlichtDevice* device = getApplication()->getDevice();
		if (device != NULL)
		{
			device->getOSOperator()->copyToClipboard(text);
		}
	}

	const char* CDesktopClipboard::getTextFromClipboard()
	{
		IrrlichtDevice* device = getApplication()->getDevice();
		if (device != NULL)
		{
			return device->getOSOperator()->getTextFromClipboard();
		}
		return NULL;
	}
}
