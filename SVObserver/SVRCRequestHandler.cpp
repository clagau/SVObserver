//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCRequestHandler.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Request handler for all the SVRC commands processed by SVObserver
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRCRequestHandler.h"
#include "SVRCCommand.h"
#include "SVProtoBuf\SVRC.h"
#pragma endregion Includes


SVRCRequestHandler::~SVRCRequestHandler()
{ 
	m_IoRunService.stop();
	if (m_IoThread.joinable())
	{
		m_IoThread.join();
	}
}


SVRCRequestHandler::SVRCRequestHandler(SVRCCommand* pCommand) :
	m_IoRunService(),
	m_IoWork(m_IoRunService),
	m_IoThread(boost::bind(&boost::asio::io_service::run, &m_IoRunService))
{
	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetSVObserverVersionRequest,
		SvPb::GetSVObserverVersionRequest,
		SvPb::GetVersionResponse>(
		[this, pCommand](SvPb::GetSVObserverVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetVersion(req, task); });
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetDeviceModeRequest,
		SvPb::GetDeviceModeRequest,
		SvPb::GetDeviceModeResponse>(
		[this, pCommand](SvPb::GetDeviceModeRequest&& req, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetDeviceMode(req, task); });
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetDeviceModeRequest,
		SvPb::SetDeviceModeRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::SetDeviceModeRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->SetDeviceMode(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetStateRequest,
		SvPb::GetStateRequest,
		SvPb::GetStateResponse>(
		[this, pCommand](SvPb::GetStateRequest&& req, SvRpc::Task<SvPb::GetStateResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetState(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetConfigRequest,
		SvPb::GetConfigRequest,
		SvPb::GetConfigResponse>(
		[this, pCommand](SvPb::GetConfigRequest&& req, SvRpc::Task<SvPb::GetConfigResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetConfig(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kPutConfigRequest,
		SvPb::PutConfigRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::PutConfigRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->PutConfig(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetOfflineCountRequest,
		SvPb::GetOfflineCountRequest,
		SvPb::GetOfflineCountResponse>(
		[this, pCommand](SvPb::GetOfflineCountRequest&& req, SvRpc::Task<SvPb::GetOfflineCountResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetOfflineCount(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kActivateMontorListRequest,
		SvPb::ActivateMonitorListRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::ActivateMonitorListRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->ActivateMonitorList(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetProductFilterRequest,
		SvPb::GetProductFilterRequest,
		SvPb::GetProductFilterResponse>(
		[this, pCommand](SvPb::GetProductFilterRequest&& req, SvRpc::Task<SvPb::GetProductFilterResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetProductFilter(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetProductFilterRequest,
		SvPb::SetProductFilterRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::SetProductFilterRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->SetProductFilter(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetItemsRequest,
		SvPb::GetItemsRequest,
		SvPb::GetItemsResponse>(
		[this, pCommand](SvPb::GetItemsRequest&& req, SvRpc::Task<SvPb::GetItemsResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetItems(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetItemsRequest,
		SvPb::SetItemsRequest,
		SvPb::SetItemsResponse>(
		[this, pCommand](SvPb::SetItemsRequest&& req, SvRpc::Task<SvPb::SetItemsResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->SetItems(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetFileRequest,
		SvPb::GetFileRequest,
		SvPb::GetFileResponse>(
		[this, pCommand](SvPb::GetFileRequest&& req, SvRpc::Task<SvPb::GetFileResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetFile(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kPutFileRequest,
		SvPb::PutFileRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::PutFileRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->PutFile(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kRegisterMonitorListRequest,
		SvPb::RegisterMonitorListRequest,
		SvPb::RegisterMonitorListResponse>(
		[this, pCommand](SvPb::RegisterMonitorListRequest&& req, SvRpc::Task<SvPb::RegisterMonitorListResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->RegisterMonitorList(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetInspectionNamesRequest,
		SvPb::GetInspectionNamesRequest,
		SvPb::NamesResponse>(
		[this, pCommand](SvPb::GetInspectionNamesRequest&& req, SvRpc::Task<SvPb::NamesResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetInspectionNames(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kShutdownRequest,
		SvPb::ShutdownRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::ShutdownRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->Shutdown(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetMonitorListPropertiesRequest,
		SvPb::GetMonitorListPropertiesRequest,
		SvPb::GetMonitorListPropertiesResponse>(
		[this, pCommand](SvPb::GetMonitorListPropertiesRequest&& req, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetMonitorListProperties(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetMaxRejectDepthRequest,
		SvPb::GetMaxRejectDepthRequest,
		SvPb::GetMaxRejectDepthResponse>(
		[this, pCommand](SvPb::GetMaxRejectDepthRequest&& req, SvRpc::Task<SvPb::GetMaxRejectDepthResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetMaxRejectDepth(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetConfigReportRequest,
		SvPb::GetConfigReportRequest,
		SvPb::GetConfigReportResponse>(
		[this, pCommand](SvPb::GetConfigReportRequest&& req, SvRpc::Task<SvPb::GetConfigReportResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetConfigReport(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetDataDefinitionListRequest,
		SvPb::GetDataDefinitionListRequest,
		SvPb::GetDataDefinitionListResponse>(
		[this, pCommand](SvPb::GetDataDefinitionListRequest&& req, SvRpc::Task<SvPb::GetDataDefinitionListResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetDataDefinitionList(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kQueryMonitorListRequest,
		SvPb::QueryMonitorListRequest,
		SvPb::NamesResponse>(
		[this, pCommand](SvPb::QueryMonitorListRequest&& req, SvRpc::Task<SvPb::NamesResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->QueryMonitorList(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kQueryMonitorListNamesRequest,
		SvPb::QueryMonitorListNamesRequest,
		SvPb::NamesResponse>(
		[this, pCommand](SvPb::QueryMonitorListNamesRequest&& req, SvRpc::Task<SvPb::NamesResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->QueryMonitorListNames(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kRunOnceRequest,
		SvPb::RunOnceRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::RunOnceRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->RunOnce(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kLoadConfigRequest,
		SvPb::LoadConfigRequest,
		SvPb::StandardResponse>(
		[this, pCommand](SvPb::LoadConfigRequest&& req, SvRpc::Task<SvPb::StandardResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->LoadConfig(req, task); });

	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetObjectSelectorItemsRequest,
		SvPb::GetObjectSelectorItemsRequest,
		SvPb::GetObjectSelectorItemsResponse>(
		[this, pCommand](SvPb::GetObjectSelectorItemsRequest&& req, SvRpc::Task<SvPb::GetObjectSelectorItemsResponse> task)
	{
		m_IoRunService.post([req, task, pCommand]() { pCommand->GetObjectSelectorItems(req, task); });

	});

	registerStreamHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetNotificationStreamRequest,
		SvPb::GetNotificationStreamRequest,
		SvPb::GetNotificationStreamResponse>(
		[this,pCommand](SvPb::GetNotificationStreamRequest&& req, SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer, SvRpc::ServerStreamContext::Ptr ctx)
	{
		pCommand->RegisterNotificationStream(&m_IoRunService,req, observer,ctx);
	});
}



