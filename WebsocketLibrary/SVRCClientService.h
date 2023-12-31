//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file SVRCClientService.h
/// All Rights Reserved 
//*****************************************************************************
/// SVRC client (used by SVRemoteControl) to send commands to the SVObserver
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVRPCLibrary/Task.h"
#pragma endregion Includes

namespace SvRpc
{
class RPCCLient;
}

namespace SvWsl
{
struct SVRCClientServiceSetting;

class SVRCClientService
{
public:
	SVRCClientService(SvRpc::RPCClient& rRpcClient, SVRCClientServiceSetting&  rSVRCSetting);
	~SVRCClientService();

	void GetGatewayVersion(SvPb::GetGatewayVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task);
	void GetWebAppVersion(SvPb::GetWebAppVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task);
	void GetProduct(SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task);
	void GetReject(SvPb::GetRejectRequest&& req, SvRpc::Task<SvPb::GetRejectResponse> task);
	void GetFailStatus(SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task);
	void GetImageFromId(SvPb::GetImageFromIdRequest&& req, SvRpc::Task<SvPb::GetImageFromIdResponse> task);
	void QueryListName(SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task);
	void QueryListItem(SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task);
	SvRpc::ClientStreamContext GetNotificationStream(SvPb::GetNotificationStreamRequest&& req,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer);
	SvRpc::ClientStreamContext GetMessageStream(SvPb::GetMessageStreamRequest&& req,
		SvRpc::Observer<SvPb::GetMessageStreamResponse> observer);

