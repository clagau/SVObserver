//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file RemoteControlImpl.cpp
/// (see header file of the same name)
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EventLog.h"
#include "RCSettingsLoader.h"
#include "RemoteControlImpl.h"
#include "RemoteStructs.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary/CommandLineArgs.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "WebsocketLibrary\RunRequest.inl"
#pragma endregion Includes

#define HANDLE_EXCEPTION(Status)\
catch (std::invalid_argument & e)\
{\
Status = E_INVALIDARG; \
SVLog(e.what(), __FILE__, __LINE__); \
}\
catch (boost::system::system_error &e)\
{\
Status = E_FAIL; \
SVLog(e.what(), __FILE__, __LINE__); \
}\
catch (std::exception & e)\
{\
Status = E_UNEXPECTED; \
SVLog(e.what(), __FILE__, __LINE__); \
}\
catch (...)\
{\
Status = E_UNEXPECTED; \
SVLog(_T("Unknown exception"), __FILE__, __LINE__); \
}

namespace SvRc
{
RemoteControlImpl::RemoteControlImpl() :
	m_notificationHandler{*this}
{
	HINSTANCE hInst = ::GetModuleHandle(_T("RemoteControl.dll"));
	std::string IniFile;
	SvStl::CommandLineArgs::GetModulDirName(IniFile, hInst);
	IniFile.append("\\SVRemoteCtrl.ini");
	RCSettingsLoader settingsLoader;
	settingsLoader.loadFromIni(IniFile.c_str(), m_settings);
	try
	{
		SvLog::init_logging(m_settings.m_logSettings);
	}
	catch (std::runtime_error& rRuntimeError)
	{
		SV_LOG_GLOBAL(error) << std::string(rRuntimeError.what());
	}
	SV_LOG_GLOBAL(info) << "SVRemotecontrolIniPath:" << IniFile;
}

RemoteControlImpl::~RemoteControlImpl()
{
	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();
}

RemoteControlImpl& RemoteControlImpl::Instance()
{
	static RemoteControlImpl remoteControlImpl;
	return remoteControlImpl;
}

HRESULT RemoteControlImpl::Connect(LPCTSTR serverName, long timeout)
{
	HRESULT result{ S_OK };
	m_pRpcClient.reset();
	m_pSvrcClientService.reset();
	SV_LOG_GLOBAL(info) << "Connect";
	boost::posix_time::milliseconds connectTimeout(timeout);

	if (nullptr != serverName)
	{
		//First try to connect to Gateway
		m_settings.m_httpClientSettings.Host = serverName;
		m_pRpcClient = std::make_unique<SvRpc::RPCClient>(m_settings.m_httpClientSettings);

		if (nullptr != m_pRpcClient)
		{
			m_pRpcClient->addStatusListener(std::bind(&RemoteControlImpl::OnConnectionStatus, this, std::placeholders::_1));
			m_pSvrcClientService = std::make_unique<SvWsl::SVRCClientService>(*m_pRpcClient, m_settings.m_svrcClientSettings);
			m_pRpcClient->waitForConnect(connectTimeout);

			if (false == m_pRpcClient->isConnected())
			{
				m_pRpcClient = std::make_unique<SvRpc::RPCClient>(m_settings.m_httpClientSettings);
				m_pRpcClient->addStatusListener(std::bind(&RemoteControlImpl::OnConnectionStatus, this, std::placeholders::_1));
				m_pSvrcClientService = std::make_unique<SvWsl::SVRCClientService>(*m_pRpcClient, m_settings.m_svrcClientSettings);
				m_pRpcClient->waitForConnect(connectTimeout);
			}

		}
		if (nullptr != m_pRpcClient && m_pRpcClient->isConnected())
		{
			StartNotificationStreaming();
		}
		else
		{
			result = RPC_E_TIMEOUT;
			m_pSvrcClientService.reset();
			m_pRpcClient.reset();
		}
	}
	else
	{
		result = INET_E_INVALID_URL;
	}
	SVLog(result, __FILE__, __LINE__);
	return result;
}

void RemoteControlImpl::Disconnect()
{
	SV_LOG_GLOBAL(info) << "ResetConnection";
	StopNotificationStreaming();
	if (m_pRpcClient)
	{
		m_pRpcClient->stop();
	}
	m_pRpcClient.reset(nullptr);
	m_pSvrcClientService.reset(nullptr);
}

HRESULT RemoteControlImpl::LoadConfig(LPCTSTR sourceFile) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		std::string filePath = SvUl::to_utf8(std::string(sourceFile));
		if (!filePath.empty())
		{
			SvPb::LoadConfigRequest Request;
			Request.set_filename(filePath);
			SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::LoadConfig,
				std::move(Request)).get();
			result = Response.hresult();
		}
		else
		{
			result = E_INVALIDARG;
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetConfig(LPCTSTR destinationFile) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//rFilePath is local so change it to ANSI
		std::string destinationPath = destinationFile;
		if (destinationPath.length() > 0)
		{
			SvPb::GetConfigRequest Request;
			Request.set_filename(SvUl::to_utf8(destinationPath));
			SvPb::GetConfigResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::GetConfig,
				std::move(Request)).get();

			result = Response.hresult();

			if (S_OK == result)
			{
				std::ofstream FileStream;

				FileStream.open(destinationPath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
				if (FileStream.is_open())
				{
					FileStream.write(Response.filedata().c_str(), Response.filedata().length());
					FileStream.close();
				}
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::PutConfig(LPCTSTR sourceFile) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//FilePath is local so change it to ANSI
		std::string sourcePath = sourceFile;
		if (sourcePath.length() > 0)
		{
			std::ifstream fileStream;

			fileStream.open(sourcePath.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
			if (fileStream.is_open())
			{
				size_t FileSize(0);
				FileSize = static_cast<size_t> (fileStream.tellg());
				std::vector<char> FileData;
				if (0 != FileSize)
				{
					FileData.resize(FileSize);
					fileStream.seekg(0, std::ios::beg);
					fileStream.read(&FileData.at(0), FileSize);
					fileStream.close();

					SvPb::PutConfigRequest PutConfigRequest;
					PutConfigRequest.set_filename(SvUl::to_utf8(sourcePath));
					PutConfigRequest.set_filedata(&FileData[0], FileSize);
					SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
						&SvWsl::SVRCClientService::PutConfig,
						std::move(PutConfigRequest)).get();
					result = Response.hresult();
				}
				else
				{
					fileStream.close();
					result = E_INVALIDARG;
				}
			}
			else
			{
				result = E_UNEXPECTED;
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetState(unsigned long& rState) const
{
	HRESULT result{ S_OK };
	
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetStateRequest Request;
		SvPb::GetStateResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetState,
			std::move(Request)).get();

		rState = Response.state();
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::RunOnce(LPCTSTR inspectionName) const
{
	HRESULT result{ S_OK };

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		std::string Inspection = SvUl::to_utf8(std::string(inspectionName));

		if (false == Inspection.empty())
		{
			SvPb::RunOnceRequest Request;
			Request.set_inspectionname(Inspection);
			SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::RunOnce,
				std::move(Request)).get();
			result = Response.hresult();
		}
		else
		{
			result = E_INVALIDARG;
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::GetDeviceMode(long& rMode) const
{
	HRESULT result{ S_OK };
	
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetDeviceModeRequest Request;
		SvPb::GetDeviceModeResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetDeviceMode,
			std::move(Request)).get();

		rMode = static_cast<unsigned long> (Response.mode());
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::SetDeviceMode(long mode) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		if (false == SvPb::DeviceModeType_IsValid(mode))
		{
			throw std::invalid_argument("Invalid SVObserver mode");
		}

		SvPb::SetDeviceModeRequest Request;
		Request.set_mode(static_cast<SvPb::DeviceModeType> (mode));
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::SetDeviceMode,
			std::move(Request)).get();

		result = Response.hresult();
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetVersion(std::string& rSVObserver, std::string& rSVOGateway) const
{
	HRESULT result{ S_OK };
	try
	{
		rSVObserver = _T("N/A");
		rSVOGateway = _T("N/A");
		if (nullptr != m_pRpcClient && nullptr != m_pSvrcClientService && m_pRpcClient->isConnected())
		{
			SvPb::GetSVObserverVersionRequest SvoRequest;
			try
			{
				SvPb::GetVersionResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
					&SvWsl::SVRCClientService::GetSVObserverVersion,
					std::move(SvoRequest)).get();

				rSVObserver = SvUl::to_ansi(Response.version());
			}
			catch (std::exception& e) //catch to get all versions
			{
				result = E_UNEXPECTED;
				SVLog(e.what(), __FILE__, __LINE__);
			}

			try
			{
				SvPb::GetGatewayVersionRequest GatewayRequest;
				SvPb::GetVersionResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
					&SvWsl::SVRCClientService::GetGatewayVersion,
					std::move(GatewayRequest)).get();
				rSVOGateway = SvUl::to_ansi(Response.version());
			}
			catch (std::exception& e) //catch to get all versions
			{
				result = E_UNEXPECTED;
				SVLog(e.what(), __FILE__, __LINE__);
			}
		}
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::GetDeviceReport(std::string& rReport) const
{
	HRESULT result{ S_OK };
	
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetConfigReportRequest Request;
		SvPb::GetConfigReportResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetConfigReport,
			std::move(Request)).get();

		rReport = SvUl::to_ansi(Response.report());

		result = Response.hresult();
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::GetItems(const std::vector<std::string>& rItemNameVector, SvRc::Product& rProduct) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetItemsRequest Request;

		for (const auto& rName : rItemNameVector)
		{
			std::string* pName = Request.add_itemnamelist();
			*pName = SvUl::to_utf8(rName);
		}

		SvPb::GetItemsResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetItems,
			std::move(Request)).get();

		ProtobufToProduct(Response, rProduct);

		result = Response.hresult();
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::SetItems(SvRc::Product& rProduct, bool runOnce) const
{
	HRESULT result{ S_OK };
	
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::SetItemsRequest Request;
		Request.set_runonce(runOnce);
		for(const auto& rValue : rProduct.m_valueVector)
		{
			SvPb::Value* pValue = Request.add_valuelist();
			pValue->set_name(SvUl::to_utf8(rValue.m_name));
			pValue->set_count(rValue.m_count);
			SvPb::ConvertVariantToProtobuf(rValue.m_item, pValue->mutable_item());
		}
		for (const auto& rValue : rProduct.m_imageVector)
		{
			SvPb::Value* pValue = Request.add_imagelist();
			pValue->set_name(SvUl::to_utf8(rValue.m_name));
			pValue->set_count(rValue.m_count);
			SvPb::ConvertVariantToProtobuf(rValue.m_item, pValue->mutable_item());
		}

		SvPb::SetItemsResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::SetItems,
			std::move(Request)).get();

		rProduct.m_errorVector.resize(Response.errorlist_size());
		for (int i = 0; i < Response.errorlist_size(); i++)
		{
			const SvPb::Value& rValue = Response.errorlist(i);
			rProduct.m_errorVector[i].m_name = rValue.name();
			rProduct.m_errorVector[i].m_count = rValue.count();
			SvPb::ConvertProtobufToVariant(rValue.item(), rProduct.m_errorVector[i].m_item);
		}

		result = Response.hresult();
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::GetFile(LPCTSTR sourceFile, LPCTSTR destinationFile) const
{
	HRESULT result{ S_OK };
	
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//DestinationPath is local so change it to ANSI
		std::string destinationPath = destinationFile;
		if (destinationPath.length() > 0)
		{

			SvPb::GetFileRequest Request;
			Request.set_sourcepath(SvUl::to_utf8(std::string(sourceFile)));
			SvPb::GetFileResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::GetFile,
				std::move(Request)).get();

			result = Response.hresult();

			if (S_OK == result)
			{
				std::ofstream FileStream;

				FileStream.open(destinationPath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
				if (FileStream.is_open())
				{
					FileStream.write(Response.filedata().c_str(), Response.filedata().length());
					FileStream.close();
				}
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::PutFile(LPCTSTR sourceFile, LPCTSTR destinationFile, bool saveInConfig) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//SourthPath is local so change it to ANSI
		std::string sourcePath = sourceFile;
		if (sourcePath.length() > 0)
		{
			std::ifstream fileStream;
			fileStream.open(sourcePath.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
			if (fileStream.is_open())
			{
				size_t FileSize(0);
				FileSize = static_cast<size_t> (fileStream.tellg());
				std::vector<char> FileData;
				if (0 != FileSize)
				{
					FileData.resize(FileSize);
					fileStream.seekg(0, std::ios::beg);
					fileStream.read(&FileData.at(0), FileSize);
					fileStream.close();

					SvPb::PutFileRequest Request;
					Request.set_destinationpath(SvUl::to_utf8(std::string(destinationFile)));
					Request.set_filedata(&FileData[0], FileSize);
					Request.set_saveinconfig(saveInConfig);
					SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
						&SvWsl::SVRCClientService::PutFile,
						std::move(Request)).get();
					result = Response.hresult();
				}
				else
				{
					fileStream.close();
					result = E_INVALIDARG;
				}
			}
			else
			{
				result = E_UNEXPECTED;
			}
		}
		else
		{
			result = E_INVALIDARG;
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)
	
	return result;
}

HRESULT RemoteControlImpl::GetInspectionNames(variant_t& rInspectionNames) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetInspectionNamesRequest Request;
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetInspectionNames,
			std::move(Request)).get();

		result = Response.hresult();

		if (S_OK == result)
		{
			SvPb::ConvertProtobufToVariant(Response.names().item(), rInspectionNames);
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetDataDefinitionList(LPCTSTR inspectionName, long listType, std::vector<DataDefinition>& rDataDefVector) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetDataDefinitionListRequest Request;
		Request.set_inspectionname(SvUl::to_utf8(std::string(inspectionName)));
		Request.set_type(static_cast<SvPb::DataDefinitionListType> (listType));
		SvPb::GetDataDefinitionListResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetDataDefinitionList,
			std::move(Request)).get();


		result = Response.hresult();

		if (S_OK == result)
		{
			rDataDefVector.clear();
			for (int i = 0; i < Response.list_size(); i++)
			{
				DataDefinition dataDef;
				dataDef.m_name = SvUl::to_ansi(Response.list(i).name());
				dataDef.m_writeable = Response.list(i).writable();
				dataDef.m_published = Response.list(i).published();
				dataDef.m_type = SvUl::to_ansi(Response.list(i).type());
				for (int j = 0; j < Response.list(i).additionalinfo_size(); j++)
				{
					dataDef.m_additionalInfo.emplace_back(Response.list(i).additionalinfo(j));
				}
				rDataDefVector.emplace_back(std::move(dataDef));
			}
		}
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetDeviceOfflineCount(long& rOfflineCount) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetOfflineCountRequest Request;
		SvPb::GetOfflineCountResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetOfflineCount,
			std::move(Request)).get();
		rOfflineCount = Response.count();
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::ActivateMonitorList(LPCTSTR listName, bool activate) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::ActivateMonitorListRequest Request;
		Request.set_listname(SvUl::to_utf8(std::string(listName)));
		Request.set_activate(activate);
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::ActivateMonitorList,
			std::move(Request)).get();

		result = Response.hresult();

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::QueryMonitorListNames(variant_t& rNames) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::QueryMonitorListNamesRequest Request;
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::QueryMonitorListNames,
			std::move(Request)).get();

		result = Response.hresult();

		if (S_OK == result)
		{
			SvPb::ConvertProtobufToVariant(Response.names().item(), rNames);
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::QueryMonitorList(LPCTSTR listName, SvPb::ListType listType, variant_t& rItemNames) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::QueryMonitorListRequest Request;
		Request.set_listname(SvUl::to_utf8(std::string(listName)));
		Request.set_type(listType);
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::QueryMonitorList,
			std::move(Request)).get();

		result = Response.hresult();
		if (S_OK == result)
		{
			SvPb::ConvertProtobufToVariant(Response.names().item(), rItemNames);
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::RegisterMonitorList(const SvRc::MonitorList& rMonitorList, std::vector<SvRc::Value>& rErrorVector) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::RegisterMonitorListRequest Request;
		Request.set_listname(SvUl::to_utf8(rMonitorList.m_name));
		Request.set_ppqname(SvUl::to_utf8(rMonitorList.m_ppqName));
		Request.set_rejectdepth(rMonitorList.m_rejectDepth);
		StringVectorToProtobuf(rMonitorList.m_productVector, Request.mutable_productitemlist());
		StringVectorToProtobuf(rMonitorList.m_rejectVector, Request.mutable_rejectconditionlist());
		StringVectorToProtobuf(rMonitorList.m_failStatusVector, Request.mutable_failstatuslist());
		SvPb::RegisterMonitorListResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::RegisterMonitorList,
			std::move(Request)).get();

		result = Response.hresult();

		rErrorVector.resize(Response.errorlist_size());
		for (int i = 0; i < Response.errorlist_size(); ++i)
		{
			const SvPb::Value& rValue = Response.errorlist(i);
			rErrorVector[i].m_name = SvUl::to_ansi(rValue.name());
			rErrorVector[i].m_count = rValue.count();
			rErrorVector[i].m_status = rValue.status();
			SvPb::ConvertProtobufToVariant(rValue.item(), rErrorVector[i].m_item);
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetProduct(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetProductRequest Request;
		Request.set_listname(SvUl::to_utf8(std::string(listName)));
		Request.set_triggercount((-1 < triggerCount) ? triggerCount : -1);
		Request.set_pevioustrigger(-1);

		Request.set_nameinresponse(true);
		SvPb::GetProductResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(), &SvWsl::SVRCClientService::GetProduct, std::move(Request)).get();


		result = static_cast<HRESULT> (Response.productitem().status());
		if (Response.productitem().status() == SvPb::State::isValid)
		{
			result = S_OK;
			ProtobufToMonitorListProduct(Response.productitem(), rProduct);
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetReject(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetRejectRequest Request;
		Request.set_listname(SvUl::to_utf8(std::string(listName)));
		Request.set_triggercount((-1 < triggerCount) ? triggerCount : -1);
		Request.set_pevioustrigger(-1);

		Request.set_nameinresponse(true);
		SvPb::GetRejectResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(), &SvWsl::SVRCClientService::GetReject, std::move(Request)).get();

		result = static_cast<HRESULT> (Response.productitem().status());
		if (Response.productitem().status() == SvPb::State::isValid)
		{
			result = S_OK;
			ProtobufToMonitorListProduct(Response.productitem(), rProduct);
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetFailStatus(LPCTSTR listName, std::vector<SvRc::Product>& rProductVector) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetFailStatusRequest FailstatusRequest;
		FailstatusRequest.set_listname(SvUl::to_utf8(std::string(listName)));

		FailstatusRequest.set_nameinresponse(true);
		SvPb::GetFailStatusResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetFailStatus,
			std::move(FailstatusRequest)).get();

		result = static_cast<HRESULT> (Response.status());
		if (SvPb::State::isValid == Response.status())
		{
			result = S_OK;
			rProductVector.resize(Response.products_size());
			for (int i = 0; i < Response.products_size(); i++)
			{
				ProtobufToMonitorListProduct(Response.products(i), rProductVector[i]);
			}
		}

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::GetProductFilter(LPCTSTR listName, unsigned long& rFilter) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetProductFilterRequest Request;
		Request.set_listname(SvUl::to_utf8(std::string(listName)));
		SvPb::GetProductFilterResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetProductFilter,
			std::move(Request)).get();

		rFilter = static_cast<unsigned long> (Response.filter());
		result = Response.hresult();

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

HRESULT RemoteControlImpl::SetProductFilter(LPCTSTR listName, unsigned long filter) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::SetProductFilterRequest Request;
		Request.set_listname(SvUl::to_utf8(std::string(listName)));
		Request.set_filter(static_cast<SvPb::ProductFilterEnum> (filter));
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::SetProductFilter,
			std::move(Request)).get();

		result = Response.hresult();

		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

		return result;
}

HRESULT RemoteControlImpl::ShutDown(long option) const
{
	HRESULT result{ S_OK };
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || false == m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::ShutdownRequest Request;
		//Option == 1 Graceful; Option == 2 Restart
		SvPb::ShutdownOption  optionProto = (2L == option) ? SvPb::ShutdownOption::shutdownSvObserverAndRestartWindows : SvPb::ShutdownOption::shutdownSvObserverAndWindows;
		Request.set_option(optionProto);
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::Shutdown,
			std::move(Request)).get();

		result = Response.hresult();
		SVLog(result, __FILE__, __LINE__);
	}
	HANDLE_EXCEPTION(result)

	return result;
}

