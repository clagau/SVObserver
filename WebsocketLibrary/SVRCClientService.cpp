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

using time_duration = boost::posix_time::time_duration;
const  time_duration cSixSeconds = boost::posix_time::seconds(6);
const  time_duration cTwoMinutes = boost::posix_time::minutes(2);
const  time_duration cFiveMinutes = boost::posix_time::minutes(5);

namespace SvWsl
{

SVRCClientService::SVRCClientService(SvRpc::RPCClient& rRpcClient) :
	m_GetVersionClient(rRpcClient),
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
	m_QueryMonitorListNamesClient(rRpcClient)
	{

}


SVRCClientService::~SVRCClientService()
{
}


void SVRCClientService::GetVersion(SvPb::GetVersionRequest&& Request, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	m_GetVersionClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetDeviceMode(SvPb::GetDeviceModeRequest&& Request, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
{
	m_GetDeviceModeClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::SetDeviceMode(SvPb::SetDeviceModeRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_SetDeviceModeClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::GetState(SvPb::GetStateRequest&& Request, SvRpc::Task<SvPb::GetStateResponse> task)
{
	m_GetStateClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetConfig(SvPb::GetConfigRequest&& Request, SvRpc::Task<SvPb::GetConfigResponse> task)
{
	m_GetConfigClient.request(std::move(Request), task, cFiveMinutes);
}

void SVRCClientService::PutConfig(SvPb::PutConfigRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_PutConfigClient.request(std::move(Request), task, cFiveMinutes);
}

void SVRCClientService::GetOfflineCount(SvPb::GetOfflineCountRequest&& Request, SvRpc::Task<SvPb::GetOfflineCountResponse> task)
{
	m_GetOfflineCountClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::ActivateMonitorList(SvPb::ActivateMonitorListRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_ActivateMonitorListClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetProductFilter(SvPb::GetProductFilterRequest&& Request, SvRpc::Task<SvPb::GetProductFilterResponse> task)
{
	m_GetProductFilterClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::SetProductFilter(SvPb::SetProductFilterRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_SetProductFilterClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetItems(SvPb::GetItemsRequest&& Request, SvRpc::Task<SvPb::GetItemsResponse> task)
{
	m_GetItemsClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::SetItems(SvPb::SetItemsRequest&& Request, SvRpc::Task<SvPb::SetItemsResponse> task)
{
	m_SetItemsClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::GetFile(SvPb::GetFileRequest&& Request, SvRpc::Task<SvPb::GetFileResponse> task)
{
	m_GetFileClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::PutFile(SvPb::PutFileRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_PutFileClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::RegisterMonitorList(SvPb::RegisterMonitorListRequest&& Request, SvRpc::Task<SvPb::RegisterMonitorListResponse> task)
{
	m_RegisterMontorListClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::GetInspectionNames(SvPb::GetInspectionNamesRequest&& Request, SvRpc::Task<SvPb::NamesResponse> task)
{
	m_GetInspectionNamesClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::Shutdown(SvPb::ShutdownRequest&& Request, SvRpc::Task<SvPb::StandardResponse> task)
{
	m_ShutdownClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::GetMonitorListProperties(SvPb::GetMonitorListPropertiesRequest&& Request, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse> task)
{
	m_GetMonitorListPropertiesClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetMaxRejectDepth(SvPb::GetMaxRejectDepthRequest&& Request, SvRpc::Task<SvPb::GetMaxRejectDepthResponse> task)
{
	m_GetMaxRejectDepthClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetConfigReport(SvPb::GetConfigReportRequest&& Request, SvRpc::Task<SvPb::GetConfigReportResponse> task)
{
	m_GetConfigReportClient.request(std::move(Request), task, cSixSeconds);
}

void SVRCClientService::GetDataDefinitionList(SvPb::GetDataDefinitionListRequest&& Request, SvRpc::Task<SvPb::GetDataDefinitionListResponse> task)
{
	m_GetDataDefinitionListClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::QueryMonitorList(SvPb::QueryMonitorListRequest&& Request, SvRpc::Task<SvPb::NamesResponse> task)
{
	m_QueryMonitorListClient.request(std::move(Request), task, cTwoMinutes);
}

void SVRCClientService::QueryMonitorListNames(SvPb::QueryMonitorListNamesRequest&& Request, SvRpc::Task<SvPb::NamesResponse> task)
{
	m_QueryMonitorListNamesClient.request(std::move(Request), task, cTwoMinutes);
}

}//namespace SvWsl