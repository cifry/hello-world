#ifndef esq_service_hpp
#define esq_service_hpp

#include "esq.h"

_ESQ_BEGIN
_ESQ_SERVICE_BEGIN

inline constexpr _esq_socket invalid()
{
	return INVALID_SOCKET;
}

inline bool invalid(_esq_socket s)
{
	return invalid() == s;
}

inline void startup() {
	WSADATA data;
	if (::WSAStartup(MAKEWORD(2, 2), &data))
	{
		throw _ESQ exception("::WSAStartup failed!", GetLastError());
	}
}

inline void shutdown() {
	::WSACleanup();
}

namespace tcpip {
	inline _esq_socket allocate() {
		_esq_socket s = ::socket(AF_INET, SOCK_STREAM, 0);
		if (s == invalid()) {
			throw _ESQ exception("allocate a socket failed!", WSAGetLastError());
		}
		return s;
	}

	inline void release(_esq_socket socket) {
		int rev = ::closesocket(socket);
		if (rev != 0) {
			throw _ESQ exception("release a socket failed!", WSAGetLastError());
		}
	}

	inline void reuse(_esq_socket socket) {
		BOOL p = TRUE;
		int rev = ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&p, sizeof(p));
		if (rev != 0) {
			throw _ESQ exception("reuse a socket failed!", WSAGetLastError());
		}
	}

	inline void bind(_esq_socket socket, _esq_ulong ip, _esq_uint port) {
		_esq_sockaddr_in address;
		::memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = ::htonl(ip);
		address.sin_port = ::htons(port);
		int rev = ::bind(socket, (_esq_sockaddr*)&address, sizeof(address));
		if (rev != 0) {
			throw _ESQ exception("bind a socket failed!", WSAGetLastError());
		}
	}

	inline void bind(_esq_socket socket, const _esq_string& ip, _esq_uint port) {
		_esq_ulong _ip = INADDR_ANY;
		if (!ip.empty()) {
			_ip = ::inet_addr(ip.c_str());
		}
		bind(socket, _ip, port);
	}

	inline void listen(_esq_socket socket) {
		int rev = ::listen(socket, SOMAXCONN);
		if (rev != 0) {
			throw _ESQ exception("listen a socket failed!", WSAGetLastError());
		}
	}

	inline _esq_socket accept(_esq_socket socket, _esq_sockaddr_in* address) {
		_esq_socket s = invalid();
		if (address) {
			_esq_int length = sizeof(_esq_sockaddr_in);
			s = ::accept(socket, (_esq_sockaddr*)address, &length);
		} else {
			s = ::accept(socket, nullptr, nullptr);
		}
		if (invalid(s)) {
			throw _ESQ exception("accept a socket failed!", WSAGetLastError());
		}
		return s;
	}

	inline void address(const _esq_sockaddr_in* address, _esq_string& ip) {
		ip = ::inet_ntoa(address->sin_addr);
	}

	inline IO_STATUS recv(_esq_socket socket, _esq_byte* buf, _esq_uint& length) {
		int rev = ::recv(socket, buf, length, 0);
		if (rev == 0) {
			return IO_CLOSE;
		}
		else if (rev > 0) {
			if (rev == length) {
				return IO_CONTINUE;
			}
			else {
				length = rev;
				return IO_DONE;
			}
		}
		else if (rev == SOCKET_ERROR) {
			int errorcode = ::WSAGetLastError();
			if (errorcode == WSAEWOULDBLOCK) {
				return IO_SLEEP;
			}
			else {
				//throw _ESQ exception("a socket recv failed!", errorcode);
			}
		}
		return IO_CLOSE;
	}

	inline IO_STATUS send(_esq_socket socket, const _esq_byte* buf, _esq_uint& length) {
		int rev = ::send(socket, buf, length, 0);
		if (rev == 0) {
			return IO_CLOSE;
		}
		else if (rev > 0) {
			if (rev == length) {
				return IO_CONTINUE;
			}
			else {
				length = rev;
				return IO_DONE;
			}
		}
		else if (rev == SOCKET_ERROR) {
			int errorcode = ::WSAGetLastError();
			if (errorcode == WSAEWOULDBLOCK) {
				return IO_SLEEP;
			}
			else {
				//throw _ESQ exception("a socket send failed!", errorcode);
			}
		}
		return IO_CLOSE;
	}

	inline int select(_esq_fd_set* readfds, _esq_fd_set* writefds, const _esq_timeval* timeout) {
		return ::select(0, readfds, writefds, nullptr, timeout);
	}
}

_ESQ_SERVICE_END
_ESQ_END

#endif