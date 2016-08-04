#ifndef esq_setter_hpp
#define esq_setter_hpp

#include "esq.h"

_ESQ_BEGIN

class setter : public _ESQ ISetter {
public:
	setter(_ESQ IHandler& handler) : port_(0), cache_size_(1024U), number_of_processer_(5U), timeout_(30U){
		handler.onConfig(*this);
	}
	~setter() {}

public:
	virtual void listen(const _esq_string& ip, _esq_uint port) {
		ip_ = ip;
		port_ = port;
	}

	virtual const _esq_string& listenIp() const {
		return ip_;
	}

	virtual _esq_uint listenPort() const {
		return port_;
	}

	virtual bool cachesize(_esq_uint size) {
		if(size > 0U) {
			cache_size_ = size;
			return true;
		}
		return false;
	}

	virtual _esq_uint cachesize() const {
		return cache_size_;
	}

	virtual bool number_of_processer(_esq_uint number) {
		if (number > 0U) {
			number_of_processer_ = number;
			return true;
		}
		return false;
	}

	virtual _esq_uint number_of_processer() const {
		return number_of_processer_;
	}

	virtual void timeout(_esq_uint sec) {
		timeout_ = sec;
	}

	virtual _esq_uint timeout() const {
		return timeout_;
	}

private:
	_esq_string ip_;
	_esq_uint port_;
	_esq_uint cache_size_;
	_esq_uint number_of_processer_;
	_esq_uint timeout_;
};

_ESQ_END

#endif