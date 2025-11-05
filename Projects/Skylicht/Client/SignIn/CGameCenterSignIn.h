
#pragma once

#include "Utils/CSingleton.h"
#include "ISignIn.h"

namespace Skylicht
{
	class CGameCenterSignIn : public ISignIn
	{
	public:
		CGameCenterSignIn();

		virtual ~CGameCenterSignIn();

		DECLARE_SINGLETON(CGameCenterSignIn)

		virtual void signIn();
	};
}
