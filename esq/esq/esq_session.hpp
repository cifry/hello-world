#ifndef esq_session_hpp
#define esq_session_hpp

#include "esq.h"
#include "esq_socket.hpp"
#include <ctime>

_ESQ_BEGIN

class session : public ISession {
public:
	session() : sec_(0L), context_(nullptr){}
	virtual ~session() {
		if (context_) {
			delete context_;
			context_ = nullptr;
		}
	}

public:
	virtual const _ESQ ISocket& socket() const {
		return socket_;
	}

	virtual _ESQ ISocket& socket() {
		return socket_;
	}

	virtual void allocate(_ESQ context* context) {
		if(context_) {
			delete context_;
		}
		context_ = context;
	}

	virtual _ESQ context* context() const {
		return context_;
	}

	virtual ISession* generate() const {
		return new _ESQ session();
	}

public:
	_ESQ socket* getSocket() {
		return &socket_;
	}

	const _ESQ socket* getSocket() const {
		return &socket_;
	}

	void timing() {
		sec_ = getCurrentTime();
	}

	bool expired(_ESQ _esq_uint sec) const {
		return ((sec_ + sec) < getCurrentTime());
	}

private:
	static _esq_ulong getCurrentTime() {
		std::time_t t;
		std::time(&t);
		return (_esq_ulong)t;
	}

private:
	_ESQ socket socket_;
	_esq_ulong sec_;
	_ESQ context* context_;
};

_ESQ_END

#endif