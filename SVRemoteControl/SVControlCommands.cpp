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
#include "SVStringConversions.h"
#include "SVProductItems.h"
#include "SVValueObject.h"
#include "Logging.h"
#include "ProtoBufGetter.h"
#include "ProtoBufSetter.h"
#include "WebsocketLibrary\RunRequest.inl"
#include "WebsocketLibrary\Definition.h"
#pragma endregion Includes

static const unsigned long FiveSeconds = 5;
static const unsigned long TenSeconds = 10;
static const unsigned long FifteenSeconds = 15;
static const unsigned long TwentySeconds = 20;
static const unsigned long ThirtySeconds = 30;
static const unsigned long SixtySeconds = 60;
static const unsigned long TwoMinutes = 120;
static const WCHAR* const NotConnectedToSVobserver = L"Not Connected to SVObserver";

#define HANDLE_EXCEPTION(Status, CmdStatus )\
catch (std::invalid_argument & e)\
{\
Status = E_INVALIDARG; \
CmdStatus.hResult = E_INVALIDARG; \
CmdStatus.errorText = SVStringConversions::to_utf16(e.what()); \
SVLOG(CmdStatus.errorText.c_str()); \
}\
catch (boost::system::system_error &e)\
{\
Status = E_FAIL; \
CmdStatus.hResult = E_FAIL; \
CmdStatus.errorText = SVStringConversions::to_utf16(e.what()); \
SVLOG(CmdStatus.errorText.c_str()); \
}\
catch (std::exception & e)\
{\
Status = E_UNEXPECTED; \
CmdStatus.hResult = E_UNEXPECTED; \
CmdStatus.errorText = SVStringConversions::to_utf16(e.what()); \
SVLOG(CmdStatus.errorText.c_str()); \
}\
catch (ATL::CAtlException & e)\
{\
Status = e.m_hr; \
CmdStatus.hResult = e.m_hr; \
CmdStatus.errorText = L"ATL exception"; \
SVLOG(CmdStatus.errorText.c_str()); \
}\
catch (...)\
{\
Status = E_UNEXPECTED; \
CmdStatus.hResult = E_UNEXPECTED; \
CmdStatus.errorText = L"Unknown exception"; \
SVLOG(CmdStatus.errorText.c_str()); \
}

SVControlCommands::SVControlCommands(NotifyFunctor p_Func)
	: m_ServerName(""),
	m_CommandPort(svr::cmdPort),
	m_Connected(false),
	m_Notifier(p_Func)
{
}


SVControlCommands::~SVControlCommands()
{
}

HRESULT SVControlCommands::SetConnectionData(const _bstr_t& p_rServerName, unsigned short p_CommandPort, long timeout)
{
	HRESULT hr = S_OK;
	m_WebClient.reset();
	m_SvrcClient.reset();
	m_Connected = false;
	m_ServerName = p_rServerName;
	m_CommandPort = p_CommandPort;

	if (0 < m_ServerName.length())
	{

		std::string host(m_ServerName);
		m_WebClient.SetConnectionData(host, SvWsl::Default_Port);
		m_SvrcClient.SetConnectionData(host, SvWsl::Default_SecondPort);
	
		//m_Connected represent only the client socket connection state 
		m_Connected = true;
		m_WebClient.WaitForConnect(timeout);
		m_SvrcClient.WaitForConnect(timeout);
		bool Connected = m_WebClient.isConnected() && m_SvrcClient.isConnected();
		if (!Connected)
		{
			HRESULT h = RPC_E_TIMEOUT;
			SVLOG(h);
		}
	}
	else
	{
		hr = INET_E_INVALID_URL;
		SVLOG(hr);
	}
	return hr;
}

