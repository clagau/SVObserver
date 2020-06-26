//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVControlCommands
//* .File Name       : $Workfile:   SVControlCommands.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.38  $
//* .Check In Date   : $Date:   16 Jan 2015 10:38:30  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <atlenc.h>

//Moved to precompiled header: #include <fstream>

#include "SVControlCommands.h"
#include "SVProductItems.h"
#include "SVValueObject.h"
#include "EventLog.h"
#include "ProtoBufGetter.h"
#include "ProtoBufSetter.h"
#include "WebsocketLibrary\RunRequest.inl"
#include "NotificationHandler.h"
#include "SVLogLibrary/Logging.h"
#include "RCSettings.h"
#include "RCSettingsLoader.h"
#include "SVStatusLibrary/CommandLineArgs.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes


static const WCHAR* const NotConnectedToSVobserver = L"Not connected to neither SVOGateway nor SVObserver";

#define HANDLE_EXCEPTION(Status, CmdStatus )\
catch (std::invalid_argument & e)\
{\
Status = E_INVALIDARG; \
CmdStatus.hResult = E_INVALIDARG; \
CmdStatus.errorText = SvUl::to_utf16(e.what()); \
SVLOG(CmdStatus.errorText.c_str()); \
SV_LOG_GLOBAL(error) << e.what()  << std::endl; \
}\
catch (boost::system::system_error &e)\
{\
Status = E_FAIL; \
CmdStatus.hResult = E_FAIL; \
CmdStatus.errorText = SvUl::to_utf16(e.what()); \
SVLOG(CmdStatus.errorText.c_str()); \
SV_LOG_GLOBAL(error) << e.what()  << std::endl; \
}\
catch (std::exception & e)\
{\
Status = E_UNEXPECTED; \
CmdStatus.hResult = E_UNEXPECTED; \
CmdStatus.errorText = SvUl::to_utf16(e.what()); \
SVLOG(CmdStatus.errorText.c_str()); \
SV_LOG_GLOBAL(error) << e.what()  << std::endl; \
}\
catch (ATL::CAtlException & e)\
{\
Status = e.m_hr; \
CmdStatus.hResult = e.m_hr; \
CmdStatus.errorText = L"ATL exception"; \
SVLOG(CmdStatus.errorText.c_str()); \
SV_LOG_GLOBAL(error) << "ATL exception"  << std::endl; \
}\
catch (...)\
{\
Status = E_UNEXPECTED; \
CmdStatus.hResult = E_UNEXPECTED; \
CmdStatus.errorText = L"Unknown exception"; \
SVLOG(CmdStatus.errorText.c_str()); \
SV_LOG_GLOBAL(error) << "Unknown exception"  << std::endl; \
}

SVControlCommands::SVControlCommands(NotifyFunctor p_Func)
	:m_Notifier(p_Func),
	m_notificationHandler(this)
{

	HINSTANCE hInst = (HINSTANCE)_AtlBaseModule.m_hInst;
	std::string IniFile;
	SvStl::CommandLineArgs::GetModulDirName(IniFile, hInst);
	IniFile.append("\\SVRemoteCtrl.ini");
	RCSettingsLoader settingsLoader;
	settingsLoader.loadFromIni(IniFile.c_str(), m_settings);
	try
	{
		SvLog::init_logging(m_settings.logSettings);
	}
	catch (std::runtime_error& rRuntimeError)
	{
		SV_LOG_GLOBAL(error) << std::string(rRuntimeError.what());
	}
	SV_LOG_GLOBAL(info) << "SVRemotecontrolIniPath:" << IniFile;
	
}


SVControlCommands::~SVControlCommands()
{
}
void SVControlCommands::ResetConnection()
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

