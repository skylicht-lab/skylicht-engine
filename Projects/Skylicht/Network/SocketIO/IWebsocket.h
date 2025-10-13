#pragma once

#include "pch.h"

#include <string>
#include <functional>

namespace Skylicht
{
	namespace Network
	{
		class IWebsocket
		{
		public:
			std::function<void()> OnConnected;
			std::function<void()> OnConnectFailed;

		public:
			IWebsocket();

			virtual ~IWebsocket();

			static IWebsocket* create();

			virtual bool connect(const std::string& url, const std::string& origin) = 0;

			virtual void send(const std::string& message) = 0;

			virtual void poll(int timeout = 0) = 0;

			virtual void dispatch(std::function<void(const std::string&)> callable) = 0;

			virtual bool isClosed() = 0;
		};
	}
}