HRESULT SVControlCommands::GetVersion(_bstr_t& rSVObserverVersion, _bstr_t& rWebServerVersion, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		rSVObserverVersion = _bstr_t(L"N/A");
		if (m_SvrcClient.isConnected())
		{
			SvPb::GetVersionRequest Request;
			SvPb::GetVersionResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																&SvWsl::SVRCClientService::GetVersion,
																std::move(Request)).get();

			rSVObserverVersion = _bstr_t(Response.version().c_str());
		}

		rWebServerVersion = _bstr_t(L"N/A");
		if (m_WebClient.isConnected())
		{
			SvPb::GetVersionRequest Request;
			auto Response = SvWsl::runRequest(*m_WebClient.m_pClientService.get(), 
											  &SvWsl::ClientService::getVersion, 
											  std::move(Request)).get();
			rWebServerVersion = _bstr_t(Response.version().c_str());
		}

		SVLOG((Result = rStatus.hResult));
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::GetState(unsigned long& rState, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (false == m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetStateRequest Request;
		SvPb::GetStateResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
															&SvWsl::SVRCClientService::GetState,
															std::move(Request)).get();

		rState = Response.state();
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::GetOfflineCount(unsigned long& rCount, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (false == m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetOfflineCountRequest Request;
		SvPb::GetOfflineCountResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
															&SvWsl::SVRCClientService::GetOfflineCount,
															std::move(Request)).get();
		rCount = Response.count();
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::GetConfigReport(BSTR& rReport, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetConfigReportRequest Request;
		SvPb::GetConfigReportResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																 &SvWsl::SVRCClientService::GetConfigReport,
																   std::move(Request)).get();

		rReport = _bstr_t(Response.report().c_str()).Detach();
		Result = Response.hresult();

		SVLOG((Result = rStatus.hResult));
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::GetMode(unsigned long&  rMode, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};
	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetDeviceModeRequest Request;
		SvPb::GetDeviceModeResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(), 
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
	HRESULT Result{S_OK};
	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		if (false == SvPb::DeviceModeType_IsValid(Mode))
		{
			throw std::invalid_argument("Invalid SVObserver mode");
		}
		
		SvPb::SetDeviceModeRequest Request;
		Request.set_mode(static_cast<SvPb::DeviceModeType> (Mode));
		SvPb::StandardResponse Response =
			SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(), &SvWsl::SVRCClientService::SetDeviceMode,
			std::move(Request)).get();

		Result =  Response.hresult();
		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)
	return Result;
}

HRESULT SVControlCommands::GetItems(CComVariant ItemNames, ISVProductItems** ppItems, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetItemsRequest Request;

		if (ItemNames.vt == VT_BSTR)
		{
			std::string* pName = Request.add_itemnamelist();
			*pName = SVStringConversions::to_utf8(_bstr_t(ItemNames.bstrVal, false));
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
				*pName = SVStringConversions::to_utf8(bString);
				bString.Detach();
			}
			::SafeArrayUnaccessData(pArray);
		}
		else
		{
			throw CAtlException(OLE_E_CANTCONVERT);
		}

		SvPb::GetItemsResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(), 
															&SvWsl::SVRCClientService::GetItems, std::move(Request)).get();

		GetItemsPtr(Response)->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppItems));

		Result = Response.hresult();
		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::SetItems(ISVProductItems* pItems, ISVProductItems ** ppErrors, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		if(nullptr != pItems)
		{
			SvPb::SetItemsRequest Request;
			SetItemsRequest(ProductPtr(pItems), &Request);
			SvPb::SetItemsResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																&SvWsl::SVRCClientService::SetItems, std::move(Request)).get();

			GetItemsPtr(Response.mutable_errorlist())->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppErrors));

			Result = Response.hresult();
		}
		else
		{
			Result = E_POINTER;
		}

		rStatus.hResult = Result;
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

