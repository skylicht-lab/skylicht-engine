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
#include "CRawSocketTransport.h"
#include <climits>
#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <cerrno>
#include <poll.h>
#include <sys/socket.h>
#endif

namespace Skylicht
{
	namespace Network
	{
		CRawSocketTransport::CRawSocketTransport(ETransportMode mode) : CBaseTransport(mode) {}

		ETransportStatus CRawSocketTransport::socketFailure()
		{
			int error = CSocketUtil::getSocketError();
			if (CSocketUtil::wouldBlock(error))
				return ETransportStatus::WouldBlock;
			if (CSocketUtil::interrupted(error))
				return ETransportStatus::Interrupted;
			return setError(error);
		}

		ETransportStatus CRawSocketTransport::open(const STransportEndpoint& local)
		{
			close();
			m_lastError = STransportError();
			if (!initializeRuntime())
				return ETransportStatus::Error;
			sockaddr_in address;
			if (!CSocketUtil::parseAddress(local.Address.c_str(), local.Port, address))
			{
				close();
				return setError(CSocketUtil::invalidArgument(), true);
			}
			bool stream = m_mode == ETransportMode::Stream;
			m_socket = ::socket(AF_INET, stream ? SOCK_STREAM : SOCK_DGRAM, stream ? IPPROTO_TCP : IPPROTO_UDP);
			if (m_socket == CSocketUtil::InvalidSocket || !CSocketUtil::setNonBlocking(m_socket) ||
				::bind(m_socket, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0)
			{
				int error = CSocketUtil::getSocketError();
				close();
				return setError(error, true);
			}
			m_state = stream ? ETransportState::Bound : ETransportState::Ready;
			return ETransportStatus::Done;
		}

		ETransportStatus CRawSocketTransport::connect(const STransportEndpoint& remote)
		{
			if (m_mode != ETransportMode::Stream)
				return ETransportStatus::Unsupported;
			sockaddr_in address;
			if (m_state != ETransportState::Bound || !remote.Port ||
				!CSocketUtil::parseAddress(remote.Address.c_str(), remote.Port, address))
				return setError(CSocketUtil::invalidArgument());
			if (::connect(m_socket, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == 0)
			{
				m_state = ETransportState::Ready;
				return ETransportStatus::Done;
			}
			int error = CSocketUtil::getSocketError();
#if defined(_WIN32)
			bool pending = error == WSAEINPROGRESS;
#else
			bool pending = error == EINPROGRESS;
#endif
			if (pending || CSocketUtil::wouldBlock(error) || CSocketUtil::interrupted(error))
			{
				m_state = ETransportState::Connecting;
				return ETransportStatus::Pending;
			}
			return setError(error, true);
		}

		ETransportStatus CRawSocketTransport::listen(int backlog)
		{
			if (m_mode != ETransportMode::Stream)
				return ETransportStatus::Unsupported;
			if (m_state != ETransportState::Bound || backlog <= 0)
				return setError(CSocketUtil::invalidArgument());
			if (::listen(m_socket, backlog) != 0)
				return setError(CSocketUtil::getSocketError(), true);
			m_state = ETransportState::Listening;
			return ETransportStatus::Done;
		}

		ETransportStatus CRawSocketTransport::accept(ITransport*& accepted)
		{
			// Never overwrite a live owning pointer.
			if (accepted)
				return setError(CSocketUtil::invalidArgument());
			if (m_mode != ETransportMode::Stream)
				return ETransportStatus::Unsupported;
			if (m_state != ETransportState::Listening)
				return setError(CSocketUtil::invalidArgument());
			Socket peer = ::accept(m_socket, nullptr, nullptr);
			if (peer == CSocketUtil::InvalidSocket)
				return socketFailure();
			if (!CSocketUtil::setNonBlocking(peer))
			{
				int error = CSocketUtil::getSocketError();
				CSocketUtil::closeHandle(peer);
				return setError(error);
			}
			CRawSocketTransport* transport = new CRawSocketTransport(m_mode);
			if (!transport->initializeRuntime())
			{
				int error = transport->getLastError().Code;
				CSocketUtil::closeHandle(peer);
				delete transport;
				return setError(error);
			}
			transport->m_socket = peer;
			transport->m_state = ETransportState::Ready;
			accepted = transport;
			return ETransportStatus::Done;
		}

		ETransportStatus CRawSocketTransport::update(std::size_t budget)
		{
			if (m_state == ETransportState::Closed)
				return ETransportStatus::Closed;
			if (m_state == ETransportState::Failed)
				return ETransportStatus::Error;
			if (m_state != ETransportState::Connecting)
				return ETransportStatus::Done;
			if (!budget)
				return ETransportStatus::Pending;
#if defined(_WIN32)
			fd_set writable, errors;
			FD_ZERO(&writable);
			FD_ZERO(&errors);
			FD_SET(m_socket, &writable);
			FD_SET(m_socket, &errors);
			timeval wait = {};
			int ready = ::select(0, nullptr, &writable, &errors, &wait);
#else
			pollfd descriptor = {};
			descriptor.fd = m_socket;
			descriptor.events = POLLOUT;
			int ready = ::poll(&descriptor, 1, 0);
#endif
			if (ready < 0)
			{
				int error = CSocketUtil::getSocketError();
				return CSocketUtil::interrupted(error) ? ETransportStatus::Pending : setError(error, true);
			}
			if (!ready)
				return ETransportStatus::Pending;
			int error = 0;
			CSocketUtil::AddressLength length = sizeof(error);
			if (::getsockopt(m_socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &length) != 0)
				error = CSocketUtil::getSocketError();
			if (error)
				return setError(error, true);
			m_state = ETransportState::Ready;
			return ETransportStatus::Done;
		}

		STransportResult CRawSocketTransport::send(const void* data, std::size_t size, bool /*binary*/)
		{
			if (m_mode != ETransportMode::Stream)
				return ETransportStatus::Unsupported;
			if (m_state != ETransportState::Ready)
				return m_state == ETransportState::Connecting ? ETransportStatus::Pending : ETransportStatus::Closed;
			return sendBytes(data, size);
		}

		STransportResult CRawSocketTransport::sendBytes(const void* data, std::size_t size)
		{
			if ((!data && size) || size > INT_MAX)
				return setError(CSocketUtil::invalidArgument());
			if (!size)
				return ETransportStatus::Done;
			int flags = 0;
#if defined(MSG_NOSIGNAL)
			flags = MSG_NOSIGNAL;
#endif
			int sent = static_cast<int>(::send(m_socket, reinterpret_cast<const char*>(data), static_cast<int>(size), flags));
			if (sent < 0)
				return socketFailure();
			if (!sent)
				return setError(CSocketUtil::invalidArgument(), true);
			return { ETransportStatus::Done, static_cast<std::size_t>(sent) };
		}

		STransportResult CRawSocketTransport::receive(void* buffer, std::size_t capacity)
		{
			if (m_mode != ETransportMode::Stream)
				return ETransportStatus::Unsupported;
			if (m_state != ETransportState::Ready)
				return m_state == ETransportState::Connecting ? ETransportStatus::Pending : ETransportStatus::Closed;
			return receiveBytes(buffer, capacity);
		}

		STransportResult CRawSocketTransport::receiveBytes(void* buffer, std::size_t capacity)
		{
			if (!buffer || !capacity || capacity > INT_MAX)
				return setError(CSocketUtil::invalidArgument());
			// open() and accept() configure the socket as non-blocking, so recv() does
			// not wait for data. If none is available, it immediately returns an error
			// (WSAEWOULDBLOCK on Windows, EAGAIN/EWOULDBLOCK on POSIX), which
			// socketFailure() maps to WouldBlock so the caller can retry next update.
			int received = static_cast<int>(::recv(m_socket, reinterpret_cast<char*>(buffer), static_cast<int>(capacity), 0));
			if (received < 0)
				return socketFailure();
			if (!received)
			{
				close();
				return ETransportStatus::Closed;
			}
			return { ETransportStatus::Done, static_cast<std::size_t>(received) };
		}

		STransportResult CRawSocketTransport::sendTo(const STransportEndpoint& destination, const void* data, std::size_t size)
		{
			if (m_mode != ETransportMode::Datagram)
				return ETransportStatus::Unsupported;
			if (m_state != ETransportState::Ready)
				return ETransportStatus::Closed;
			sockaddr_in address;
			if ((!data && size) || size > 65507 || !destination.Port ||
				!CSocketUtil::parseAddress(destination.Address.c_str(), destination.Port, address))
				return setError(CSocketUtil::invalidArgument());
			int sent = static_cast<int>(::sendto(m_socket, size ? reinterpret_cast<const char*>(data) : "", static_cast<int>(size), 0,
				reinterpret_cast<const sockaddr*>(&address), sizeof(address)));
			if (sent < 0)
				return socketFailure();
			if (static_cast<std::size_t>(sent) != size)
				return setError(CSocketUtil::invalidArgument());
			return { ETransportStatus::Done, size };
		}

		STransportResult CRawSocketTransport::receiveFrom(STransportEndpoint& source, void* buffer, std::size_t capacity)
		{
			// UDP only: receive one datagram and its source endpoint, preserving packet boundaries.
			if (m_mode != ETransportMode::Datagram)
				return ETransportStatus::Unsupported;
			if (m_state != ETransportState::Ready)
				return ETransportStatus::Closed;
			if (!buffer || !capacity || capacity > INT_MAX)
				return setError(CSocketUtil::invalidArgument());
			// open() configures the UDP socket as non-blocking, so recvfrom()/recvmsg()
			// never wait for a datagram. If none is available, WSAEWOULDBLOCK on Windows
			// or EAGAIN/EWOULDBLOCK on POSIX is mapped by socketFailure() to WouldBlock;
			// the caller can retry next update. A zero-byte result is a valid empty UDP
			// datagram, not a closed connection.
			sockaddr_in address = {};
#if defined(_WIN32)
			CSocketUtil::AddressLength length = sizeof(address);
			int received = ::recvfrom(m_socket, reinterpret_cast<char*>(buffer), static_cast<int>(capacity), 0,
				reinterpret_cast<sockaddr*>(&address), &length);
			if (received < 0 && CSocketUtil::getSocketError() == WSAEMSGSIZE)
			{
				// The receive buffer is too small for this UDP datagram (WSAEMSGSIZE).
				// The datagram was consumed and excess bytes were discarded by the OS.
				// Truncated rejects the incomplete payload; it cannot be resumed next update.
				return ETransportStatus::Truncated;
			}
#else
			iovec vector = {};
			vector.iov_base = buffer;
			vector.iov_len = capacity;
			msghdr message = {};
			message.msg_name = &address;
			message.msg_namelen = sizeof(address);
			message.msg_iov = &vector;
			message.msg_iovlen = 1;
			int received = static_cast<int>(::recvmsg(m_socket, &message, 0));
			if (received >= 0 && (message.msg_flags & MSG_TRUNC))
			{
				// The receive buffer is too small for this UDP datagram (MSG_TRUNC).
				// The datagram was consumed and excess bytes were discarded by the OS.
				// Truncated rejects the incomplete payload; it cannot be resumed next update.
				return ETransportStatus::Truncated;
			}
#endif
			if (received < 0)
				return socketFailure();
			if (!CSocketUtil::copyEndpoint(address, source))
				return setError(CSocketUtil::invalidArgument());
			return { ETransportStatus::Done, static_cast<std::size_t>(received) };
		}

		bool CRawSocketTransport::getLocalEndpoint(STransportEndpoint& result) const
		{
			sockaddr_in address = {};
			CSocketUtil::AddressLength length = sizeof(address);
			return m_socket != CSocketUtil::InvalidSocket &&
				::getsockname(m_socket, reinterpret_cast<sockaddr*>(&address), &length) == 0 && CSocketUtil::copyEndpoint(address, result);
		}

		bool CRawSocketTransport::getRemoteEndpoint(STransportEndpoint& result) const
		{
			sockaddr_in address = {};
			CSocketUtil::AddressLength length = sizeof(address);
			return m_socket != CSocketUtil::InvalidSocket &&
				::getpeername(m_socket, reinterpret_cast<sockaddr*>(&address), &length) == 0 && CSocketUtil::copyEndpoint(address, result);
		}
	}
}
