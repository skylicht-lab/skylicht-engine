#include "pch.h"
#include "CPlayGamesSignIn.h"

#ifdef ANDROID
extern "C"
{
	void playGamesSignIn_signIn();
	void playGamesSignIn_signOut();
	bool playGamesSignIn_isSignedIn();

	void playGamesSignIn_onSignInSuccess(const char* id, const char* name, const char* code)
	{
		CPlayGamesSignIn::getInstance()->OnSignInSuccess(std::string(id), std::string(name), std::string(code));
	}

	void playGamesSignIn_onSignInFailed(const char* message)
	{
		CPlayGamesSignIn::getInstance()->OnSignInFailed(std::string(message));
	}
};
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CPlayGamesSignIn);

	CPlayGamesSignIn::CPlayGamesSignIn()
	{

	}

	CPlayGamesSignIn::~CPlayGamesSignIn()
	{

	}

	void CPlayGamesSignIn::signIn()
	{
#ifdef ANDROID
		playGamesSignIn_signIn();
#else
		OnSignInFailed(std::string());
#endif
	}

	void CPlayGamesSignIn::signOut()
	{
#ifdef ANDROID
		playGamesSignIn_signOut();
#endif
	}

	bool CPlayGamesSignIn::isSignedIn()
	{
#ifdef ANDROID
		return playGamesSignIn_isSignedIn();
#else
		return false;
#endif
	}
}
