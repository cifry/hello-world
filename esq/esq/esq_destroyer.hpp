#ifndef esq_destroyer_hpp
#define esq_destroyer_hpp

#include "esq.h"

_ESQ_BEGIN

class destroyer {
public:
	destroyer(const _ESQ ISetter& setter, _ESQ IDispatcher& dispatchor, _ESQ IHandler& handler)
		: setter_(setter), dispatchor_(dispatchor), handler_(handler) {}
	~destroyer() {}

public:
	void run(_ESQ thread::status* stat, _ESQ IEngine* engine) {
		try {
			while (stat->enabled()) {
				_ESQ session_set session_set;
				if (!dispatchor_.takeDiscard(session_set, 0U)) {
					stat->close();
				}
				for (auto iter = session_set.data().begin(); iter != session_set.data().end(); ++iter) {
					handler_.onDiscard((*iter).release(), dispatchor_);
				}
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