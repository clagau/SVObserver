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
#include "ServerRequestHandler.h"
#include "SharedMemoryAccess.h"
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

namespace SvOgw
{
ServerRequestHandler::ServerRequestHandler(SharedMemoryAccess* sma, SvAuth::AuthManager* am)
{
	registerAuthHandler(
		[am](const std::string& token, SvAuth::SessionContext& rSessionContext) -> bool
	{
		return am->rpcAuth(token, rSessionContext);
	});

	registerRequestMiddleware([sma](const SvAuth::SessionContext& rSessionCtx, const SvPenv::Envelope& rEnvelope, SvRpc::Task<SvPenv::Envelope> task) -> bool
	{
		return sma->CheckRequestPermissions(rSessionCtx, rEnvelope, task);
	});

	registerStreamMiddleware([sma](const SvAuth::SessionContext& rSessionCtx, const SvPenv::Envelope& rEnvelope, SvRpc::Observer<SvPenv::Envelope> observer, SvRpc::ServerStreamContext::Ptr streamCtx) -> bool
	{
		return sma->CheckStreamPermissions(rSessionCtx, rEnvelope, observer, streamCtx);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetGatewayVersionRequest,
		SvPb::GetGatewayVersionRequest,
		SvPb::GetVersionResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetGatewayVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
	{
		sma->GetVersion(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetWebAppVersionRequest,
		SvPb::GetWebAppVersionRequest,
		SvPb::GetVersionResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetWebAppVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
	{
		sma->GetWebAppVersion(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetInspectionsRequest,
		SvPb::GetInspectionsRequest,
		SvPb::GetInspectionsResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetInspectionsRequest&& req, SvRpc::Task<SvPb::GetInspectionsResponse> task)
	{
		sma->GetInspections(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetProductRequest,
		SvPb::GetProductRequest,
		SvPb::GetProductResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task)
	{
		sma->GetProduct(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetProductDataRequest,
		SvPb::GetProductDataRequest,
		SvPb::GetProductDataResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetProductDataRequest&& req, SvRpc::Task<SvPb::GetProductDataResponse> task)
	{
		sma->GetProductData(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetRejectRequest,
		SvPb::GetRejectRequest,
		SvPb::GetRejectResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetRejectRequest&& req, SvRpc::Task<SvPb::GetRejectResponse> task)
	{
		sma->GetReject(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetFailStatusRequest,
		SvPb::GetFailStatusRequest,
		SvPb::GetFailStatusResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task)
	{
		sma->GetFailstatus(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetImageFromIdRequest,
		SvPb::GetImageFromIdRequest,
		SvPb::GetImageFromIdResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetImageFromIdRequest&& req, SvRpc::Task<SvPb::GetImageFromIdResponse> task)
	{
		sma->GetImageFromId(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetRejectStreamPauseStateRequest,
		SvPb::SetRejectStreamPauseStateRequest,
		SvPb::EmptyResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::SetRejectStreamPauseStateRequest&& req, SvRpc::Task<SvPb::EmptyResponse> task)
	{
		sma->SetRejectStreamPauseState(req, task);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetGatewayNotificationStreamRequest,
		SvPb::GetGatewayNotificationStreamRequest,
		SvPb::GetGatewayNotificationStreamResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetGatewayNotificationStreamRequest&& req, SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetGatewayNotificationStream(req, observer, ctx);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetGatewayMessageStreamRequest,
		SvPb::GetGatewayMessageStreamRequest,
		SvPb::GetGatewayMessageStreamResponse>(
			[sma](const SvAuth::SessionContext&, SvPb::GetGatewayMessageStreamRequest&& req, SvRpc::Observer<SvPb::GetGatewayMessageStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
			{
				sma->GetGatewayMessageStream(req, observer, ctx);
			});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetProductStreamRequest,
		SvPb::GetProductStreamRequest,
		SvPb::GetProductStreamResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetProductStreamRequest&& req, SvRpc::Observer<SvPb::GetProductStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		sma->GetProductStream(req, observer, ctx);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kQueryListNameRequest,
		SvPb::QueryListNameRequest,
		SvPb::QueryListNameResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task)
	{
		sma->QueryListName(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kQueryListItemRequest,
		SvPb::QueryListItemRequest,
		SvPb::QueryListItemResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task)
	{
		sma->QueryListItem(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetTriggerItemsRequest,
		SvPb::GetTriggerItemsRequest,
		SvPb::GetTriggerItemsResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::GetTriggerItemsRequest&& req, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
	{
		sma->GetTriggerItems(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kStoreClientLogsRequest,
		SvPb::StoreClientLogsRequest,
		SvPb::EmptyResponse>(
		[sma](const SvAuth::SessionContext&, SvPb::StoreClientLogsRequest&& req, SvRpc::Task<SvPb::EmptyResponse> task)
	{
		sma->StoreClientLogs(req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetMyPermissionsRequest,
		SvPb::GetMyPermissionsRequest,
		SvPb::GetMyPermissionsResponse>(
		[sma](const SvAuth::SessionContext& rSessionContext, SvPb::GetMyPermissionsRequest&& req, SvRpc::Task<SvPb::GetMyPermissionsResponse> task)
	{
		sma->GetMyPermissions(rSessionContext, req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetGroupDetailsRequest,
		SvPb::GetGroupDetailsRequest,
		SvPb::GetGroupDetailsResponse>(
		[sma](const SvAuth::SessionContext& rSessionContext, SvPb::GetGroupDetailsRequest&& req, SvRpc::Task<SvPb::GetGroupDetailsResponse> task)
	{
		sma->GetGroupDetails(rSessionContext, req, task);
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kUpdateGroupPermissionsRequest,
		SvPb::UpdateGroupPermissionsRequest,
		SvPb::UpdateGroupPermissionsResponse>(
		[sma](const SvAuth::SessionContext& rSessionContext, SvPb::UpdateGroupPermissionsRequest&& req, SvRpc::Task<SvPb::UpdateGroupPermissionsResponse> task)
	{
		sma->UpdateGroupPermissions(rSessionContext, req, task);
	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kLockAcquisitionStreamRequest,
		SvPb::LockAcquisitionStreamRequest,
		SvPb::LockAcquisitionStreamResponse>([sma](
			const SvAuth::SessionContext& sessionContext,
			SvPb::LockAcquisitionStreamRequest&& request,
			SvRpc::Observer<SvPb::LockAcquisitionStreamResponse> observer,
			SvRpc::ServerStreamContext::Ptr streamContext)
			{
				sma->AcquireLockStream(sessionContext, request, observer, streamContext);
			});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kLockTakeoverRequest,
		SvPb::LockTakeoverRequest,
		SvPb::LockTakeoverResponse>([sma](
			const SvAuth::SessionContext& sessionContext,
			SvPb::LockTakeoverRequest&& request,
			SvRpc::Task<SvPb::LockTakeoverResponse> task)
			{
				sma->TakeoverLock(sessionContext, request, task);
			});
}

}
