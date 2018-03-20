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

	SvOa::Init();

}
SharedMemoryAccess::~SharedMemoryAccess()
{
	SvOa::Finish();
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

	if (SvOa::GetProductOrReject(request, resp, err))
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
	if (SvOa::GetFailstatus(request, resp, err))
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

	if (SvOa::GetImageFromCurId(request, resp, err))
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
void SharedMemoryAccess::GetItems(const SvPb::GetItemsRequest&, SvRpc::Task<SvPb::GetItemsResponse> task)
{
	SvPenv::Error err;
	err.set_error_code(SvPenv::ErrorCode::NotImplemented);
	task.error(err);
}
void SharedMemoryAccess::QueryListName(const SvPb::QueryListNameRequest& request, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	SvPb::QueryListNameResponse resp;
	SvPenv::Error err;

	if (SvOa::QueryListName(request, resp, err))
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
	if (SvOa::QueryListItem(request, Resp, err))
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
