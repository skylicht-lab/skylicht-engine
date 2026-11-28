/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CUDPClient.h"
#include "Utils/CMemoryStream.h"

namespace Skylicht
{
	namespace Network
	{
		CUDPClient::CUDPClient() : m_serverID(0)
		{
		}

		bool CUDPClient::open(const char* serverAddress, irr::u16 serverPort,
			irr::u16 localPort, const char* bindAddress)
		{
			if (!serverPort || !openSocket(bindAddress, localPort, 1))
				return false;
			STransportEndpoint endpoint;
			if (!CSocketUtil::parseEndpoint(serverAddress, serverPort, endpoint))
			{
				m_lastError = CSocketUtil::invalidArgument();
				close();
				return false;
			}
			setRemote(endpoint);
			CConnection* server = addConnection(endpoint);
			if (!server)
			{
				close();
				return false;
			}
			m_serverID = server->getID();
			return notifyConnected(m_serverID);
		}

		CConnection* CUDPClient::getServer() const
		{
			return getConnection(m_serverID);
		}

		bool CUDPClient::send(const void* data, std::size_t size)
		{
			return CUDPSocket::send(m_serverID, data, size);
		}

		bool CUDPClient::send(CMemoryStream& packet)
		{
			return send(packet.getData(), packet.getSize());
		}
	}
}
