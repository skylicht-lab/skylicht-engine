#pragma once

#include "IClipboard.h"
#include "Utils/CSingleton.h"

namespace Skylicht
{
	class CDesktopClipboard : public IClipboard
	{
	public:
		DECLARE_SINGLETON(CDesktopClipboard)

		CDesktopClipboard();

		virtual ~CDesktopClipboard();

		virtual void copyToClipboard(const char* text);

		virtual const char* getTextFromClipboard();
	};
}
