#ifndef esq_thread_hpp
#define esq_thread_hpp

#include "esq.h"

_ESQ_BEGIN

class thread {
private:
	class _stat_ {
	public:
		_stat_() : enabled_(true) {}
		~_stat_() {}
		_stat_(const _stat_&) = delete;
		_stat_& operator = (const _stat_&) = delete;

	public:
		void close() {
			std::lock_guard<std::mutex> guard(mutex_);
			enabled_ = false;
		}

		bool enabled() const {
			std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
			if (lock.owns_lock()) {
				return enabled_;
			}
			else {
				return true;
			}
		}

	private:
		bool enabled_;
		mutable std::mutex mutex_;
	};

public:
	typedef typename _stat_ status;

public:
	thread() {}
	virtual ~thread() {
		wait();
	}

public:
	template<typename TData>
	void start(TData* data, _ESQ IEngine* engine)
	{
		if (data) {
			thread_ = std::thread(thread::run<TData>, data, &status_, engine);
		}
	}

	void close() {
		status_.close();
	}

	void wait() {
		if (thread_.joinable()) {
			thread_.join();
		}
	}

private:
	template<typename TData>
	static void run(TData* data, status* stat, _ESQ IEngine* engine) {
		data->run(stat, engine);
		delete data;
	}

private:
	status status_;
	std::thread thread_;
};

_ESQ_END

#endif