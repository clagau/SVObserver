//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCCommand.h
/// All Rights Reserved 
//*****************************************************************************
/// Mapping from incoming RPC requests to SVObserver
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVStorageResult.h"
#pragma endregion Includes

struct SVStorageResult;

class SVRCCommand 
{
public:
	SVRCCommand();
	~SVRCCommand();

	void GetVersion(const SvPb::GetSVObserverVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>);
	void GetDeviceMode(const SvPb::GetDeviceModeRequest&, SvRpc::Task<SvPb::GetDeviceModeResponse>);
	void SetDeviceMode(const SvPb::SetDeviceModeRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetState(const SvPb::GetStateRequest&, SvRpc::Task<SvPb::GetStateResponse>);
	void GetConfig(const SvPb::GetConfigRequest&, SvRpc::Task<SvPb::GetConfigResponse>);
	void PutConfig(const SvPb::PutConfigRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetOfflineCount(const SvPb::GetOfflineCountRequest&, SvRpc::Task<SvPb::GetOfflineCountResponse>);
	void ActivateMonitorList(const SvPb::ActivateMonitorListRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetProductFilter(const SvPb::GetProductFilterRequest&, SvRpc::Task<SvPb::GetProductFilterResponse>);
	void SetProductFilter(const SvPb::SetProductFilterRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetItems(const SvPb::GetItemsRequest&, SvRpc::Task<SvPb::GetItemsResponse>);
	void SetItems(const SvPb::SetItemsRequest&, SvRpc::Task<SvPb::SetItemsResponse>);
	void GetFile(const SvPb::GetFileRequest&, SvRpc::Task<SvPb::GetFileResponse>);
	void PutFile(const SvPb::PutFileRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void RegisterMonitorList(const SvPb::RegisterMonitorListRequest&, SvRpc::Task<SvPb::RegisterMonitorListResponse>);
	void GetInspectionNames(const SvPb::GetInspectionNamesRequest&, SvRpc::Task<SvPb::NamesResponse>);
	void Shutdown(const SvPb::ShutdownRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetMonitorListProperties(const SvPb::GetMonitorListPropertiesRequest&, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse>);
	void GetMaxRejectDepth(const SvPb::GetMaxRejectDepthRequest&, SvRpc::Task<SvPb::GetMaxRejectDepthResponse>);
	void GetConfigReport(const SvPb::GetConfigReportRequest&, SvRpc::Task<SvPb::GetConfigReportResponse>);
	void GetDataDefinitionList(const SvPb::GetDataDefinitionListRequest&, SvRpc::Task<SvPb::GetDataDefinitionListResponse>);
	void QueryMonitorList(const SvPb::QueryMonitorListRequest&, SvRpc::Task<SvPb::NamesResponse>);
	void QueryMonitorListNames(const SvPb::QueryMonitorListNamesRequest&, SvRpc::Task<SvPb::NamesResponse>);
	void RunOnce(const SvPb::RunOnceRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void LoadConfig(const SvPb::LoadConfigRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetObjectSelectorItems(const SvPb::GetObjectSelectorItemsRequest&, SvRpc::Task<SvPb::GetObjectSelectorItemsResponse>);

	void RegisterNotificationStream(const SvPb::GetNotificationStreamRequest& rRequest,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse>& rObserver,
		SvRpc::ServerStreamContext::Ptr ctx);
	
private:
	std::string GetFileNameFromFilePath(const std::string& rFilePath, const std::string& rExtension = std::string());
	HRESULT ConvertStorageValueToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue);
	HRESULT ConvertStorageImageToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue);
	HRESULT AddValuesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems);
	HRESULT AddImagesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems);
	void ConvertTreeNames(SvPb::TreeItem* pTreeItem) const;
};

