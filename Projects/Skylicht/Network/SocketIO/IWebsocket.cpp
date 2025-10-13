#include "pch.h"
#include "IWebsocket.h"

#ifdef __EMSCRIPTEN__
#include "CEmscriptenWebsocket.h"
#else
#include "CWebsocket.h"
#endif

namespace Skylicht
{
	namespace Network
	{
		IWebsocket* IWebsocket::create()
		{
#ifdef __EMSCRIPTEN__
			return new CEmscriptenWebsocket();
#else
			return new CWebsocket();
#endif
		}

		IWebsocket::IWebsocket()
		{

		}

		IWebsocket::~IWebsocket()
		{

		}
	}
}