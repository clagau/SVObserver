//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RequestHandler.h
/// All Rights Reserved
//******************************************************************************
/// The RequestHandler<> class is an easy-to-use version of the
/// RequestHandlerBase interface. It takes care of unwrapping/wrapping messages
/// from the incoming envelopes and calling the registered request
/// implementations.
///
/// You can either inherit from it and register the callbacks in the constructor
/// or just instantiate it and register the callbacks on the instance.
//******************************************************************************

#pragma once

#include <map>
#include <memory>

#include <boost/log/trivial.hpp>

#include "SVRPCLibrary/EnvelopeUtil.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ObserverWrapper.h"
#include "SVRPCLibrary/OneOfUtil.h"
#include "SVRPCLibrary/RequestHandlerBase.h"
#include "SVRPCLibrary/Task.h"
#include "SVRPCLibrary/TaskWrapper.h"

namespace SVRPC
{
class RequestHandler : public RequestHandlerBase
{
public:
	~RequestHandler() override {}

	template <typename TPayload, int message_case, typename TReq, typename TRes>
	void registerRequestHandler(std::function<void(TReq&&, Task<TRes>)> fn)
	{
		m_request_handler[message_case] = std::make_shared<TaskWrapper<TPayload, TReq, TRes>>(std::move(fn));
	}

	template <typename TPayload, int message_case, typename TReq, typename TRes>
	void registerStreamHandler(std::function<void(TReq&&, Observer<TRes>)> fn)
	{
		m_stream_handler[message_case] = std::make_shared<ObserverWrapper<TPayload, TReq, TRes>>(std::move(fn));
	}

	void registerDefaultRequestHandler(std::function<void(Envelope&&, Task<Envelope>)> fn)
	{
		m_default_request_handler = std::move(fn);
	}

	void registerDefaultStreamHandler(std::function<void(Envelope&&, Observer<Envelope>)> fn)
	{
		m_default_stream_handler = std::move(fn);
	}

protected:
	void onRequest(Envelope&& envelope, Task<Envelope> task) override
	{
		auto payload_type = envelope.payload_type();
		auto it = m_request_handler.find(payload_type);
		if (it != m_request_handler.end())
		{
			(*it->second)(std::move(envelope), std::move(task));
			return;
		}

		if (m_default_request_handler)
		{
			m_default_request_handler(std::move(envelope), std::move(task));
			return;
		}

		BOOST_LOG_TRIVIAL(warning) << "No request handler for payload type " << payload_type;
		task.error(build_error(ErrorCode::NotImplemented, "No handler for given payload type."));
	}

	void onStream(Envelope&& envelope, Observer<Envelope> observer) override
	{
		auto payload_type = envelope.payload_type();
		auto it = m_stream_handler.find(payload_type);
		if (it != m_stream_handler.end())
		{
			(*it->second)(std::move(envelope), std::move(observer));
			return;
		}

		if (m_default_stream_handler)
		{
			m_default_stream_handler(std::move(envelope), std::move(observer));
			return;
		}

		BOOST_LOG_TRIVIAL(warning) << "No request handler for payload type " << payload_type;
		observer.error(build_error(ErrorCode::NotImplemented, "No handler for given payload type."));
	}

private:
	std::map<int, std::shared_ptr<TaskWrapperBase>> m_request_handler;
	std::map<int, std::shared_ptr<ObserverWrapperBase>> m_stream_handler;
	std::function<void(Envelope&&, Task<Envelope>)> m_default_request_handler;
	std::function<void(Envelope&&, Observer<Envelope>)> m_default_stream_handler;
};
}
