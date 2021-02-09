//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Pure virtual interface that allows plugging either the real shared memory
/// access class or a mock which returns dummy data only.
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVProtoBuf/SVRC.h"
#include "SVProtobuf/SVAuth.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#include "SVRPCLibrary/Task.h"
#include "SVSharedMemoryLibrary/ShareControl.h"
#pragma endregion Includes

namespace SvOgw
{

class SharedMemoryAccessInterface
{
public:
	virtual ~SharedMemoryAccessInterface() {}

	virtual void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) = 0;
	virtual void GetWebAppVersion(const SvPb::GetWebAppVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) = 0;
	virtual void GetInspections(const SvPb::GetInspectionsRequest&, SvRpc::Task<SvPb::GetInspectionsResponse>) = 0;
	virtual void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>) = 0;
	virtual void GetProductData(const SvPb::GetProductDataRequest&, SvRpc::Task<SvPb::GetProductDataResponse>) = 0;
	virtual void GetReject(const SvPb::GetRejectRequest&, SvRpc::Task<SvPb::GetRejectResponse>) = 0;
	virtual void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>) = 0;
	virtual void GetImageFromId(const SvPb::GetImageFromIdRequest&, SvRpc::Task<SvPb::GetImageFromIdResponse>) = 0;
	virtual void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>) = 0;
	virtual void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>) = 0;
	virtual void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>) = 0;
	virtual void StoreClientLogs(const SvPb::StoreClientLogsRequest&, SvRpc::Task<SvPb::EmptyResponse>) = 0;
	virtual void SetRejectStreamPauseState(const SvPb::SetRejectStreamPauseStateRequest&, SvRpc::Task<SvPb::EmptyResponse>) = 0;
	virtual void GetGatewayNotificationStream(const SvPb::GetGatewayNotificationStreamRequest&, SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse>, SvRpc::ServerStreamContext::Ptr) = 0;
	virtual void GetProductStream(const SvPb::GetProductStreamRequest&, SvRpc::Observer<SvPb::GetProductStreamResponse>, SvRpc::ServerStreamContext::Ptr) = 0;

	virtual void GetMyPermissions(const SvAuth::SessionContext&, const SvPb::GetMyPermissionsRequest&, SvRpc::Task<SvPb::GetMyPermissionsResponse>) = 0;
	virtual void GetGroupDetails(const SvAuth::SessionContext&, const SvPb::GetGroupDetailsRequest&, SvRpc::Task<SvPb::GetGroupDetailsResponse>) = 0;
	virtual void UpdateGroupPermissions(const SvAuth::SessionContext&, const SvPb::UpdateGroupPermissionsRequest&, SvRpc::Task<SvPb::UpdateGroupPermissionsResponse>) = 0;

	virtual bool CheckRequestPermissions(const SvAuth::SessionContext&, const SvPenv::Envelope&, SvRpc::Task<SvPenv::Envelope>) = 0;
	virtual bool CheckStreamPermissions(const SvAuth::SessionContext&, const SvPenv::Envelope&, SvRpc::Observer<SvPenv::Envelope>, SvRpc::ServerStreamContext::Ptr) = 0;
};
}// namespace SvOgw
