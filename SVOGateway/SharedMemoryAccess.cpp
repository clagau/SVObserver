//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.cpp
/// All Rights Reserved
//******************************************************************************
/// Mapping from incoming RPC requests to shared memory wrapper.
//******************************************************************************

#include "stdafx.h"

//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <queue>

#include "SharedMemoryAccess.h"

#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"

namespace SvOgw
{
SharedMemoryAccess::SharedMemoryAccess()
	: m_io_service(), m_io_work(m_io_service), m_io_thread(boost::bind(&boost::asio::io_service::run, &m_io_service))
{

	m_pShareControlInstance = std::make_unique<SvSml::ShareControl>();

}
SharedMemoryAccess::~SharedMemoryAccess()
{
	if (!m_io_service.stopped())
	{
		m_io_service.stop();
	}
	if (m_io_thread.joinable())
	{
		m_io_thread.join();
	}
}

void SharedMemoryAccess::GetVersion(const SvPb::GetGatewayVersionRequest& rRequest, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;
	Response.set_version(SvSyl::SVVersionInfo::GetVersion());
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetProduct(const SvPb::GetProductRequest& rRequest, SvRpc::Task<SvPb::GetProductResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetProductResponse Response;

	if (m_pShareControlInstance->GetProduct(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::GetReject(const SvPb::GetRejectRequest& rRequest, SvRpc::Task<SvPb::GetRejectResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetRejectResponse Response;

	if (m_pShareControlInstance->GetReject(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::GetFailstatus(const SvPb::GetFailStatusRequest& rRequest, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetFailStatusResponse Response;
	if (m_pShareControlInstance->GetFailstatus(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}

}

void SharedMemoryAccess::GetImageFromId(const SvPb::GetImageFromIdRequest& rRequest,
	SvRpc::Task<SvPb::GetImageFromIdResponse> task)
{
	SvPb::GetImageFromIdResponse Response;
	SvPenv::Error Error;

	if (m_pShareControlInstance->GetImageFromId(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}

}
void SharedMemoryAccess::GetImageStreamFromId(const SvPb::GetImageStreamFromIdRequest& rRequest,
	SvRpc::Observer<SvPb::GetImageStreamFromIdResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	SvPenv::Error Error;
	Error.set_error_code(SvPenv::ErrorCode::NotImplemented);
	observer.error(Error);
}
void SharedMemoryAccess::GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
{
	SvPenv::Error Error;
	Error.set_error_code(SvPenv::ErrorCode::NotImplemented);
	task.error(Error);
}

void SharedMemoryAccess::QueryListName(const SvPb::QueryListNameRequest& rRequest, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	SvPb::QueryListNameResponse Response;
	SvPenv::Error Error;

	if (m_pShareControlInstance->QueryListName(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}
void SharedMemoryAccess::QueryListItem(const SvPb::QueryListItemRequest& rRequest, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	SvPb::QueryListItemResponse Response;
	SvPenv::Error Error;
	if (m_pShareControlInstance->QueryListItem(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

}// namespace SvOgw
