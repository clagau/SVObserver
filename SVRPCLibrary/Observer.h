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

//Moved to precompiled header: #include <functional>
//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <memory>
#include "SVProtoBuf/envelope.h"

namespace SvRpc
{
// typename is typically a result class
template <typename T> struct Observer
{
public:
	using OnNextFn = std::function<std::future<void>(T&& t)>;
	using OnFinishFn = std::function<void()>;
	using OnErrorFn = std::function<void(const SvPenv::Error&)>;

	Observer(OnNextFn OnNext, OnFinishFn OnFinish, OnErrorFn OnError) : m_OnNext(OnNext), m_OnFinish(OnFinish), m_OnError(OnError)
	{
	}

	std::future<void> onNext(T&& t) const
	{
		if (!m_OnNext)
		{
			return std::future<void>();
		}
		return m_OnNext(std::move(t));
	}

	void finish() const
	{
		if (m_OnFinish)
		{
			m_OnFinish();
		}
	}

	void error(const SvPenv::Error& err) const
	{
		if (m_OnError)
		{
			m_OnError(err);
		}
	}

public:
	OnNextFn m_OnNext;
	OnFinishFn m_OnFinish;
	OnErrorFn m_OnError;
};

} // namespace SvRpc
