//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.cpp
/// All Rights Reserved
//******************************************************************************
/// Mapping from incoming RPC requests to shared memory wrapper.
//******************************************************************************

#include "stdafx.h"

#include <future>
#include <queue>

#include "SharedMemoryAccess.h"

#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVObserverAccess/SVObserverAccess.h"
namespace SvOws
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

void SharedMemoryAccess::GetVersion(const SvPb::GetVersionRequest& req, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse resp;
	resp.set_version(SvSyl::SVVersionInfo::GetVersion());
	task.finish(std::move(resp));
}

void SharedMemoryAccess::GetProduct(const SvPb::GetProductRequest& request, SvRpc::Task<SvPb::GetProductResponse> task)
{
	SvPenv::Error err;
	SvPb::GetProductResponse resp;

	if (m_pShareControlInstance->GetProductOrReject(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}
}

void SharedMemoryAccess::GetFailstatus(const SvPb::GetFailStatusRequest& request, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	SvPenv::Error err;
	SvPb::GetFailStatusResponse resp;
	if (m_pShareControlInstance->GetFailstatus(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}

}

void SharedMemoryAccess::GetImageFromCurId(const SvPb::GetImageFromCurIdRequest& request,
	SvRpc::Task<SvPb::GetImageFromCurIdResponse> task)
{
	SvPb::GetImageFromCurIdResponse resp;
	SvPenv::Error err;

	if (m_pShareControlInstance->GetImageFromCurId(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}

}
void SharedMemoryAccess::GetImageStreamFromCurId(const SvPb::GetImageStreamFromCurIdRequest& req,
	SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	SvPenv::Error err;
	err.set_error_code(SvPenv::ErrorCode::NotImplemented);
	observer.error(err);
}
void SharedMemoryAccess::GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
{
	SvPenv::Error err;
	err.set_error_code(SvPenv::ErrorCode::NotImplemented);
	task.error(err);
}

void SharedMemoryAccess::QueryListName(const SvPb::QueryListNameRequest& request, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	SvPb::QueryListNameResponse resp;
	SvPenv::Error err;

	if (m_pShareControlInstance->QueryListName(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}
}
void SharedMemoryAccess::QueryListItem(const SvPb::QueryListItemRequest& request, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	SvPb::QueryListItemResponse Resp;
	SvPenv::Error err;
	if (m_pShareControlInstance->QueryListItem(request, Resp, err))
	{
		task.finish(std::move(Resp));
	}
	else
	{
		task.error(err);
	}
}
void SharedMemoryAccess::GetNotificationStream(const SvPb::GetNotificationStreamRequest& request,
	SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	SvPenv::Error err;
	err.set_error_code(SvPenv::ErrorCode::NotImplemented);
	observer.error(err);
}
}