HRESULT SVControlCommands::SetConnectionData(const _bstr_t& rServerName, boost::posix_time::time_duration timeout)
{

	HRESULT hr = S_OK;
	m_pRpcClient.reset();
	m_pSvrcClientService.reset();
	SV_LOG_GLOBAL(info) << "SetConnectionData";

	std::string host(rServerName);
	if (0 < host.length())
	{
		//First try to connect to Gateway
		m_settings.httpClientSettings.Host = host;
		m_pRpcClient = std::make_unique<SvRpc::RPCClient>(m_settings.httpClientSettings);
		
		if (nullptr != m_pRpcClient)
		{
			m_pRpcClient->addStatusListener(std::bind(&SVControlCommands::OnConnectionStatus, this, std::placeholders::_1));
			m_pSvrcClientService = std::make_unique<SvWsl::SVRCClientService>(*m_pRpcClient, m_settings.svrcClientSettings);
			m_pRpcClient->waitForConnect(timeout);

			if (!m_pRpcClient->isConnected())
			{
				m_pRpcClient = std::make_unique<SvRpc::RPCClient>(m_settings.httpClientSettings );
				m_pRpcClient->addStatusListener(std::bind(&SVControlCommands::OnConnectionStatus, this, std::placeholders::_1));
				m_pSvrcClientService = std::make_unique<SvWsl::SVRCClientService>(*m_pRpcClient, m_settings.svrcClientSettings);
				m_pRpcClient->waitForConnect(timeout);
			}

		}
		if (nullptr != m_pRpcClient && m_pRpcClient->isConnected())
		{

			StartNotificationStreaming();

		}
		else
		{
			hr = RPC_E_TIMEOUT;
			SVLOG(hr);
			m_pSvrcClientService.reset();
			m_pRpcClient.reset();
		}
	}
	else
	{
		hr = INET_E_INVALID_URL;
		SVLOG(hr);
	}
	return hr;
}

void SVControlCommands::StartNotificationStreaming()
{
	SvRpc::Observer<SvPb::GetNotificationStreamResponse> NotifikationObserver(boost::bind(&NotificationHandler::OnNext, &m_notificationHandler, _1),
		boost::bind(&NotificationHandler::OnFinish, &m_notificationHandler),
		boost::bind(&NotificationHandler::OnError, &m_notificationHandler, _1));

	if (m_pSvrcClientService)
	{
		SV_LOG_GLOBAL(info) << "StartNotificationStreaming";
		m_csx = m_pSvrcClientService->GetNotificationStream(SvPb::GetNotificationStreamRequest(), NotifikationObserver);
	}
}

void  SVControlCommands::StopNotificationStreaming()
{
	m_csx.cancel();
	m_csx = SvRpc::ClientStreamContext{nullptr};
	SV_LOG_GLOBAL(info) << "StopNotificationStreaming";

}


HRESULT SVControlCommands::GetVersion(_bstr_t& rSVObserverVersion, _bstr_t& rGatewayServerVersion, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		rSVObserverVersion = _bstr_t(L"N/A");
		rGatewayServerVersion = _bstr_t(L"N/A");
		if (nullptr != m_pRpcClient && nullptr != m_pSvrcClientService && m_pRpcClient->isConnected())
		{
			SvPb::GetSVObserverVersionRequest SvoRequest;
			try
			{
				SvPb::GetVersionResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
					&SvWsl::SVRCClientService::GetSVObserverVersion,
					std::move(SvoRequest)).get();

				rSVObserverVersion = _bstr_t(SvUl::to_ansi(Response.version()).c_str());
			}
			catch (std::exception&)
			{
				//Catch exceptions to be able to call both versions
			}

			try
			{
				SvPb::GetGatewayVersionRequest GatewayRequest;
				SvPb::GetVersionResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
					&SvWsl::SVRCClientService::GetGatewayVersion,
					std::move(GatewayRequest)).get();
				rGatewayServerVersion = _bstr_t(SvUl::to_ansi(Response.version()).c_str());
			}
			catch (std::exception&)
			{
				//Catch exceptions to be able to call both versions
			}
		}

		SVLOG((Result = rStatus.hResult));
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetState(unsigned long& rState, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetStateRequest Request;
		SvPb::GetStateResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetState,
			std::move(Request)).get();

		rState = Response.state();
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetOfflineCount(unsigned long& rCount, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetOfflineCountRequest Request;
		SvPb::GetOfflineCountResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetOfflineCount,
			std::move(Request)).get();
		rCount = Response.count();
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetConfigReport(BSTR& rReport, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetConfigReportRequest Request;
		SvPb::GetConfigReportResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetConfigReport,
			std::move(Request)).get();


		rReport = _bstr_t(SvUl::to_ansi(Response.report()).c_str()).Detach();

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG((Result = rStatus.hResult));
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetMode(unsigned long&  rMode, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetDeviceModeRequest Request;
		SvPb::GetDeviceModeResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetDeviceMode,
			std::move(Request)).get();

		rStatus.hResult = S_OK;
		auto ProtoMode = Response.mode();
		//@Todo[MEC][8.00] [16.03.2018] make a function to conver enums
		rMode = static_cast<unsigned long> (ProtoMode);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)
		return Result;
}

