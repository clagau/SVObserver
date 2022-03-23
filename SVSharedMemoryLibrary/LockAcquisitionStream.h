#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/asio/io_service.hpp>

#include "SVProtoBuf/SVAuth.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#pragma endregion Includes

struct lock_acquisition_stream_t
{
	lock_acquisition_stream_t(
		const SvPb::LockAcquisitionStreamRequest&,
		const SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>&,
		const SvRpc::ServerStreamContext::Ptr&,
		const SvAuth::SessionContext&);

	static std::shared_ptr<lock_acquisition_stream_t> create(
		const SvPb::LockAcquisitionStreamRequest&,
		const SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>&,
		const SvRpc::ServerStreamContext::Ptr&,
		const SvAuth::SessionContext&);

	static std::uint32_t ID_COUNTER;

	std::uint32_t id;
	SvPb::LockAcquisitionStreamRequest request;
	SvRpc::Observer<SvPb::LockAcquisitionStreamResponse> observer;
	SvRpc::ServerStreamContext::Ptr streamContext;
	SvAuth::SessionContext sessionContext;
	bool isLockOwner;
};
