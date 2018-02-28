//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file TaskWrapper.h
/// All Rights Reserved
//******************************************************************************
/// When having a function that takes a certain request of type `Req` and a Task
/// of type `Task<Res>`, you can use this util to automatically unwrap the Req
/// from the incoming payload and also automatically wrapping the response into
/// an Envelope again.
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
#include "SVRPCLibrary/OneOfUtil.h"
#include "SVRPCLibrary/Task.h"
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{

class TaskWrapperBase
{
public:
	virtual ~TaskWrapperBase() {}
	virtual void operator()(Envelope&&, Task<Envelope>) = 0;
};

template <typename TPayload, typename TReq, typename TRes> class TaskWrapper : public TaskWrapperBase
{
public:
	TaskWrapper(std::function<void(TReq&&, Task<TRes>)>&& fn) : m_fn(std::move(fn)) {}

	~TaskWrapper() override {}

	void operator()(Envelope&& envelope, Task<Envelope> task) override
	{
		TReq req;
		if (!m_req_unwrapper.unwrap(req, std::move(envelope)))
		{
			BOOST_LOG_TRIVIAL(warning) << "Envelope with unknown payload!";
			task.error(build_error(ErrorCode::InternalError, "Unknown payload"));
			return;
		}

		m_fn(std::move(req),
			Task<TRes>(
			[this, task](TRes&& res)
		{
			SVRPC::Envelope resEnvelope;
			m_res_wrapper.wrap(resEnvelope, std::move(res));

			task.finish(std::move(resEnvelope));
		},
			[task](const SVRPC::Error& err) { task.error(err); }));
	}

private:
	std::function<void(TReq&&, Task<TRes>)> m_fn;
	OneOfUtil<TPayload, TReq> m_req_unwrapper;
	OneOfUtil<TPayload, TRes> m_res_wrapper;
};

} // namespace SVRPC
