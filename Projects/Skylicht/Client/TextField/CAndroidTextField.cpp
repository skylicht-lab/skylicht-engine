#include "pch.h"
#include "CAndroidTextField.h"

#ifdef ANDROID
extern "C"
{
	void textfield_show(const char* text, int maxLength, int height, bool password);

	void textfield_on_change(const char* text)
	{
		CAndroidTextField::getInstance()->OnChanged(std::string(text));
	}

	void textfield_on_done(const char* text)
	{
		CAndroidTextField::getInstance()->OnDone(std::string(text));
	}
}
#endif

namespace Skylicht
{
IMPLEMENT_SINGLETON(CAndroidTextField);

CAndroidTextField::CAndroidTextField()
{
	
}

CAndroidTextField::~CAndroidTextField()
{
	
}

void CAndroidTextField::show(const char* text, int maxLength, int height, bool password)
{
#ifdef ANDROID
	textfield_show(text, maxLength, height, password);
#else
	OnDone(std::string(text));
#endif
}
}
