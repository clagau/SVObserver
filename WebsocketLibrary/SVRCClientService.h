//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCClientService.h
/// All Rights Reserved 
//*****************************************************************************
/// SVRC client (used by SVRemoteControl) to send commands to the SVObserver
//******************************************************************************

#pragma once
#include "SVProtobuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/RPCCLient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVRPCLibrary/Task.h"
#pragma endregion Includes

namespace SvWsl
{
class SVRCClientService
{
public:
	SVRCClientService(SvRpc::RPCClient& rRpcClient);
	~SVRCClientService();
	void GetVersion(SvPb::GetVersionRequest&&, SvRpc::Task<SvPb::GetVersionResponse> task);
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

private:
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetVersionRequest, SvPb::GetVersionResponse> m_GetVersionClient;
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
};

}//namespace SvWsl
