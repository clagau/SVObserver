//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SimpleClient.h
/// All Rights Reserved
//******************************************************************************
/// Util that takes care of wrapping your Request into an envelope and also
/// unwrapping the response from the response envelope.
///
/// There is also one function that maps the `Task<TRes>` to a
/// `std::future<TRes>` for easier integration into async flows using stl.
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <memory>

#include "EnvelopeUtil.h"
#include "ErrorUtil.h"
#include "OneOfUtil.h"
#include "RPCClient.h"
#include "SVProtoBuf/envelope.h"

namespace SvRpc
{
template <typename TPayload, typename TReq, typename TRes> class SimpleClient
{
public:
	SimpleClient(RPCClient& rClient)
		: m_rClient(rClient)
		, m_UnwrapError(build_error(SvPenv::ErrorCode::internalError, "Error while unwrapping envelope!"))
	{
	}

	void request(TReq&& req, Task<TRes> task, boost::posix_time::time_duration timeout)
	{
		SvPenv::Envelope Envelope;
		m_ReqWrapper.wrap(Envelope, std::move(req));
		m_rClient.request(std::move(Envelope),
			Task<SvPenv::Envelope>(
			[this, task](SvPenv::Envelope&& resEnv)
		{
			TRes res;
			if (!m_ResUnwrapper.unwrap(res, std::move(resEnv)))
			{
				task.error(m_UnwrapError);
				return;
			}

			task.finish(std::move(res));
		},
			[task](const SvPenv::Error& rError)
		{
			task.error(rError);
		}),
			timeout);
	}

	std::future<TRes> request(TReq&& req, boost::posix_time::time_duration timeout)
	{
		auto promise = std::make_shared<std::promise<TRes>>();

		SvPenv::Envelope Envelope;
		m_ReqWrapper.wrap(Envelope, std::move(req));

		m_rClient.request(std::move(Envelope),
			Task<SvPenv::Envelope>(
			[this, promise](SvPenv::Envelope&& resEnv)
		{
			TRes res;
			if (!m_ResUnwrapper.unwrap(res, std::move(resEnv)))
			{
				promise->set_exception(errorToExceptionPtr(m_UnwrapError));
				return;
			}

			promise->set_value(std::move(res));
		},
			[promise](const SvPenv::Error& err) { promise->set_exception(errorToExceptionPtr(err)); }),
			timeout
		);

		return promise->get_future();
	}

	SvRpc::ClientStreamContext stream(TReq&& Request, Observer<TRes> observer)
	{
		SvPenv::Envelope Envelope;
		m_ReqWrapper.wrap(Envelope, std::move(Request));

		return m_rClient.stream(std::move(Envelope),
			Observer<SvPenv::Envelope>(
			[this, observer](SvPenv::Envelope&& resEnv) -> std::future<void>
		{
			TRes res;
			if (!m_ResUnwrapper.unwrap(res, std::move(resEnv)))
			{
				observer.error(m_UnwrapError);
				return std::future<void>();
			}

			return observer.onNext(std::move(res));
		},
			[observer]() { observer.finish(); },
			[observer](const SvPenv::Error& rError) { observer.error(rError); }));
	}

private:
	RPCClient& m_rClient;
	OneOfUtil<TPayload, TReq> m_ReqWrapper;
	OneOfUtil<TPayload, TRes> m_ResUnwrapper;
	SvPenv::Error m_UnwrapError;
};

} // namespace SvRpc
