
#pragma once

#include "Utils/CSingleton.h"
#include "ITextField.h"

namespace Skylicht
{
	class CIOSTextField : public ITextField
	{
	public:
		CIOSTextField();

		virtual ~CIOSTextField();

		DECLARE_SINGLETON(CIOSTextField)

		virtual void show(const char* text, int maxLength, int height);
	};
}
