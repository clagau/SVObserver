//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RRSSharedMemoryAccess.cpp
/// All Rights Reserved
//******************************************************************************
/// Mapping from incoming RPC requests to shared memory wrapper.
//******************************************************************************

#include "stdafx.h"

#include <future>
#include <queue>

#include "WebsocketServer/RRSSharedMemoryAccess.h"

#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"

namespace RRWS
{
RRSSharedMemoryAccess::RRSSharedMemoryAccess()
	: m_io_service(), m_io_work(m_io_service), m_io_thread(boost::bind(&boost::asio::io_service::run, &m_io_service))
{
	m_pShareControl = std::make_unique<SvSml::ShareControl>();
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(
		boost::bind(&SvSml::ShareControl::EventHandler, boost::ref((*m_pShareControl.get())), _1));
	SvSml::ShareEvents::GetInstance().StartWatch();
}
RRSSharedMemoryAccess::~RRSSharedMemoryAccess()
{
	SvSml::ShareEvents::GetInstance().StopWatch();
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(0);
}

void RRSSharedMemoryAccess::GetVersion(const GetVersionRequest& req, SVRPC::Task<GetVersionResponse> task)
{
	GetVersionResponse resp;
	resp.set_version(SvSyl::SVVersionInfo::GetVersion());
	task.finish(std::move(resp));
}

void RRSSharedMemoryAccess::GetProduct(const GetProductRequest& request, SVRPC::Task<GetProductResponse> task)
{
	SVRPC::Error err;
	GetProductResponse resp;

	if (m_pShareControl->GetProductOrReject(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}
}

void RRSSharedMemoryAccess::GetFailstatus(const GetFailStatusRequest& request, SVRPC::Task<GetFailStatusResponse> task)
{
	SVRPC::Error err;
	GetFailStatusResponse resp;
	if (m_pShareControl->GetFailstatus(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}

}

void RRSSharedMemoryAccess::GetImageFromCurId(const GetImageFromCurIdRequest& request,
	SVRPC::Task<GetImageFromCurIdResponse> task)
{
	GetImageFromCurIdResponse resp;
	SVRPC::Error err;

	if (m_pShareControl->GetImageFromCurId(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}

}
void RRSSharedMemoryAccess::GetImageStreamFromCurId(const GetImageStreamFromCurIdRequest& req,
	SVRPC::Observer<GetImageStreamFromCurIdResponse> observer)
{
	SVRPC::Error err;
	err.set_error_code(SVRPC::ErrorCode::NotImplemented);
	observer.error(err);
}
void RRSSharedMemoryAccess::GetItems(const GetItemsRequest&, SVRPC::Task<GetItemsResponse> task)
{
	SVRPC::Error err;
	err.set_error_code(SVRPC::ErrorCode::NotImplemented);
	task.error(err);
}
void RRSSharedMemoryAccess::QueryListName(const QueryListNameRequest& request, SVRPC::Task<QueryListNameResponse> task)
{
	QueryListNameResponse resp;
	SVRPC::Error err;

	if (m_pShareControl->QueryListName(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}
}
void RRSSharedMemoryAccess::QueryListItem(const QueryListItemRequest& request, SVRPC::Task<QueryListItemResponse> task)
{
	QueryListItemResponse Resp;
	SVRPC::Error err;
	if (m_pShareControl->QueryListItem(request, Resp, err))
	{
		task.finish(std::move(Resp));
	}
	else
	{
		task.error(err);
	}
}
}
