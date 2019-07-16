//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCCommand.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Mapping from incoming RPC requests to SVObserver
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRCCommand.h"
#include "RemoteCommand.h"
#include "SVConfigurationObject.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"
#include "SVVisionProcessorHelper.h"
#include "SVRemoteControlConstants.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVIMCommand.h"
#include "Definitions/SVUserMessage.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVStorageResult.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes

constexpr char* c_DefaultConfigurationName = _T("Configuration");

SVRCCommand::SVRCCommand()
{
}

SVRCCommand::~SVRCCommand()
{
}

void SVRCCommand::GetVersion(const SvPb::GetSVObserverVersionRequest& rRequest, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Response.set_version(SvUl::to_utf8(SvSyl::SVVersionInfo::GetShortTitleVersion()));
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	task.finish(std::move(Response));
}

void SVRCCommand::GetDeviceMode(const SvPb::GetDeviceModeRequest& rRequest, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
{
	SvPb::GetDeviceModeResponse Response;
	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	Response.set_mode(SVSVIMStateClass::getCurrentMode());
	task.finish(std::move(Response));
}

void SVRCCommand::SetDeviceMode(const SvPb::SetDeviceModeRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	SvPb::DeviceModeType DesiredMode = rRequest.mode();

	HRESULT Status {E_INVALIDARG};

	if (SvPb::DeviceModeType::unknownMode != DesiredMode)
	{
		SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			//Note this needs to be done using SendMessage due to this being a worker thread
			Status = GlobalRCSetMode(static_cast<unsigned long> (DesiredMode));
		}
		else
		{
			Status = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
		SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	}

	SvPb::StandardResponse Response;
	Response.set_hresult(Status);
	task.finish(std::move(Response));
}

void SVRCCommand::GetState(const SvPb::GetStateRequest& rRequest, SvRpc::Task<SvPb::GetStateResponse> task)
{
	SvPb::GetStateResponse Response;
	unsigned long State {0L};

	State |= SVSVIMStateClass::CheckState(SV_STATE_READY) ? SvDef::SVIM_CONFIG_LOADED : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_SAVING) ? SvDef::SVIM_SAVING_CONFIG : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_LOADING) ? SvDef::SVIM_CONFIG_LOADING : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_EDIT) ? SvDef::SVIM_SETUPMODE : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_CLOSING) ? SvDef::SVIM_STOPPING : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_START_PENDING) ? SvDef::SVIM_ONLINE_PENDING : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_RAID_FAILURE) ? SvDef::SVIM_RAID_FAILURE : 0;

	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		State |= SvDef::SVIM_ONLINE;
		// testing (but not regression testing) sets the running flag
		State |= !SVSVIMStateClass::CheckState(SV_STATE_TEST) ? SvDef::SVIM_RUNNING : 0;
	}

	if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION))
	{
		State |= SvDef::SVIM_REGRESSION_TEST;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_TEST))// can be testing without regression testing, but can't be regression testing without testing
	{
		State |= SvDef::SVIM_RUNNING_TEST;
	}

	Response.set_state(State);
	task.finish(std::move(Response));
}

