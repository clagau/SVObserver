//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#pragma region Includes
//Moved to precompiled header: #include <functional>
//Moved to precompiled header: #include <memory>

#include "OneOfUtil.h"
#include "Task.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/Envelope.h"
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace SvRpc
{

class TaskWrapperBase
{
public:
	virtual ~TaskWrapperBase() {}
	virtual void operator()(const SvAuth::SessionContext&, SvPenv::Envelope&&, Task<SvPenv::Envelope>) = 0;
};

template <typename TPayload, typename TReq, typename TRes>
class TaskWrapper : public TaskWrapperBase
{
public:
	TaskWrapper(std::function<void(const SvAuth::SessionContext&, TReq&&, Task<TRes>)>&& Handler) : m_Handler(std::move(Handler)) {}

	~TaskWrapper() override {}

	void operator()(const SvAuth::SessionContext& rSessionContext, SvPenv::Envelope&& envelope, Task<SvPenv::Envelope> task) override
	{
		TReq req;
		if (!m_ReqUnwrapper.unwrap(req, std::move(envelope)))
		{
			SV_LOG_GLOBAL(warning) << "Envelope with unknown payload!";
			task.error(SvStl::build_error(SvPenv::ErrorCode::internalError, "Unknown payload"));
			return;
		}

		m_Handler(rSessionContext, std::move(req),
			Task<TRes>(
			[this, task](TRes&& res)
		{
			SvPenv::Envelope resEnvelope;
			m_ResWrapper.wrap(resEnvelope, std::move(res));

			task.finish(std::move(resEnvelope));
		},
			[task](const SvPenv::Error& err) { task.error(err); }));
	}

private:
	std::function<void(const SvAuth::SessionContext&, TReq&&, Task<TRes>)> m_Handler;
	OneOfUtil<TPayload, TReq> m_ReqUnwrapper;
	OneOfUtil<TPayload, TRes> m_ResWrapper;
};

} // namespace SvRpc