	void GetSVObserverVersion(SvPb::GetSVObserverVersionRequest&&, SvRpc::Task<SvPb::GetVersionResponse> task);
	void GetDeviceMode(SvPb::GetDeviceModeRequest&&, SvRpc::Task<SvPb::GetDeviceModeResponse> task);
	void SetDeviceMode(SvPb::SetDeviceModeRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void GetState(SvPb::GetStateRequest&&, SvRpc::Task<SvPb::GetStateResponse> task);
	void GetConfig(SvPb::GetConfigRequest&&, SvRpc::Task<SvPb::GetConfigResponse> task);
	void PutConfig(SvPb::PutConfigRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void GetOfflineCount(SvPb::GetOfflineCountRequest&&, SvRpc::Task<SvPb::GetOfflineCountResponse> task);
	void ActivateMonitorList(SvPb::ActivateMonitorListRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void GetProductFilter(SvPb::GetProductFilterRequest&&, SvRpc::Task<SvPb::GetProductFilterResponse> task);
	void SetProductFilter(SvPb::SetProductFilterRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void GetItems(SvPb::GetItemsRequest&&, SvRpc::Task<SvPb::GetItemsResponse> task);
	void SetItems(SvPb::SetItemsRequest&&, SvRpc::Task<SvPb::SetItemsResponse> task);
	void GetFile(SvPb::GetFileRequest&&, SvRpc::Task<SvPb::GetFileResponse> task);
	void PutFile(SvPb::PutFileRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void RegisterMonitorList(SvPb::RegisterMonitorListRequest&&, SvRpc::Task<SvPb::RegisterMonitorListResponse> task);
	void GetInspectionNames(SvPb::GetInspectionNamesRequest&&, SvRpc::Task<SvPb::NamesResponse> task);
	void Shutdown(SvPb::ShutdownRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void GetMonitorListProperties(SvPb::GetMonitorListPropertiesRequest&&, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse> task);
	void GetMaxRejectDepth(SvPb::GetMaxRejectDepthRequest&&, SvRpc::Task<SvPb::GetMaxRejectDepthResponse> task);
	void GetConfigReport(SvPb::GetConfigReportRequest&&, SvRpc::Task<SvPb::GetConfigReportResponse> task);
	void GetDataDefinitionList(SvPb::GetDataDefinitionListRequest&&, SvRpc::Task<SvPb::GetDataDefinitionListResponse> task);
	void QueryMonitorList(SvPb::QueryMonitorListRequest&&, SvRpc::Task<SvPb::NamesResponse> task);
	void QueryMonitorListNames(SvPb::QueryMonitorListNamesRequest&&, SvRpc::Task<SvPb::NamesResponse> task);
	void RunOnce(SvPb::RunOnceRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void LoadConfig(SvPb::LoadConfigRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void SetTriggerConfig(SvPb::SetTriggerConfigRequest&&, SvRpc::Task<SvPb::StandardResponse> task);
	void GetConfigurationInfo(SvPb::GetConfigurationInfoRequest&&, SvRpc::Task<SvPb::GetConfigurationInfoResponse> );
	void SoftwareTrigger(SvPb::SoftwareTriggerRequest&&, SvRpc::Task<SvPb::StandardResponse> task);

private:
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetGatewayVersionRequest, SvPb::GetVersionResponse> m_GetGatewayVersionClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetWebAppVersionRequest, SvPb::GetVersionResponse> m_GetWebAppVersionClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::QueryListNameRequest, SvPb::QueryListNameResponse> m_QueryListNameClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::QueryListItemRequest, SvPb::QueryListItemResponse> m_QueryListItemClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetProductRequest, SvPb::GetProductResponse> m_GetProductClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetRejectRequest, SvPb::GetRejectResponse> m_GetRejectClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetImageFromIdRequest, SvPb::GetImageFromIdResponse> m_GetImageFromIdClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetFailStatusRequest, SvPb::GetFailStatusResponse> m_GetFailStatusClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetNotificationStreamRequest, SvPb::GetNotificationStreamResponse> m_GetNotificationStreamClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetMessageStreamRequest, SvPb::GetMessageStreamResponse> m_GetMessageStreamClient;

	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetSVObserverVersionRequest, SvPb::GetVersionResponse> m_GetSVObserverVersionClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetDeviceModeRequest, SvPb::GetDeviceModeResponse> m_GetDeviceModeClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetDeviceModeRequest, SvPb::StandardResponse> m_SetDeviceModeClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetStateRequest, SvPb::GetStateResponse> m_GetStateClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigRequest, SvPb::GetConfigResponse> m_GetConfigClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::PutConfigRequest, SvPb::StandardResponse> m_PutConfigClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetOfflineCountRequest, SvPb::GetOfflineCountResponse> m_GetOfflineCountClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ActivateMonitorListRequest, SvPb::StandardResponse> m_ActivateMonitorListClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetProductFilterRequest, SvPb::GetProductFilterResponse> m_GetProductFilterClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetProductFilterRequest, SvPb::StandardResponse> m_SetProductFilterClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetItemsRequest, SvPb::GetItemsResponse> m_GetItemsClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetItemsRequest, SvPb::SetItemsResponse> m_SetItemsClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetFileRequest, SvPb::GetFileResponse> m_GetFileClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::PutFileRequest, SvPb::StandardResponse> m_PutFileClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::RegisterMonitorListRequest, SvPb::RegisterMonitorListResponse> m_RegisterMontorListClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetInspectionNamesRequest, SvPb::NamesResponse> m_GetInspectionNamesClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ShutdownRequest, SvPb::StandardResponse> m_ShutdownClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetMonitorListPropertiesRequest, SvPb::GetMonitorListPropertiesResponse> m_GetMonitorListPropertiesClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetMaxRejectDepthRequest, SvPb::GetMaxRejectDepthResponse> m_GetMaxRejectDepthClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigReportRequest, SvPb::GetConfigReportResponse> m_GetConfigReportClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetDataDefinitionListRequest, SvPb::GetDataDefinitionListResponse> m_GetDataDefinitionListClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::QueryMonitorListRequest, SvPb::NamesResponse> m_QueryMonitorListClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::QueryMonitorListNamesRequest, SvPb::NamesResponse> m_QueryMonitorListNamesClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::RunOnceRequest, SvPb::StandardResponse> m_RunOnceClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::LoadConfigRequest, SvPb::StandardResponse> m_LoadConfigClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetTriggerConfigRequest, SvPb::StandardResponse> m_SetTriggerConfigClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigurationInfoRequest, SvPb::GetConfigurationInfoResponse> m_GetConfigurationInfoClient;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SoftwareTriggerRequest, SvPb::StandardResponse> m_SoftwareTriggerClient;
	SVRCClientServiceSetting&  m_rSVRCSettings;

};

using SVRCClientServicePtr = std::unique_ptr<SvWsl::SVRCClientService>;

}//namespace SvWsl
