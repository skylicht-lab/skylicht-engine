#include "pch.h"
#include "CGameCenterSignIn.h"

#ifdef IOS
void gamecenter_signIn();
bool gamecenter_isSignedIn();

void gamecenter_signInSuccess(const char* playerId, const char* playerName, const char *code)
{
	CGameCenterSignIn::getInstance()->OnSignInSuccess(std::string(playerId), std::string(playerName), std::string(code));
}

void gamecenter_signInFailed(const char* message)
{
	CGameCenterSignIn::getInstance()->OnSignInFailed(std::string(message));
}
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CGameCenterSignIn);

	CGameCenterSignIn::CGameCenterSignIn()
	{

	}

	CGameCenterSignIn::~CGameCenterSignIn()
	{

	}

	void CGameCenterSignIn::signIn()
	{
#ifdef IOS
		gamecenter_signIn();
#else
		OnSignInFailed(std::string());
#endif
	}

	void CGameCenterSignIn::signOut()
	{
	}

	bool CGameCenterSignIn::isSignedIn()
	{
#ifdef IOS
		return gamecenter_isSignedIn();
#else
		return false;
#endif
	}
}