void RemoteControlImpl::StartNotificationStreaming()
{
	SvRpc::Observer<SvPb::GetNotificationStreamResponse> NotificationObserver(boost::bind(&NotificationHandler::OnNext, &m_notificationHandler, _1),
		boost::bind(&NotificationHandler::OnFinish, &m_notificationHandler),
		boost::bind(&NotificationHandler::OnError, &m_notificationHandler, _1));

	if (m_pSvrcClientService)
	{
		SV_LOG_GLOBAL(info) << "StartNotificationStreaming";
		m_csx = m_pSvrcClientService->GetNotificationStream(SvPb::GetNotificationStreamRequest(), NotificationObserver);
	}
}

void  RemoteControlImpl::StopNotificationStreaming()
{
	m_csx.cancel();
	m_csx = SvRpc::ClientStreamContext{ nullptr };
	SV_LOG_GLOBAL(info) << "StopNotificationStreaming";

}

void RemoteControlImpl::OnConnectionStatus(SvRpc::ClientStatus Status)
{
	switch (Status)
	{
		case SvRpc::ClientStatus::Connected:
		{
			SV_LOG_GLOBAL(info) << "OnConnectionStatus Connect";
			if (nullptr != m_pNotifier)
			{
				_variant_t connected(_T("Conneted"));
				m_pNotifier(connected, static_cast<long> (NotificationType::Connected));
			}
			StartNotificationStreaming();
			break;
		}
		case SvRpc::ClientStatus::Disconnected:
		{
			SV_LOG_GLOBAL(info) << "OnConnectionStatus Disconnect";
			if (nullptr != m_pNotifier)
			{
				_variant_t disconnected(_T("Disconneted"));
				m_pNotifier(disconnected, static_cast<long> (NotificationType::Disconnected));
			}
			break;
		}
	}
}
void RemoteControlImpl::ProtobufToProduct(SvPb::GetItemsResponse& rResponse, SvRc::Product& rProduct) const
{
	rProduct.m_valueVector.resize(rResponse.valuelist_size());
	for (int i = 0; i < rResponse.valuelist_size(); ++i)
	{
		rProduct.m_valueVector[i] = ProtobufToValue(rResponse.valuelist(i));
	}

	rProduct.m_imageVector.resize(rResponse.imagelist_size());
	for (int i = 0; i < rResponse.imagelist_size(); ++i)
	{
		///SVObserver Version < 10.10  has the variant count value set to 0 which is not correct
		if (0 == rResponse.imagelist(i).item().count())
		{
			rResponse.mutable_imagelist(i)->mutable_item()->set_count(static_cast<uint32_t> (rResponse.imagelist(i).item().bytesval().length()));
		}
		rProduct.m_imageVector[i] = ProtobufToValue(rResponse.imagelist(i));
	}

	rProduct.m_errorVector.resize(rResponse.errorlist_size());
	for (int i = 0; i < rResponse.errorlist_size(); ++i)
	{
		rProduct.m_errorVector[i] = ProtobufToValue(rResponse.errorlist(i));
	}
}

