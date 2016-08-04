#ifndef esq_session_set_hpp
#define esq_session_set_hpp

#include "esq.h"
#include "esq_session.hpp"
#include <vector>

_ESQ_BEGIN

class session_set : public ISessionSet {
public:
	typedef std::unique_ptr<_ESQ session> _Ptr;
	typedef std::vector<_Ptr> _Set;

public:
	session_set() {}
	virtual ~session_set() {}

public:
	virtual void put(_ESQ ISession* session) {
		putSession(static_cast<_ESQ session*>(session));
	}

	virtual _esq_uint size() const {
		return (_esq_uint)data_.size();
	}

	virtual bool empty() const {
		return data_.empty();
	}

public:
	void putSession(_ESQ session* session) {
		session->timing();
		data_.push_back(std::move(_Ptr(session)));
	}

public:
	void getFD(_ESQ_SERVICE _esq_fd_set& set) const {
		FD_ZERO(&set);
		for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
			FD_SET((*iter)->getSocket()->handle(), &set);
		}
	}

	void fetchActive(const _ESQ_SERVICE _esq_fd_set& set, _Set& data) {
		auto iter = data_.begin();
		while (iter != data_.end()) {
			const _ESQ socket& socket = *(*iter)->getSocket();
			if (FD_ISSET(socket.handle(), &set)) {
				data.push_back(std::move(*iter));
				iter = data_.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	void fetchExpired(_esq_uint sec, _Set& data) {
		auto iter = data_.begin();
		while (iter != data_.end()) {
			if ((*iter)->expired(sec)) {
				data.push_back(std::move(*iter));
				iter = data_.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	_Set& data() {
		return data_;
	}

	const _Set& data() const {
		return data_;
	}

private:
	_Set data_;
};

_ESQ_END

#endif