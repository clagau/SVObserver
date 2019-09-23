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

#include "SVAuthLibrary/AuthManager.h"
#include "SharedMemoryAccess.h"
#include "ServerRequestHandler.h"
#pragma endregion Includes

namespace SvOgw
{
ServerRequestHandler::ServerRequestHandler(SharedMemoryAccessInterface* sma, SvAuth::AuthManager* am)
{
	registerAuthHandler(
		[am](const std::string& token) -> bool
	{
		return am->rpcAuth(token);
	});

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
		SvPb::SVRCMessages::kGetWebAppVersionRequest,
		SvPb::GetWebAppVersionRequest,
		SvPb::GetVersionResponse>(
		[sma](SvPb::GetWebAppVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
	{
		sma->GetWebAppVersion(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetInspectionsRequest,
		SvPb::GetInspectionsRequest,
		SvPb::GetInspectionsResponse>(
		[sma](SvPb::GetInspectionsRequest&& req, SvRpc::Task<SvPb::GetInspectionsResponse> task)
	{
		sma->GetInspections(req, task);
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
		SvPb::SVRCMessages::kGetProductDataRequest,
		SvPb::GetProductDataRequest,
		SvPb::GetProductDataResponse>(
		[sma](SvPb::GetProductDataRequest&& req, SvRpc::Task<SvPb::GetProductDataResponse> task)
	{
		sma->GetProductData(req, task);
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

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetRejectStreamPauseStateRequest,
		SvPb::SetRejectStreamPauseStateRequest,
		SvPb::EmptyResponse>(
		[sma](SvPb::SetRejectStreamPauseStateRequest&& req, SvRpc::Task<SvPb::EmptyResponse> task)
	{
		sma->SetRejectStreamPauseState(req, task);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetGatewayNotificationStreamRequest,
		SvPb::GetGatewayNotificationStreamRequest,
		SvPb::GetGatewayNotificationStreamResponse>(
		[sma](SvPb::GetGatewayNotificationStreamRequest&& req, SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetGatewayNotificationStream(req, observer, ctx);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetProductStreamRequest,
		SvPb::GetProductStreamRequest,
		SvPb::GetProductStreamResponse>(
		[sma](SvPb::GetProductStreamRequest&& req, SvRpc::Observer<SvPb::GetProductStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetProductStream(req, observer, ctx);
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

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kStoreClientLogsRequest,
		SvPb::StoreClientLogsRequest,
		SvPb::EmptyResponse>(
		[sma](SvPb::StoreClientLogsRequest&& req, SvRpc::Task<SvPb::EmptyResponse> task)
	{
		sma->StoreClientLogs(req, task);
	});
}

}
