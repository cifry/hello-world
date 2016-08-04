#ifndef esq_type_h
#define esq_type_h

#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#define _ESQ_BEGIN namespace esq {
#define _ESQ_END		}
#define _ESQ ::esq::
#define _ESQ_SERVICE_BEGIN namespace service {
#define _ESQ_SERVICE_END		}
#define _ESQ_SERVICE ::esq::service::
#define _ESQ_SERVICE_TCPIP ::esq::service::tcpip::

_ESQ_BEGIN

typedef char _esq_byte;
typedef int _esq_int;
typedef unsigned int _esq_uint;
typedef long _esq_long;
typedef unsigned long _esq_ulong;
typedef std::string _esq_string;

enum IO_STATUS {
	IO_CONTINUE,
	IO_DONE,
	IO_SLEEP,
	IO_CLOSE,
};

class context {
public:
	context() {}
	virtual ~context() = 0 {};
};

class exception : public std::exception {
public:
	exception(const _esq_string& message, _esq_uint error) throw()
		: std::exception(message.c_str()), error_code_(error) {
	}

	_esq_uint errorCode() const {
		return error_code_;
	}

private:
	_esq_uint error_code_;
};

_ESQ_SERVICE_BEGIN

typedef ::SOCKET _esq_socket;
typedef struct ::sockaddr _esq_sockaddr;
typedef struct ::sockaddr_in _esq_sockaddr_in;
typedef struct ::fd_set _esq_fd_set;
typedef struct ::timeval _esq_timeval;

_ESQ_SERVICE_END	

_ESQ_END

#endif