#ifndef esq_dispatcher_hpp
#define esq_dispatcher_hpp

#include "esq.h"
#include "esq_session_set.hpp"
#include <queue>
#include <mutex>

_ESQ_BEGIN

class pipe {
public:
	pipe() : capacity_(0U) {}
	virtual ~pipe() {}

public:
	typedef std::unique_ptr<_ESQ session> _Ptr;
	typedef typename std::queue<_Ptr> _Queue;
	typedef typename std::vector<_Ptr> _Set;
	typedef typename std::unique_lock<std::mutex> _UniqueLock;

public:
	void put(_ESQ session* session) {
		_UniqueLock lock(mutex_);
		put_condition_.wait(lock, [&]() { return generate(session); });
		lock.unlock();
		take_condition_.notify_all();
	}

	void take(_ESQ session_set& set, _esq_uint maximum) {
		_UniqueLock lock(mutex_);
		take_condition_.wait(lock, [&]() { return consume(set, maximum); });
		lock.unlock();
		put_condition_.notify_all();
	}

private:
	bool generate(_ESQ session* session) {
		if ((capacity_ == 0U) || (queue_.size() < capacity_)) {
			queue_.push(std::move(_Ptr(session)));
			return true;
		}
		return false;
	}

	bool consume(_ESQ session_set& set, _esq_uint maximum) {
		if (queue_.empty()) return false;
		while (!queue_.empty()) {
			if ((maximum > 0U) && (set.size() >= maximum)) break;
			set.putSession(queue_.front().release());
			queue_.pop();
		}
		return true;
	}

private:
	std::mutex mutex_;
	std::condition_variable put_condition_;
	std::condition_variable take_condition_;
	_ESQ _esq_uint capacity_;
	_Queue queue_;
};

class recvsend_pipe {
public:
	recvsend_pipe() : capacity_(0U) {}
	virtual ~recvsend_pipe() {}

public:
	typedef std::unique_ptr<_ESQ session> _Ptr;
	typedef std::queue<_Ptr> _Queue;
	typedef std::vector<_Ptr> _Set;
	typedef std::unique_lock<std::mutex> _UniqueLock;

public:
	void putRecv(_ESQ session* session) {
		_UniqueLock lock(mutex_);
		put_recv_condition_.wait(lock, [&]() { return generateRecv(session); });
		lock.unlock();
		take_condition_.notify_one();
	}

	void putSend(_ESQ session* session) {
		_UniqueLock lock(mutex_);
		put_send_condition_.wait(lock, [&]() { return generateSend(session); });
		lock.unlock();
		take_condition_.notify_one();
	}

	void takeRecvSend(_ESQ session_set& recv_set, _esq_uint recv_maximum, _ESQ session_set& send_set, _esq_uint send_maximum) {
		_esq_uint recv_set_size = recv_set.size();
		_esq_uint send_set_size = send_set.size();
		_UniqueLock lock(mutex_);
		take_condition_.wait(lock, [&]() { return consume(recv_set, recv_maximum, send_set, send_maximum); });
		lock.unlock();
		if (recv_set.size() > recv_set_size) {
			put_recv_condition_.notify_one();
		}
		if (send_set.size() > send_set_size) {
			put_send_condition_.notify_one();
		}
	}

private:
	bool generateRecv(_ESQ session* session) {
		if ((capacity_ == 0U) || (recv_queue_.size() < capacity_)) {
			recv_queue_.push(std::move(_Ptr(session)));
			return true;
		}
		return false;
	}

	bool generateSend(_ESQ session* session) {
		if ((capacity_ == 0U) || (send_queue_.size() < capacity_)) {
			send_queue_.push(std::move(_Ptr(session)));
			return true;
		}
		return false;
	}

	bool consume(_ESQ session_set& recv_set, _esq_uint recv_maximum, _ESQ session_set& send_set, _esq_uint send_maximum) {
		if (recv_queue_.empty() && send_queue_.empty() && recv_set.empty() && send_set.empty()) return false;
		while (!recv_queue_.empty()) {
			if ((recv_maximum > 0U) && (recv_set.size() >= recv_maximum)) break;
			recv_set.putSession(recv_queue_.front().release());
			recv_queue_.pop();
		}
		while (!send_queue_.empty()) {
			if ((send_maximum > 0U) && (send_set.size() >= send_maximum)) break;
			send_set.putSession(send_queue_.front().release());
			send_queue_.pop();
		}
		return true;
	}

private:
	std::mutex mutex_;
	std::condition_variable put_recv_condition_;
	std::condition_variable put_send_condition_;
	std::condition_variable take_condition_;
	_ESQ _esq_uint capacity_;
	_Queue recv_queue_;
	_Queue send_queue_;
};

class dispatcher : public _ESQ IDispatcher {
public:
	dispatcher(const _ESQ ISetter& setter) {}
	virtual ~dispatcher() {}

public:
	virtual void putRecv(_ESQ ISession* session) {
		recvsend_pipe_.putRecv(static_cast<_ESQ session*>(session));
	}

	virtual void putSend(_ESQ ISession* session) {
		recvsend_pipe_.putSend(static_cast<_ESQ session*>(session));
	}

	virtual void putProcess(_ESQ ISession* session) {
		process_pipe_.put(static_cast<_ESQ session*>(session));
	}

	virtual void putRecy(_ESQ ISession* session) {
		recy_pipe_.put(static_cast<_ESQ session*>(session));
	}

	virtual void putDiscard(_ESQ ISession* session) {
		discard_pipe_.put(static_cast<_ESQ session*>(session));
	}

	virtual bool takeRecvSend(_ESQ ISessionSet& recv_set, _esq_uint recv_maximum, _ESQ ISessionSet& send_set, _esq_uint send_maximum) {
		if ((recv_set.size() < recv_maximum) || send_set.size() < send_maximum) {
			recvsend_pipe_.takeRecvSend(static_cast<_ESQ session_set&>(recv_set), recv_maximum, static_cast<_ESQ session_set&>(send_set), send_maximum);
		}
		return true;
	}

	virtual bool takeProcess(_ESQ ISessionSet& set, _esq_uint maximum) {
		process_pipe_.take(static_cast<_ESQ session_set&>(set), maximum);
		return true;
	}

	virtual bool takeRecy(_ESQ ISessionSet& set, _esq_uint maximum) {
		recy_pipe_.take(static_cast<_ESQ session_set&>(set), maximum);
		return true;
	}

	virtual bool takeDiscard(_ESQ ISessionSet& set, _esq_uint maximum) {
		discard_pipe_.take(static_cast<_ESQ session_set&>(set), maximum);
		return true;
	}

private:
	pipe process_pipe_;
	pipe recy_pipe_;
	pipe discard_pipe_;
	recvsend_pipe recvsend_pipe_;
};

_ESQ_END

#endif