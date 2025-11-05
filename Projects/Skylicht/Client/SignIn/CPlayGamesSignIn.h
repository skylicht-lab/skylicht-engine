#pragma once

#include "Utils/CSingleton.h"
#include "ISignIn.h"

namespace Skylicht
{
	class CPlayGamesSignIn : public ISignIn
	{
	public:
		CPlayGamesSignIn();

		virtual ~CPlayGamesSignIn();

		DECLARE_SINGLETON(CPlayGamesSignIn)

		virtual void signIn();
	};
}