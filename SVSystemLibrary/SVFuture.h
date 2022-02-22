//******************************************************************************
/// \copyright (c) 2017,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Future.h
/// All Rights Reserved
//******************************************************************************
/// Future class which can execute the continuation on in a provided executor.
//******************************************************************************

#pragma once

#include <exception>
#include <vector>

#include <boost/asio/io_context.hpp>

namespace SvSyl
{

namespace detail
{
template<class T>
struct State;

template<class T>
using StatePtr = std::shared_ptr<State<T>>;
}

template<class T>
class SVFuture
{
private:
	SVFuture() = delete;
	
	template <class T> friend class SVPromise;
	SVFuture(detail::StatePtr<T>);

public:
	SVFuture(const SVFuture&) = default;

	static SVFuture<T> make_ready(T);
	static SVFuture<T> make_exceptional(std::exception_ptr);

public:
	T get();

	template <class R>
	SVFuture<R> then(boost::asio::io_context&, std::function<R(SVFuture<T>)>);

protected:
	detail::StatePtr<T> m_state;
};

template<>
class SVFuture<void>
{
private:
	SVFuture() = delete;

	template <class T> friend class SVPromise;
	SVFuture(detail::StatePtr<void>);

public:
	SVFuture(const SVFuture&) = default;

	static SVFuture<void> make_ready();
	static SVFuture<void> make_exceptional(std::exception_ptr);

	static SVFuture<void> all(boost::asio::io_context&, std::vector<SVFuture<void>>);

public:
	void get();
	SVFuture<void> then(boost::asio::io_context&, std::function<void(SVFuture<void>)>);

protected:
	detail::StatePtr<void> m_state;

private:
	static void all_step(boost::asio::io_context& ctx, std::vector<SVFuture<void>>, std::shared_ptr<SVPromise<void>>, SVFuture<void>);
};

template<class T>
class SVPromise : public SVFuture<T>
{
public:
	SVPromise();

	void set_value(T);
	void set_exception(std::exception_ptr);
	SVFuture<T> get_future();
};

} // namespace SvSyl

#include "SVSystemLibrary/SVFuture.inl"
