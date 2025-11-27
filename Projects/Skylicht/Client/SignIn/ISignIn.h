#pragma once

#include <functional>

namespace Skylicht
{
	class ISignIn
	{
	public:
		std::function<void(std::string id, std::string name, std::string code)> OnSignInSuccess;

		std::function<void(std::string message)> OnSignInFailed;

	public:
		ISignIn();

		virtual ~ISignIn();

		virtual void signIn() = 0;
	};

	ISignIn* getOSSignIn();
}
