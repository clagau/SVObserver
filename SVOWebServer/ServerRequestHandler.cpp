//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ServerRequestHandler.cpp
/// All Rights Reserved
//******************************************************************************
/// Glue code for unwrapping incoming request envelopes, calling the according
/// function in the shared memory access and wrapping the response into an
/// envelope again.
//******************************************************************************

#include "stdafx.h"

#include "SharedMemoryAccess.h"
#include "ServerRequestHandler.h"

#include <iostream>

#include <boost/log/trivial.hpp>
#include <boost/thread/future.hpp>

namespace SvOws
{
ServerRequestHandler::ServerRequestHandler(SharedMemoryAccessInterface* sma)
{
	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetVersionRequest,
		SvPb::GetVersionRequest,
		SvPb::GetVersionResponse>(
		[sma](SvPb::GetVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
	{
		sma->GetVersion(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kQueryListNameRequest,
		SvPb::QueryListNameRequest,
		SvPb::QueryListNameResponse>(
		[sma](SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task)
	{
		sma->QueryListName(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kQueryListItemRequest,
		SvPb::QueryListItemRequest,
		SvPb::QueryListItemResponse>(
		[sma](SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task)
	{
		sma->QueryListItem(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetProductRequest,
		SvPb::GetProductRequest,
		SvPb::GetProductResponse>(
		[sma](SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task)
	{
		sma->GetProduct(req, task);
	});

	//registerRequestHandler<
	//	RunRejectMessages,
	//	RunRejectMessages::kGetRejectRequest,
	//	GetRejectRequest,
	//	GetRejectResponse>(
	//	[sma](GetRejectRequest&& req, SvRpc::Task<GetRejectResponse> task)
	//{
	//	sma->GetReject(req, task);
	//});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetImageFromCurIdRequest,
		SvPb::GetImageFromCurIdRequest,
		SvPb::GetImageFromCurIdResponse>(
		[sma](SvPb::GetImageFromCurIdRequest&& req, SvRpc::Task<SvPb::GetImageFromCurIdResponse> task)
	{
		sma->GetImageFromCurId(req, task);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetImageStreamFromCurIdRequest,
		SvPb::GetImageStreamFromCurIdRequest,
		SvPb::GetImageStreamFromCurIdResponse>(
		[sma](SvPb::GetImageStreamFromCurIdRequest&& req, SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetImageStreamFromCurId(req, observer, ctx);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetFailStatusRequest,
		SvPb::GetFailStatusRequest,
		SvPb::GetFailStatusResponse>(
		[sma](SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task)
	{
		sma->GetFailstatus(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetItemsRequest,
		SvPb::GetItemsRequest,
		SvPb::GetItemsResponse>(
		[sma](SvPb::GetItemsRequest&& req, SvRpc::Task<SvPb::GetItemsResponse> task)
	{
		sma->GetItems(req, task);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetNotificationStreamRequest,
		SvPb::GetNotificationStreamRequest,
		SvPb::GetNotificationStreamResponse>(
		[sma](SvPb::GetNotificationStreamRequest&& req, SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetNotificationStream(req, observer, ctx);
	});

}
}
