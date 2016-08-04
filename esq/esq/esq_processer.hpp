#ifndef esq_processer_hpp
#define esq_processer_hpp

#include "esq.h"

_ESQ_BEGIN

class processer {
public:
	processer(const _ESQ ISetter& setter, _ESQ IDispatcher& dispatchor, _ESQ IHandler& handler)
		: setter_(setter_), dispatchor_(dispatchor), handler_(handler) {}
	~processer() {}

public:
	void run(_ESQ thread::status* stat, _ESQ IEngine* engine) {
		try {
			while (stat->enabled()) {
				_ESQ session_set session_set;
				if (!dispatchor_.takeProcess(session_set, 0U)) {
					stat->close();
				}
				for (auto iter = session_set.data().begin(); iter != session_set.data().end(); ++iter) {
					handler_.onProcess((*iter).release(), dispatchor_);
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