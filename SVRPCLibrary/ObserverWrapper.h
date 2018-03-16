//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ObserverWrapper.h
/// All Rights Reserved
//******************************************************************************
/// When having a function that takes a certain request of type `Req` and
/// an Observer of type `Observer<Res>`, you can use this util to automatically
/// unwrap the Req from the incoming payload and also automatically wrapping
/// the response into an Envelope again.
///
/// The container hierarchy is as follows:
/// -> Envelope
///    -> Payload (containing oneof with all messages)
///       -> Single Req/Res Message
///
/// The OneOfUtil<> is used for unwrapping/wrapping the request/response and
/// the EnvelopeUtil is used for wrapping the payload into the result envelope.
//******************************************************************************

#pragma once

#include <functional>
#include <memory>

#include "SVRPCLibrary/EnvelopeUtil.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/OneOfUtil.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#include "SVProtoBuf/envelope.h"

namespace SVRPC
{
class ObserverWrapperBase
{
public:
	virtual ~ObserverWrapperBase() {}
	virtual void operator()(Envelope&&, Observer<Envelope>, ServerStreamContext::Ptr) = 0;
};

template <typename TPayload, typename TReq, typename TRes> class ObserverWrapper : public ObserverWrapperBase
{
public:
	ObserverWrapper(std::function<void(TReq&&, Observer<TRes>, ServerStreamContext::Ptr)>&& Handler) : m_Handler(std::move(Handler)) {}
	~ObserverWrapper() override {}

	void operator()(Envelope&& envelope, Observer<Envelope> observer, ServerStreamContext::Ptr ctx) override
	{
		TReq req;
		if (!m_ReqUnwrapper.unwrap(req, std::move(envelope)))
		{
			BOOST_LOG_TRIVIAL(warning) << "Envelope with unknown payload!";
			observer.error(build_error(ErrorCode::InternalError, "Unknown payload"));
			return;
		}

		m_Handler(std::move(req),
			Observer<TRes>(
			[this, observer](TRes&& res) -> std::future<void>
		{
			Envelope resEnvelope;
			m_ResWrapper.wrap(resEnvelope, std::move(res));

			return observer.onNext(std::move(resEnvelope));
		},
			[observer]() { observer.finish(); },
			[observer](const Error& err) { observer.error(err); }),
			ctx);
	}

private:
	std::function<void(TReq&&, Observer<TRes>, ServerStreamContext::Ptr)> m_Handler;
	OneOfUtil<TPayload, TReq> m_ReqUnwrapper;
	OneOfUtil<TPayload, TRes> m_ResWrapper;
};

} // namespace SVRPC
