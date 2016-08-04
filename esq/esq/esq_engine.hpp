#ifndef esq_engine_hpp
#define esq_engine_hpp

#include "esq.h"
#include "esq_setter.hpp"
#include "esq_dispatcher.hpp"
#include "esq_accepter.hpp"
#include "esq_transceiver.hpp"
#include "esq_recycler.hpp"
#include "esq_destroyer.hpp"
#include "esq_processer_pools.hpp"
#include "esq_thread.hpp"

_ESQ_BEGIN

class engine : public _ESQ IEngine {
public:
	engine(IHandler& handler)
		: handler_(handler), setter_(handler_), dispatcher_(setter_), processer_pools_(setter_, dispatcher_, handler_){
	}

	~engine() {
		wait();
		shutdown();
	}

public:
	virtual void startup() {
		thread_accepter_.start(new _ESQ accepter(setter_, dispatcher_, handler_), this);
		thread_transceiver_.start(new _ESQ transceiver(setter_, dispatcher_, handler_), this);
		thread_recycler_.start(new _ESQ recycler(setter_, dispatcher_, handler_), this);
		thread_destroyer_.start(new _ESQ destroyer(setter_, dispatcher_, handler_), this);
		processer_pools_.start(this);
	}

	virtual void wait() {
		thread_accepter_.wait();
		thread_transceiver_.wait();
		thread_recycler_.wait();
		thread_destroyer_.wait();
		processer_pools_.wait();
	}

	virtual void shutdown() {
		thread_accepter_.close();
		thread_transceiver_.close();
		thread_recycler_.close();
		thread_destroyer_.close();
		processer_pools_.close();
	}

private:
	IHandler& handler_;
	_ESQ setter setter_;
	_ESQ dispatcher dispatcher_;
	_ESQ thread thread_accepter_;
	_ESQ thread thread_transceiver_;
	_ESQ thread thread_recycler_;
	_ESQ thread thread_destroyer_;
	_ESQ processer_pools processer_pools_;
};

_ESQ_END

#endif