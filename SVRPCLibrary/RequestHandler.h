//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <memory>

#include "SVStatusLibrary/ErrorUtil.h"
#include "Observer.h"
#include "ObserverWrapper.h"
#include "RequestHandlerBase.h"
#include "Task.h"
#include "TaskWrapper.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace SvRpc
{
class RequestHandler : public RequestHandlerBase
{
public:
	using TRequestMiddleware = std::function<bool(const SvAuth::SessionContext&, const SvPenv::Envelope&, Task<SvPenv::Envelope>)>;
	using TStreamMiddleware = std::function<bool(const SvAuth::SessionContext&, const SvPenv::Envelope&, Observer<SvPenv::Envelope>, ServerStreamContext::Ptr)>;

	~RequestHandler() override {}

	void registerAuthHandler(std::function<bool(const std::string&, SvAuth::SessionContext&)> AuthHandler)
	{
		m_AuthHandler = std::move(AuthHandler);
	}

	void registerRequestMiddleware(TRequestMiddleware fn)
	{
		m_RequestMiddlewares.push_back(fn);
	}

	void registerStreamMiddleware(TStreamMiddleware fn)
	{
		m_StreamMiddlewares.push_back(fn);
	}

	template <typename TPayload, int MessageCase, typename TReq, typename TRes>
	void registerRequestHandler(std::function<void(const SvAuth::SessionContext&, TReq&&, Task<TRes>)> Handler)
	{
		m_RequestHandler[MessageCase] = std::make_shared<TaskWrapper<TPayload, TReq, TRes>>(std::move(Handler));
	}

	template <typename TPayload, int MessageCase, typename TReq, typename TRes>
	void registerStreamHandler(std::function<void(const SvAuth::SessionContext&, TReq&&, Observer<TRes>, ServerStreamContext::Ptr)> Handler)
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
	virtual bool onHandshake(const std::string& token, SvAuth::SessionContext& rSessionContext) override
	{
		if (!m_AuthHandler)
		{
			return true;
		}

		return m_AuthHandler(token, rSessionContext);
	}

	void onRequest(const SvAuth::SessionContext& rSessionContext, SvPenv::Envelope&& Envelope, Task<SvPenv::Envelope> Task) override
	{
		auto payloadType = Envelope.payloadtype();

		for (auto& rMiddleware : m_RequestMiddlewares)
		{
			if (rMiddleware(rSessionContext, Envelope, Task))
			{
				SV_LOG_GLOBAL(trace) << "Request already handled by middleware:" << payloadType;
				return;
			}
		}

		auto it = m_RequestHandler.find(payloadType);
		if (it != m_RequestHandler.end())
		{
			SV_LOG_GLOBAL(trace) << "Request with SVRC MessageType:" << payloadType;
			(*it->second)(rSessionContext, std::move(Envelope), std::move(Task));
			return;
		}

		if (m_DefaultRequestHandler)
		{
			SV_LOG_GLOBAL(trace) << "Forward Request   with SVRC MessageType:" << payloadType;
			m_DefaultRequestHandler(std::move(Envelope), std::move(Task));
			return;
		}

		SV_LOG_GLOBAL(warning) << "No request handler for payload type " << payloadType;
		Task.error(SvStl::build_error(SvPenv::ErrorCode::notImplemented, "No handler for given payload type."));
	}

	void onStream(const SvAuth::SessionContext& rSessionContext, SvPenv::Envelope&& Envelope, Observer<SvPenv::Envelope> Observer, ServerStreamContext::Ptr Context) override
	{
		auto payloadType = Envelope.payloadtype();

		for (auto& rMiddleware : m_StreamMiddlewares)
		{
			if (rMiddleware(rSessionContext, Envelope, Observer, Context))
			{
				SV_LOG_GLOBAL(trace) << "Stream already handled by middleware:" << payloadType;
				return;
			}
		}

		auto it = m_StreamHandler.find(payloadType);
		if (it != m_StreamHandler.end())
		{
			(*it->second)(rSessionContext, std::move(Envelope), std::move(Observer), Context);
			return;
		}

		if (m_DefaultStreamHandler)
		{
			m_DefaultStreamHandler(std::move(Envelope), std::move(Observer), Context);
			return;
		}

		SV_LOG_GLOBAL(warning) << "No request handler for payload type " << payloadType;
		Observer.error(SvStl::build_error(SvPenv::ErrorCode::notImplemented, "No handler for given payload type."));
	}

private:
	std::function<bool(const std::string&, SvAuth::SessionContext&)> m_AuthHandler;
	std::vector<TRequestMiddleware> m_RequestMiddlewares;
	std::map<int, std::shared_ptr<TaskWrapperBase>> m_RequestHandler;
	std::vector<TStreamMiddleware> m_StreamMiddlewares;
	std::map<int, std::shared_ptr<ObserverWrapperBase>> m_StreamHandler;
	std::function<void(SvPenv::Envelope&&, Task<SvPenv::Envelope>)> m_DefaultRequestHandler;
	std::function<void(SvPenv::Envelope&&, Observer<SvPenv::Envelope>, ServerStreamContext::Ptr)> m_DefaultStreamHandler;
};

} // namespace SvRpc