HRESULT SVControlCommands::GetConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		std::string fPath = SVStringConversions::to_utf8(rFilePath);
		if (IsValidFilePath(fPath))
		{
			SvPb::GetConfigRequest Request;
			Request.set_filename(fPath.c_str());
			SvPb::GetConfigResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																&SvWsl::SVRCClientService::GetConfig,
																std::move(Request)).get();
			Result = Response.hresult();
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
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		std::string fPath = SVStringConversions::to_utf8(rFilePath);
		if (IsValidFilePath(fPath))
		{
			std::ifstream FileStream;

			FileStream.open(fPath.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
			if (FileStream.is_open())
			{
				size_t FileSize(0);
				FileSize = static_cast<size_t> (FileStream.tellg());
				std::vector<char> FileData;
				if(0 != FileSize)
				{
					FileData.resize(FileSize);
					FileStream.seekg(0, std::ios::beg);
					FileStream.read(&FileData.at(0), FileSize);
					FileStream.close();

					SvPb::PutConfigRequest PutConfigRequest;
					PutConfigRequest.set_filename(fPath.c_str());
					PutConfigRequest.set_filedata(&FileData[0], FileSize);
					SvPb::StandardResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
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
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}
		std::string DestinationPath = SVStringConversions::to_utf8(rDestinationPath);
		if (IsValidFilePath(DestinationPath))
		{

			SvPb::GetFileRequest Request;
			Request.set_sourcepath(SVStringConversions::to_utf8(rSourcePath));
			SvPb::GetFileResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																 &SvWsl::SVRCClientService::GetFile,
																 std::move(Request)).get();
			Result = Response.hresult();
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

HRESULT SVControlCommands::PutFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		std::string SourcePath = SVStringConversions::to_utf8(rSourcePath);
		std::string DestinationPath = SVStringConversions::to_utf8(rDestinationPath);
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
					SvPb::StandardResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
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
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetDataDefinitionListRequest Request;
		Request.set_inspectionname(SVStringConversions::to_utf8(rInspectionName));
		Request.set_type(static_cast<SvPb::DataDefinitionListType> (ListType));
		SvPb::GetDataDefinitionListResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
															&SvWsl::SVRCClientService::GetDataDefinitionList,
															std::move(Request)).get();

		GetDataDefList(Response)->QueryInterface(IID_ISVDataDefObjectList, reinterpret_cast<void**> (ppEntries));

		Result = Response.hresult();
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::RegisterMonitorList(const _bstr_t& rListName, const _bstr_t& rPpqName, int rejectDepth,
	const CComVariant& rProductItemList, const CComVariant& rRejectCondList,
	const CComVariant& rFailStatusList, ISVErrorObjectList** ppErrors, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::RegisterMonitorListRequest Request;
		Request.set_name(SVStringConversions::to_utf8(rListName));
		Request.set_ppqname(SVStringConversions::to_utf8(rPpqName));
		Request.set_rejectdepth(rejectDepth);
		SetStringList(rProductItemList, Request.mutable_productitemlist());
		SetStringList(rRejectCondList, Request.mutable_rejectconditionlist());
		SetStringList(rFailStatusList, Request.mutable_failstatuslist());
		SvPb::RegisterMonitorListResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																 &SvWsl::SVRCClientService::RegisterMonitorList,
																 std::move(Request)).get();

		GetErrorListPtr(Response.mutable_errorlist())->QueryInterface(IID_ISVErrorObjectList, reinterpret_cast<void**> (ppErrors));

		Result = Response.hresult();
		rStatus.hResult = Result;
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
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::QueryMonitorListRequest Request;
		Request.set_listname(SVStringConversions::to_utf8(rListName));
		Request.set_type(Type);
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
														 &SvWsl::SVRCClientService::QueryMonitorList, std::move(Request)).get();

		Result = Response.hresult();

		if (S_OK == Result)
		{
			SvPb::Value* pValue = Response.mutable_names();
			rItemNames = GetComVariant(*pValue->mutable_item(), pValue->count());
		}
		else
		{
			rItemNames = CComVariant();
		}

		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::GetProduct(bool bGetReject, const _bstr_t& rListName, long TriggerCount, long ImageScale, ISVProductItems** ppViewItems, SVCommandStatus& rStatus)
{
	HRESULT Status = E_FAIL;
	try
	{
		if (0 == rListName.length())
		{
			throw std::invalid_argument("missing Monitorlistname");
		}
		//if (0 == m_pRpcClient.get() || false == m_pRpcClient->isConnected())
		if (false == m_WebClient.isConnected())
		{
			throw std::invalid_argument("Not connected To RRServer");
		}

		SvPb::GetProductRequest ProductRequest;
		ProductRequest.set_name(static_cast<const char*>(rListName));
		ProductRequest.set_trigger((-1 < TriggerCount) ? TriggerCount : -1);
		ProductRequest.set_pevioustrigger(-1);
		ProductRequest.set_breject(bGetReject);

		ProductRequest.set_nameinresponse(true);
		SvPb::GetProductResponse resp =
			SvWsl::runRequest(*m_WebClient.m_pClientService.get(), &SvWsl::ClientService::getProduct, std::move(ProductRequest)).get();

		rStatus.hResult = (HRESULT)resp.product().status();
		if (resp.product().status() == SvPb::IsValid)
		{
			Status = S_OK;
			GetProductPtr(m_WebClient.m_pClientService, resp.product())->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(ppViewItems));
		}
		else
		{
			rStatus.errorText = SVStringConversions::to_utf16(SvPb::State_Name(resp.product().status()));
		}

		SVLOG(Status);
	}
	HANDLE_EXCEPTION(Status, rStatus)


		return Status;
}
HRESULT SVControlCommands::GetProduct(const _bstr_t& rListName, long TriggerCount, long ImageScale, ISVProductItems** ppViewItems, SVCommandStatus& rStatus)
{
	return GetProduct(false, rListName, TriggerCount, ImageScale, ppViewItems, rStatus);
}
HRESULT SVControlCommands::GetRejects(const _bstr_t& rListName, long TriggerCount, long ImageScale, ISVProductItems** ppViewItems, SVCommandStatus& rStatus)
{
	return GetProduct(true, rListName, TriggerCount, ImageScale, ppViewItems, rStatus);

}

