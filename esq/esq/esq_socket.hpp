#ifndef esq_socket_hpp
#define esq_socket_hpp

#include "esq.h"
#include "esq_service.hpp"
#include <memory>

_ESQ_BEGIN

class socket : public ISocket {
public:
	socket() : cache_size_(1024U), socket_(_ESQ_SERVICE invalid()) {
		std::memset(&address_, 0U, sizeof(address_));
	}

	socket(_ESQ_SERVICE _esq_socket s) : cache_size_(1024U), socket_(s) {
		std::memset(&address_, 0U, sizeof(address_));
	}

	virtual ~socket() {
		release();
	}

public:
	void allocate() {
		socket_ = _ESQ_SERVICE_TCPIP allocate();
	}

	virtual void release() {
		if (!_ESQ_SERVICE invalid(socket_)) {
			_ESQ_SERVICE_TCPIP release(socket_);
			socket_ = _ESQ_SERVICE invalid();
		}
	}

	void cachesize(_esq_uint size) {
		cache_size_ = size;
	}

	void reuse() const {
		_ESQ_SERVICE_TCPIP reuse(socket_);
	}

	void bind(const _esq_string& ip, _esq_uint port) const {
		_ESQ_SERVICE_TCPIP bind(socket_, ip, port);
	}

	void listen() const {
		_ESQ_SERVICE_TCPIP listen(socket_);
	}

	void accpet(socket* s) const {
		try {
			s->socket_ = _ESQ_SERVICE_TCPIP accept(socket_, &(s->address_));
		}
		catch (_ESQ exception e) {
			throw e;
		}
	}

public:
	virtual bool valid() const {
		return !_ESQ_SERVICE invalid(socket_);
	}

	virtual void join(_ESQ_SERVICE _esq_socket s) {
		release();
		socket_ = s;
	}
	
	virtual _ESQ_SERVICE _esq_socket handle() const {
		return socket_;
	}

	virtual _ESQ_SERVICE _esq_socket detach() {
		_ESQ_SERVICE _esq_socket s = socket_;
		socket_ = _ESQ_SERVICE invalid();
		return s;
	}

	virtual void address(_esq_string& ip) const {
		_ESQ_SERVICE_TCPIP address(&address_, ip);
	}

	virtual bool recv(IRecvCache& cache) const {
		if (cache_size_ > 0U) {
			std::unique_ptr<_esq_byte> buf(new _esq_byte[cache_size_]);
			while (true) {
				_esq_uint length = cache_size_;
				IO_STATUS status = _ESQ_SERVICE_TCPIP recv(socket_, buf.get(), length);
				switch(status) {
				case IO_SLEEP:
					return true;
				case IO_CLOSE:
					return false;
				default:
					if (!cache.onRecv(status, buf.get(), length)) return true;
				}
			}
			return true;
		} else {
			throw std::exception("cache_size is zero!");
			return false;
		}
	}

	virtual bool send(ISendCache& cache) const {
		if (cache_size_ > 0U) {
			while (true) {
				const _esq_byte* cursor = cache.cursor();
				if (!cursor) {
					throw std::exception("cursor is null!");
				}
				_esq_uint remain = cache.remain();
				if (remain == 0U) {
					break;
				}
				_esq_uint length = cache_size_;
				if (length > remain) {
					length = remain;
				}
				IO_STATUS status = _ESQ_SERVICE_TCPIP send(socket_, cursor, length);
				switch (status) {
				case IO_SLEEP:
					return true;
				case IO_CLOSE:
					return false;
				default:
					if (!cache.onSend(status, cursor + length, remain - length)) return true;
				}
			}
			return true;
		}
		else {
			throw std::exception("cache_size is zero!");
			return false;
		}
	}

private:
	_ESQ _esq_uint cache_size_;
	_ESQ_SERVICE _esq_socket socket_;
	_ESQ_SERVICE _esq_sockaddr_in address_;
};

_ESQ_END

#endif