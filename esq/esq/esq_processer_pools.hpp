#ifndef esq_processer_pools_hpp
#define esq_processer_pools_hpp

#include "esq.h"
#include "esq_thread.hpp"
#include "esq_processer.hpp"

_ESQ_BEGIN

class processer_pools {
public:
	typedef std::unique_ptr<_ESQ thread> _Ptr;
	typedef std::vector<_Ptr> _Pools;

public:
	processer_pools(const _ESQ ISetter& setter, _ESQ IDispatcher& dispatchor, _ESQ IHandler& handler)
		: setter_(setter), dispatchor_(dispatchor), handler_(handler) {}
	~processer_pools() {}

public:
	void start(_ESQ IEngine* engine)
	{
		_ESQ _esq_uint number = setter_.number_of_processer();
		while (number--) {
			_Ptr ptr(new _ESQ thread());
			ptr->start(new _ESQ processer(setter_, dispatchor_, handler_), engine);
			pools_.push_back(std::move(ptr));
		}
	}

	void close() {
		for (auto iter = pools_.begin(); iter != pools_.end(); ++iter) {
			(*iter)->close();
		}
	}

	void wait() {
		for (auto iter = pools_.begin(); iter != pools_.end(); ++iter) {
			(*iter)->wait();
		}
	}

private:
	const _ESQ ISetter& setter_;
	_ESQ IDispatcher& dispatchor_;
	_ESQ IHandler& handler_;
	_Pools pools_;
};

_ESQ_END

#endif