HRESULT SVControlCommands::SetMode(unsigned long Mode, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		if (false == SvPb::DeviceModeType_IsValid(Mode))
		{
			throw std::invalid_argument("Invalid SVObserver mode");
		}

		SvPb::SetDeviceModeRequest Request;
		Request.set_mode(static_cast<SvPb::DeviceModeType> (Mode));
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::SetDeviceMode,
			std::move(Request)).get();

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)
		return Result;
}

HRESULT SVControlCommands::GetItems(CComVariant ItemNames, ISVProductItems** ppItems, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetItemsRequest Request;

		if (ItemNames.vt == VT_BSTR)
		{
			std::string* pName = Request.add_itemnamelist();
			*pName = SvUl::to_utf8(_bstr_t(ItemNames.bstrVal, false));
		}
		else if (ItemNames.vt == (VT_ARRAY | VT_BSTR))
		{
			SAFEARRAY* pArray = ItemNames.parray;
			LONG LowerBound;
			LONG UpperBound;

			::SafeArrayGetLBound(pArray, 1, &LowerBound);
			::SafeArrayGetUBound(pArray, 1, &UpperBound);
			BSTR* pData;
			::SafeArrayAccessData(pArray, reinterpret_cast<void**> (&pData));
			for (LONG i = LowerBound; i <= UpperBound; i++)
			{
				_bstr_t bString(pData[i], false);

				std::string* pName = Request.add_itemnamelist();
				*pName = SvUl::to_utf8(bString);
				bString.Detach();
			}
			::SafeArrayUnaccessData(pArray);
		}
		else
		{
			throw CAtlException(OLE_E_CANTCONVERT);
		}

		SvPb::GetItemsResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetItems,
			std::move(Request)).get();
		

		GetItemsPtr(Response)->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppItems));

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::SetItems(ISVProductItems* pItems, bool RunOnce, ISVProductItems ** ppErrors, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		if (nullptr != pItems)
		{
			SvPb::SetItemsRequest Request;
			Request.set_runonce(RunOnce);
			SetItemsRequest(ProductPtr(pItems), &Request);
			SvPb::SetItemsResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::SetItems,
				std::move(Request)).get();

			GetItemsPtr(Response.mutable_errorlist())->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppErrors));

			Result = Response.hresult();
		}
		else
		{
			Result = E_POINTER;
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

static bool IsValidFilePath(const std::string& rFilePath)
{
	bool bRetVal = false;

	// check for valid characters ?
	size_t len = rFilePath.length();
	if (len)
	{
		// check that drive and directory exists
		bRetVal = true;
	}
	return bRetVal;
}

HRESULT SVControlCommands::RunOnce(const _bstr_t& rInspectionName, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		std::string Inspection = SvUl::to_utf8(rInspectionName);

		if (!Inspection.empty())
		{
			SvPb::RunOnceRequest Request;
			Request.set_inspectionname(Inspection);
			SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::RunOnce,
				std::move(Request)).get();
			Result = Response.hresult();
			rStatus.hResult = Result;
			rStatus.errorText = ConvertResult(Result);
		}
		else
		{
			Result = E_INVALIDARG;
			rStatus.hResult = Result;
			rStatus.errorText = L"Inspection name missing";
		}

		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//rFilePath is local so change it to ANSI
		std::string fPath = SvUl::createStdString(rFilePath);
		if (IsValidFilePath(fPath))
		{
			SvPb::GetConfigRequest Request;
			Request.set_filename(SvUl::to_utf8(fPath));
			SvPb::GetConfigResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::GetConfig,
				std::move(Request)).get();

			Result = Response.hresult();
			rStatus.hResult = Result;
			rStatus.errorText = ConvertResult(Result);

			if (S_OK == Result)
			{
				std::ofstream FileStream;

				FileStream.open(fPath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
				if (FileStream.is_open())
				{
					FileStream.write(Response.filedata().c_str(), Response.filedata().length());
					FileStream.close();
				}
			}
			else
			{
				SVLOG(Result);
			}
		}
		else
		{
			Result = E_INVALIDARG;
			SVLOG(Result);

			rStatus.hResult = Result;
			rStatus.errorText = L"Invalid File Path";
		}
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::PutConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//FilePath is local so change it to ANSI
		std::string fPath = SvUl::createStdString(rFilePath);
		if (IsValidFilePath(fPath))
		{
			std::ifstream FileStream;

			FileStream.open(fPath.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
			if (FileStream.is_open())
			{
				size_t FileSize(0);
				FileSize = static_cast<size_t> (FileStream.tellg());
				std::vector<char> FileData;
				if (0 != FileSize)
				{
					FileData.resize(FileSize);
					FileStream.seekg(0, std::ios::beg);
					FileStream.read(&FileData.at(0), FileSize);
					FileStream.close();

					SvPb::PutConfigRequest PutConfigRequest;
					PutConfigRequest.set_filename(SvUl::to_utf8(rFilePath));
					PutConfigRequest.set_filedata(&FileData[0], FileSize);
					SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
						&SvWsl::SVRCClientService::PutConfig,
						std::move(PutConfigRequest)).get();
					Result = Response.hresult();
				}
				else
				{
					FileStream.close();
					Result = E_INVALIDARG;
				}
			}
			else
			{
				Result = E_UNEXPECTED;
			}

			rStatus.hResult = Result;
			rStatus.errorText = ConvertResult(Result);
			SVLOG(Result);
		}
		else
		{
			Result = E_INVALIDARG;
			SVLOG(Result);

			rStatus.hResult = Result;
			rStatus.errorText = L"Invalid File Path";
		}
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::LoadConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		std::string fPath = SvUl::to_utf8(rFilePath);
		if (!fPath.empty())
		{
			SvPb::LoadConfigRequest Request;
			Request.set_filename(fPath);
			SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::LoadConfig,
				std::move(Request)).get();
			Result = Response.hresult();
			rStatus.hResult = Result;
			rStatus.errorText = ConvertResult(Result);
			SVLOG(Result);
		}
		else
		{
			Result = E_INVALIDARG;
			SVLOG(Result);

			rStatus.hResult = Result;
			rStatus.errorText = L"Invalid File Path";
		}
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//DestinationPath is local so change it to ANSI
		std::string DestinationPath = SvUl::createStdString(rDestinationPath);
		if (IsValidFilePath(DestinationPath))
		{

			SvPb::GetFileRequest Request;
			Request.set_sourcepath(SvUl::to_utf8(rSourcePath));
			SvPb::GetFileResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
				&SvWsl::SVRCClientService::GetFile,
				std::move(Request)).get();

			Result = Response.hresult();
			rStatus.hResult = Result;
			rStatus.errorText = ConvertResult(Result);

			if (S_OK == Result)
			{
				std::ofstream FileStream;

				FileStream.open(DestinationPath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
				if (FileStream.is_open())
				{
					FileStream.write(Response.filedata().c_str(), Response.filedata().length());
					FileStream.close();
				}
			}
			else
			{
				SVLOG(Result);
			}
		}
		else
		{
			Result = E_INVALIDARG;
			SVLOG(Result);

			rStatus.hResult = Result;
			rStatus.errorText = L"Invalid File Path";
		}
		rStatus.hResult = Result;
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::PutFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, bool saveInConfig, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		//SourthPath is local so change it to ANSI
		std::string SourcePath = SvUl::createStdString(rSourcePath);
		std::string DestinationPath = SvUl::to_utf8(rDestinationPath);
		if (IsValidFilePath(SourcePath))
		{
			std::ifstream FileStream;

			FileStream.open(SourcePath.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
			if (FileStream.is_open())
			{
				size_t FileSize(0);
				FileSize = static_cast<size_t> (FileStream.tellg());
				std::vector<char> FileData;
				if (0 != FileSize)
				{
					FileData.resize(FileSize);
					FileStream.seekg(0, std::ios::beg);
					FileStream.read(&FileData.at(0), FileSize);
					FileStream.close();

					SvPb::PutFileRequest Request;
					Request.set_destinationpath(DestinationPath);
					Request.set_filedata(&FileData[0], FileSize);
					Request.set_saveinconfig(saveInConfig);
					SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
						&SvWsl::SVRCClientService::PutFile,
						std::move(Request)).get();
					Result = Response.hresult();
				}
				else
				{
					FileStream.close();
					Result = E_INVALIDARG;
				}
			}
			else
			{
				Result = E_UNEXPECTED;
			}

			rStatus.hResult = Result;
			rStatus.errorText = ConvertResult(Result);
			SVLOG(Result);
		}
		else
		{
			Result = E_INVALIDARG;
			SVLOG(Result);

			rStatus.hResult = Result;
			rStatus.errorText = L"Invalid File Path";
		}
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetDataDefinitionList(const _bstr_t& rInspectionName, long ListType, ISVDataDefObjectList** ppEntries, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetDataDefinitionListRequest Request;
		Request.set_inspectionname(SvUl::to_utf8(rInspectionName));
		Request.set_type(static_cast<SvPb::DataDefinitionListType> (ListType));
		SvPb::GetDataDefinitionListResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetDataDefinitionList,
			std::move(Request)).get();

		GetDataDefList(Response)->QueryInterface(IID_ISVDataDefObjectList, reinterpret_cast<void**> (ppEntries));

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::RegisterMonitorList(const _bstr_t& rListName, const _bstr_t& rPpqName, int rejectDepth,
	const CComVariant& rProductItemList, const CComVariant& rRejectCondList,
	const CComVariant& rFailStatusList, ISVErrorObjectList** ppErrors, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::RegisterMonitorListRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		Request.set_ppqname(SvUl::to_utf8(rPpqName));
		Request.set_rejectdepth(rejectDepth);
		SetStringList(rProductItemList, Request.mutable_productitemlist());
		SetStringList(rRejectCondList, Request.mutable_rejectconditionlist());
		SetStringList(rFailStatusList, Request.mutable_failstatuslist());
		SvPb::RegisterMonitorListResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::RegisterMonitorList,
			std::move(Request)).get();

		GetErrorListPtr(Response.mutable_errorlist())->QueryInterface(IID_ISVErrorObjectList, reinterpret_cast<void**> (ppErrors));

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::QueryMonitorList(const _bstr_t& rListName, SvPb::ListType Type, CComVariant& rItemNames, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::QueryMonitorListRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		Request.set_type(Type);
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::QueryMonitorList,
			std::move(Request)).get();

		Result = Response.hresult();
		if (S_OK == Result)
		{
			SvPb::Value* pValue = Response.mutable_names();
			rItemNames = GetComVariant(*pValue->mutable_item());
		}
		else
		{
			rItemNames = CComVariant("");
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetProduct(const _bstr_t& rListName, long TriggerCount, long , ISVProductItems** ppViewItems, SVCommandStatus& rStatus)
{
	HRESULT Result {E_FAIL};
	try
	{
		if (0 == rListName.length())
		{
			throw std::invalid_argument("missing Monitorlistname");
		}
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetProductRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		Request.set_triggercount((-1 < TriggerCount) ? TriggerCount : -1);
		Request.set_pevioustrigger(-1);

		Request.set_nameinresponse(true);
		SvPb::GetProductResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(), &SvWsl::SVRCClientService::GetProduct, std::move(Request)).get();

		
		Result = static_cast<HRESULT> (Response.productitem().status());
		if (Response.productitem().status() == SvPb::State::isValid)
		{
			Result = S_OK;
			GetProductPtr(m_pSvrcClientService, Response.productitem())->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppViewItems));
		}
		else
		{
			rStatus.errorText = SvUl::to_utf16(SvPb::State_Name(Response.productitem().status()));
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}
HRESULT SVControlCommands::GetRejects(const _bstr_t& rListName, long TriggerCount, long , ISVProductItems** ppViewItems, SVCommandStatus& rStatus)
{
	HRESULT Result {E_FAIL};
	try
	{
		if (0 == rListName.length())
		{
			throw std::invalid_argument("missing Monitorlistname");
		}
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetRejectRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		Request.set_triggercount((-1 < TriggerCount) ? TriggerCount : -1);
		Request.set_pevioustrigger(-1);

		Request.set_nameinresponse(true);
		SvPb::GetRejectResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(), &SvWsl::SVRCClientService::GetReject, std::move(Request)).get();

		Result = static_cast<HRESULT> (Response.productitem().status());
		if (Response.productitem().status() == SvPb::State::isValid)
		{
			Result = S_OK;
			GetProductPtr(m_pSvrcClientService, Response.productitem())->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppViewItems));
		}
		else
		{
			rStatus.errorText = SvUl::to_utf16(SvPb::State_Name(Response.productitem().status()));
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::ActivateMonitorList(const _bstr_t& rListName, bool Active, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::ActivateMonitorListRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		Request.set_activate(Active);
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::ActivateMonitorList,
			std::move(Request)).get();

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetFailStatus(const _bstr_t& rListName, CComVariant& rValues, SVCommandStatus& rStatus)
{
	HRESULT Result = S_OK;
	try
	{

		if (0 == rListName.length())
		{
			throw std::invalid_argument("missing Monitorlistname");
		}
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetFailStatusRequest FailstatusRequest;
		FailstatusRequest.set_listname(SvUl::to_utf8(rListName));

		FailstatusRequest.set_nameinresponse(true);
		SvPb::GetFailStatusResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetFailStatus,
			std::move(FailstatusRequest)).get();

		Result = static_cast<HRESULT> (Response.status());
		if (Response.status() == SvPb::State::isValid)
		{
			Result = S_OK;
			CComVariant variant = GetFailList(m_pSvrcClientService, Response);
			rValues.Attach(&variant);
		}
		else
		{
			rStatus.errorText = SvUl::to_utf16(SvPb::State_Name(Response.status()));
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)
		return Result;
}



HRESULT SVControlCommands::ShutDown(SVShutdownOptionsEnum Option, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::ShutdownRequest Request;
		//Request.set_option(static_cast<long> (Option));
		SvPb::ShutdownOption  option = (Option == Restart) ?
			SvPb::ShutdownOption::shutdownSvObserverAndRestartWindows : SvPb::ShutdownOption::shutdownSvObserverAndWindows;
		Request.set_option(option);
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::Shutdown,
			std::move(Request)).get();

		Result = Response.hresult();

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}



