//******************************************************************************
/// \copyright (c) 2017,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Future.inl
/// All Rights Reserved
//******************************************************************************
/// Future implementation details.
//******************************************************************************

#pragma once

#include <mutex>
#include <atomic>

namespace SvSyl
{

////////////////////////////////////////////////////////////////////////////////
// Implementation for SVFuture<void>

namespace detail
{
template<>
struct State<void>
{
	std::mutex mutex;
	std::condition_variable cv;
	std::function<void(SVFuture<void>)> fn;
	bool is_done = false;
	bool is_success = false;
	std::exception_ptr exptr;
};
} // namespace detail

inline SVFuture<void>::SVFuture(detail::StatePtr<void> state) : m_state(state)
{
}

inline SVFuture<void> SVFuture<void>::make_ready()
{
	SVPromise<void> promise;
	promise.set_value();
	return promise.get_future();
}

inline SVFuture<void> SVFuture<void>::make_exceptional(std::exception_ptr ptr)
{
	SVPromise<void> promise;
	promise.set_exception(ptr);
	return promise.get_future();
}

inline void SVFuture<void>::all_step(boost::asio::io_context& ctx, std::vector<SVFuture<void>> futures, std::shared_ptr<SVPromise<void>> promise, SVFuture<void> f)
{
	// TODO check status of f!

	if (futures.empty())
	{
		promise->set_value();
		return;
	}

	auto first = futures.front();
	futures.erase(futures.begin());
	first.then(ctx, [&ctx, futures, promise](SVFuture<void> f)
	{
		all_step(ctx, futures, promise, f);
	});
}

inline SVFuture<void> SVFuture<void>::all(boost::asio::io_context& ctx, std::vector<SVFuture<void>> futures)
{
	if (futures.empty())
	{
		return make_ready();
	}

	auto promise = std::make_shared<SVPromise<void>>();

	auto first = futures.front();
	futures.erase(futures.begin());
	first.then(ctx, [&ctx, futures, promise](SVFuture<void> f)
	{
		all_step(ctx, futures, promise, f);
	});

	return promise->get_future();
}

inline void SVFuture<void>::get()
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	if (!m_state->is_done)
	{
		m_state->cv.wait(lk);
	}

	assert(m_state->is_done);

	if (!m_state->is_success)
	{
		std::rethrow_exception(m_state->exptr);
	}
}

inline SVFuture<void> SVFuture<void>::then(boost::asio::io_context& ctx, std::function<void(SVFuture<void>)> fn)
{
	std::unique_lock<std::mutex> lk(m_state->mutex);
	auto then_done = std::make_shared<SVPromise<void>>();

	auto on_done = [fn, then_done](SVFuture<void> f)
	{
		try
		{
			fn(f);
		}
		catch (const std::exception& err)
		{
			then_done->set_exception(std::make_exception_ptr(err));
			return;
		}

		if (!f.m_state->is_success)
		{
			then_done->set_exception(f.m_state->exptr);
			return;
		}

		then_done->set_value();
	};

	if (m_state->is_done)
	{
		auto f = *this;
		ctx.dispatch([on_done, f]()
		{
			on_done(f);
		});
	}
	else
	{
		m_state->fn = [&ctx, on_done](SVFuture<void> f)
		{
			ctx.dispatch([f, on_done]()
			{
				on_done(f);
			});
		};
	}

	return then_done->get_future();
}

template<>
inline void SVPromise<void>::set_value()
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	m_state->is_done = true;
	m_state->is_success = true;

	// trigger all future.get()
	m_state->cv.notify_all();
	
	auto fn = m_state->fn;
	lk.unlock();

	// trigger all future.then()
	if (fn)
	{
		fn(*this);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Implementation for SVFuture<T>

namespace detail
{
template<class T>
struct State
{
	std::mutex mutex;
	std::condition_variable cv;
	std::function<void(SVFuture<T>)> fn;
	bool is_done = false;
	bool is_success = false;
	T value;
	std::exception_ptr exptr;
};
} // namespace detail

template<class T>
inline SVFuture<T>::SVFuture(detail::StatePtr<T> state) : m_state(state)
{
}

template<class T>
inline SVFuture<T> SVFuture<T>::make_ready(T t)
{
	SVPromise<T> promise;
	promise.set_value(std::move(t));
	return promise.get_future();
}

template<class T>
inline SVFuture<T> SVFuture<T>::make_exceptional(std::exception_ptr ptr)
{
	class Promise<T> promise;
	promise.set_exception(ptr);
	return promise.get_future();
}

template<class T>
inline T SVFuture<T>::get()
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	if (!m_state->is_done)
	{
		m_state->cv.wait(lk, [this]
		{
			return m_state->is_done;
		});
	}

	assert(m_state->is_done);

	if (!m_state->is_success)
	{
		std::rethrow_exception(m_state->exptr);
	}

	return m_state->value;
}

template<class T>
template<class R>
inline SVFuture<R> SVFuture<T>::then(boost::asio::io_context& ctx, std::function<R(SVFuture<T>)> fn)
{
	auto then_done = std::make_shared<SVPromise<void>>();

	auto on_done = [fn, then_done](SVFuture<T> f)
	{
		try
		{
			// TODO read return value
			// TODO add specialization for void
			fn(f);
		}
		catch (const std::exception& err)
		{
			then_done->set_exception(std::make_exception_ptr(err));
			return;
		}

		if (!f.m_state->is_success)
		{
			then_done->set_exception(f.m_state->exptr);
			return;
		}

		then_done->set_value();
	};

	if (m_state->is_done)
	{
		auto f = *this;
		ctx.dispatch([on_done, f]()
		{
			on_done(f);
		});
	}
	else
	{
		std::unique_lock<std::mutex> lk(m_state->mutex);
		m_state->fn = [&ctx, on_done](SVFuture<T> f)
		{
			ctx.dispatch([f, on_done]()
			{
				on_done(f);
			});
		};
	}

	return then_done->get_future();
}

template<class T>
inline SVPromise<T>::SVPromise() : SVFuture<T>(std::make_shared<detail::State<T>>())
{
}

template<class T>
inline SVFuture<T> SVPromise<T>::get_future()
{
	return SVFuture<T>(this->m_state);
}

template<class T>
inline void SVPromise<T>::set_value(T t)
{
	std::unique_lock<std::mutex> lk(this->m_state->mutex);

	this->m_state->value = std::move(t);
	this->m_state->is_done = true;
	this->m_state->is_success = true;

	// trigger all future.get()
	this->m_state->cv.notify_all();

	auto fn = this->m_state->fn;
	lk.unlock();

	// trigger all future.then()
	if (fn)
	{
		fn(*this);
	}
}

template<class T>
inline void SVPromise<T>::set_exception(std::exception_ptr exptr)
{
	std::unique_lock<std::mutex> lk(this->m_state->mutex);

	this->m_state->exptr = exptr;
	this->m_state->is_done = true;
	this->m_state->is_success = false;

	// trigger all future.get()
	this->m_state->cv.notify_all();

	auto fn = this->m_state->fn;
	lk.unlock();

	// trigger all future.then()
	if (fn)
	{
		fn(*this);
	}
}

} // namespace SvSyl
