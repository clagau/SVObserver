//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ServerRequestHandler.cpp
/// All Rights Reserved
//******************************************************************************
/// Glue code for unwrapping incoming request envelopes, calling the according
/// function in the shared memory access and wrapping the response into an
/// envelope again.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVRPCLibrary/ErrorUtil.h"
#include "SharedMemoryAccess.h"
#include "ServerRequestHandler.h"
#pragma endregion Includes

namespace SvOgw
{
ServerRequestHandler::ServerRequestHandler(SharedMemoryAccessInterface* sma)
{
	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetGatewayVersionRequest,
		SvPb::GetGatewayVersionRequest,
		SvPb::GetVersionResponse>(
		[sma](SvPb::GetGatewayVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
	{
		sma->GetVersion(req, task);
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

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetRejectRequest,
		SvPb::GetRejectRequest,
		SvPb::GetRejectResponse>(
		[sma](SvPb::GetRejectRequest&& req, SvRpc::Task<SvPb::GetRejectResponse> task)
	{
		sma->GetReject(req, task);
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
		SvPb::SVRCMessages::kGetImageFromIdRequest,
		SvPb::GetImageFromIdRequest,
		SvPb::GetImageFromIdResponse>(
		[sma](SvPb::GetImageFromIdRequest&& req, SvRpc::Task<SvPb::GetImageFromIdResponse> task)
	{
		sma->GetImageFromId(req, task);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetImageStreamFromIdRequest,
		SvPb::GetImageStreamFromIdRequest,
		SvPb::GetImageStreamFromIdResponse>(
		[sma](SvPb::GetImageStreamFromIdRequest&& req, SvRpc::Observer<SvPb::GetImageStreamFromIdResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetImageStreamFromId(req, observer, ctx);
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
		SvPb::SVRCMessages::kGetTriggerItemsRequest,
		SvPb::GetTriggerItemsRequest,
		SvPb::GetTriggerItemsResponse>(
		[sma](SvPb::GetTriggerItemsRequest&& req, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
	{
		sma->GetTriggerItems(req, task);
	});


}

}