HRESULT SVControlCommands::GetInspectionNames(CComVariant& rNames, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetInspectionNamesRequest Request;
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetInspectionNames,
			std::move(Request)).get();

		Result = Response.hresult();

		if (S_OK == Result)
		{
			SvPb::Value* pValue = Response.mutable_names();
			rNames = GetComVariant(*pValue->mutable_item());
		}
		else
		{
			rNames = CComVariant();
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}


HRESULT SVControlCommands::QueryMonitorListNames(CComVariant& rListName, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::QueryMonitorListNamesRequest Request;
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::QueryMonitorListNames,
			std::move(Request)).get();

		Result = Response.hresult();

		if (S_OK == Result)
		{
			SvPb::Value* pValue = Response.mutable_names();
			rListName = GetComVariant(*pValue->mutable_item());
		}
		else
		{
			rListName = CComVariant();
		}

		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetMonitorListProperties(const _bstr_t & rListName, long&  rRejectDepth, bool& rIsActive, BSTR&  rPpqName, SVCommandStatus& rStatus)
{
	HRESULT Result = S_OK;
	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetMonitorListPropertiesRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		SvPb::GetMonitorListPropertiesResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetMonitorListProperties,
			std::move(Request)).get();

		rRejectDepth = Response.rejectdepth();
		rIsActive = Response.active();
		rPpqName = _bstr_t(SvUl::to_ansi(Response.ppqname()).c_str()).Detach();

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::GetMaxRejectQeueDepth(unsigned long& rDepth, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetMaxRejectDepthRequest Request;
		SvPb::GetMaxRejectDepthResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetMaxRejectDepth,
			std::move(Request)).get();
		rDepth = Response.maxrejectdepth();

		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)
		return Result;
}

