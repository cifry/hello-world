#ifndef esq_accepter_hpp
#define esq_accepter_hpp

#include "esq.h"
#include "esq_thread.hpp"
#include "esq_session.hpp"

_ESQ_BEGIN

class accepter {
public:
	accepter(const _ESQ ISetter& setter, _ESQ IDispatcher& dispatcher, _ESQ IHandler& handler)
		: setter_(setter), dispatchor_(dispatcher), handler_(handler) {}
	~accepter() {}

public:
	void run(_ESQ thread::status* stat, _ESQ IEngine* engine) {
		try{
			_ESQ socket acceptor;
			acceptor.allocate();
			acceptor.reuse();
			acceptor.bind(setter_.listenIp(), setter_.listenPort());
			acceptor.listen();
			while (stat->enabled()) {
				auto session = std::make_unique<_ESQ session>();
				acceptor.accpet(session->getSocket());
				session->getSocket()->cachesize(setter_.cachesize());
				handler_.onAccept(session.release(), dispatchor_);
			}
		}
		catch (_ESQ exception e) {
			printf("%s %d", e.what(), e.errorCode());
			engine->shutdown();
		}
	}

private:
	const _ESQ ISetter& setter_;
	_ESQ IDispatcher& dispatchor_;
	_ESQ IHandler& handler_;
};

_ESQ_END

#endif