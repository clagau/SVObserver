//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCClientService.cpp
/// All Rights Reserved 
//*****************************************************************************
/// SVRC client (used by SVRemoteControl) to send commands to the SVObserver
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRCClientService.h"
#include "SVProtoBuf\SVRC.h"
#pragma endregion Includes

namespace SvWsl
{

SVRCClientService::SVRCClientService(SvRpc::RPCClient& rRpcClient, SVRCClientServiceSetting&  rSVRCSetting) :
	m_rSVRCSettings(rSVRCSetting),
	m_GetGatewayVersionClient(rRpcClient),
	m_GetProductClient(rRpcClient),
	m_GetRejectClient(rRpcClient),
	m_GetFailStatusClient(rRpcClient),
	m_GetImageFromIdClient(rRpcClient),
	m_QueryListNameClient(rRpcClient),
	m_QueryListItemClient(rRpcClient),
	m_GetNotificationStreamClient(rRpcClient),
	m_GetSVObserverVersionClient(rRpcClient),
	m_GetDeviceModeClient(rRpcClient),
	m_SetDeviceModeClient(rRpcClient),
	m_GetStateClient(rRpcClient),
	m_GetConfigClient(rRpcClient),
	m_PutConfigClient(rRpcClient),
	m_GetOfflineCountClient(rRpcClient),
	m_ActivateMonitorListClient(rRpcClient),
	m_GetProductFilterClient(rRpcClient),
	m_SetProductFilterClient(rRpcClient),
	m_GetItemsClient(rRpcClient),
	m_SetItemsClient(rRpcClient),
	m_GetFileClient(rRpcClient),
	m_PutFileClient(rRpcClient),
	m_RegisterMontorListClient(rRpcClient),
	m_GetInspectionNamesClient(rRpcClient),
	m_ShutdownClient(rRpcClient),
	m_GetMonitorListPropertiesClient(rRpcClient),
	m_GetMaxRejectDepthClient(rRpcClient),
	m_GetConfigReportClient(rRpcClient),
	m_GetDataDefinitionListClient(rRpcClient),
	m_QueryMonitorListClient(rRpcClient),
	m_QueryMonitorListNamesClient(rRpcClient),
	m_RunOnceClient(rRpcClient),
	m_LoadConfigClient(rRpcClient)
{

}


SVRCClientService::~SVRCClientService()
{
}

void SVRCClientService::GetGatewayVersion(SvPb::GetGatewayVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	m_GetGatewayVersionClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetProduct(SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task)
{
	m_GetProductClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetReject(SvPb::GetRejectRequest&& req, SvRpc::Task<SvPb::GetRejectResponse> task)
{
	m_GetRejectClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetFailStatus(SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	m_GetFailStatusClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetImageFromId(SvPb::GetImageFromIdRequest&& req, SvRpc::Task<SvPb::GetImageFromIdResponse> task)
{
	m_GetImageFromIdClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::QueryListName(SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	m_QueryListNameClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::QueryListItem(SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	m_QueryListItemClient.request(std::move(req), task, m_rSVRCSettings.ShortTimeout);
}

SvRpc::ClientStreamContext SVRCClientService::GetNotificationStream(SvPb::GetNotificationStreamRequest&& req, SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer)
{
	return m_GetNotificationStreamClient.stream(std::move(req), observer);
}


void SVRCClientService::GetSVObserverVersion(SvPb::GetSVObserverVersionRequest&& Request, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	m_GetSVObserverVersionClient.request(std::move(Request), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetDeviceMode(SvPb::GetDeviceModeRequest&& Request, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
{
	m_GetDeviceModeClient.request(std::move(Request), task, m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::SetDeviceMode(SvPb::SetDeviceModeRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_SetDeviceModeClient.request(std::move(Request), task, m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::GetState(SvPb::GetStateRequest&& Request, SvRpc::Task<SvPb::GetStateResponse> task)
{
	m_GetStateClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetConfig(SvPb::GetConfigRequest&& Request, SvRpc::Task<SvPb::GetConfigResponse> task)
{
	m_GetConfigClient.request(std::move(Request), task,  m_rSVRCSettings.LongTimeout);
}

void SVRCClientService::PutConfig(SvPb::PutConfigRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_PutConfigClient.request(std::move(Request), task,  m_rSVRCSettings.LongTimeout);
}

void SVRCClientService::GetOfflineCount(SvPb::GetOfflineCountRequest&& Request, SvRpc::Task<SvPb::GetOfflineCountResponse> task)
{
	m_GetOfflineCountClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::ActivateMonitorList(SvPb::ActivateMonitorListRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_ActivateMonitorListClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetProductFilter(SvPb::GetProductFilterRequest&& Request, SvRpc::Task<SvPb::GetProductFilterResponse> task)
{
	m_GetProductFilterClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::SetProductFilter(SvPb::SetProductFilterRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_SetProductFilterClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetItems(SvPb::GetItemsRequest&& Request, SvRpc::Task<SvPb::GetItemsResponse> task)
{
	m_GetItemsClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::SetItems(SvPb::SetItemsRequest&& Request, SvRpc::Task<SvPb::SetItemsResponse> task)
{
	m_SetItemsClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::GetFile(SvPb::GetFileRequest&& Request, SvRpc::Task<SvPb::GetFileResponse> task)
{
	m_GetFileClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::PutFile(SvPb::PutFileRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_PutFileClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::RegisterMonitorList(SvPb::RegisterMonitorListRequest&& Request, SvRpc::Task<SvPb::RegisterMonitorListResponse> task)
{
	m_RegisterMontorListClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::GetInspectionNames(SvPb::GetInspectionNamesRequest&& Request, SvRpc::Task<SvPb::NamesResponse> task)
{
	m_GetInspectionNamesClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::Shutdown(SvPb::ShutdownRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_ShutdownClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::GetMonitorListProperties(SvPb::GetMonitorListPropertiesRequest&& Request, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse> task)
{
	m_GetMonitorListPropertiesClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetMaxRejectDepth(SvPb::GetMaxRejectDepthRequest&& Request, SvRpc::Task<SvPb::GetMaxRejectDepthResponse> task)
{
	m_GetMaxRejectDepthClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetConfigReport(SvPb::GetConfigReportRequest&& Request, SvRpc::Task<SvPb::GetConfigReportResponse> task)
{
	m_GetConfigReportClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::GetDataDefinitionList(SvPb::GetDataDefinitionListRequest&& Request, SvRpc::Task<SvPb::GetDataDefinitionListResponse> task)
{
	m_GetDataDefinitionListClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::QueryMonitorList(SvPb::QueryMonitorListRequest&& Request, SvRpc::Task<SvPb::NamesResponse> task)
{
	m_QueryMonitorListClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::QueryMonitorListNames(SvPb::QueryMonitorListNamesRequest&& Request, SvRpc::Task<SvPb::NamesResponse> task)
{
	m_QueryMonitorListNamesClient.request(std::move(Request), task,  m_rSVRCSettings.MediumTimeout);
}

void SVRCClientService::RunOnce(SvPb::RunOnceRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_RunOnceClient.request(std::move(Request), task,  m_rSVRCSettings.ShortTimeout);
}

void SVRCClientService::LoadConfig(SvPb::LoadConfigRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_LoadConfigClient.request(std::move(Request), task,  m_rSVRCSettings.LongTimeout);
}
}//namespace SvWsl