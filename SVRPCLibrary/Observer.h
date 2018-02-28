//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Observer.h
/// All Rights Reserved
//******************************************************************************
/// Like Task, the Observer is an handle for an async operation. With the main
/// difference that the Oberserver can be called with multiple return values,
/// then one finish call or an error call if sth fails. Main use case for this
/// class are streaming RPC calls where onNext is called for each entry returned
/// by the stream.
///
/// Naming is intentionally modeled after the Reactive Extensions (RX).
/// See http://reactivex.io/
/// and https://en.wikipedia.org/wiki/Reactive_extensions
//******************************************************************************

#pragma once

#include <functional>
#include <future>
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning  (pop)

namespace SVRPC
{
//typename is typically a result class 
template <typename T> struct Observer
{
public:
	using FinishFkt = std::function<void()>;
	using ErrorFkt = std::function<void(const Error&)>;
	using NextFkt = std::function<std::future<void>(T&& t)> ;

	Observer(NextFkt on_next, FinishFkt finish, ErrorFkt error) : m_on_next(on_next), m_finish(finish), m_error(error)
	{
	}
	std::future<void> onNext(T&& t) const
	{
		if (!m_on_next)
		{
			return std::future<void>();
		}
		return m_on_next(std::move(t));
	}
	void finish() const
	{
		if (m_finish)
		{
			m_finish();
		}
	}
	void error(const Error& err) const
	{
		if (m_error)
		{
			m_error(err);
		}
	}

public:
	std::function<std::future<void>(T&& t)> m_on_next;
	std::function<void()> m_finish;
	std::function<void(const Error&)> m_error;
};
}
