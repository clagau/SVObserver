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

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <memory>
#include <boost/log/trivial.hpp>

#include "EnvelopeUtil.h"
#include "ErrorUtil.h"
#include "Observer.h"
#include "ObserverWrapper.h"
#include "OneOfUtil.h"
#include "RequestHandlerBase.h"
#include "Task.h"
#include "TaskWrapper.h"

namespace SvRpc
{
class RequestHandler : public RequestHandlerBase
{
public:
	~RequestHandler() override {}

	void registerAuthHandler(std::function<bool(const std::string&)> AuthHandler)
	{
		m_AuthHandler = std::move(AuthHandler);
	}

	template <typename TPayload, int MessageCase, typename TReq, typename TRes>
	void registerRequestHandler(std::function<void(TReq&&, Task<TRes>)> Handler)
	{
		m_RequestHandler[MessageCase] = std::make_shared<TaskWrapper<TPayload, TReq, TRes>>(std::move(Handler));
	}

	template <typename TPayload, int MessageCase, typename TReq, typename TRes>
	void registerStreamHandler(std::function<void(TReq&&, Observer<TRes>, ServerStreamContext::Ptr)> Handler)
	{
		m_StreamHandler[MessageCase] = std::make_shared<ObserverWrapper<TPayload, TReq, TRes>>(std::move(Handler));
	}

	void registerDefaultRequestHandler(std::function<void(SvPenv::Envelope&&, Task<SvPenv::Envelope>)> Handler)
	{
		m_DefaultRequestHandler = std::move(Handler);
	}

	void registerDefaultStreamHandler(std::function<void(SvPenv::Envelope&&, Observer<SvPenv::Envelope>, ServerStreamContext::Ptr)> Handler)
	{
		m_DefaultStreamHandler = std::move(Handler);
	}

protected:
	virtual bool onHandshake(const std::string& token) override
	{
		if (!m_AuthHandler)
		{
			return true;
		}

		return m_AuthHandler(token);
	}

	void onRequest(SvPenv::Envelope&& Envelope, Task<SvPenv::Envelope> Task) override
	{
		auto payloadType = Envelope.payloadtype();
		auto it = m_RequestHandler.find(payloadType);
		if (it != m_RequestHandler.end())
		{
			(*it->second)(std::move(Envelope), std::move(Task));
			return;
		}

		if (m_DefaultRequestHandler)
		{
			m_DefaultRequestHandler(std::move(Envelope), std::move(Task));
			return;
		}

		BOOST_LOG_TRIVIAL(warning) << "No request handler for payload type " << payloadType;
		Task.error(build_error(SvPenv::ErrorCode::notImplemented, "No handler for given payload type."));
	}

	void onStream(SvPenv::Envelope&& Envelope, Observer<SvPenv::Envelope> Observer, ServerStreamContext::Ptr Context) override
	{
		auto payloadType = Envelope.payloadtype();
		auto it = m_StreamHandler.find(payloadType);
		if (it != m_StreamHandler.end())
		{
			(*it->second)(std::move(Envelope), std::move(Observer), Context);
			return;
		}

		if (m_DefaultStreamHandler)
		{
			m_DefaultStreamHandler(std::move(Envelope), std::move(Observer), Context);
			return;
		}

		BOOST_LOG_TRIVIAL(warning) << "No request handler for payload type " << payloadType;
		Observer.error(build_error(SvPenv::ErrorCode::notImplemented, "No handler for given payload type."));
	}

private:
	std::function<bool(const std::string&)> m_AuthHandler;
	std::map<int, std::shared_ptr<TaskWrapperBase>> m_RequestHandler;
	std::map<int, std::shared_ptr<ObserverWrapperBase>> m_StreamHandler;
	std::function<void(SvPenv::Envelope&&, Task<SvPenv::Envelope>)> m_DefaultRequestHandler;
	std::function<void(SvPenv::Envelope&&, Observer<SvPenv::Envelope>, ServerStreamContext::Ptr)> m_DefaultStreamHandler;
};

} // namespace SvRpc
