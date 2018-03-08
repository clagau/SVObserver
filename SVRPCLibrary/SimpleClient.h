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

#include <future>
#include <memory>

#include "SVRPCLibrary/EnvelopeUtil.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/OneOfUtil.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVProtoBuf/envelope.h"

namespace SVRPC
{
template <typename TPayload, typename TReq, typename TRes> class SimpleClient
{
public:
	SimpleClient(RPCClient& rClient)
		: m_rClient(rClient)
		, m_UnwrapError(build_error(ErrorCode::InternalError, "Error while unwrapping envelope!"))
	{
	}

	void request(TReq&& req, Task<TRes> task, boost::posix_time::time_duration timeout)
	{
		Envelope envelope;
		m_ReqWrapper.wrap(envelope, std::move(req));
		m_rClient.request(std::move(envelope),
			Task<Envelope>(
			[this, task](Envelope&& resEnv)
		{
			TRes res;
			if (!m_ResUnwrapper.unwrap(res, std::move(resEnv)))
			{
				task.error(m_UnwrapError);
				return;
			}

			task.finish(std::move(res));
		},
			[task](const Error& err)
		{
			task.error(err);
		}),
			timeout);
	}

	std::future<TRes> request(TReq&& req, boost::posix_time::time_duration timeout)
	{
		auto promise = std::make_shared<std::promise<TRes>>();

		Envelope envelope;
		m_ReqWrapper.wrap(envelope, std::move(req));

		m_rClient.request(std::move(envelope),
			Task<Envelope>(
			[this, promise](Envelope&& resEnv)
		{
			TRes res;
			if (!m_ResUnwrapper.unwrap(res, std::move(resEnv)))
			{
				promise->set_exception(errorToExceptionPtr(m_UnwrapError));
				return;
			}

			promise->set_value(std::move(res));
		},
			[promise](const Error& err) { promise->set_exception(errorToExceptionPtr(err)); }),
			timeout
		);

		return promise->get_future();
	}

	void stream(TReq&& req, Observer<TRes> observer)
	{
		Envelope envelope;
		m_ReqWrapper.wrap(envelope, std::move(req));

		m_rClient.stream(std::move(envelope),
			Observer<Envelope>(
			[this, observer](Envelope&& resEnv) -> std::future<void>
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
			[observer](const Error& err) { observer.error(err); }));
	}

private:
	RPCClient& m_rClient;
	OneOfUtil<TPayload, TReq> m_ReqWrapper;
	OneOfUtil<TPayload, TRes> m_ResUnwrapper;
	Error m_UnwrapError;
};

} // namespace SVRPC