void SVRCCommand::GetConfig(const SvPb::GetConfigRequest& rRequest, SvRpc::Task<SvPb::GetConfigResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::GetConfigResponse Response;

	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		Result = SVMSG_SVO_ACCESS_DENIED;
	}

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr == pConfig || !pConfig->IsConfigurationLoaded())
	{
		Result = SVMSG_CONFIGURATION_NOT_LOADED;
	}

	if (S_OK == Result)
	{
		std::string RemoteFilePath = SvUl::to_ansi(rRequest.filename());

		if (RemoteFilePath.empty())
		{
			RemoteFilePath = c_DefaultConfigurationName;
			RemoteFilePath += SvDef::cPackedConfigExtension;
		}
		std::string TempFileName = SvStl::GlobalPath::Inst().GetRamDrive(GetFileNameFromFilePath(RemoteFilePath).c_str());

		//Old .pac file extension is no longer valid!
		if (!TempFileName.empty() && std::string::npos == TempFileName.find(_T(".pac")))
		{
			try
			{
				SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
				Result = GlobalRCSaveConfiguration(TempFileName.c_str());
				SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				Result = static_cast<HRESULT> (rSvE.getMessage().m_MessageCode);
			}
		}
		else
		{
			Result = E_INVALIDARG;
		}

		std::vector<char> FileData;
		if (S_OK == Result)
		{
			Result = SVEncodeDecodeUtilities::FileToCharVector(TempFileName, FileData);
		}
		// cleanup
		::remove(TempFileName.c_str());

		if (S_OK == Result)
		{
			Response.set_filedata(&FileData[0], FileData.size());
		}
		else
		{
			// Log Exception (in case no one else did...)
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(Result, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	Response.set_hresult(Result);

	task.finish(std::move(Response));
}

void SVRCCommand::PutConfig(const SvPb::PutConfigRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::StandardResponse Response;

	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		Result = SVMSG_SVO_ACCESS_DENIED;
	}

	// Check if we are in an allowed state first
	// Not allowed to perform if in Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		Result = SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	if (S_OK == Result)
	{
		std::string RemoteFilePath = SvUl::to_ansi(rRequest.filename());
		PutConfigType type {PutConfigType::SvzFormatWithName};

		if (RemoteFilePath.empty())
		{
			RemoteFilePath = c_DefaultConfigurationName;
			RemoteFilePath += SvDef::cPackedConfigExtension;
			type = PutConfigType::SvzFormatDefaultName;
		}

		std::string TempFileName = SvStl::GlobalPath::Inst().GetRamDrive(GetFileNameFromFilePath(RemoteFilePath, SvDef::cPackedConfigExtension).c_str());

		if (0 != rRequest.filedata().length() && !TempFileName.empty())
		{
			Result = SVEncodeDecodeUtilities::StringContentToFile(TempFileName, rRequest.filedata());
			if (S_OK == Result)
			{
				//For old .pac file format the first 4 bytes are always 1
				DWORD fileVersion = (rRequest.filedata().length() > sizeof(DWORD)) ? *(reinterpret_cast<const DWORD*> (rRequest.filedata().c_str())) : 0;
				SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
				//@WARNING [gra][8.10][11.06.2018] SendMessage is used to avoid problems by accessing the SVObserverApp instance from another thread
				//This should be changed using inspection commands
				type = (1 == fileVersion) ? PutConfigType::PackedFormat : type;
				Result = GlobalRCLoadPackedConfiguration(TempFileName.c_str(), type);
				SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
				::remove(TempFileName.c_str());
			}
		}
		else
		{
			Result = E_INVALIDARG;
		}
	}

	Response.set_hresult(Result);

	task.finish(std::move(Response));
}

void SVRCCommand::GetOfflineCount(const SvPb::GetOfflineCountRequest& rRequest, SvRpc::Task<SvPb::GetOfflineCountResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::GetOfflineCountResponse Response;

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Response.set_count(TheSVObserverApp.getOfflineCount());
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	task.finish(std::move(Response));
}

void SVRCCommand::ActivateMonitorList(const SvPb::ActivateMonitorListRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::StandardResponse Response;

	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION |
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (!SVSVIMStateClass::CheckState(notAllowedStates) && SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		SVConfigurationObject* pConfig = nullptr;

		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			std::string listName = SvUl::to_ansi(rRequest.listname());
			bool bActivate = rRequest.activate();
			SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
			Result = pConfig->ActivateRemoteMonitorList(listName, bActivate);
			SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
		}
		else
		{
			Result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}
	else
	{
		Result = SVMSG_SVO_ACCESS_DENIED;
	}

	Response.set_hresult(Result);

	task.finish(std::move(Response));
}

void SVRCCommand::GetProductFilter(const SvPb::GetProductFilterRequest& rRequest, SvRpc::Task<SvPb::GetProductFilterResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::GetProductFilterResponse Response;

	SVConfigurationObject* pConfig = nullptr;

	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		SvSml::SVProductFilterEnum ProductFilter;
		std::string listName = SvUl::to_ansi(rRequest.listname());
		SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
		Result = pConfig->GetRemoteMonitorListProductFilter(listName, ProductFilter);
		SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

		switch (ProductFilter)
		{
			case SvSml::SVProductFilterEnum::LastInspectedFilter:
				Response.set_filter(SvPb::ProductFilterEnum::lastInspectedFilter);
				break;
			case SvSml::SVProductFilterEnum::LastRejectFilter:
				Response.set_filter(SvPb::ProductFilterEnum::lastRejectFilter);
				break;
			default:
				Response.set_filter(SvPb::ProductFilterEnum::noFilter);
				break;
		}
	}
	else
	{
		Result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::SetProductFilter(const SvPb::SetProductFilterRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::StandardResponse Response;

	DWORD notAllowedStates = SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION |
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING |
		SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING;

	if (!SVSVIMStateClass::CheckState(notAllowedStates))
	{
		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			SvSml::SVProductFilterEnum ProductFilter {SvSml::SVProductFilterEnum::NoFilter};
			switch (rRequest.filter())
			{
				case SvPb::ProductFilterEnum::lastInspectedFilter:
					ProductFilter = SvSml::SVProductFilterEnum::LastInspectedFilter;
					break;
				case SvPb::ProductFilterEnum::lastRejectFilter:
					ProductFilter = SvSml::SVProductFilterEnum::LastRejectFilter;
					break;
				default:
					ProductFilter = SvSml::SVProductFilterEnum::NoFilter;
					break;
			}
			std::string listName = SvUl::to_ansi(rRequest.listname());
			SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
			Result = pConfig->SetRemoteMonitorListProductFilter(listName, ProductFilter);
			SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
		}
		else
		{
			Result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}
	else
	{
		Result = SVMSG_SVO_ACCESS_DENIED;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetItems(const SvPb::GetItemsRequest& rRequest, SvRpc::Task<SvPb::GetItemsResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::GetItemsResponse Response;

	SvDef::StringSet ItemNameSet;
	SVNameStorageResultMap ResultItems;

	for (int i = 0; i < rRequest.itemnamelist_size(); i++)
	{
		ItemNameSet.insert(SvUl::to_ansi(rRequest.itemnamelist(i)));
	}

	if (!ItemNameSet.empty())
	{
		SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
		Result = SVVisionProcessorHelper::Instance().GetItems(ItemNameSet, ResultItems);
		SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

		for (const auto& rEntry : ResultItems)
		{
			HRESULT LoopStatus {S_OK};

			switch (rEntry.second.m_Storage.m_StorageType)
			{
				case SVVisionProcessor::SVStorageValue:
				{
					LoopStatus = rEntry.second.m_Status;

					if (S_OK == LoopStatus)
					{
						SvPb::Value* pValue = Response.add_valuelist();
						ConvertStorageValueToProtobuf(rEntry.first, rEntry.second, pValue);
					}

					if (S_OK != LoopStatus)
					{
						SvPb::Value* pValue = Response.add_errorlist();
						if (nullptr != pValue)
						{
							pValue->set_name(SvUl::to_utf8(rEntry.first));
							pValue->set_status(LoopStatus);
							if (S_OK == Result)
							{
								Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}
					}

					break;
				}
				case SVVisionProcessor::SVStorageImageFileName:
				{
					LoopStatus = rEntry.second.m_Status;

					if (S_OK == LoopStatus)
					{
						SvPb::Value* pValue = Response.add_imagelist();
						LoopStatus = ConvertStorageImageToProtobuf(rEntry.first, rEntry.second, pValue);
					}

					if (S_OK != LoopStatus)
					{
						std::string FileName = SvUl::createStdString(rEntry.second.m_Storage.m_Variant);

						if (!FileName.empty())
						{
							::remove(FileName.c_str());
						}

						SvPb::Value* pValue = Response.add_errorlist();
						if (nullptr != pValue)
						{
							pValue->set_name(SvUl::to_utf8(rEntry.first));
							pValue->set_status(LoopStatus);
							if (S_OK == Result)
							{
								Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
							}
						}
					}

					break;
				}
				default:
				{
					LoopStatus = rEntry.second.m_Status;

					if (S_OK == LoopStatus)
					{
						LoopStatus = E_INVALIDARG;
					}

					SvPb::Value* pValue = Response.add_errorlist();
					if (nullptr != pValue)
					{
						pValue->set_name(SvUl::to_utf8(rEntry.first));
						pValue->set_status(LoopStatus);
						if (S_OK == Result)
						{
							Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}

					break;
				}
			}
		}
	}
	else if (S_OK == Result)
	{
		Result = E_INVALIDARG;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::SetItems(const SvPb::SetItemsRequest& rRequest, SvRpc::Task<SvPb::SetItemsResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::SetItemsResponse Response;

	SVNameStorageMap Items;
	SVNameStatusMap ItemResults;

	Result = AddValuesToStorageItems(rRequest, Items);

	if (S_OK == Result)
	{
		Result = AddImagesToStorageItems(rRequest, Items);
	}

	if (S_OK == Result)
	{
		if (!Items.empty())
		{
			SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
			Result = SVVisionProcessorHelper::Instance().SetItems(Items, ItemResults, rRequest.runonce());
			SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
		}
		else
		{
			Result = E_INVALIDARG;
		}
	}

	for (const auto& rEntry : Items)
	{
		SVNameStatusMap::const_iterator StatusIter = ItemResults.find(rEntry.first);
		HRESULT Status {S_OK};

		if (ItemResults.end() == StatusIter)
		{
			Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			Status = static_cast<HRESULT> (SVMSG_OBJECT_NOT_PROCESSED);
		}
		else if (S_OK != StatusIter->second)
		{
			Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			Status = StatusIter->second;
		}

		if (S_OK != Status)
		{
			SvPb::Value* pValue = Response.add_errorlist();
			if (nullptr != pValue)
			{
				pValue->set_name(SvUl::to_utf8(rEntry.first));
				pValue->set_status(Status);
			}
		}
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetFile(const SvPb::GetFileRequest& rRequest, SvRpc::Task<SvPb::GetFileResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::GetFileResponse Response;

	std::string SourcePath = SvUl::to_ansi(rRequest.sourcepath());

	if (!SourcePath.empty())
	{
		//If only file name then add default path which is Run path
		if (std::string::npos == SourcePath.find('\\'))
		{
			SourcePath = SvStl::GlobalPath::Inst().GetRunPath(SourcePath.c_str());
		}

		std::vector<char> FileData;
		Result = SVEncodeDecodeUtilities::FileToCharVector(SourcePath, FileData);
		if (S_OK == Result && 0 != FileData.size())
		{
			Response.set_filedata(&FileData[0], FileData.size());
		}
	}
	else
	{
		Result = E_INVALIDARG;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::PutFile(const SvPb::PutFileRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::StandardResponse Response;

	std::string destinationPath = SvUl::to_ansi(rRequest.destinationpath());

	if (!destinationPath.empty())
	{
		//If only file name then add default path which is Run path
		if(std::string::npos == destinationPath.find('\\'))
		{
			destinationPath = SvStl::GlobalPath::Inst().GetRunPath(destinationPath.c_str());
		}
		std::string runPath{SvStl::GlobalPath::Inst().GetRunPath()};
		std::string comparePath{destinationPath};
		SvUl::MakeLower(comparePath);
		SvUl::MakeLower(runPath);
		if(rRequest.saveinconfig() && 0 != comparePath.find(runPath))
		{
			Result = E_INVALIDARG;
		}
		else
		{
			Result = SVEncodeDecodeUtilities::StringContentToFile(destinationPath, rRequest.filedata());
		}
		
		if(S_OK == Result && rRequest.saveinconfig())
		{
			//Note this needs to be done using SendMessage due to this being a worker thread
			Result = GlobalRCAddFileToConfig(destinationPath.c_str());
		}

	}
	else
	{
		Result = E_INVALIDARG;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::RegisterMonitorList(const SvPb::RegisterMonitorListRequest& rRequest, SvRpc::Task<SvPb::RegisterMonitorListResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::RegisterMonitorListResponse Response;

	SvDef::StringSet prodList;
	SvDef::StringSet rejectCondList;
	SvDef::StringSet failStatusList;

	std::string MonitorListName = SvUl::to_ansi(rRequest.listname());
	std::string PpqName = SvUl::to_ansi(rRequest.ppqname());
	int rejectDepth = rRequest.rejectdepth();
	for (int i = 0; i < rRequest.productitemlist_size(); i++)
	{
		prodList.insert(SvUl::to_ansi(rRequest.productitemlist(i)));
	}
	for (int i = 0; i < rRequest.rejectconditionlist_size(); i++)
	{
		rejectCondList.insert(SvUl::to_ansi(rRequest.rejectconditionlist(i)));
	}
	for (int i = 0; i < rRequest.failstatuslist_size(); i++)
	{
		failStatusList.insert(SvUl::to_ansi(rRequest.failstatuslist(i)));
	}

	SVNameStatusMap ItemResults;
	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Result = SVVisionProcessorHelper::Instance().RegisterMonitorList(MonitorListName, PpqName, rejectDepth, prodList, rejectCondList, failStatusList, ItemResults);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	for (const auto& rEntry : ItemResults)
	{
		if (S_OK != rEntry.second)
		{
			Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			SvPb::Value* pValue = Response.add_errorlist();
			if (nullptr != pValue)
			{
				pValue->set_name(SvUl::to_utf8(rEntry.first));
				pValue->set_status(rEntry.second);
			}
		}
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetInspectionNames(const SvPb::GetInspectionNamesRequest& rRequest, SvRpc::Task<SvPb::NamesResponse> task)
{
	SvPb::NamesResponse Response;

	SvDef::StringSet InspectionNames;

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	HRESULT Result = SVVisionProcessorHelper::Instance().GetInspectionNames(InspectionNames);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	SvPb::Value* pValue = Response.mutable_names();
	if (S_OK == Result && nullptr != pValue)
	{
		SvPb::ConvertStringListToProtobuf(InspectionNames, pValue->mutable_item());
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::Shutdown(const SvPb::ShutdownRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	SvPb::StandardResponse Response;

	HRESULT Result {S_OK};

	long shutdownOption = rRequest.option();

	try
	{
		//check if shutdown.exe exists. If not, the shutdown won't be called
		std::ifstream dllfile("SVShutdown.exe", std::ios::binary);
		CWnd* pWnd = AfxGetApp()->m_pMainWnd;
		if (0 > shutdownOption && !dllfile && nullptr == pWnd)
		{
			Result = E_FAIL;
		}
		else
		{
			PostMessage(pWnd->m_hWnd, SV_SHUTDOWN, shutdownOption, 0);
		}
	}
	catch (ATL::CAtlException & ex)
	{
		Result = ex;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetMonitorListProperties(const SvPb::GetMonitorListPropertiesRequest& rRequest, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse> task)
{
	HRESULT Result {S_OK};

	SvPb::GetMonitorListPropertiesResponse Response;

	MonitorlistProperties  MonitorListProp;
	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Result = SVVisionProcessorHelper::Instance().GetMonitorListProperties(SvUl::to_ansi(rRequest.listname()), MonitorListProp);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	Response.set_rejectdepth(MonitorListProp.RejectQueDepth);
	Response.set_active(MonitorListProp.isActive);
	Response.set_ppqname(SvUl::to_utf8(MonitorListProp.ppqName));
	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetMaxRejectDepth(const SvPb::GetMaxRejectDepthRequest& rRequest, SvRpc::Task<SvPb::GetMaxRejectDepthResponse> task)
{
	SvPb::GetMaxRejectDepthResponse Response;

	Response.set_maxrejectdepth(RemoteMonitorNamedList::GetMaxRejectQueueDepth());
	task.finish(std::move(Response));
}

void SVRCCommand::GetConfigReport(const SvPb::GetConfigReportRequest& rRequest, SvRpc::Task<SvPb::GetConfigReportResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::GetConfigReportResponse Response;
	std::string Report;

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Result = SVVisionProcessorHelper::Instance().GetConfigurationPrintReport(Report);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	//No conversion to utf8 as the report is already utf16 (XML)
	Response.set_report(Report);
	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetDataDefinitionList(const SvPb::GetDataDefinitionListRequest& rRequest, SvRpc::Task<SvPb::GetDataDefinitionListResponse> task)
{
	HRESULT Result {S_OK};

	SvPb::GetDataDefinitionListResponse Response;

	SVDataDefinitionStructVector DataDefinitionList;

	std::string InspectionName = SvUl::to_ansi(rRequest.inspectionname());
	SVDataDefinitionListType DataDefinitionType = static_cast<SVDataDefinitionListType> (rRequest.type());

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Result = SVVisionProcessorHelper::Instance().GetDataDefinitionList(InspectionName, DataDefinitionType, DataDefinitionList);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	for (const auto& rEntry : DataDefinitionList)
	{
		SvPb::DataDefinition* pDataDefinition = Response.add_list();
		if (nullptr != pDataDefinition)
		{
			pDataDefinition->set_name(SvUl::to_utf8(rEntry.m_Name));
			pDataDefinition->set_writable(rEntry.m_Writable);
			pDataDefinition->set_published(rEntry.m_Published);
			pDataDefinition->set_type(SvUl::to_utf8(rEntry.m_Type));
			for (const auto& rAddInfo : rEntry.m_AdditionalInfo)
			{
				std::string* pAddInfo = pDataDefinition->add_additionalinfo();
				if (nullptr != pAddInfo)
				{
					*pAddInfo = SvUl::to_utf8(rAddInfo);
				}
			}
		}
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::QueryMonitorList(const SvPb::QueryMonitorListRequest& rRequest, SvRpc::Task<SvPb::NamesResponse> task)
{
	HRESULT Result {S_OK};

	SvPb::NamesResponse Response;

	SvDef::StringSet Items;
	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	std::string listName = SvUl::to_ansi(rRequest.listname());
	switch (rRequest.type())
	{
		case SvPb::ListType::productItem:
		{
			Result = SVVisionProcessorHelper::Instance().QueryProductList(listName, Items);
			break;
		}
		case SvPb::ListType::rejectCondition:
		{
			Result = SVVisionProcessorHelper::Instance().QueryRejectCondList(listName, Items);
			break;
		}
		case SvPb::ListType::failStatus:
		{
			Result = SVVisionProcessorHelper::Instance().QueryFailStatusList(listName, Items);
			break;
		}
		default:
		{
			break;
		}
	}
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	SvPb::Value* pValue = Response.mutable_names();
	if (S_OK == Result && nullptr != pValue)
	{
		SvPb::ConvertStringListToProtobuf(Items, pValue->mutable_item());
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::QueryMonitorListNames(const SvPb::QueryMonitorListNamesRequest& rRequest, SvRpc::Task<SvPb::NamesResponse> task)
{
	HRESULT Result {S_OK};

	SvPb::NamesResponse Response;

	SvDef::StringSet Items;
	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Result = SVVisionProcessorHelper::Instance().QueryMonitorListNames(Items);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	SvPb::Value* pValue = Response.mutable_names();
	if (S_OK == Result && nullptr != pValue)
	{
		SvPb::ConvertStringListToProtobuf(Items, pValue->mutable_item());
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::RunOnce(const SvPb::RunOnceRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::StandardResponse Response;

	if(!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{

		SVConfigurationObject* pConfig{nullptr};
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if(nullptr != pConfig)
		{
			SVInspectionProcess* pInspection{nullptr};
			pConfig->GetInspection(SvUl::to_ansi(rRequest.inspectionname()).c_str(), pInspection);
			if(nullptr != pInspection)
			{
				SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
				SvCmd::RunOnceSynchronous(pInspection->GetUniqueObjectID());
				SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
			}
			else
			{
				Result = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			}
		}
		else
		{
			Result = SVMSG_CONFIGURATION_NOT_LOADED;
		}
	}
	else
	{
		Result = SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::LoadConfig(const SvPb::LoadConfigRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT Result {S_OK};
	SvPb::StandardResponse Response;
	std::string ConfigFile = SvUl::to_ansi(rRequest.filename());

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
	Result = SVVisionProcessorHelper::Instance().LoadConfiguration(ConfigFile);
	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	Response.set_hresult(Result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetObjectSelectorItems(const SvPb::GetObjectSelectorItemsRequest& rRequest, SvRpc::Task<SvPb::GetObjectSelectorItemsResponse> task)
{
	GUID inspectionID = SvPb::GetGuidFromProtoBytes(rRequest.inspectionid());

	SvPb::InspectionCmdMsgs requestCmd, responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = rRequest;
	SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);

	SvPb::GetObjectSelectorItemsResponse response{responseCmd.getobjectselectoritemsresponse()};
	ConvertTreeNames(response.mutable_tree());
	task.finish(std::move(response));
}

void SVRCCommand::ExecuteInspectionCmd(const SvPb::ExecuteInspectionCmdRequest& rRequest, SvRpc::Task<SvPb::ExecuteInspectionCmdResponse> task)
{
	GUID inspectionID = SvPb::GetGuidFromProtoBytes(rRequest.inspectionid());

	SvPb::ExecuteInspectionCmdResponse response;
	SvCmd::InspectionCommands(inspectionID, rRequest.cmdmsg(), response.mutable_cmdmsg());

	task.finish(std::move(response));
}

void SVRCCommand::RegisterNotificationStream(const SvPb::GetNotificationStreamRequest& rRequest,
											 SvRpc::Observer<SvPb::GetNotificationStreamResponse>& rObserver,
											 SvRpc::ServerStreamContext::Ptr ctx)
{
	SVVisionProcessorHelper::Instance().RegisterNotificationStream(rRequest, rObserver, ctx);
}

std::string SVRCCommand::GetFileNameFromFilePath(const std::string& rFilePath, const std::string& rExtension /*= std::string()*/)
{
	std::string Result;

	if (!rFilePath.empty())
	{
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath(rFilePath.c_str(), nullptr, nullptr, fname, ext);

		Result = fname;
		Result += rExtension.empty() ? ext : rExtension;
	}

	return Result;
}

HRESULT SVRCCommand::ConvertStorageValueToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue)
{
	HRESULT Result {S_OK};

	if (nullptr != pValue && SVVisionProcessor::SVStorageValue == rStorage.m_Storage.m_StorageType)
	{
		pValue->set_name(SvUl::to_utf8(rName));
		pValue->set_count(rStorage.m_TriggerCount);
		HRESULT ItemStatus = rStorage.m_Status;
		if (S_OK == rStorage.m_Status)
		{
			ItemStatus = SvPb::ConvertVariantToProtobuf(rStorage.m_Storage.m_Variant, pValue->mutable_item());
		}
		pValue->set_status(ItemStatus);
	}
	else
	{
		Result = E_UNEXPECTED;
	}

	return Result;
}

HRESULT SVRCCommand::ConvertStorageImageToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue)
{
	HRESULT Result {S_OK};

	if (nullptr != pValue && SVVisionProcessor::SVStorageImageFileName == rStorage.m_Storage.m_StorageType  && VT_BSTR == rStorage.m_Storage.m_Variant.vt)
	{
		HRESULT ItemStatus = rStorage.m_Status;
		pValue->set_name(SvUl::to_utf8(rName));
		pValue->set_count(rStorage.m_TriggerCount);

		if (S_OK == ItemStatus)
		{
			std::string FileName = SvUl::createStdString(rStorage.m_Storage.m_Variant);

			std::vector<char> FileData;
			ItemStatus = SVEncodeDecodeUtilities::FileToCharVector(FileName, FileData);
			if (S_OK == ItemStatus)
			{
				::remove(FileName.c_str());

				pValue->mutable_item()->set_type(VT_UI1 | VT_ARRAY);
				pValue->mutable_item()->set_bytesval(&FileData[0], FileData.size());
			}
		}

		pValue->set_status(ItemStatus);

		if (S_OK == Result && S_OK != ItemStatus)
		{
			Result = E_UNEXPECTED;
		}
	}
	else
	{
		Result = E_UNEXPECTED;
	}

	return Result;
}

HRESULT SVRCCommand::AddValuesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems)
{
	HRESULT Result {S_OK};

	for (int i = 0; i < rRequest.valuelist_size(); i++)
	{
		const SvPb::Value& rValue = rRequest.valuelist(i);

		std::string name = SvUl::to_ansi(rValue.name());
		SVStorage Storage;
		Storage.m_StorageType = SVVisionProcessor::SVStorageValue;

		Result = SvPb::ConvertProtobufToVariant(rValue.item(), Storage.m_Variant);

		if (S_OK == Result)
		{
			rItems[name] = Storage;
		}
	}

	return Result;
}

HRESULT SVRCCommand::AddImagesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems)
{
	HRESULT Result {S_OK};

	for (int i = 0; i < rRequest.imagelist_size(); i++)
	{
		const SvPb::Value& rImage = rRequest.imagelist(i);
		std::string name = SvUl::to_ansi(rImage.name());
		std::string FilePath;

		if (rImage.item().type() == (VT_ARRAY | VT_UI1))
		{
			const std::string& rContent = rImage.item().bytesval();
			std::string FileName {name};
			FileName += _T(".bmp");
			FilePath = SvStl::GlobalPath::Inst().GetRamDrive(GetFileNameFromFilePath(FileName).c_str());
			if (!FilePath.empty())
			{
				Result = SVEncodeDecodeUtilities::StringContentToFile(FilePath, rContent);
			}
		}
		else if (rImage.item().type() == VT_BSTR)
		{
			FilePath = SvStl::GlobalPath::Inst().GetRamDrive(rImage.item().strval().c_str());
		}

		if (S_OK == Result)
		{
			SVStorage Storage;
			Storage.m_StorageType = SVVisionProcessor::SVStorageImageFileName;
			Storage.m_Variant.SetString(FilePath.c_str());
			rItems[name] = Storage;
		}
	}

	return Result;
}

void SVRCCommand::ConvertTreeNames(SvPb::TreeItem* pTreeItem) const
{
	if(nullptr != pTreeItem)
	{
		pTreeItem->set_name(SvUl::to_utf8(pTreeItem->name()));
		for(int i=0; i < pTreeItem->children_size(); i++)
		{
			ConvertTreeNames(pTreeItem->mutable_children(i));
		}
	}
}
