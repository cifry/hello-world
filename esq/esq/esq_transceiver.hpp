#ifndef esq_transceiver_hpp
#define esq_transceiver_hpp

#include "esq.h"
#include "esq_session_set.hpp"
#include <sstream>

_ESQ_BEGIN

class transceiver {
public:
	transceiver(const _ESQ ISetter& setter, _ESQ IDispatcher& dispatchor, _ESQ IHandler& handler)
		: expire_(0), setter_(setter), dispatchor_(dispatchor), handler_(handler) {}
	~transceiver() {}

public:
	void run(_ESQ thread::status* stat, _ESQ IEngine* engine) {
		try {
			
			_ESQ session_set recv_set;
			_ESQ session_set send_set;
			std::ostringstream os_status;
			while (stat->enabled()) {
				if (!dispatchor_.takeRecvSend(recv_set, 64U, send_set, 64U)) {
					stat->close();
				}
				os_status << "take recv/send : " << recv_set.size() << " " << send_set.size() << " \r\n";
				_ESQ_SERVICE _esq_fd_set fd_set_recv;
				recv_set.getFD(fd_set_recv);
				_ESQ_SERVICE _esq_fd_set fd_set_send;
				send_set.getFD(fd_set_send);
				_ESQ_SERVICE _esq_timeval timeval = {0, expire_ };
				if (_ESQ_SERVICE_TCPIP select(&fd_set_recv, &fd_set_send, &timeval) > 0) {
					expire(false);
					_ESQ session_set::_Set recv_active_set;
					recv_set.fetchActive(fd_set_recv, recv_active_set);
					for (auto iter = recv_active_set.begin(); iter != recv_active_set.end(); ++iter) {
						if (handler_.onRecv(iter->get(), dispatchor_)) {
							recv_set.putSession(iter->release());
						}
						else {
							iter->release();
						}
					}
					_ESQ session_set::_Set send_active_set;
					send_set.fetchActive(fd_set_send, send_active_set);
					for (auto iter = send_active_set.begin(); iter != send_active_set.end(); ++iter) {
						if (handler_.onSend(iter->get(), dispatchor_)) {
							send_set.putSession(iter->release());
						}
						else {
							iter->release();
						}
					}
					os_status << "active recv/send : " << recv_active_set.size() << " " << send_active_set.size() << " \r\n";
				} else {
					expire(true);
				}
				_ESQ session_set::_Set recv_expired_set;
				recv_set.fetchExpired(setter_.timeout(), recv_expired_set);
				for (auto iter = recv_expired_set.begin(); iter != recv_expired_set.end(); ++iter) {
					handler_.onDiscard(iter->release(), dispatchor_);
				}
				_ESQ session_set::_Set send_expired_set;
				send_set.fetchExpired(setter_.timeout(), send_expired_set);
				for (auto iter = send_expired_set.begin(); iter != send_expired_set.end(); ++iter) {
					handler_.onDiscard(iter->release(), dispatchor_);
				}
				os_status << "expired recv/send : " << recv_expired_set.size() << " " << send_expired_set.size() << " \r\n";
				handler_.onStatus(os_status.str());
				os_status.str("");
			}
		}
		catch (_ESQ exception e) {
			printf("%s %d", e.what(), e.errorCode());
			engine->shutdown();
		}
	}

private:
	inline void expire(bool increase) {
		if (increase) {
			expire_ += 50;
			if(expire_ > 1000) {
				expire_ = 1000;
			}
		} else {
			expire_ = 0;
			//expire_ -= 50;
			//if (expire_ < 0) {
			//	expire_ = 0;
			//}
		}
	}

private:
	_ESQ _esq_int expire_;
	const _ESQ ISetter& setter_;
	_ESQ IDispatcher& dispatchor_;
	_ESQ IHandler& handler_;
};

_ESQ_END

#endif