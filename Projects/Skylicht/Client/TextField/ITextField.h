#pragma once

#include <functional>

namespace Skylicht
{
	class ITextField
	{
	public:
		std::function<void(std::string text)> OnChanged;
		
		std::function<void(std::string text)> OnDone;

	public:
		ITextField();

		virtual ~ITextField();

		virtual void show(const char* text, int maxLength, int height) = 0;
	};

	ITextField *getOSTextField();
}
