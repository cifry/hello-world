#include "../esq/esq_export.h"

class recv_cache_demo : public esq::IRecvCache {
public:
	recv_cache_demo(){}
	virtual ~recv_cache_demo(){}

public:
	virtual bool onRecv(esq::IO_STATUS status, esq::_esq_byte* buf, esq::_esq_uint length) {
		return false;
	}
};

class send_cache_demo : public esq::ISendCache {
public:
	send_cache_demo() {}
	virtual ~send_cache_demo() {}

public:
	virtual const esq::_esq_byte* cursor() const {
		return nullptr;
	}
	
	virtual esq::_esq_uint remain() const {
		return 0;
	}

	virtual bool onSend(esq::IO_STATUS status, const esq::_esq_byte* cursor, esq::_esq_uint remain) {
		return false;
	}
};

class context_demo : public esq::context {
public:
	context_demo() {}
	virtual ~context_demo() {}

public:
	recv_cache_demo recv_cache_;
	send_cache_demo send_cache_;
};

class handler_demo : public esq::IHandler {
	virtual void onConfig(_ESQ ISetter& setter) {
		setter.listen("", 4080);
	}

	virtual void onAccept(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) {
		session->allocate(new context_demo());
		dispatcher.putRecv(session);
	}

	virtual bool onRecv(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) {
		context_demo* context = static_cast<context_demo *>(session->context());
		return session->socket().recv(context->recv_cache_);
	}

	virtual bool onSend(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) {
		context_demo* context = static_cast<context_demo *>(session->context());
		return session->socket().send(context->send_cache_);
	}

	virtual void onRecy(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) {

	}

	virtual void onProcess(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) {

	}

	virtual void onDiscard(_ESQ ISession* session, _ESQ IDispatcher& dispatcher) {

	}

	virtual void onStatus(const _ESQ _esq_string& status) {

	}
};

int main()
{
	handler_demo handler;
	_ESQ IEngine* engine = _ESQ exprot::engine("esq.dll", &handler);
	engine->startup();
	engine->wait();
	engine->shutdown();
	delete engine;
}