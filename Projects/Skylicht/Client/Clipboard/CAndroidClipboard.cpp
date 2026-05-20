#include "pch.h"
#include "CAndroidClipboard.h"

#ifdef ANDROID
extern "C"
{
	void clipboard_copy(const char* text);
	const char* clipboard_paste();
}
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CAndroidClipboard);

	CAndroidClipboard::CAndroidClipboard()
	{
	}

	CAndroidClipboard::~CAndroidClipboard()
	{
	}

	void CAndroidClipboard::copyToClipboard(const char* text)
	{
#ifdef ANDROID
		clipboard_copy(text);
#endif
	}

	const char* CAndroidClipboard::getTextFromClipboard()
	{
#ifdef ANDROID
		m_clipboardText = clipboard_paste();
		return m_clipboardText.c_str();
#else
		return "";
#endif
	}
}