HRESULT SVControlCommands::GetProductFilter(const _bstr_t& rListName, unsigned long& rFilter, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::GetProductFilterRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		SvPb::GetProductFilterResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::GetProductFilter,
			std::move(Request)).get();
		rFilter = static_cast<unsigned long> (Response.filter());

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

HRESULT SVControlCommands::SetProductFilter(const _bstr_t& rListName, unsigned long Filter, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (nullptr == m_pRpcClient || nullptr == m_pSvrcClientService || !m_pRpcClient->isConnected())
		{
			throw std::invalid_argument("Not connected to neither SVOGateway nor SVObserver");
		}

		SvPb::SetProductFilterRequest Request;
		Request.set_listname(SvUl::to_utf8(rListName));
		Request.set_filter(static_cast<SvPb::ProductFilterEnum> (Filter));
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_pSvrcClientService.get(),
			&SvWsl::SVRCClientService::SetProductFilter,
			std::move(Request)).get();

		Result = Response.hresult();
		rStatus.hResult = Result;
		rStatus.errorText = ConvertResult(Result);
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

		return Result;
}

void SVControlCommands::OnConnectionStatus(SvRpc::ClientStatus Status)
{

	switch (Status)
	{
		case SvRpc::ClientStatus::Connected:
		{
			SV_LOG_GLOBAL(info) << "OnConnectionStatus Connect";
			_variant_t connected(_T("Conneted"));
			m_Notifier(connected, SVNotificationTypesEnum::Connected);
			StartNotificationStreaming();
			break;
		}
		case SvRpc::ClientStatus::Disconnected:
		{
			SV_LOG_GLOBAL(info) << "OnConnectionStatus Disconnect";
			_variant_t disconnected(_T("Disconneted"));
			m_Notifier(disconnected, SVNotificationTypesEnum::Disconnected);
			break;
		}
	}
}

std::wstring SVControlCommands::ConvertResult(HRESULT hResult)
{
	std::wstring ResultText;

	if (S_OK != hResult)
	{
		wchar_t Text[50];
		swprintf_s(Text, 50, L"Fehlernummer: 0X%08X", hResult);
		ResultText = Text;
	}

	return ResultText;
}
