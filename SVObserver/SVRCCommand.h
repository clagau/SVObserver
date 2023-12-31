//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file SVRCCommand.h
/// All Rights Reserved 
//*****************************************************************************
/// Mapping from incoming RPC requests to SVObserver
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#include "SVRPCLibrary/Task.h"
#include "SVOLibrary/SVStorageResult.h"
#pragma endregion Includes


struct SVStorageResult;

class SVRCCommand 
{
public:
	~SVRCCommand() = default;

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
	void ExecuteInspectionCmd(const SvPb::InspectionCmdRequest&, SvRpc::Task<SvPb::InspectionCmdResponse>);
	void GetConfigurationTree(const SvPb::GetConfigurationTreeRequest&, SvRpc::Task<SvPb::GetConfigurationTreeResponse>);
	void ConfigCommand(const SvPb::ConfigCommandRequest&, SvRpc::Task<SvPb::ConfigCommandResponse>);
	void SetTriggerConfig(const SvPb::SetTriggerConfigRequest&, SvRpc::Task<SvPb::StandardResponse>);
	void GetConfigurationInfo(const SvPb::GetConfigurationInfoRequest&, SvRpc::Task<SvPb::GetConfigurationInfoResponse>);
	void SoftwareTrigger(const SvPb::SoftwareTriggerRequest&, SvRpc::Task<SvPb::StandardResponse>);

	void RegisterNotificationStream(const SvPb::GetNotificationStreamRequest& rRequest,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse>& rObserver,
		SvRpc::ServerStreamContext::Ptr ctx);
	void RegisterMessageStream(const SvPb::GetMessageStreamRequest& rRequest,
		SvRpc::Observer<SvPb::GetMessageStreamResponse>& rObserver,
		SvRpc::ServerStreamContext::Ptr ctx);

	
private:

	std::string GetFileNameFromFilePath(const std::string& rFilePath, const std::string& rExtension = std::string()) const;
	HRESULT ConvertStorageValueToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue) const;
	HRESULT ConvertStorageImageToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue) const;
	HRESULT AddValuesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems) const;
	HRESULT AddImagesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems) const;
	void ConvertTreeNames(SvPb::TreeItem* pTreeItem) const;
	void addObjectChildren(uint32_t inspectionID, uint32_t parentID, std::back_insert_iterator<std::vector<uint32_t>> inserter) const;
	void addConfigItem(uint32_t inspectionID, uint32_t objectID, std::back_insert_iterator<std::vector<SvPb::ConfigTreeItem>> inserter) const;
	void clipboardAction(const SvPb::ClipboardRequest rRequest, SvPb::StandardResponse* pResponse) const;
};

