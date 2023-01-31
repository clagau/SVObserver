//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVIPDoc.h"
#include "SVObserverOuttakes.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "SVVisionProcessorHelper.h"
#include "SVRemoteControlConstants.h"
#include "SVStorageResult.h"
#include "SVToolSet.h"
#include "ToolClipboard.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVIMCommand.h"
#include "Definitions/SVUserMessage.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "Triggering/SVTriggerClass.h"
#include "Triggering/SVTriggerObject.h"
#pragma endregion Includes

constexpr const char* c_DefaultConfigurationName = _T("Configuration");

void SVRCCommand::GetVersion(const SvPb::GetSVObserverVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	DWORD notAllowedStates = SV_STATE_UNAVAILABLE;

	SvPb::GetVersionResponse Response;
	if (false == SVSVIMStateClass::CheckState(notAllowedStates))
	{
		SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
		Response.set_version(SvUl::to_utf8(SvSyl::SVVersionInfo::GetShortTitleVersion()));
	}
	task.finish(std::move(Response));
}

void SVRCCommand::GetDeviceMode(const SvPb::GetDeviceModeRequest&, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
{
	SvPb::GetDeviceModeResponse Response;
	{
		SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
		Response.set_mode(SVSVIMStateClass::getCurrentMode());
	}
	task.finish(std::move(Response));
}

void SVRCCommand::SetDeviceMode(const SvPb::SetDeviceModeRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};

	if (S_OK == result)
	{
		SvPb::DeviceModeType DesiredMode = rRequest.mode();

		if (SvPb::DeviceModeType::unknownMode != DesiredMode)
		{

			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				//Note this needs to be done using SendMessage due to this being a worker thread
				result = GlobalRCSetMode(static_cast<unsigned long> (DesiredMode));
			}
			else
			{
				result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
	}
	SvPb::StandardResponse Response;
	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetState(const SvPb::GetStateRequest&, SvRpc::Task<SvPb::GetStateResponse> task)
{
	SvPb::GetStateResponse Response;
	unsigned long State{ 0L };

	State |= SVSVIMStateClass::CheckState(SV_STATE_READY) ? SvDef::SVIM_CONFIG_LOADED : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_SAVING) ? SvDef::SVIM_SAVING_CONFIG : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_LOADING) ? SvDef::SVIM_CONFIG_LOADING : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_EDIT) ? SvDef::SVIM_SETUPMODE : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_CLOSING) ? SvDef::SVIM_STOPPING : 0;
	State |= SVSVIMStateClass::CheckState(SV_STATE_START_PENDING) ? SvDef::SVIM_ONLINE_PENDING : 0;

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
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetConfigResponse Response;

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr == pConfig || !pConfig->IsConfigurationLoaded())
		{
			Log_Error(SvStl::Tid_ConfigurationNotLoaded);
			result = SVMSG_CONFIGURATION_NOT_LOADED;
		}

		if (S_OK == result)
		{
			std::string RemoteFilePath = SvUl::to_ansi(rRequest.filename());

			if (RemoteFilePath.empty())
			{
				RemoteFilePath = c_DefaultConfigurationName;
				RemoteFilePath += SvDef::cPackedConfigExtension;
			}
			std::string TempFileName = SvStl::GlobalPath::Inst().GetPathInTempFolderOnC_Drive(GetFileNameFromFilePath(RemoteFilePath).c_str());

			//Old .pac file extension is no longer valid!
			if (!TempFileName.empty() && std::string::npos == TempFileName.find(_T(".pac")))
			{
				try
				{
					SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
					result = GlobalRCSaveConfiguration(TempFileName.c_str());
				}
				catch (const SvStl::MessageContainer& rSvE)
				{
					result = static_cast<HRESULT> (rSvE.getMessage().m_MessageCode);
				}
			}
			else
			{
				result = E_INVALIDARG;
			}

			std::vector<char> FileData;
			if (S_OK == result)
			{
				result = SVEncodeDecodeUtilities::FileToCharVector(TempFileName, FileData);
			}
			// cleanup
			::remove(TempFileName.c_str());

			if (S_OK == result)
			{
				Response.set_filedata(&FileData[0], FileData.size());
			}
			else
			{
				// Log Exception (in case no one else did...)
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(result, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::PutConfig(const SvPb::PutConfigRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::StandardResponse Response;

	// Check if we are in an allowed state first
	// Not allowed to perform if in Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		result = SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	if (S_OK == result)
	{
		std::string RemoteFilePath = SvUl::to_ansi(rRequest.filename());
		ConfigFileType fileType{ ConfigFileType::SvzFormatPutConfig };

		if (RemoteFilePath.empty())
		{
			RemoteFilePath = c_DefaultConfigurationName;
			RemoteFilePath += SvDef::cPackedConfigExtension;
			fileType = ConfigFileType::SvzFormatDefaultName;
		}

		std::string TempFileName = SvStl::GlobalPath::Inst().GetPathInTempFolderOnC_Drive(GetFileNameFromFilePath(RemoteFilePath, SvDef::cPackedConfigExtension).c_str());

		if (0 != rRequest.filedata().length() && !TempFileName.empty())
		{
			result = SVEncodeDecodeUtilities::StringContentToFile(TempFileName, rRequest.filedata());
			if (S_OK == result)
			{
				
#ifdef LOG_LOADING
				std::string msg = std::format("SVRCCommand::PutConfig: Write {}", TempFileName);
				Log_Info(msg.c_str());
#endif				
				
				//For old .pac file format the first 4 bytes are always 1
				DWORD fileVersion = (rRequest.filedata().length() > sizeof(DWORD)) ? *(reinterpret_cast<const DWORD*> (rRequest.filedata().c_str())) : 0;
				//@WARNING [gra][8.10][11.06.2018] SendMessage is used to avoid problems by accessing the SVObserverApp instance from another thread
				//This should be changed using inspection commands
				fileType = (1 == fileVersion) ? ConfigFileType::PackedFormat : fileType;

				{
					SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
					result = GlobalRCLoadPackedConfiguration(TempFileName.c_str(), fileType);
				}
				::remove(TempFileName.c_str());
			}
			else
			{
				::remove(TempFileName.c_str());
				
				SvDef::StringVector stv;
				stv.push_back(TempFileName);
				Log_Error(SvStl::Tid_CouldNotWrite_File, stv);
				result = E_FAIL;
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
	}

	Response.set_hresult(result);

	task.finish(std::move(Response));
#ifdef LOG_LOADING
	std::string  msg = std::format("SVRCCommand::PutConfig RESULT:{}  ", result);
	Log_Info(msg.c_str());
#endif 	
	
}

void SVRCCommand::GetOfflineCount(const SvPb::GetOfflineCountRequest&, SvRpc::Task<SvPb::GetOfflineCountResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetOfflineCountResponse Response;

	if (S_OK == result)
	{
		SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
		Response.set_count(getSvoOfflineCount());
	}

	task.finish(std::move(Response));
}

void SVRCCommand::ActivateMonitorList(const SvPb::ActivateMonitorListRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{ SVSVIMStateClass::CheckNotAllowedState(SV_DEFAULT_NOT_ALLOWED_STATES |SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) };
	SvPb::StandardResponse Response;

	if (S_OK == result && SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		SVConfigurationObject* pConfig = nullptr;

		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			std::string listName = SvUl::to_ansi(rRequest.listname());
			bool bActivate = rRequest.activate();
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = pConfig->ActivateRemoteMonitorList(listName, bActivate);
		}
		else
		{
			result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}
	else
	{
		result = SVMSG_SVO_ACCESS_DENIED;
	}

	Response.set_hresult(result);

	task.finish(std::move(Response));
}

void SVRCCommand::GetProductFilter(const SvPb::GetProductFilterRequest& rRequest, SvRpc::Task<SvPb::GetProductFilterResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetProductFilterResponse Response;

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig = nullptr;

		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			SvSml::SVProductFilterEnum ProductFilter;
			std::string listName = SvUl::to_ansi(rRequest.listname());

			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				result = pConfig->GetRemoteMonitorListProductFilter(listName, ProductFilter);
			}

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
			result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::SetProductFilter(const SvPb::SetProductFilterRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{ SVSVIMStateClass::CheckNotAllowedState(SV_DEFAULT_NOT_ALLOWED_STATES | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) };
	SvPb::StandardResponse Response;

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			SvSml::SVProductFilterEnum ProductFilter{ SvSml::SVProductFilterEnum::NoFilter };
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
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = pConfig->SetRemoteMonitorListProductFilter(listName, ProductFilter);
		}
		else
		{
			result = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetItems(const SvPb::GetItemsRequest& rRequest, SvRpc::Task<SvPb::GetItemsResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetItemsResponse Response;

	if (S_OK == result)
	{
		SvDef::StringSet ItemNameSet;
		SVNameStorageResultMap ResultItems;

		for (int i = 0; i < rRequest.itemnamelist_size(); i++)
		{
			ItemNameSet.insert(SvUl::to_ansi(rRequest.itemnamelist(i)));
		}

		if (false == ItemNameSet.empty())
		{
			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				result = SVVisionProcessorHelper::Instance().GetItems(ItemNameSet, ResultItems);
			}

			for (const auto& rEntry : ResultItems)
			{
				HRESULT LoopStatus{ S_OK };

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
							if (S_OK == result)
							{
								result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
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
							if (S_OK == result)
							{
								result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
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
						if (S_OK == result)
						{
							result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}

					break;
				}
				}
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::SetItems(const SvPb::SetItemsRequest& rRequest, SvRpc::Task<SvPb::SetItemsResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::SetItemsResponse Response;

	if (S_OK == result)
	{
		SVNameStorageMap Items;
		SVNameStatusMap ItemResults;

		result = AddValuesToStorageItems(rRequest, Items);

		if (S_OK == result)
		{
			result = AddImagesToStorageItems(rRequest, Items);
		}

		if (S_OK == result)
		{
			if (!Items.empty())
			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				result = SVVisionProcessorHelper::Instance().SetItems(Items, ItemResults, rRequest.runonce());
			}
			else
			{
				result = E_INVALIDARG;
			}
		}

		for (const auto& rEntry : Items)
		{
			SVNameStatusMap::const_iterator StatusIter = ItemResults.find(rEntry.first);
			HRESULT Status{ S_OK };

			if (ItemResults.end() == StatusIter)
			{
				result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				Status = static_cast<HRESULT> (SVMSG_OBJECT_NOT_PROCESSED);
			}
			else if (S_OK != StatusIter->second)
			{
				result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
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
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetFile(const SvPb::GetFileRequest& rRequest, SvRpc::Task<SvPb::GetFileResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetFileResponse Response;

	if (S_OK == result)
	{
		std::string SourcePath = SvUl::to_ansi(rRequest.sourcepath());

		if (!SourcePath.empty())
		{
			//If only file name then add default path which is Run path
			if (std::string::npos == SourcePath.find('\\'))
			{
				SourcePath = SvStl::GlobalPath::Inst().GetRunPath(SourcePath.c_str());
			}

			std::vector<char> FileData;
			result = SVEncodeDecodeUtilities::FileToCharVector(SourcePath, FileData);
			if (S_OK == result && 0 != FileData.size())
			{
				Response.set_filedata(&FileData[0], FileData.size());
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::PutFile(const SvPb::PutFileRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::StandardResponse Response;

	if (S_OK == result)
	{
		std::string destinationPath = SvUl::to_ansi(rRequest.destinationpath());

		if (!destinationPath.empty())
		{
			//If only file name then add default path which is Run path
			if (std::string::npos == destinationPath.find('\\'))
			{
				destinationPath = SvStl::GlobalPath::Inst().GetRunPath(destinationPath.c_str());
			}
			std::string runPath = SvUl::MakeLower(SvStl::GlobalPath::Inst().GetRunPath().c_str());
			std::string comparePath = SvUl::MakeLower(destinationPath.c_str());
			if (rRequest.saveinconfig() && false == comparePath.starts_with(runPath))
			{
				result = E_INVALIDARG;
			}
			else
			{	
				std::filesystem::path pathWithoutFilename = std::filesystem::path(destinationPath).parent_path();
				if (false == std::filesystem::exists(pathWithoutFilename))
				{
					std::filesystem::create_directories(pathWithoutFilename);
				}

				result = SVEncodeDecodeUtilities::StringContentToFile(destinationPath, rRequest.filedata());
			}

			if (S_OK == result && rRequest.saveinconfig())
			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				//Note this needs to be done using SendMessage due to this being a worker thread
				result = GlobalRCAddFileToConfig(destinationPath.c_str());
			}

		}
		else
		{
			result = E_INVALIDARG;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::RegisterMonitorList(const SvPb::RegisterMonitorListRequest& rRequest, SvRpc::Task<SvPb::RegisterMonitorListResponse> task)
{
	HRESULT result{ SVSVIMStateClass::CheckNotAllowedState(SV_DEFAULT_NOT_ALLOWED_STATES | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) };
	SvPb::RegisterMonitorListResponse Response;

	if (S_OK == result)
	{
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

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().RegisterMonitorList(MonitorListName, PpqName, rejectDepth, prodList, rejectCondList, failStatusList, ItemResults);
		}

		for (const auto& rEntry : ItemResults)
		{
			if (S_OK != rEntry.second)
			{
				result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				SvPb::Value* pValue = Response.add_errorlist();
				if (nullptr != pValue)
				{
					pValue->set_name(SvUl::to_utf8(rEntry.first));
					pValue->set_status(rEntry.second);
				}
			}
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetInspectionNames(const SvPb::GetInspectionNamesRequest&, SvRpc::Task<SvPb::NamesResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::NamesResponse Response;

	if (S_OK == result)
	{
		SvDef::StringSet InspectionNames;

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().GetInspectionNames(InspectionNames);
		}

		SvPb::Value* pValue = Response.mutable_names();
		if (S_OK == result && nullptr != pValue)
		{
			SvPb::ConvertStringListToProtobuf(InspectionNames, pValue->mutable_item());
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::Shutdown(const SvPb::ShutdownRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::StandardResponse Response;

	if (S_OK == result)
	{
		long shutdownOption = rRequest.option();

		try
		{
			//check if shutdown.exe exists. If not, the shutdown won't be called
			std::ifstream dllfile("SVShutdown.exe", std::ios::binary);
			HWND hMainWnd = SVVisionProcessorHelper::Instance().getMainhWnd();
			if (0 > shutdownOption && !dllfile && nullptr == hMainWnd)
			{
				result = E_FAIL;
			}
			else
			{
				::PostMessage(hMainWnd, SV_SHUTDOWN, shutdownOption, 0);
			}
		}
		catch (ATL::CAtlException& ex)
		{
			result = ex;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetMonitorListProperties(const SvPb::GetMonitorListPropertiesRequest& rRequest, SvRpc::Task<SvPb::GetMonitorListPropertiesResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetMonitorListPropertiesResponse Response;

	if (S_OK == result)
	{
		MonitorlistProperties  MonitorListProp;

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().GetMonitorListProperties(SvUl::to_ansi(rRequest.listname()), MonitorListProp);
		}

		Response.set_rejectdepth(MonitorListProp.RejectQueDepth);
		Response.set_active(MonitorListProp.isActive);
		Response.set_ppqname(SvUl::to_utf8(MonitorListProp.ppqName));
	}
	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetMaxRejectDepth(const SvPb::GetMaxRejectDepthRequest&, SvRpc::Task<SvPb::GetMaxRejectDepthResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetMaxRejectDepthResponse Response;

	if (S_OK == result)
	{
		SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
		Response.set_maxrejectdepth(RemoteMonitorNamedList::GetMaxRejectQueueDepth());
	}
	task.finish(std::move(Response));
}

void SVRCCommand::GetConfigReport(const SvPb::GetConfigReportRequest&, SvRpc::Task<SvPb::GetConfigReportResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetConfigReportResponse Response;

	if (S_OK == result)
	{
		std::string Report;

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().GetConfigurationPrintReport(Report);
		}
		//No conversion to utf8 as the report is already utf16 (XML)
		Response.set_report(Report);
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetDataDefinitionList(const SvPb::GetDataDefinitionListRequest& rRequest, SvRpc::Task<SvPb::GetDataDefinitionListResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetDataDefinitionListResponse Response;

	if (S_OK == result)
	{
		SVDataDefinitionStructVector DataDefinitionList;

		std::string InspectionName = SvUl::to_ansi(rRequest.inspectionname());
		SVDataDefinitionListType DataDefinitionType = static_cast<SVDataDefinitionListType> (rRequest.type());

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().GetDataDefinitionList(InspectionName, DataDefinitionType, DataDefinitionList);
		}

		for (const auto& rEntry : DataDefinitionList)
		{
			SvPb::DataDefinition* pDataDefinition = Response.add_list();
			if (nullptr != pDataDefinition)
			{
				pDataDefinition->set_name(SvUl::to_utf8(rEntry.m_Name));
				pDataDefinition->set_writable(rEntry.m_Writable);
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
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::QueryMonitorList(const SvPb::QueryMonitorListRequest& rRequest, SvRpc::Task<SvPb::NamesResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::NamesResponse Response;

	if(S_OK == result)
	{
		SvDef::StringSet Items;

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			std::string listName = SvUl::to_ansi(rRequest.listname());
			switch (rRequest.type())
			{
				case SvPb::ListType::productItem:
				{
					result = SVVisionProcessorHelper::Instance().QueryProductList(listName, Items);
					break;
				}
				case SvPb::ListType::rejectCondition:
				{
					result = SVVisionProcessorHelper::Instance().QueryRejectCondList(listName, Items);
					break;
				}
				case SvPb::ListType::failStatus:
				{
					result = SVVisionProcessorHelper::Instance().QueryFailStatusList(listName, Items);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		SvPb::Value* pValue = Response.mutable_names();
		if (S_OK == result && nullptr != pValue)
		{
			SvPb::ConvertStringListToProtobuf(Items, pValue->mutable_item());
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::QueryMonitorListNames(const SvPb::QueryMonitorListNamesRequest&, SvRpc::Task<SvPb::NamesResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::NamesResponse Response;

	if (S_OK == result)
	{
		SvDef::StringSet Items;
		
		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().QueryMonitorListNames(Items);
		}

		SvPb::Value* pValue = Response.mutable_names();
		if (S_OK == result && nullptr != pValue)
		{
			SvPb::ConvertStringListToProtobuf(Items, pValue->mutable_item());
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::RunOnce(const SvPb::RunOnceRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{ SVSVIMStateClass::CheckNotAllowedState(SV_DEFAULT_NOT_ALLOWED_STATES| SV_STATE_RUNNING) };
	SvPb::StandardResponse Response;

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig{ nullptr };
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			SVInspectionProcess* pInspection{ nullptr };
			pConfig->GetInspection(SvUl::to_ansi(rRequest.inspectionname()).c_str(), pInspection);
			if (nullptr != pInspection)
			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				SvCmd::RunOnceSynchronous(pInspection->getObjectId());
			}
			else
			{
				result = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			}
		}
		else
		{
			Log_Error(SvStl::Tid_ConfigurationNotLoaded);
			result = SVMSG_CONFIGURATION_NOT_LOADED;
		}
	}

	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::LoadConfig(const SvPb::LoadConfigRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::StandardResponse Response;

	// Check if we are in an allowed state first
	// Not allowed to perform if in Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		result = SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	if (S_OK == result)
	{
		std::string configFile = SvUl::to_ansi(rRequest.filename());
		
		if (configFile.empty())
		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			GlobalRCCloseConfiguration();
		}
		else
		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			result = SVVisionProcessorHelper::Instance().LoadConfiguration(configFile);
		}
	}
	
	Response.set_hresult(result);
	task.finish(std::move(Response));
}

void SVRCCommand::GetObjectSelectorItems(const SvPb::GetObjectSelectorItemsRequest& rRequest, SvRpc::Task<SvPb::GetObjectSelectorItemsResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetObjectSelectorItemsResponse selectorResponse;

	if (S_OK == result)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		*requestCmd.mutable_getobjectselectoritemsrequest() = rRequest;

		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			SvCmd::InspectionCommands(rRequest.inspectionid(), requestCmd, &responseCmd);
		}

		selectorResponse = responseCmd.getobjectselectoritemsresponse();
		ConvertTreeNames(selectorResponse.mutable_tree());
		int childrenSize = selectorResponse.mutable_tree()->children_size();
		if (0 < childrenSize)
		{
			std::vector<SvPb::TreeItem> itemVector;
			itemVector.insert(itemVector.begin(), selectorResponse.mutable_tree()->children().begin(), selectorResponse.mutable_tree()->children().end());
			selectorResponse.mutable_tree()->clear_children();
			SvPb::convertVectorToTree(itemVector, selectorResponse.mutable_tree());
		}
	}

	task.finish(std::move(selectorResponse));
}

void SVRCCommand::ExecuteInspectionCmd(const SvPb::InspectionCmdRequest& rRequest, SvRpc::Task<SvPb::InspectionCmdResponse> task)
{
	SvPb::InspectionCmdResponse response;
	uint32_t inspectionID = rRequest.inspectionid();
	HRESULT result = SvCmd::InspectionCommandsCheckState(rRequest, &response);
	if (S_OK == result)
	{
		std::string deleteObjectName;
		if (SvPb::InspectionCmdRequest::kDeleteObjectRequest == rRequest.message_case())
		{
			SvOi::IObjectClass* pObject = SvOi::getObject(rRequest.deleteobjectrequest().objectid());
			SvOi::IObjectClass* pParent = (nullptr != pObject) ? SvOi::getObject(pObject->GetParentID()) : nullptr;
			//Only when a tools parent is the Tool Set then the name needs to be removed from the tool set grouping
			if (nullptr != pParent && SvPb::SVToolSetObjectType == pParent->GetObjectType())
			{
				deleteObjectName = pObject->GetName();
			}
		}
		
		{
			SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
			SvCmd::InspectionCommands(inspectionID, rRequest, &response);
		}

		///@WARNING [gra][10.10][21.05.2021] This next section to update SVIPDoc should be done in the inspection commands
		SVIPDoc* pDoc = GetIPDocByInspectionID(inspectionID);
		if (S_OK == response.hresult() && nullptr != pDoc)
		{
			switch (rRequest.message_case())
			{
				case SvPb::InspectionCmdRequest::kDeleteObjectRequest:
				{
					if (false == deleteObjectName.empty())
					{
						pDoc->GetToolGroupings().RemoveTool(deleteObjectName);
					}
					pDoc->RunOnce();
					pDoc->SetModifiedFlag();

					HWND hMainWnd = SVVisionProcessorHelper::Instance().getMainhWnd();
					if (nullptr != hMainWnd)
					{
						::PostMessage(hMainWnd, SV_UPDATE_IPDOC_VIEWS, static_cast<WPARAM> (inspectionID), static_cast<LPARAM> (SVIPDoc::SVIPViewUpdateHints::RefreshDelete));
					}
					break;
				}
				case SvPb::InspectionCmdRequest::kCreateObjectRequest:
				{
					SvOi::IObjectClass* pObject = SvOi::getObject(response.createobjectresponse().objectid());
					SvOi::IObjectClass* pParent = (nullptr != pObject) ? SvOi::getObject(pObject->GetParentID()) : nullptr;
					pObject = SvOi::getObject(rRequest.createobjectrequest().taskobjectinsertbeforeid());
					//Only when a tools parent is the Tool Set then the name needs to be added to the tool set grouping
					if (nullptr != pParent && nullptr != pObject && SvPb::SVToolSetObjectType == pParent->GetObjectType())
					{
						pDoc->GetToolGroupings().AddTool(response.createobjectresponse().name(), pObject->GetName());
					}
					pDoc->RunOnce();
					pDoc->SetModifiedFlag();
					pDoc->SetSelectedToolID(response.createobjectresponse().objectid());

					HWND hMainWnd = SVVisionProcessorHelper::Instance().getMainhWnd();
					if (nullptr != hMainWnd)
					{
						::PostMessage(hMainWnd, SV_UPDATE_IPDOC_VIEWS, inspectionID, static_cast<WPARAM> (SVIPDoc::SVIPViewUpdateHints::RefreshView));
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	task.finish(std::move(response));
}

void SVRCCommand::GetConfigurationTree(const SvPb::GetConfigurationTreeRequest&, SvRpc::Task<SvPb::GetConfigurationTreeResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::GetConfigurationTreeResponse response;

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig{ nullptr };
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			std::vector<SvPb::ConfigTreeItem> configVector;

			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				const SVInspectionProcessVector& rInspectionVector = pConfig->GetInspections();
				for (const auto* pInspection : rInspectionVector)
				{
					if (nullptr != pInspection)
					{
						uint32_t inspectionID {pInspection->getObjectId()};
						std::vector<uint32_t> objectVector;
						objectVector.emplace_back(inspectionID);
						SVToolSet* pToolSet = pInspection->GetToolSet();
						if (nullptr != pToolSet)
						{
							uint32_t toolsetID {pToolSet->getObjectId()};
							addObjectChildren(inspectionID, toolsetID, std::back_inserter(objectVector));

							for (const auto& rObjectID : objectVector)
							{
								addConfigItem(inspectionID, rObjectID, std::back_inserter(configVector));
							}
						}
					}
				}
			}
			SvPb::convertVectorToTree(configVector, response.mutable_tree());
		}
	}

	task.finish(std::move(response));
}

void SVRCCommand::ConfigCommand(const SvPb::ConfigCommandRequest& rRequest, SvRpc::Task<SvPb::ConfigCommandResponse> task)
{
	HRESULT result{SVSVIMStateClass::CheckNotAllowedState()};
	SvPb::ConfigCommandResponse response;

	if (S_OK == result)
	{
		switch (rRequest.message_case())
		{
			case SvPb::ConfigCommandRequest::kClipboardRequest:
			{
				clipboardAction(rRequest.clipboardrequest(), response.mutable_standardresponse());
				break;
			}
			case SvPb::ConfigCommandRequest::kConfigDataRequest:
			{
				SvPb::ConfigDataResponse* pConfigDataResponse = response.mutable_configdataresponse();
				if (nullptr != pConfigDataResponse)
				{
					SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
					pConfigDataResponse->set_configfileloaded(GlobalRCGetConfigurationName(false));
					pConfigDataResponse->set_lastmodified(static_cast<unsigned long>(SVSVIMStateClass::getLastModifiedTime()));
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	task.finish(std::move(response));
}

void SVRCCommand::SetTriggerConfig(const SvPb::SetTriggerConfigRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	HRESULT result{ SVSVIMStateClass::CheckNotAllowedState(SV_DEFAULT_NOT_ALLOWED_STATES  |SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) };

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig{ nullptr };
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			result = E_INVALIDARG;
			std::string fileName = SvUl::to_ansi(rRequest.plcsimulatefile());

			int ppqCount = pConfig->GetPPQCount();
			for (int i = 0; i < ppqCount; ++i)
			{
				SVPPQObject* pPPQ = pConfig->GetPPQ(i);
				if (nullptr != pPPQ && nullptr != pPPQ->GetTrigger())
				{
					SvTrig::SVTriggerClass* pTrigger = pPPQ->GetTrigger()->getDevice();
					if (nullptr != pTrigger && nullptr != pTrigger->getDLLTrigger())
					{
						_variant_t plcSimulatedFile;
						plcSimulatedFile.SetString(fileName.c_str());
						SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
						result = pTrigger->getDLLTrigger()->SetParameterValue(pTrigger->getTriggerChannel() + 1, SVIOParameterEnum::PlcSimulatedTrigger, plcSimulatedFile);
						break;
					}
				}
			}
		}
	}
	SvPb::StandardResponse response;
	response.set_hresult(result);
	task.finish(std::move(response));
}

void SVRCCommand::GetConfigurationInfo(const SvPb::GetConfigurationInfoRequest&, SvRpc::Task<SvPb::GetConfigurationInfoResponse> task)
{
	constexpr DWORD cNotAllowedStates = SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING | SV_STATE_CANCELING | SV_STATE_INTERNAL_RUN | 
		SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING | SV_STATE_REMOTE_CMD;

	SvPb::GetConfigurationInfoResponse Response;

	if (SVSVIMStateClass::CheckState(cNotAllowedStates) == false)
	{
		SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
		Response.set_filename(GlobalRCGetConfigurationName(false).c_str());
		Response.set_lastmodified(SVSVIMStateClass::getLastModifiedTime());
		Response.set_loadedsince(SVSVIMStateClass::getLoadedSinceTime());
		Response.set_hash(SVSVIMStateClass::GetHash());
	}

	task.finish(std::move(Response));

}

void SVRCCommand::SoftwareTrigger(const SvPb::SoftwareTriggerRequest& rRequest, SvRpc::Task<SvPb::StandardResponse> task)
{
	DWORD checkAdditional = (0 < rRequest.period()) ? SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION : 0;
	HRESULT result{ SVSVIMStateClass::CheckNotAllowedState(SV_DEFAULT_NOT_ALLOWED_STATES  | checkAdditional) };

	if (S_OK == result)
	{
		SVConfigurationObject* pConfig{ nullptr };
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		if (nullptr != pConfig)
		{
			result = E_INVALIDARG;
			
			int ppqCount = pConfig->GetPPQCount();
			for (int i = 0; i < ppqCount; ++i)
			{
				SVPPQObject* pPPQ = pConfig->GetPPQ(i);
				if (nullptr != pPPQ && nullptr != pPPQ->GetTrigger())
				{
					for (long j = 0; j < pPPQ->GetInspectionCount(); ++j)
					{
						SVInspectionProcess* pInspection{ nullptr };
						pPPQ->GetInspection(j, pInspection);
						if (nullptr != pInspection)
						{
							std::string inspectionName = SvUl::to_ansi(rRequest.inspectionname());
							if (pInspection->GetName() == inspectionName)
							{
								SvTrig::SVTriggerObject* pTrigger = pPPQ->GetTrigger();
								if (nullptr != pTrigger)
								{
									//Period of 0 is a single trigger
									if (0 == rRequest.period())
									{
										SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras();
										bool canExternalSoftwareTrigger {cameraVector.size() > 0};
										canExternalSoftwareTrigger &= std::all_of(cameraVector.begin(), cameraVector.end(), [](const auto* pCamera) { return pCamera->canExternalSoftwareTrigger(); });
										canExternalSoftwareTrigger |= SvDef::TriggerType::SoftwareTrigger == pTrigger->getType();
										if (canExternalSoftwareTrigger)
										{
											pTrigger->Fire();
											result = S_OK;
										}
										else
										{
											result = E_FAIL;
										}
									}
									else
									{
										SvTrig::ObjectIDParameters ObjectIDParams;
										ObjectIDParams.m_startObjectID = rRequest.startobjectid();
										ObjectIDParams.m_triggerPerObjectID = rRequest.triggerperobjectid();
										ObjectIDParams.m_objectIDCount = rRequest.numberobjectid();
										pTrigger->setObjectIDParameters(ObjectIDParams);
										pTrigger->SetSoftwareTriggerPeriod(rRequest.period());
										result = S_OK;
									}
								}
								else
								{
									result = E_POINTER;
								}
							}
						}
					}
				}
			}
		}
	}
	SvPb::StandardResponse response;
	response.set_hresult(result);
	task.finish(std::move(response));
}

void SVRCCommand::RegisterNotificationStream(const SvPb::GetNotificationStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetNotificationStreamResponse>& rObserver,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	SVVisionProcessorHelper::Instance().RegisterNotificationStream(rRequest, rObserver, ctx);
}

void SVRCCommand::RegisterMessageStream(const SvPb::GetMessageStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetMessageStreamResponse>& rObserver,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	SVVisionProcessorHelper::Instance().RegisterMessageStream(rRequest, rObserver, ctx);
}


std::string SVRCCommand::GetFileNameFromFilePath(const std::string& rFilePath, const std::string& rExtension /*= std::string()*/) const
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

HRESULT SVRCCommand::ConvertStorageValueToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue) const
{
	HRESULT Result{ S_OK };

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

HRESULT SVRCCommand::ConvertStorageImageToProtobuf(const std::string& rName, const SVStorageResult& rStorage, SvPb::Value* pValue) const
{
	HRESULT Result{ S_OK };

	if (nullptr != pValue && SVVisionProcessor::SVStorageImageFileName == rStorage.m_Storage.m_StorageType && VT_BSTR == rStorage.m_Storage.m_Variant.vt)
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
				pValue->mutable_item()->set_count(static_cast<uint32_t> (FileData.size()));
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

HRESULT SVRCCommand::AddValuesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems) const
{
	HRESULT Result{ S_OK };

	for (int i = 0; i < rRequest.valuelist_size(); i++)
	{
		const SvPb::Value& rValue = rRequest.valuelist(i);

		std::string name = SvUl::to_ansi(rValue.name());

		if (rItems.end() == rItems.find(name))
		{
			SVStorage Storage;
			Storage.m_StorageType = SVVisionProcessor::SVStorageValue;
			Result = SvPb::ConvertProtobufToVariant(rValue.item(), Storage.m_Variant);
			if (S_OK == Result)
			{
				rItems.insert(std::pair(name, SVStorage(Storage.m_StorageType, Storage.m_Variant)));
			}
		}
	}

	return Result;
}

HRESULT SVRCCommand::AddImagesToStorageItems(const SvPb::SetItemsRequest& rRequest, SVNameStorageMap& rItems) const
{
	HRESULT Result{ S_OK };

	for (int i = 0; i < rRequest.imagelist_size(); i++)
	{
		const SvPb::Value& rImage = rRequest.imagelist(i);
		std::string name = SvUl::to_ansi(rImage.name());
		std::string FilePath;

		if (rImage.item().type() == (VT_ARRAY | VT_UI1))
		{
			const std::string& rContent = rImage.item().bytesval();
			std::string FileName{ name };
			FileName += _T(".bmp");
			FilePath = SvStl::GlobalPath::Inst().GetPathOnRamDrive(GetFileNameFromFilePath(FileName).c_str());
			if (!FilePath.empty())
			{
				Result = SVEncodeDecodeUtilities::StringContentToFile(FilePath, rContent);
			}
		}
		else if (rImage.item().type() == VT_BSTR)
		{
			FilePath = SvStl::GlobalPath::Inst().GetPathOnRamDrive(rImage.item().strval().c_str());
		}

		if (S_OK == Result)
		{

			if (rItems.end() == rItems.find(name))
			{
				SVStorage Storage;
				Storage.m_StorageType = SVVisionProcessor::SVStorageImageFileName;
				Storage.m_Variant.SetString(FilePath.c_str());
				rItems.insert(std::pair(name, SVStorage(Storage.m_StorageType, Storage.m_Variant)));
			}

		}
	}

	return Result;
}

void SVRCCommand::ConvertTreeNames(SvPb::TreeItem* pTreeItem) const
{
	if (nullptr != pTreeItem)
	{
		pTreeItem->set_name(SvUl::to_utf8(pTreeItem->name()));
		for (int i = 0; i < pTreeItem->children_size(); i++)
		{
			ConvertTreeNames(pTreeItem->mutable_children(i));
		}
	}
}

void SVRCCommand::addObjectChildren(uint32_t inspectionID, uint32_t parentID, std::back_insert_iterator<std::vector<uint32_t>> inserter) const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_taskobjectlistrequest();
	pRequest->set_taskobjectid(parentID);

	SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
	if (true == responseCmd.has_taskobjectlistresponse())
	{
		for (int i = 0; i < responseCmd.taskobjectlistresponse().taskobjectinfos_size(); ++i)
		{
			auto rTaskObj = responseCmd.taskobjectlistresponse().taskobjectinfos(i);
			SvOi::IObjectClass* pTaskObject = SvOi::getObject(rTaskObj.taskobjectid());
			if (nullptr != pTaskObject && SvPb::SVToolObjectType == pTaskObject->GetObjectType())
			{
				inserter = rTaskObj.taskobjectid();
				addObjectChildren(inspectionID, rTaskObj.taskobjectid(), inserter);
			}
		}
	}
}

void SVRCCommand::addConfigItem(uint32_t inspectionID, uint32_t objectID, std::back_insert_iterator<std::vector<SvPb::ConfigTreeItem>> inserter) const
{
	SvOi::IObjectClass* pObject = SvOi::getObject(objectID);

	if (nullptr != pObject)
	{
		std::vector<std::string> toolAdjustNameList;
		std::string text(pObject->GetName());
		bool isValid{ true };
		long toolPos{ -1 };
		if (SvPb::SVToolObjectType == pObject->GetObjectType())
		{
			SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (pObject);
			if (nullptr != pTool)
			{
				toolPos = pTool->getToolPosition();
				toolAdjustNameList = pTool->getToolAdjustNameList();
			}
			isValid = false;
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			SvPb::GetObjectParametersRequest* pIsValidRequest = requestCmd.mutable_getobjectparametersrequest();
			pIsValidRequest->set_objectid(objectID);

			HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
			{
				isValid = responseCmd.getobjectparametersresponse().isvalid();
			}
		}
		SvPb::ConfigTreeItem item;
		item.set_name(SvUl::to_utf8(text));
		text = pObject->GetObjectNameToObjectType(SvPb::SVInspectionObjectType);
		///Tool Set should be removed from the location name
		SvUl::searchAndReplace(text, _T("Tool Set."), _T(""));
		item.set_location(SvUl::to_utf8(text));
		item.set_objectid(objectID);
		item.set_isvalid(isValid);
		item.set_position(toolPos);
		item.set_objecttype(pObject->GetObjectType());
		item.set_objectsubtype(pObject->GetObjectSubType());
		inserter = item;

		//For every tool adjust name an entry is made in the config tree with the object id from the parent tool
		for (const auto& rName : toolAdjustNameList)
		{
			item.set_name(SvUl::to_utf8(rName));
			text = pObject->GetObjectNameToObjectType(SvPb::SVInspectionObjectType);
			text += '.' + rName;
			///Tool Set should be removed from the location name
			SvUl::searchAndReplace(text, _T("Tool Set."), _T(""));
			item.set_location(SvUl::to_utf8(text));
			toolPos = -1;
			item.set_position(toolPos);
			item.set_objecttype(SvPb::SVNotSetObjectType);
			item.set_objectsubtype(SvPb::SVNotSetSubObjectType);
			inserter = item;
		}
	}
}

void SVRCCommand::clipboardAction(const SvPb::ClipboardRequest rRequest, SvPb::StandardResponse* pResponse) const
{
	if (nullptr == pResponse)
	{
		return;
	}

	switch (rRequest.action())
	{
		case SvPb::ClipboardActionEnum::Copy:
		{
			ToolClipboard toolClipboard;
			HRESULT result {S_OK};

			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				result = toolClipboard.writeXmlToolData({rRequest.objectid()});
			}
			pResponse->set_hresult(result);
			if (S_OK != result)
			{
				const auto& rMessage = toolClipboard.getLastErrorMessage().getMessageContainer();
				convertMessageToProtobuf(rMessage, pResponse->mutable_errormessages()->add_messages());
			}
			break;
		}
		case SvPb::ClipboardActionEnum::Paste:
		{
			if (toolClipboardDataPresent())
			{
				ToolClipboard toolClipboard;
				uint32_t postID{ rRequest.objectid() };
				SvOi::IObjectClass* pObject = SvOi::getObject(postID);
				if (nullptr != pObject)
				{
					uint32_t ownerID = pObject->GetParentID();
					std::vector<uint32_t> pastedToolIDs;
					HRESULT result {S_OK};

					{
						SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
						auto XmlData = toolClipboard.readXmlToolData();
						if (false == XmlData.empty())
						{
							pastedToolIDs = toolClipboard.createToolsFromXmlData(XmlData, ownerID);
						}
					}
					pResponse->set_hresult(result);
					if (S_OK == result)
					{
						SvOi::IObjectClass* pInspection = pObject->GetAncestorInterface(SvPb::SVInspectionObjectType);
						///@WARNING [gra][10.10][21.05.2021] SVIPDoc should be done in the inspection commands
						SVIPDoc* pDoc = (nullptr != pInspection) ? GetIPDocByInspectionID(pInspection->getObjectId()) : nullptr;
						if (nullptr != pDoc)
						{
							pDoc->updateToolsetView(pastedToolIDs, postID, ownerID);
						}
					}
					else
					{
						const auto& rMessage = toolClipboard.getLastErrorMessage().getMessageContainer();
						convertMessageToProtobuf(rMessage, pResponse->mutable_errormessages()->add_messages());
					}
				}
			}
			else
			{
				SvStl::MessageManager message(SvStl::MsgType::Log);
				message.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
				convertMessageToProtobuf(message.getMessageContainer(), pResponse->mutable_errormessages()->add_messages());
			}
			break;
		}
		case SvPb::ClipboardActionEnum::Cut:
		{
			ToolClipboard toolClipboard;
			HRESULT result {S_OK};

			{
				SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
				result = toolClipboard.writeXmlToolData({rRequest.objectid()});
			}
			pResponse->set_hresult(result);
			if (S_OK == result)
			{
				SvOi::IObjectClass* pObject = SvOi::getObject(rRequest.objectid());
				SvOi::IObjectClass* pInspection = (nullptr != pObject) ? pObject->GetAncestorInterface(SvPb::SVInspectionObjectType) : nullptr;
				if (nullptr != pInspection)
				{
					std::string deleteObjectName;
					SvOi::IObjectClass* pParent = SvOi::getObject(pObject->GetParentID());
					//Only when a tools parent is the Tool Set then the name needs to be removed from the tool set grouping
					if (nullptr != pParent && SvPb::SVToolSetObjectType == pParent->GetObjectType())
					{
						deleteObjectName = pObject->GetName();
					}
					uint32_t inspectionID{pInspection->getObjectId()};
					SvPb::InspectionCmdRequest requestCmd;
					SvPb::InspectionCmdResponse responseCmd;;
					requestCmd.set_inspectionid(inspectionID);
					SvPb::DeleteObjectRequest* pDelObj = requestCmd.mutable_deleteobjectrequest();
					pDelObj->set_objectid(rRequest.objectid());

					SVSVIMStateClass::SetResetState remoteCmd {SV_STATE_REMOTE_CMD};
					SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
					if (false == deleteObjectName.empty())
					{
						///@WARNING [gra][10.10][21.05.2021] SVIPDoc should be done in the inspection commands
						SVIPDoc* pDoc = GetIPDocByInspectionID(pInspection->getObjectId());
						if (nullptr != pDoc)
						{
							pDoc->GetToolGroupings().RemoveTool(deleteObjectName);
							pDoc->RunOnce();
							pDoc->SetModifiedFlag();
						}
					}
					HWND hMainWnd = SVVisionProcessorHelper::Instance().getMainhWnd();
					if (nullptr != hMainWnd)
					{
						::PostMessage(hMainWnd,SV_UPDATE_IPDOC_VIEWS, inspectionID, static_cast<WPARAM> (SVIPDoc::SVIPViewUpdateHints::RefreshDelete));
					}
				}
			}
			else
			{
				const auto& rMessage = toolClipboard.getLastErrorMessage().getMessageContainer();
				convertMessageToProtobuf(rMessage, pResponse->mutable_errormessages()->add_messages());
			}
			break;
		}
		default:
		{
			break;
		}
	}
}