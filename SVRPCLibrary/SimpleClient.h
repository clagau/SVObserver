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
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{
template <typename TPayload, typename TReq, typename TRes> class SimpleClient
{
public:
	SimpleClient(RPCClient& client)
		: m_client(client),
		m_unwrap_error(build_error(ErrorCode::InternalError, "Error while unwrapping envelope!"))
	{
	}

	void request(TReq&& req, Task<TRes> task, boost::posix_time::time_duration timeout)
	{
		Envelope envelope;
		m_req_wrapper.wrap(envelope, std::move(req));
		m_client.request(std::move(envelope),
			Task<Envelope>
			([this, task](Envelope&& resEnv)
		{
			TRes res;
			if (!m_res_unwrapper.unwrap(res, std::move(resEnv)))
			{
				task.error(m_unwrap_error);
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
		m_req_wrapper.wrap(envelope, std::move(req));

		m_client.request(std::move(envelope),
			Task<Envelope>(
			[this, promise](Envelope&& resEnv)
		{
			TRes res;
			if (!m_res_unwrapper.unwrap(res, std::move(resEnv)))
			{
				promise->set_exception(errorToExceptionPtr(m_unwrap_error));
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
		m_req_wrapper.wrap(envelope, std::move(req));

		m_client.stream(std::move(envelope),
			Observer<Envelope>(
			[this, observer](Envelope&& resEnv) -> std::future<void>
		{
			TRes res;
			if (!m_res_unwrapper.unwrap(res, std::move(resEnv)))
			{
				observer.error(m_unwrap_error);
				return std::future<void>();
			}

			return observer.onNext(std::move(res));
		},
			[observer]() { observer.finish(); },
			[observer](const Error& err) { observer.error(err); }));
	}

private:
	RPCClient& m_client;
	OneOfUtil<TPayload, TReq> m_req_wrapper;
	OneOfUtil<TPayload, TRes> m_res_unwrapper;
	Error m_unwrap_error;
};

} // namespace SVRPC
