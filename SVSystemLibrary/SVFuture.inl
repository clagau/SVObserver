//******************************************************************************
/// \copyright (c) 2017,2019 by Seidenader Maschinenbau GmbH
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

inline void SVFuture<void>::then(boost::asio::io_context& ctx, std::function<void(SVFuture<void>)> fn)
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	if (m_state->is_done)
	{
		auto f = *this;
		ctx.dispatch([fn, f]()
		{
			fn(f);
		});
	}
	else
	{
		m_state->fn = [&ctx, fn](SVFuture<void> f)
		{
			ctx.dispatch([fn, f]()
			{
				fn(f);
			});
		};
	}
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
	Promise<T> promise;
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
inline void SVFuture<T>::then(boost::asio::io_context& ctx, std::function<void(SVFuture<T>)> fn)
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	if (m_state->is_done)
	{
		auto f = *this;
		ctx.dispatch([fn, f]()
		{
			fn(f);
		});
	}
	else
	{
		m_state->fn = [&ctx, fn](SVFuture<T> f)
		{
			ctx.dispatch([fn, f]()
			{
				fn(f);
			});
		};
	}
}

template<class T>
inline SVPromise<T>::SVPromise() : SVFuture<T>(std::make_shared<detail::State<T>>())
{
}

template<class T>
inline SVFuture<T> SVPromise<T>::get_future()
{
	return SVFuture<T>(m_state);
}

template<class T>
inline void SVPromise<T>::set_value(T t)
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	m_state->value = std::move(t);
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

template<class T>
inline void SVPromise<T>::set_exception(std::exception_ptr exptr)
{
	std::unique_lock<std::mutex> lk(m_state->mutex);

	m_state->exptr = exptr;
	m_state->is_done = true;
	m_state->is_success = false;

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

} // namespace SvSyl
