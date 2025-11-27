#include "pch.h"
#include "CIOSTextField.h"

#ifdef IOS
void textfield_show(const char* text, int maxLength, int height);

void textfield_on_change(const char* text)
{
	CIOSTextField::getInstance()->OnChanged(std::string(text));
}

void textfield_on_done(const char* text)
{
	CIOSTextField::getInstance()->OnDone(std::string(text));
}
#endif

namespace Skylicht
{
IMPLEMENT_SINGLETON(CIOSTextField);

CIOSTextField::CIOSTextField()
{
	
}

CIOSTextField::~CIOSTextField()
{
	
}

void CIOSTextField::show(const char* text, int maxLength, int height)
{
#ifdef IOS
	textfield_show(text, maxLength, height);
#else
	OnDone(std::string(text));
#endif
}
}