void RemoteControlImpl::ProtobufToMonitorListProduct(const SvPb::Product& rProductItem, SvRc::Product& rProduct) const
{
	rProduct.m_valueVector.resize(rProductItem.values_size());
	for (int i = 0; i < rProductItem.values_size() && i < rProductItem.valuenames_size(); ++i)
	{
		rProduct.m_valueVector[i].m_name = SvUl::to_ansi(rProductItem.valuenames(i));
		rProduct.m_valueVector[i].m_count = rProductItem.trigger();
		rProduct.m_valueVector[i].m_status = S_OK;
		SvPb::ConvertProtobufToVariant(rProductItem.values(i), rProduct.m_valueVector[i].m_item);
	}

	rProduct.m_imageVector.resize(rProductItem.images_size());
	for (int i = 0; i < rProductItem.images_size() && i < rProductItem.imagenames_size(); ++i)
	{
		rProduct.m_imageVector[i].m_name = SvUl::to_ansi(rProductItem.imagenames(i));
		rProduct.m_imageVector[i].m_count = rProductItem.trigger();
		rProduct.m_imageVector[i].m_status = S_OK;

		//Now fetch the image using inspection ID, image index and trigger ID
		SvPb::GetImageFromIdRequest Request;
		Request.mutable_id()->set_inspectionid(rProductItem.images(i).inspectionid());
		Request.mutable_id()->set_imageindex(rProductItem.images(i).imageindex());
		Request.mutable_id()->set_trid(rProductItem.images(i).trid());
		SvPb::GetImageFromIdResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(), &SvWsl::SVRCClientService::GetImageFromId, std::move(Request)).get();


		///Here we convert the image data into a SafeArray of VT_UI1 variant
		VARIANT imageArray;
		::VariantInit(&imageArray);
		imageArray.vt = VT_ARRAY | VT_UI1;
		imageArray.parray = ::SafeArrayCreateVector(VT_UI1, 0, static_cast<unsigned long> (Response.imagedata().rgbdata().length()));
		void* pData{ nullptr };
		rProduct.m_imageVector[i].m_status = ::SafeArrayAccessData(imageArray.parray, &pData);
		if (S_OK == rProduct.m_imageVector[i].m_status)
		{
			memcpy(pData, Response.imagedata().rgbdata().c_str(), Response.imagedata().rgbdata().length());
			::SafeArrayUnaccessData(imageArray.parray);
		}
		rProduct.m_imageVector[i].m_item.Attach(imageArray);
	}
}

SvRc::Value RemoteControlImpl::ProtobufToValue(const SvPb::Value& rValue) const
{
	SvRc::Value result;
	result.m_name = SvUl::to_ansi(rValue.name());
	result.m_count = rValue.count();
	result.m_status = rValue.status();
	SvPb::ConvertProtobufToVariant(rValue.item(), result.m_item);
	return result;
}

void RemoteControlImpl::StringVectorToProtobuf(const std::vector<std::string>& rStringVector, ::google::protobuf::RepeatedPtrField<::std::string>* pStringList) const
{
	for (const auto& rString : rStringVector)
	{
		std::string* pName = pStringList->Add();
		*pName = SvUl::to_utf8(rString);
	}
}
} //namespace SvRc