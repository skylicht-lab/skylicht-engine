
#pragma once

#include "Utils/CSingleton.h"
#include "ITextField.h"

namespace Skylicht
{
	class CAndroidTextField : public ITextField
	{
	public:
		CAndroidTextField();

		virtual ~CAndroidTextField();

		DECLARE_SINGLETON(CAndroidTextField)

		virtual void show(const char* text, int maxLength, int height);
	};
}
