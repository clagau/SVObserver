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

#include "WebsocketServer/RRSSharedMemoryAccess.h"
#include "WebsocketServer/ServerRequestHandler.h"

#include <iostream>

#include <boost/log/trivial.hpp>
#include <boost/thread/future.hpp>

namespace RRWS
{
ServerRequestHandler::ServerRequestHandler(RRSSharedMemoryAccessInterface* sma)
{
	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kGetVersionRequest,
		GetVersionRequest,
		GetVersionResponse>(
		[sma](GetVersionRequest&& req, SVRPC::Task<GetVersionResponse> task)
	{
		sma->GetVersion(req, task);
	});

	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kQueryListNameRequest,
		QueryListNameRequest,
		QueryListNameResponse>(
		[sma](QueryListNameRequest&& req, SVRPC::Task<QueryListNameResponse> task)
	{
		sma->QueryListName(req, task);
	});

	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kQueryListItemRequest,
		QueryListItemRequest,
		QueryListItemResponse>(
		[sma](QueryListItemRequest&& req, SVRPC::Task<QueryListItemResponse> task)
	{
		sma->QueryListItem(req, task);
	});

	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kGetProductRequest,
		GetProductRequest,
		GetProductResponse>(
		[sma](GetProductRequest&& req, SVRPC::Task<GetProductResponse> task)
	{
		sma->GetProduct(req, task);
	});

	//registerRequestHandler<
	//	RunRejectMessages,
	//	RunRejectMessages::kGetRejectRequest,
	//	GetRejectRequest,
	//	GetRejectResponse>(
	//	[sma](GetRejectRequest&& req, SVRPC::Task<GetRejectResponse> task)
	//{
	//	sma->GetReject(req, task);
	//});

	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kGetImageFromCurIdRequest,
		GetImageFromCurIdRequest,
		GetImageFromCurIdResponse>(
		[sma](GetImageFromCurIdRequest&& req, SVRPC::Task<GetImageFromCurIdResponse> task)
	{
		sma->GetImageFromCurId(req, task);
	});

	registerStreamHandler<
		RunRejectMessages,
		RunRejectMessages::kGetImageStreamFromCurIdRequest,
		GetImageStreamFromCurIdRequest,
		GetImageStreamFromCurIdResponse>(
		[sma](GetImageStreamFromCurIdRequest&& req, SVRPC::Observer<GetImageStreamFromCurIdResponse> observer)
	{
		sma->GetImageStreamFromCurId(req, observer);
	});

	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kGetFailStatusRequest,
		GetFailStatusRequest,
		GetFailStatusResponse>(
		[sma](GetFailStatusRequest&& req, SVRPC::Task<GetFailStatusResponse> task)
	{
		sma->GetFailstatus(req, task);
	});

	registerRequestHandler<
		RunRejectMessages,
		RunRejectMessages::kGetItemsRequest,
		GetItemsRequest,
		GetItemsResponse>(
		[sma](GetItemsRequest&& req, SVRPC::Task<GetItemsResponse> task)
	{
		sma->GetItems(req, task);
	});
}
}
