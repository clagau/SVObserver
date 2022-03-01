#include "stdafx.h"

#pragma region Includes
#include "LockAcquisitionStream.h"
#pragma endregion Includes

std::uint32_t lock_acquisition_stream_t::ID_COUNTER = 0;

lock_acquisition_stream_t::lock_acquisition_stream_t(
	const SvPb::LockAcquisitionStreamRequest& req,
	const SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>& obs,
	const SvRpc::ServerStreamContext::Ptr& streamCtx,
	const SvAuth::SessionContext& sessionCtx)
	: id(++ID_COUNTER)
	, observer(obs)
	, streamContext(streamCtx)
	, sessionContext(sessionCtx)
	, isLockOwner(false)
{
	this->request.CopyFrom(req);
}

std::shared_ptr<lock_acquisition_stream_t> lock_acquisition_stream_t::create(
	const SvPb::LockAcquisitionStreamRequest& request,
	const SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>& observer,
	const SvRpc::ServerStreamContext::Ptr& streamContext,
	const SvAuth::SessionContext& sessionContext)
{
	return std::make_shared<lock_acquisition_stream_t>(request, observer, streamContext, sessionContext);
}