HRESULT SVControlCommands::ActivateMonitorList(const _bstr_t& rListName, bool Active, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (false == m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::ActivateMonitorListRequest Request;
		Request.set_listname(SVStringConversions::to_utf8(rListName));
		Request.set_activate(Active);
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																   &SvWsl::SVRCClientService::ActivateMonitorList,
																   std::move(Request)).get();
		Result = Response.hresult();
		rStatus.hResult = Result;
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
		if (false == m_WebClient.isConnected())
		{
			throw std::invalid_argument("Not connected To RRServer");
		}

		SvPb::GetFailStatusRequest FailstatusRequest;
		FailstatusRequest.set_name(static_cast<const char*>(rListName));

		FailstatusRequest.set_nameinresponse(true);
		SvPb::GetFailStatusResponse resp
			= SvWsl::runRequest(*m_WebClient.m_pClientService.get(), &SvWsl::ClientService::getFailStatus, std::move(FailstatusRequest)).get();

		rStatus.hResult = (HRESULT)resp.status();
		if (resp.status() == SvPb::IsValid)
		{
			Result = S_OK;
			CComVariant variant = GetFailList(m_WebClient.m_pClientService, resp);
			rValues.Attach(&variant);
		}
		else
		{
			rStatus.errorText = SVStringConversions::to_utf16(SvPb::State_Name(resp.status()));
		}


		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)
		return Result;
}



HRESULT SVControlCommands::ShutDown(SVShutdownOptionsEnum Options, SVCommandStatus& rStatus)
{
	HRESULT Result {S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::ShutdownRequest Request;
		Request.set_options(static_cast<long> (Options));
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
															&SvWsl::SVRCClientService::Shutdown, std::move(Request)).get();

		Result = Response.hresult();

		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}



HRESULT SVControlCommands::GetInspectionNames(CComVariant& rNames, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetInspectionNamesRequest Request;
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
															&SvWsl::SVRCClientService::GetInspectionNames, std::move(Request)).get();

		Result = Response.hresult();

		if(S_OK == Result)
		{
			SvPb::Value* pValue = Response.mutable_names();
			rNames = GetComVariant(*pValue->mutable_item(), pValue->count());
		}
		else
		{
			rNames = CComVariant();
		}

		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}


HRESULT SVControlCommands::QueryMonitorListNames(CComVariant& rListName, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::QueryMonitorListNamesRequest Request;
		SvPb::NamesResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
														 &SvWsl::SVRCClientService::QueryMonitorListNames, std::move(Request)).get();

		Result = Response.hresult();

		if (S_OK == Result)
		{
			SvPb::Value* pValue = Response.mutable_names();
			rListName = GetComVariant(*pValue->mutable_item(), pValue->count());
		}
		else
		{
			rListName = CComVariant();
		}

		rStatus.hResult = Result;
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

		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetMonitorListPropertiesRequest Request;
		Request.set_listname(SVStringConversions::to_utf8(rListName));
		SvPb::GetMonitorListPropertiesResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																 &SvWsl::SVRCClientService::GetMonitorListProperties,
																 std::move(Request)).get();

		rRejectDepth = Response.rejectdepth();
		rIsActive = Response.active();
		rPpqName = _bstr_t(Response.ppqname().c_str()).Detach();

		Result = Response.hresult();
		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::GetMaxRejectQeueDepth(unsigned long& rDepth, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (!m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetMaxRejectDepthRequest Request;
		SvPb::GetMaxRejectDepthResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
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
	HRESULT Result{S_OK};

	try
	{
		if (false == m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::GetProductFilterRequest Request;
		Request.set_listname(SVStringConversions::to_utf8(rListName));
		SvPb::GetProductFilterResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
															&SvWsl::SVRCClientService::GetProductFilter,
															std::move(Request)).get();
		Result = Response.hresult();
		rFilter = static_cast<unsigned long> (Response.filter());
		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}

HRESULT SVControlCommands::SetProductFilter(const _bstr_t& rListName, unsigned long Filter, SVCommandStatus& rStatus)
{
	HRESULT Result{S_OK};

	try
	{
		if (false == m_SvrcClient.isConnected())
		{
			throw std::invalid_argument("Not connected To SVObserver");
		}

		SvPb::SetProductFilterRequest Request;
		Request.set_listname(SVStringConversions::to_utf8(rListName));
		Request.set_filter(static_cast<SvPb::ProductFilterEnum> (Filter));
		SvPb::StandardResponse Response = SvWsl::runRequest(*m_SvrcClient.m_pClientService.get(),
																	&SvWsl::SVRCClientService::SetProductFilter,
																	std::move(Request)).get();
		Result = Response.hresult();
		rStatus.hResult = Result;
		SVLOG(Result);
	}
	HANDLE_EXCEPTION(Result, rStatus)

	return Result;
}
