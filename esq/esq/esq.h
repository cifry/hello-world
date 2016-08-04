#ifndef esq_h
#define esq_h

#include "esq_type.h"

_ESQ_BEGIN

struct IRecvCache {
	virtual ~IRecvCache() = 0 {};
	virtual bool onRecv(IO_STATUS status, _esq_byte* buf, _esq_uint length) = 0;
};

struct ISendCache {
	virtual ~ISendCache() = 0 {};
	virtual const _esq_byte* cursor() const = 0;
	virtual _esq_uint remain() const = 0;
	virtual bool onSend(IO_STATUS status, const _esq_byte* cursor, _esq_uint remain) = 0;
};

struct ISocket {
	virtual ~ISocket() = 0 {};
	virtual bool valid() const = 0;
	virtual void join(_ESQ_SERVICE _esq_socket socket) = 0;
	virtual _ESQ_SERVICE _esq_socket handle() const = 0;
	virtual _ESQ_SERVICE _esq_socket detach() = 0;
	virtual void address(_esq_string& ip) const = 0;
	virtual bool recv(IRecvCache& cache) const = 0;
	virtual bool send(ISendCache& cache) const = 0;
};

struct ISession {
	virtual ~ISession() = 0 {};
	virtual _ESQ ISocket& socket() = 0;
	virtual const _ESQ ISocket& socket() const = 0;
	virtual void allocate(_ESQ context* context) = 0;
	virtual _ESQ context* context() const = 0;
	virtual ISession* generate() const = 0;
};

struct ISessionSet {
	virtual ~ISessionSet() = 0 {};
	virtual void put(_ESQ ISession* session) = 0;
	virtual _esq_uint size() const = 0;
	virtual bool empty() const = 0;
};

struct IDispatcher {
	virtual ~IDispatcher() = 0 {};
	virtual void putRecv(_ESQ ISession* session) = 0;
	virtual void putSend(_ESQ ISession* session) = 0;
	virtual void putProcess(_ESQ ISession* session) = 0;
	virtual void putRecy(_ESQ ISession* session) = 0;
	virtual void putDiscard(_ESQ ISession* session) = 0;
	virtual bool takeRecvSend(_ESQ ISessionSet& recv_set, _esq_uint recv_maximum, _ESQ ISessionSet& send_set, _esq_uint send_maximum) = 0;
	virtual bool takeProcess(_ESQ ISessionSet& set, _esq_uint maximum) = 0;
	virtual bool takeRecy(_ESQ ISessionSet& set, _esq_uint maximum) = 0;
	virtual bool takeDiscard(_ESQ ISessionSet& set, _esq_uint maximum) = 0;
};

struct ISetter {
	virtual ~ISetter() = 0 {};
	virtual void listen(const _esq_string& ip, _esq_uint port) = 0;
	virtual const _esq_string& listenIp() const = 0;
	virtual _esq_uint listenPort() const = 0;
	virtual bool cachesize(_esq_uint size) = 0;
	virtual _esq_uint cachesize() const = 0;
	virtual bool number_of_processer(_esq_uint number) = 0;
	virtual _esq_uint number_of_processer() const = 0;
	virtual void timeout(_esq_uint sec) = 0;
	virtual _esq_uint timeout() const = 0;
};

struct IHandler {
	virtual ~IHandler() = 0 {};
	virtual void onConfig(_ESQ ISetter& setter) = 0;
	virtual void onAccept(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) = 0;
	virtual bool onRecv(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) = 0;
	virtual bool onSend(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) = 0;
	virtual void onRecy(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) = 0;
	virtual void onProcess(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) = 0;
	virtual void onDiscard(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) = 0;
	virtual void onStatus(const _ESQ _esq_string& status) = 0;
};

struct IEngine {
	virtual ~IEngine() = 0 {};
	virtual void startup() = 0;
	virtual void shutdown() = 0;
	virtual void wait() = 0;
};

_ESQ_END

#endif