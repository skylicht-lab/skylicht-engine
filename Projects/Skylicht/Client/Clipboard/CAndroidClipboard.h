#pragma once

#include "IClipboard.h"
#include "Utils/CSingleton.h"

namespace Skylicht
{
	class CAndroidClipboard : public IClipboard
	{
	public:
		DECLARE_SINGLETON(CAndroidClipboard)

		CAndroidClipboard();

		virtual ~CAndroidClipboard();

		virtual void copyToClipboard(const char* text);

		virtual const char* getTextFromClipboard();

	private:
		std::string m_clipboardText;
	};
}
