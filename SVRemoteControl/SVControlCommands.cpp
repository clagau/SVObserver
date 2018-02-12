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

#pragma warning(push)
#pragma warning(disable: 4482)

#include "SVControlCommands.h"
#include "SVStringConversions.h"
#include "SVJsonCmdBuilder.h"
#include "SVJsonValueGetter.h"
#include "Logging.h"
#include "SVJsonParser.h"
#include "JsonDataHelper.h"
#include "RunReApi\Definition.h"
#include "ProtoBufGetter.h"
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
SVLOG(p_rStatus.errorText.c_str()); \
}\
catch (boost::system::system_error &e)\
{\
Status = E_FAIL; \
CmdStatus.hResult = E_FAIL; \
CmdStatus.errorText = SVStringConversions::to_utf16(e.what()); \
SVLOG(p_rStatus.errorText.c_str()); \
}\
catch (std::exception & e)\
{\
Status = E_UNEXPECTED; \
CmdStatus.hResult = E_UNEXPECTED; \
CmdStatus.errorText = SVStringConversions::to_utf16(e.what()); \
SVLOG(p_rStatus.errorText.c_str()); \
}\
catch (ATL::CAtlException & e)\
{\
Status = e.m_hr; \
CmdStatus.hResult = e.m_hr; \
CmdStatus.errorText = L"ATL exception"; \
SVLOG(p_rStatus.errorText.c_str()); \
}\
catch (...)\
{\
Status = E_UNEXPECTED; \
CmdStatus.hResult = E_UNEXPECTED; \
CmdStatus.errorText = L"Unknown exception"; \
SVLOG(p_rStatus.errorText.c_str()); \
}


typedef Json::FastWriter JsonWriter;
template<>
Json::Value SVJsonCmdBuilder::MkJson<CComVariant>(const CComVariant & var)
{
	switch (var.vt)
	{
	case VT_BSTR:
		return Json::Value(SVStringConversions::to_utf8(_bstr_t(var.bstrVal)));
	case VT_I4:
		return Json::Value(var.iVal);
	case VT_UI4:
		return Json::Value(var.uiVal);
	case VT_BOOL:
		return Json::Value(static_cast<bool>(var.boolVal != VARIANT_FALSE));
	case VT_R8:
		return Json::Value(var.dblVal);
	default:
		return Json::Value();
	}
}

template<>
Json::Value SVJsonCmdBuilder::MkJson<ValueObjectPtr>(const ValueObjectPtr & ptr)
{
	Json::Value val(Json::objectValue);
	_bstr_t name;
	ptr->get_Name(name.GetAddress());
	val[SVRC::vo::name] = SVStringConversions::to_utf8(name);
	long status;
	ptr->get_Status(&status);
	val[SVRC::vo::status] = status;
	ptr->get_TriggerCount(&status);
	val[SVRC::vo::count] = status;
	Json::Value arr(Json::arrayValue);
	ptr->get_Count(&status);
	for (long i = 0; i < status; ++i)
	{
		CComVariant var;
		CComVariant idx = i;
		ptr->get_Item(idx, &var);
		arr.append(MkJson(var));
	}

	val[SVRC::vo::array] = arr;
	return val;
}

template<>
Json::Value SVJsonCmdBuilder::MkJson<ValueListPtr>(const ValueListPtr & ptr)
{
	Json::Value val(Json::arrayValue);
	long count;
	ptr->get_Count(&count);
	for (long i = 0; i < count; ++i)
	{
		CComVariant idx = i;
		CComVariant var;
		ptr->get_Item(idx, &var);
		ValueObjectPtr vptr((ISVValueObject*)var.pdispVal);
		val.append(MkJson(vptr));
	}
	return val;
}

template<>
Json::Value SVJsonCmdBuilder::MkJson<ImageObjectPtr>(const ImageObjectPtr & ptr)
{
	Json::Value val(Json::objectValue);
	_bstr_t name;
	ptr->get_Name(name.GetAddress());
	val[SVRC::io::name] = SVStringConversions::to_utf8(name);
	long status;
	ptr->get_Status(&status);
	val[SVRC::io::status] = status;
	ptr->get_TriggerCount(&status);
	val[SVRC::io::count] = status;

	CComVariant l_TempImage;

	ptr->GetImage(false, 1.0, BMP, &l_TempImage);

	LONG len = 0;
	::SafeArrayGetUBound(l_TempImage.parray, 1, &len);
	len++;
	BYTE * src = 0;
	::SafeArrayAccessData(l_TempImage.parray, reinterpret_cast<void**>(&src));
	int enc_len = ::Base64EncodeGetRequiredLength(len, ATL_BASE64_FLAG_NOCRLF);
	boost::scoped_array<char>  enc_buff(new char[enc_len + 1]);
	enc_buff[enc_len] = '\0';
	::Base64Encode(src, len, enc_buff.get(), &enc_len, ATL_BASE64_FLAG_NOCRLF);
	::SafeArrayUnaccessData(l_TempImage.parray);

	val[SVRC::io::image] = enc_buff.get();

	return val;
}

template<>
Json::Value SVJsonCmdBuilder::MkJson<ImageListPtr>(const ImageListPtr & ptr)
{
	Json::Value val(Json::arrayValue);
	long count;
	ptr->get_Count(&count);
	for (long i = 0; i < count; ++i)
	{
		CComVariant idx = i;
		CComVariant var;
		ptr->get_Item(idx, &var);
		ImageObjectPtr vptr((ISVImageObject *)var.pdispVal);
		val.append(MkJson(vptr));
	}
	return val;
}

template<>
Json::Value SVJsonCmdBuilder::MkJson<ProductPtr>(const ProductPtr & ptr)
{
	Json::Value val(Json::objectValue);
	ISVImageObjectList* pImageList(nullptr);
	ptr->get_Images(&pImageList); // will call AddRef
	ImageListPtr ilist(pImageList); // will call AddRef
	val[SVRC::arg::images] = MkJson(ilist);
	// Release the pointer obtained via get_Images
	if (pImageList)
	{
		pImageList->Release();
	}

	ISVValueObjectList* pValueList(nullptr);
	ptr->get_Values(&pValueList); // will call AddRef
	ValueListPtr vlist(pValueList); // will call AddRef
	val[SVRC::arg::values] = MkJson(vlist);
	// Release the pointer obtained via get_Values
	if (pValueList)
	{
		pValueList->Release();
	}
	return val;
}

inline Json::Value MkArray(const CComVariant & items)
{
	LONG l, u;
	Json::Value arr(Json::arrayValue);
	if (items.vt == VT_BSTR)
	{
		arr.append(SVStringConversions::to_utf8(_bstr_t(items.bstrVal, FALSE)));
	}
	else if (items.vt == (VT_ARRAY | VT_BSTR))
	{
		SAFEARRAY * p_array = items.parray;
		::SafeArrayGetLBound(p_array, 1, &l);
		::SafeArrayGetUBound(p_array, 1, &u);
		BSTR * data;
		::SafeArrayAccessData(p_array, (void**)&data);
		for (LONG i = l; i <= u; ++i)
		{
			_bstr_t b(data[i], FALSE);
			arr.append(SVStringConversions::to_utf8(b));
			b.Detach();
		}
		::SafeArrayUnaccessData(p_array);
	}
	else
	{
		throw CAtlException(OLE_E_CANTCONVERT);
	}
	return arr;
}

SVControlCommands::SVControlCommands(NotifyFunctor p_Func, bool connectToRRS)
	: m_ServerName(""),
	m_CommandPort(svr::cmdPort),
	m_RejectPort(svr::rjctPort),
	m_RunServerPort(svr::runpgePort),
	m_Connected(false),
	m_RRSConnected(false),
	m_Notifier(p_Func)
{
	m_ClientSocket.SetConnectionStatusCallback(boost::bind(&SVControlCommands::OnConnectionStatus, this, _1));
	m_ClientSocket.SetDataReceivedCallback(boost::bind(&SVControlCommands::OnControlDataReceived, this, _1));
	if (connectToRRS)
	{
		m_pioServiceWork = std::make_unique<boost::asio::io_service::work>(m_io_service);

		for (int i = 0; i < RRApi::Default_NrOfIoThreads; ++i)
		{
			m_Threads.push_back(std::make_unique<std::thread>(boost::bind(&boost::asio::io_service::run, &m_io_service)));
		}

		m_pFrontEndApi = RRApi::ClientFrontEndApi::Init(m_io_service);
	}
}


SVControlCommands::~SVControlCommands()
{
	m_ClientSocket.Disconnect();
	if (nullptr != m_pFrontEndApi.get())
	{
		m_pFrontEndApi->DisConnect();
	}
}

HRESULT SVControlCommands::SetConnectionData(const _bstr_t& p_rServerName, unsigned short p_CommandPort, long timeout)
{
	HRESULT hr = S_OK;
	m_ClientSocket.Disconnect();
	if (m_pFrontEndApi.get())
	{
		m_pFrontEndApi->DisConnect();
	}
	m_Connected = false;
	m_ServerName = p_rServerName;
	m_CommandPort = p_CommandPort;

	if (0 < m_ServerName.length())
	{
		if (m_pFrontEndApi.get())
			m_pFrontEndApi->Connect(RRApi::Default_Port, static_cast<char*>(m_ServerName));
		SvSol::SVSocketError::ErrorEnum err = SvSol::SVSocketError::Success;
		if ((err = m_ClientSocket.BuildConnection(m_ServerName, svr::cmdPort, timeout)) == SvSol::SVSocketError::Success)
		{
			if (m_pFrontEndApi.get())
				m_RRSConnected = m_pFrontEndApi->IsConnected();
			m_Connected = true;
		}
		else
		{
			hr = SvSol::SVSocketError::HrFromSocketError(err);
			SVLOG(hr);
		}
	}
	else
	{
		hr = INET_E_INVALID_URL;
		SVLOG(hr);
	}
	return hr;
}

HRESULT SVControlCommands::GetVPName(_bstr_t& p_rVPName, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getVPName);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rVPName = rsp.results[SVRC::result::name].bstrVal;
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetVersion(_bstr_t& p_rSVObserverVersion, _bstr_t& p_rRunRejectServerVersion, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getVersion);
		SVControlResponse rsp;
		if (m_ClientSocket.IsConnected())
		{
			CComVariant vrnt;

			rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
			vrnt = rsp.results[SVRC::result::SVO_ver];
			if (S_OK == vrnt.ChangeType(VT_BSTR))
			{
				p_rSVObserverVersion = vrnt.bstrVal;
			}
		}
		else
		{
			rsp.m_Status.hResult = E_ACCESSDENIED;
			rsp.m_Status.errorText = NotConnectedToSVobserver;
		}
		// get version from run reject server
		if (m_ClientSocket.IsConnected())
		{
			p_rRunRejectServerVersion = _bstr_t(L"N/A");
			if (m_pFrontEndApi.get())
			{
				auto version = m_pFrontEndApi->GetVersion(RRApi::GetVersionRequest()).get();
				if (version.has_version())
				{
					p_rRunRejectServerVersion = _bstr_t(version.version().c_str());
				}
			}
		}
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetState(unsigned long& p_rState, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getState);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rState = rsp.results[SVRC::result::state].lVal;
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetOfflineCount(unsigned long& p_rCount, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getOLCount);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rCount = rsp.results[SVRC::result::count].lVal;
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetConfigReport(BSTR& p_rReport, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getReport);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FifteenSeconds);
		p_rReport = ::SysAllocString(rsp.results[SVRC::result::report].bstrVal);
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetMode(unsigned long& p_rMode, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getMode);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rMode = rsp.results[SVRC::result::mode].lVal;
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::SetMode(unsigned long p_Mode, bool p_UseSyncSocket, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVControlResponse rsp;
		if (m_ClientSocket.IsConnected())
		{
			SVLogDebug(L"DEBUG: Start SetMode");
			SVJsonCmdBuilder builder(SVRC::cmdName::setMode);
			builder.Argument(SVRC::arg::desiredMode, static_cast<long>(p_Mode));
			rsp = SendCommand(m_ClientSocket, builder.Value(), TwoMinutes);
			SVLogDebug(L"DEBUG: End SetMode");
		}
		else
		{
			rsp.m_Status.hResult = E_ACCESSDENIED;
			rsp.m_Status.errorText = NotConnectedToSVobserver;
		}
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));

	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetItems(CComVariant p_pItemNames, ISVProductItems** p_ppItems, LONG p_scale, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getItems);
		builder.Argument(SVRC::arg::item_names, MkArray(p_pItemNames));
		builder.Argument(SVRC::arg::scale, Json::Value(p_scale));

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), ThirtySeconds);
		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			if (rsp.results[SVRC::result::items].vt == VT_DISPATCH && rsp.results[SVRC::result::items].pdispVal)
			{
				l_Status = rsp.results[SVRC::result::items].pdispVal->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(p_ppItems));
			}
			else
			{
				l_Status = E_POINTER;
				SVLOG(l_Status);
			}
		}

		p_rStatus = rsp.m_Status;

		if (l_Status == S_OK)
		{
			l_Status = rsp.m_Status.hResult;
			SVLOG(l_Status);
		}
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::SetItems(ISVProductItems* p_pItems, ISVProductItems ** faults, bool p_UseSyncSocket, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;
	SVLogDebug(L"DEBUG: Start of SetItems");

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::setItems);
		builder.Argument(SVRC::arg::item_names, builder.MkJson(ProductPtr(p_pItems)));
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), 30);
		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			typedef CComObject<SVProductItems> Product;
			Product * pi = 0;
			rsp.m_Status.hResult = Product::CreateInstance(&pi);
			if (VT_DISPATCH == rsp.results[SVRC::result::faults].vt && rsp.results[SVRC::result::faults].pdispVal)
			{
				pi->AddRef();
				pi->put_Errors((ISVErrorObjectList *)rsp.results[SVRC::result::faults].pdispVal);
				*faults = pi;
			}
		}
		l_Status = rsp.m_Status.hResult;
		SVLOG(l_Status);
		p_rStatus = rsp.m_Status;
		SVLogDebug(L"DEBUG: End of SetItems");
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
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

HRESULT SVControlCommands::GetConfig(const _bstr_t& p_rFilePath, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		std::string fPath = SVStringConversions::to_utf8(p_rFilePath);
		if (IsValidFilePath(fPath))
		{
			SVJsonCmdBuilder builder(SVRC::cmdName::getConfig);
			builder.Argument(SVRC::arg::filePath, fPath);
			SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value());
			if (rsp.m_Status.hResult == S_OK)
			{
				if (rsp.results[SVRC::result::contents].vt == (VT_I1 | VT_BYREF) && rsp.results[SVRC::result::contents].pcVal)
				{
					char* contents = rsp.results[SVRC::result::contents].pcVal;
					int str_sz = static_cast<int>(::strlen(contents));

					int buff_len = ::Base64DecodeGetRequiredLength(str_sz);
					HANDLE hFile = ::CreateFile(p_rFilePath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (INVALID_HANDLE_VALUE != hFile)
					{
						HANDLE hMapping = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, buff_len, NULL);
						BYTE* buff = 0;
						if (hMapping && (buff = (BYTE *)::MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, buff_len)) != 0)
						{
							::Base64Decode(contents, str_sz, buff, &buff_len);
							::UnmapViewOfFile(buff);
						}
						else
						{
							l_Status = ::GetLastError();
							SVLOG(l_Status);
						}
						::CloseHandle(hMapping);
						::SetFilePointer(hFile, buff_len, NULL, FILE_BEGIN);
						::SetEndOfFile(hFile);
						::CloseHandle(hFile);
					}
					else
					{
						l_Status = ::GetLastError();
						SVLOG(l_Status);
					}
					::free(contents);
				}
				rsp.results[SVRC::result::contents].Clear();
			}
			else
			{
				l_Status = rsp.m_Status.hResult;
				SVLOG(l_Status);
			}
			p_rStatus = rsp.m_Status;
		}
		else
		{
			l_Status = E_INVALIDARG;
			SVLOG(l_Status);

			p_rStatus.hResult = l_Status;
			p_rStatus.errorText = L"Invalid File Path";
		}
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::PutConfig(const _bstr_t& p_rFilePath, const _bstr_t& p_rProductName, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		HANDLE hFile = ::CreateFile(p_rFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			l_Status = E_INVALIDARG;
			SVControlResponse rsp;
			rsp.m_Status.hResult = l_Status;
			rsp.m_Status.errorText = L"File Not Found - ";
			rsp.m_Status.errorText += p_rFilePath;
			SVLOG(l_Status);
			p_rStatus = rsp.m_Status;
		}
		else
		{
			::CloseHandle(hFile);
			std::string fPath = SVStringConversions::to_utf8(p_rFilePath);
			std::string productName = SVStringConversions::to_utf8(p_rProductName);
			SVJsonCmdBuilder builder(SVRC::cmdName::putConfig);
			builder.Argument(SVRC::arg::filePath, fPath);
			builder.Argument(SVRC::arg::productName, productName);
			builder.Argument(SVRC::arg::contents, "");
			SVControlResponse rsp = SendCommandWithFile(m_ClientSocket, builder.Value(), p_rFilePath);
			l_Status = rsp.m_Status.hResult;
			SVLOG(l_Status);
			p_rStatus = rsp.m_Status;
		}
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetFile(const _bstr_t& p_rSourcePath, const _bstr_t& p_rDestinationPath, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		std::string fSourcePath = SVStringConversions::to_utf8(p_rSourcePath);
		std::string fDestPath = SVStringConversions::to_utf8(p_rDestinationPath);
		SVJsonCmdBuilder builder(SVRC::cmdName::getFile);
		builder.Argument(SVRC::arg::sourcePath, fSourcePath);
		builder.Argument(SVRC::arg::destinationPath, fDestPath);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), 600);
		if (rsp.m_Status.hResult == S_OK)
		{
			CHAR * contents = rsp.results[SVRC::result::contents].pcVal;
			int str_sz = static_cast<int>(::strlen(contents));

			int buff_len = ::Base64DecodeGetRequiredLength(str_sz);
			HANDLE hFile = ::CreateFile(p_rDestinationPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			HANDLE hMapping = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, buff_len, NULL);
			BYTE * buff = 0;
			if (hMapping && (buff = (BYTE *)::MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, buff_len)) != 0)
			{
				::Base64Decode(contents, str_sz, buff, &buff_len);
				::UnmapViewOfFile(buff);
			}
			else
			{
				l_Status = ::GetLastError();
				SVLOG(l_Status);
			}
			::CloseHandle(hMapping);
			::SetFilePointer(hFile, buff_len, NULL, FILE_BEGIN);
			::SetEndOfFile(hFile);
			::CloseHandle(hFile);
		}
		else
		{
			l_Status = rsp.m_Status.hResult;
			SVLOG(l_Status);
		}
		p_rStatus = rsp.m_Status;
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::PutFile(const _bstr_t& p_rSourcePath, const _bstr_t& p_rDestinationPath, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		std::string fSourcePath = SVStringConversions::to_utf8(p_rSourcePath);
		std::string fDestPath = SVStringConversions::to_utf8(p_rDestinationPath);
		SVJsonCmdBuilder builder(SVRC::cmdName::putFile);
		builder.Argument(SVRC::arg::sourcePath, fSourcePath);
		builder.Argument(SVRC::arg::destinationPath, fDestPath);
		HANDLE hFile = ::CreateFile(p_rSourcePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			l_Status = E_INVALIDARG;
			SVLOG(l_Status);
		}
		else
		{
			DWORD file_sz;
			file_sz = ::GetFileSize(hFile, NULL);
			HANDLE hMapping = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, file_sz, NULL);
			BYTE * buff = (BYTE *)::MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, file_sz);
			int enc_len = ::Base64EncodeGetRequiredLength(file_sz, ATL_BASE64_FLAG_NOCRLF);
			boost::scoped_array<char> enc_buff(new char[enc_len + 1]);
			enc_buff[enc_len] = '\0';
			::Base64Encode(buff, file_sz, enc_buff.get(), &enc_len, ATL_BASE64_FLAG_NOCRLF);

			::UnmapViewOfFile(buff);
			::CloseHandle(hMapping);
			::CloseHandle(hFile);

			builder.Argument(SVRC::arg::contents, enc_buff.get());
			SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), 600);
			l_Status = rsp.m_Status.hResult;
			SVLOG(l_Status);
			p_rStatus = rsp.m_Status;
		}
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetDataDefinitionList(const _bstr_t& p_rInspectionName, long p_ListType, ISVDataDefObjectList** p_ppEntries, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getDefList);

		std::string l_InspectionName = SVStringConversions::to_utf8(p_rInspectionName);
		builder.Argument(SVRC::arg::inspectionName, l_InspectionName);
		builder.Argument(SVRC::arg::listType, Json::Value(p_ListType));

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), 30);
		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			if (rsp.results[SVRC::result::items].vt == VT_DISPATCH && rsp.results[SVRC::result::items].pdispVal)
			{
				l_Status = rsp.results[SVRC::result::items].pdispVal->QueryInterface(IID_ISVDataDefObjectList, reinterpret_cast<void**>(p_ppEntries));
			}
			else
			{
				l_Status = E_POINTER;
				SVLOG(l_Status);
			}
		}

		p_rStatus = rsp.m_Status;

		if (l_Status == S_OK)
		{
			l_Status = rsp.m_Status.hResult;
			SVLOG(l_Status);
		}
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}


std::string trim(const std::string & str) // for debugging 
{
	if (str.length() < 250)
	{
		return str;
	}
	return str.substr(0, 60) + " ... " + str.substr(str.length() - 140);
}

template<typename API>
struct port;

template<> struct port<SvSol::TcpApi> { enum : u_short { no = svr::rjctPort }; };
template<> struct port<SvSol::UdpApi> { enum : u_short { no = svr::runpgePort }; };

// This function sends to the Run/Reject server
template<typename API>
SVControlResponse SVControlCommands::SendRequest(SvSol::SVClientSocket<API> & sock, const Json::Value & req, unsigned long timeout)
{
	SVControlResponse rsp;
	JsonWriter writer;
	typedef SvSol::Err Err;
	Err error = Err::Success;
	bytes buff;
	u_long sz = 0;
	if (!sock.IsConnected())
	{
		short portNo = port<API>::no;
		error = sock.Connect(m_ServerName, portNo);
	}
	if (Err::Success == error
		&& Err::Success == (error = sock.Write(writer.write(req), true))
		&& Err::Success == (error = sock.ReadAll(buff, sz, true)))
	{
		if (sz > 0)
		{
			Json::Reader reader;
			Json::Value val;
			char * buf_start = reinterpret_cast<char *>(buff.get());
			reader.parse(buf_start, buf_start + sz, val, false);
			///The getter set the imagesocket in imageobject
			SVJsonValueGetter<SVControlResponse> getter(this);
			rsp = getter(val);
		}
		else
		{
			error = Err::NoData;
			rsp.m_Status.hResult = SvSol::SVSocketError::HrFromSocketError(error);
			SVLOG((rsp.m_Status.errorText = SVStringConversions::to_utf16(SvSol::SVSocketError::GetErrorText(error))).c_str());
		}
	}
	else
	{
		rsp.m_Status.hResult = SvSol::SVSocketError::HrFromSocketError(error);
		SVLOG((rsp.m_Status.errorText = SVStringConversions::to_utf16(SvSol::SVSocketError::GetErrorText(error))).c_str());
	}
	sock.Disconnect();
	return rsp;
}

// This function sends to SVObserver
template<typename API>
SVControlResponse SVControlCommands::SendCommand(SvSol::SVRCClientSocket<API> & p_rSocket, const Json::Value & p_cmd, unsigned long p_TimeoutInSeconds)
{
	SVControlResponse l_Status;

	if (m_Command.IsJsonCommandEmpty())
	{
		JsonWriter writer;
		std::string cmd = writer.write(p_cmd);

		if (!cmd.empty())
		{
			try
			{
				l_Status.m_Status.hResult = m_Command.SetJsonCommand(cmd, p_TimeoutInSeconds * 1000);

				if (l_Status.m_Status.hResult == S_OK)
				{
					SVLOG((l_Status.m_Status.hResult = WriteToSocket(cmd, p_rSocket)));

					if (l_Status.m_Status.hResult == S_OK)
					{
						SVLOG((l_Status.m_Status.hResult = m_Command.WaitForRequest(p_TimeoutInSeconds * 1000)));

						if (l_Status.m_Status.hResult == S_OK)
						{
							const Json::Value& val = m_Command.GetJsonResults();
							SVJsonValueGetter<SVControlResponse> rspGetter(this);
							l_Status = rspGetter(val);
						}
					}
				}
				if (l_Status.m_Status.errorText.empty())
				{
					l_Status.m_Status.FormatMessage();
				}
			}
			catch (std::exception & ex)
			{
				SVLOG((l_Status.m_Status.hResult = E_UNEXPECTED));
				l_Status.m_Status.errorText = SVStringConversions::to_utf16(ex.what());
			}

			m_Command.ClearJsonCommand();
		}
		else
		{
			SVLOG((l_Status.m_Status.hResult = E_INVALIDARG));
			l_Status.m_Status.errorText = L"Command string empty.";
		}
	}
	else
	{
		SVLOG((l_Status.m_Status.hResult = STG_E_INUSE));
		l_Status.m_Status.errorText = L"System busy.";
	}

	return l_Status;
}

static void findToken(const SVJson::Parser::TokenList& tokens, std::istream& is, const std::string& tokenName, std::string& value)
{
	is.clear(); // clear error/eof bits

	for (SVJson::Parser::TokenList::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		const SVJson::jsontok_t& rToken = (*it);
		if (SVJson::JSON_STRING == rToken.type)
		{
			int len = rToken.end - rToken.start;
			if (len == tokenName.length())
			{
				char key[80];
				// read the key
				is.seekg(rToken.start, std::ios_base::beg);
				is.read(key, len);
				int amtRead = static_cast<int>(is.gcount());
				if (amtRead == len)
				{
					key[len] = 0;
					if (tokenName == key)
					{
						++it;
						const SVJson::jsontok_t& rToken = (*it);
						if (rToken.type == SVJson::JSON_STRING)
						{
							int len = rToken.end - rToken.start;
							boost::scoped_array<char> buff(new char[len + 1]);
							if (buff.get())
							{
								char* ptr = buff.get();
								is.seekg(rToken.start, std::ios_base::beg);
								is.read(ptr, len);
								int amtRead = static_cast<int>(is.gcount());
								if (amtRead == len)
								{
									ptr[len] = 0;
									value = ptr;
								}
							}
						}
					}
				}
			}
		}
	}
}

static void findToken(const SVJson::Parser::TokenList& tokens, std::istream& is, const std::string& tokenName, POINT& pos)
{
	is.clear(); // clear error/eof bits

	for (SVJson::Parser::TokenList::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		const SVJson::jsontok_t& rToken = (*it);
		if (SVJson::JSON_STRING == rToken.type)
		{
			int len = rToken.end - rToken.start;
			if (len == tokenName.length())
			{
				char key[80];
				// read the key
				is.seekg(rToken.start, std::ios_base::beg);
				is.read(key, len);
				int amtRead = static_cast<int>(is.gcount());
				if (amtRead == len)
				{
					key[len] = 0;
					if (tokenName == key)
					{
						++it;
						const SVJson::jsontok_t& rToken = (*it);
						if (rToken.type == SVJson::JSON_STRING)
						{
							pos.x = rToken.start;
							pos.y = rToken.end;
						}
					}
				}
			}
		}
	}
}

// This method will decode the base64 encoded stream and write it to a file
static HRESULT ExtractBase64Contents(std::istream& is, std::stringstream& ss)
{
	HRESULT hr = S_FALSE;
	SVJson::Parser parser;
	SVJson::jsonerr_t rc = parser.Parse(SVJson::ParseStreamAdapter(is));
	if (rc == SVJson::JSON_SUCCESS)
	{
		const SVJson::Parser::TokenList& tokens = parser.GetTokens();

		// check the HResult first
		std::string status;
		findToken(tokens, is, SVRC::cmd::hr, status);
		if (!status.empty())
		{
			if ("0" != status)
			{
				return hr;
			}
		}
		POINT contentsPos = { -1, -1 };
		std::string filename;
		findToken(tokens, is, SVRC::result::contents, contentsPos);
		findToken(tokens, is, SVRC::result::filePath, filename);

		if (!filename.empty() && (-1 != contentsPos.x && -1 != contentsPos.y) && contentsPos.y > contentsPos.x)
		{
			hr = S_OK;
			// Decode Base64 to file
			const size_t enc_len = 1024 * 1024;	// base64 encoded data
			int amtToWrite = (enc_len / 4) * 3; // unencoded data

			try
			{
				boost::scoped_array<char> enc_buff(new char[enc_len + 1]);
				boost::scoped_array<unsigned char> file_buff(new unsigned char[amtToWrite + 1]);
				char* src = enc_buff.get();
				unsigned char* dst = file_buff.get();
				if (src && dst)
				{
					is.clear(); // clear any errors/eof bits...
					is.seekg(contentsPos.x, std::ios_base::beg);

					std::ofstream os;
					os.open(filename.c_str(), std::ios::out | std::ios::binary);
					if (os.is_open())
					{
						int len = contentsPos.y - contentsPos.x;
						while (!is.eof() && len > 0 && S_OK == hr)
						{
							is.read(src, (len < enc_len) ? len : enc_len);
							int amt_read = static_cast<int>(is.gcount());
							if (amt_read)
							{
								::Base64Decode(src, amt_read, dst, &amtToWrite);
								dst[amtToWrite] = '\0';
								os.write(reinterpret_cast<char *>(dst), amtToWrite);
								// Check for write failure
								if (os.fail() || os.bad())
								{
									hr = HRESULT_FROM_WIN32(ERROR_WRITE_FAULT);
								}
								len -= amt_read;
							}
							else
							{
								break;
							}
						}
						os.close();
						// Reset the input stream to the beginning
						is.clear();
						is.seekg(0, std::ios_base::beg);

						if (S_OK == hr)
						{
							// Reassamble Json removing Base64 contents
							for (long i = 0; i < contentsPos.x; i++)
							{
								ss.put(is.get());
							}
							is.seekg(contentsPos.y, std::ios_base::beg);
							while (!is.eof())
							{
								ss.put(is.get());
							}
						}
					}
					else
					{
						hr = HRESULT_FROM_WIN32(ERROR_OPEN_FAILED); // can't open file
					}
				}
				else
				{
					hr = E_POINTER; // can't get memory
				}
			}
			catch (std::bad_alloc& badAllocException)
			{
				std::cout << badAllocException.what() << std::endl;
				hr = E_POINTER;
			}
			catch (...)
			{
				hr = E_POINTER;
			}
		}
	}
	return hr;
}

// This method will base64 encode the file and send it over the socket
template<typename API>
HRESULT SVControlCommands::SendFile(SvSol::SVRCClientSocket<API>& rSocket, std::istream& is)
{
	HRESULT hr = S_OK;

	int enc_len = 64 * 1024;  // base64 encoded data
	const size_t amtToRead = (enc_len / 4) * 3; // unencoded file data

	try
	{
		boost::scoped_array<unsigned char> file_buff(new unsigned char[amtToRead]);
		boost::scoped_array<char> enc_buff(new char[enc_len + 1]);

		unsigned char* src = file_buff.get();
		char* dst = enc_buff.get();
		if (src && dst)
		{
			while (!is.eof() && S_OK == hr)
			{
				is.read(reinterpret_cast<char *>(src), amtToRead);
				int amt_read = static_cast<int>(is.gcount());
				if (amt_read)
				{
					::Base64Encode(src, amt_read, dst, &enc_len, ATL_BASE64_FLAG_NOCRLF);
					dst[enc_len] = '\0';
					hr = WriteToSocket(dst, rSocket);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			hr = E_POINTER;
		}
	}
	catch (std::bad_alloc& badAllocException)
	{
		std::wstring msg(L"SendFile");
		msg += SVStringConversions::to_utf16(badAllocException.what());
		SVLOG(msg.c_str());
		hr = E_POINTER;
	}
	catch (...)
	{
		hr = E_POINTER;
	}
	return hr;
}

static HRESULT SplitCmd(const std::string& rToken, const std::string& cmd, std::string& cmdPart1, std::string& cmdPart2)
{
	size_t pos = cmd.find(rToken);
	if (pos != std::string::npos)
	{
		pos = cmd.find('\"', pos + rToken.length());
		if (pos != std::string::npos)
		{
			pos = cmd.find(':', pos + 1);
			if (pos != std::string::npos)
			{
				pos = cmd.find('\"', pos + 1);
				if (pos != std::string::npos)
				{
					cmdPart1 = cmd.substr(0, pos + 1);
					cmdPart2 = cmd.substr(pos + 1);
				}
			}
		}
	}
	return (!cmdPart1.empty() && !cmdPart2.empty()) ? S_OK : E_INVALIDARG;
}

template<typename API>
SVControlResponse SVControlCommands::SendCommandWithFile(SvSol::SVRCClientSocket<API> & p_rSocket, const Json::Value & p_cmd, _bstr_t filepath, unsigned long p_TimeoutInSeconds)
{
	SVControlResponse l_Status;

	if (m_Command.IsJsonCommandEmpty())
	{
		JsonWriter writer;
		std::string cmd = writer.write(p_cmd);

		if (!cmd.empty())
		{
			std::string fPath = SVStringConversions::to_utf8(filepath);
			std::ifstream is;
			is.open(fPath, std::ios::in | std::ios::binary);
			if (is.is_open())
			{
				p_rSocket.SetBlocking();
				try
				{
					l_Status.m_Status.hResult = m_Command.SetJsonCommand(cmd, p_TimeoutInSeconds * 1000);

					if (S_OK == l_Status.m_Status.hResult)
					{
						std::string cmdPart1;
						std::string cmdPart2;
						SVLOG((l_Status.m_Status.hResult = SplitCmd(SVRC::arg::contents, cmd, cmdPart1, cmdPart2)));
						if (S_OK == l_Status.m_Status.hResult)
						{
							// send 1st part of the Json (stop just after contents: ")
							SVLOG((l_Status.m_Status.hResult = WriteToSocket(cmdPart1, p_rSocket)));
						}
						if (S_OK == l_Status.m_Status.hResult)
						{
							SVLOG((l_Status.m_Status.hResult = SendFile(p_rSocket, is)));
						}
						if (S_OK == l_Status.m_Status.hResult)
						{
							// send remaining Json
							SVLOG((l_Status.m_Status.hResult = WriteToSocket(cmdPart2, p_rSocket)));
						}
						if (S_OK == l_Status.m_Status.hResult)
						{
							SVLOG((l_Status.m_Status.hResult = m_Command.WaitForRequest(p_TimeoutInSeconds * 1000)));

							if (S_OK == l_Status.m_Status.hResult)
							{
								const Json::Value& val = m_Command.GetJsonResults();

								SVJsonValueGetter<SVControlResponse> rspGetter(this);

								l_Status = rspGetter(val);
							}
						}
					}
					l_Status.m_Status.FormatMessage();
				}
				catch (std::exception & ex)
				{
					SVLOG((l_Status.m_Status.hResult = E_UNEXPECTED));
					l_Status.m_Status.errorText = SVStringConversions::to_utf16(ex.what());
				}
				catch (...)
				{
					SVLOG((l_Status.m_Status.hResult = E_UNEXPECTED));
					l_Status.m_Status.errorText = L"Unknown Error occurred";
				}
				p_rSocket.SetNonBlocking();
				is.close();
			}
			else
			{
				SVLOG((l_Status.m_Status.hResult = E_INVALIDARG));
				l_Status.m_Status.errorText = L"Unable to open file.";
			}
			m_Command.ClearJsonCommand();
		}
		else
		{
			SVLOG((l_Status.m_Status.hResult = E_INVALIDARG));
			l_Status.m_Status.errorText = L"Command string empty.";
		}
	}
	else
	{
		SVLOG((l_Status.m_Status.hResult = STG_E_INUSE));
		l_Status.m_Status.errorText = L"System busy.";
	}
	return l_Status;
}

HRESULT SVControlCommands::CheckSocketConnection()
{
	HRESULT l_Status = S_OK;
	SvSol::SVSocketError::ErrorEnum err = SvSol::SVSocketError::Success;
	if (!(m_ClientSocket.IsConnected()))
	{
		if (0 < m_ServerName.length())
		{
			l_Status = SvSol::SVSocketError::HrFromSocketError(m_ClientSocket.BuildConnection(m_ServerName, m_CommandPort, 1000));
		}
		else
		{
			l_Status = INET_E_INVALID_URL;
		}
		SVLOG(l_Status);
	}

	if (l_Status == S_OK && m_pFrontEndApi.get() && !m_pFrontEndApi->IsConnected())
	{
		m_pFrontEndApi->Connect(m_RunServerPort, static_cast<char*>(m_ServerName));
		m_RRSConnected = m_pFrontEndApi->IsConnected();
	}

	m_Connected = l_Status == S_OK;
	return l_Status;
}

void SVControlCommands::OnConnectionStatus(SvSol::ConnectionState state)
{
	if (state == SvSol::Disconnected)
	{
		m_Connected = false;
		Json::Value notify(Json::objectValue);
		notify[SVRC::cmd::notfctn] = SVNotificationTypes::Disconnected;
		notify[SVRC::cmd::dataItems] = Json::Value(Json::objectValue);
		Notify(notify);
		SVLogDebug(L"Connection Disconnected");
	}
	else if (state == SvSol::Connected)
	{
		m_Connected = true;
	}
}

// check to see if the current command is GetConfig and handle the response.
// This method will extract the base64 contents from the stream and write it to disk.
static HRESULT HandleGetCommandWithContents(std::istream& is, SVJsonCommandHelper& rCommand, const std::string& commandName)
{
	HRESULT hr = S_FALSE;

	std::string jsonCommand;
	if (S_OK == rCommand.GetJsonCommand(jsonCommand, INFINITE))
	{
		Json::Reader reader;
		Json::Value jsonValues;
		Json::Value jsonCommandValues;

		if (reader.parse(jsonCommand, jsonCommandValues, false))
		{
			if (jsonCommandValues[SVRC::cmd::name] == commandName)
			{
				std::stringstream ss;
				hr = ExtractBase64Contents(is, ss);
				if (S_OK == hr)
				{
					if (reader.parse(ss, jsonValues, false))
					{
						bool bNotify = JsonDataHelper::DoesCommandMatchResponseWithID(jsonCommandValues, jsonValues);

						if (bNotify)
						{
							bNotify = (S_OK == rCommand.SetJsonResults(jsonValues));
						}

						if (bNotify)
						{
							rCommand.NotifyRequestComplete();
						}
					}
				}
				else
				{
					is.seekg(0, std::ios_base::beg);
				}
			}
		}
	}
	return hr;
}

void SVControlCommands::OnControlDataReceived(std::istream& is)
{
	// check for GetConfig for now...
	HRESULT hr = HandleGetCommandWithContents(is, m_Command, SVRC::cmdName::getConfig);

	if (S_FALSE == hr)
	{
		// parse the json
		Json::Reader l_Reader;
		Json::Value l_JsonValues;
		if (l_Reader.parse(is, l_JsonValues, false))
		{
			if (JsonDataHelper::IsNotification(l_JsonValues))
			{
				HRESULT hr = Notify(l_JsonValues);
			}
			else
			{
				std::string l_JsonCommand;

				if (m_Command.GetJsonCommand(l_JsonCommand, INFINITE) == S_OK)
				{
					Json::Value l_JsonCommandValues;

					if (l_Reader.parse(l_JsonCommand, l_JsonCommandValues, false))
					{
						bool l_Notify = JsonDataHelper::DoesCommandMatchResponseWithID(l_JsonCommandValues, l_JsonValues);

						if (l_Notify)
						{
							l_Notify = (m_Command.SetJsonResults(l_JsonValues) == S_OK);
						}

						if (l_Notify)
						{
							m_Command.NotifyRequestComplete();
						}
					}
				}
			}
		}
	}
	else if (S_OK != hr)
	{
		// Build error response
		Json::Reader l_Reader;
		std::string l_JsonCommand;
		Json::Value l_JsonValues;

		if (S_OK == m_Command.GetJsonCommand(l_JsonCommand, INFINITE))
		{
			Json::Value l_JsonCommandValues;
			if (l_Reader.parse(l_JsonCommand, l_JsonCommandValues, false))
			{
				l_JsonValues = l_JsonCommandValues;
			}
		}
		if (!l_JsonValues.isObject())
		{
			l_JsonValues = Json::Value(Json::objectValue);
			l_JsonValues[SVRC::cmd::name] = SVRC::cmdName::getConfig;
			l_JsonValues[SVRC::cmd::id] = 0;
		}
		l_JsonValues[SVRC::cmd::hr] = hr;

		if (S_OK == m_Command.SetJsonResults(l_JsonValues))
		{
			m_Command.NotifyRequestComplete();
		}
	}
}

HRESULT SVControlCommands::Notify(const Json::Value& p_JsonValues)
{
	CComBSTR l_Output;
	SVNotificationTypesEnum l_Type;

	HRESULT hr = BuildDatagram(l_Output, p_JsonValues, l_Type);
	SVLOG(hr);

	if (hr == S_OK)
	{
		_variant_t l_Data(l_Output.Detach());
		m_Notifier(l_Data, l_Type);
	}

	return hr;
}

HRESULT SVControlCommands::BuildDatagram(CComBSTR& p_BSTR, const Json::Value& p_JsonValue, SVNotificationTypesEnum& p_Type)
{
	HRESULT hr = S_OK;

	Json::FastWriter writer;
#ifdef SEND_JSON
	p_BSTR = writer.write(p_JsonValue).c_str();
#else
	// Put the JSON source into a stringstream to use with property_tree.
	std::stringstream l_InStream;
	l_InStream << writer.write(p_JsonValue).c_str();

	// Populate the property_tree
	boost::property_tree::ptree l_Tree;
	boost::property_tree::read_json(l_InStream, l_Tree);
	boost::property_tree::ptree l_NewTree;
	l_NewTree.put("SVRC", "");
	l_NewTree.put_child("SVRC", l_Tree);
	std::string l_NotificationNode = l_Tree.get< std::string >(SVRC::cmd::notfctn);

	try
	{
		if (0 == l_NotificationNode.compare(SVRC::notification::lastmodified))
		{
			p_Type = LastModified;
		}
		else if (0 == l_NotificationNode.compare(SVRC::notification::currentmode))
		{
			p_Type = CurrentMode;
		}
		else if (0 == l_NotificationNode.compare(SVRC::notification::MessageNotification))
		{
			p_Type = MessageNotification;
		}

		else
		{
			int l_Type = 0;
			std::stringstream l_Convert(l_NotificationNode);
			l_Convert >> l_Type;
			p_Type = static_cast<SVNotificationTypesEnum>(l_Type);
		}
	}
	catch (...)
	{
		// Unable to parse the type.
		p_Type = UnknownNotificationType;
	}

	// Create the XML from the property_tree.
	std::stringstream l_OutStream;
	boost::property_tree::xml_writer_settings< boost::property_tree::ptree::key_type > l_Settings(' ', 4);
	boost::property_tree::write_xml(l_OutStream, l_NewTree, l_Settings);
	p_BSTR = l_OutStream.str().c_str();
#endif
	return hr;
}

HRESULT SVControlCommands::RegisterMonitorList(const _bstr_t & listName, const _bstr_t & ppqName, int rejectDepth,
	const CComVariant & productItemList, const CComVariant & rejectCondList,
	const CComVariant & failStatusList, ISVErrorObjectList ** errors, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::regMon);

		builder.Argument(SVRC::arg::listName, Json::Value(SVStringConversions::to_utf8(listName)));
		builder.Argument(SVRC::arg::ppqName, Json::Value(SVStringConversions::to_utf8(ppqName)));
		builder.Argument(SVRC::arg::rejectDepth, Json::Value(rejectDepth));
		builder.Argument(SVRC::arg::prodList, MkArray(productItemList));
		builder.Argument(SVRC::arg::rjctList, MkArray(rejectCondList));
		builder.Argument(SVRC::arg::failList, MkArray(failStatusList));

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);

		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			if (rsp.results[SVRC::result::faults].vt == VT_DISPATCH && rsp.results[SVRC::result::faults].pdispVal)
			{
				l_Status = rsp.results[SVRC::result::faults].pdispVal->QueryInterface(IID_ISVErrorObjectList, reinterpret_cast<void**>(errors));
			}
			else
			{
				l_Status = E_POINTER;
			}
			SVLOG(l_Status);
		}

		p_rStatus = rsp.m_Status;

		if (l_Status == S_OK)
		{
			l_Status = rsp.m_Status.hResult;
		}
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::QueryProductList(const _bstr_t & listName, const std::string & cmd, CComVariant & itemNames, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(cmd);

		builder.Argument(SVRC::arg::listName, Json::Value(SVStringConversions::to_utf8(listName)));

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			std::map<std::string, CComVariant>::iterator it = rsp.results.find(SVRC::result::names);
			if (it != rsp.results.end())
			{
				itemNames.Attach(&(it->second));
			}
			else
			{
				itemNames = CComVariant();
			}
		}

		p_rStatus = rsp.m_Status;
		l_Status = rsp.m_Status.hResult;
		SVLOG(l_Status);
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetProduct(bool bGetReject, const _bstr_t & listName, long triggerCount, long imageScale, ISVProductItems ** currentViewItems, SVCommandStatus& p_rStatus)
{
	HRESULT Status = E_FAIL;
	try
	{
		if (0 == listName.length())
		{
			throw std::invalid_argument("missing Monitorlistname");
		}
		if(0 == m_pFrontEndApi)
		{
			throw std::invalid_argument("Not connected To RRServer");
		}

		RRApi::GetProductRequest ProductRequest;
		ProductRequest.set_name(static_cast<const char*>(listName));
		ProductRequest.set_trigger(( -1 < triggerCount ) ? triggerCount : -1);
		ProductRequest.set_pevioustrigger(-1);

		ProductRequest.set_nameinresponse(true);
		RRApi::GetProductResponse resp;
		if (bGetReject)
		{
			resp = m_pFrontEndApi->GetReject(ProductRequest).get();
		}
		else
		{
			resp = m_pFrontEndApi->GetProduct(ProductRequest).get();
		}
		if (resp.has_status())
		{
			p_rStatus.hResult = (HRESULT)resp.status();
			if (resp.status() == RRApi::IsValid)
			{
				Status = S_OK;
				GetProductPtr(*(m_pFrontEndApi.get()), resp)->QueryInterface(IID_ISVProductItems, reinterpret_cast<void**>(currentViewItems));
			}
			else
			{
				p_rStatus.errorText = SVStringConversions::to_utf16(RRApi::State_Name(resp.status()));
			}

		}
		SVLOG(Status);
	}
	HANDLE_EXCEPTION(Status, p_rStatus)


	return Status;
}
HRESULT SVControlCommands::GetProduct(const _bstr_t & listName, long triggerCount, long imageScale, ISVProductItems ** currentViewItems, SVCommandStatus& p_rStatus)
{
	return GetProduct(false, listName, triggerCount, imageScale, currentViewItems, p_rStatus);
}
HRESULT SVControlCommands::GetRejects(const _bstr_t & listName, long triggerCount, long imageScale, ISVProductItems ** currentViewItems, SVCommandStatus& p_rStatus)
{
	return GetProduct(true, listName, triggerCount, imageScale, currentViewItems, p_rStatus);

}

HRESULT SVControlCommands::ActivateMonitorList(const _bstr_t & listName, bool activ, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::actvMonList);

		builder.Argument(SVRC::arg::listName, Json::Value(SVStringConversions::to_utf8(listName)));
		builder.Argument(SVRC::arg::active, Json::Value(activ));

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);

		p_rStatus = rsp.m_Status;
		l_Status = rsp.m_Status.hResult;
		SVLOG(l_Status);
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetFailStatus(const _bstr_t & listName, CComVariant & values, SVCommandStatus& p_rStatus)
{
	HRESULT Status = S_OK;
	try
	{
		RRApi::GetFailStatusRequest FailstatusRequest;
		if (0 == listName.length())
		{
			throw std::invalid_argument("missing Monitorlistname");
		}
		if (0 == m_pFrontEndApi)
		{
			throw std::invalid_argument("Not connected To RRServer");
		}
		FailstatusRequest.set_name(static_cast<const char*>(listName));

		FailstatusRequest.set_nameinresponse(true);
		RRApi::GetFailStatusResponse resp;
		resp = m_pFrontEndApi->GetFailStatus(FailstatusRequest).get();
		if (resp.has_status())
		{
			p_rStatus.hResult = (HRESULT)resp.status();
			if (resp.status() == RRApi::IsValid)
			{
				Status = S_OK;
				CComVariant variant = GetFailList(*m_pFrontEndApi.get(), resp);
				values.Attach(&variant);
			}
			else
			{
				p_rStatus.errorText = SVStringConversions::to_utf16(RRApi::State_Name(resp.status()));
			}

		}
		SVLOG(Status);
	}
	HANDLE_EXCEPTION(Status, p_rStatus)
		return Status;
}
		


HRESULT SVControlCommands::ShutDown(SVShutdownOptionsEnum options, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::shutdownSVIM);
		builder.Argument(SVRC::arg::options, static_cast<long>(options));
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rStatus = rsp.m_Status;
		l_Status = p_rStatus.hResult;
		SVLOG(l_Status);
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}



HRESULT SVControlCommands::GetInspectionNames(CComVariant & listNames, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getInspectionNames);

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			std::map<std::string, CComVariant>::iterator it = rsp.results.find(SVRC::result::names);
			if (it != rsp.results.end())
			{
				listNames.Attach(&(it->second));
			}
			else
			{
				listNames = CComVariant();
			}
		}

		p_rStatus = rsp.m_Status;
		l_Status = rsp.m_Status.hResult;
		SVLOG(l_Status);
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}


HRESULT SVControlCommands::QueryMonitorListNames(CComVariant & listNames, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::qryMonListNames);

		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		if (SUCCEEDED(rsp.m_Status.hResult))
		{
			std::map<std::string, CComVariant>::iterator it = rsp.results.find(SVRC::result::names);
			if (it != rsp.results.end())
			{
				listNames.Attach(&(it->second));
			}
			else
			{
				listNames = CComVariant();
			}
		}

		p_rStatus = rsp.m_Status;
		l_Status = rsp.m_Status.hResult;
		SVLOG(l_Status);
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::GetMonitorListProperties(const _bstr_t & rListName, long&  RejectDepth, bool& IsActive, BSTR&  ppqName, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;
	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getMonitorListPropCmd);
		builder.Argument(SVRC::arg::listName, Json::Value(SVStringConversions::to_utf8(rListName)));
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), TwoMinutes);
		if (rsp.m_Status.hResult == S_OK)
		{
			RejectDepth = rsp.results[SVRC::result::rejectDepth].lVal;
			IsActive = rsp.results[SVRC::result::active].boolVal ? true : false;
			ppqName = ::SysAllocString(rsp.results[SVRC::result::ppqName].bstrVal);
		}

		l_Status = rsp.m_Status.hResult;
		SVLOG(l_Status);
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)
		return l_Status;

}

HRESULT SVControlCommands::GetMaxRejectQeueDepth(unsigned long& p_rState, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getMaxRejectDeptCmd);
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rState = rsp.results[SVRC::result::maxRejectDepth].lVal;
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)
		return l_Status;
}



HRESULT SVControlCommands::GetProductFilter(const _bstr_t& rListName, unsigned long& rFilter, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::getProductFilter);
		builder.Argument(SVRC::arg::listName, Json::Value(SVStringConversions::to_utf8(rListName)));
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		rFilter = rsp.results[SVRC::result::filter].lVal;
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

HRESULT SVControlCommands::SetProductFilter(const _bstr_t& rListName, unsigned long filter, SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	try
	{
		SVJsonCmdBuilder builder(SVRC::cmdName::setProductFilter);
		builder.Argument(SVRC::arg::listName, Json::Value(SVStringConversions::to_utf8(rListName)));
		builder.Argument(SVRC::arg::filter, static_cast<long>(filter));
		SVControlResponse rsp = SendCommand(m_ClientSocket, builder.Value(), FiveSeconds);
		p_rStatus = rsp.m_Status;
		SVLOG((l_Status = p_rStatus.hResult));
	}
	HANDLE_EXCEPTION(l_Status, p_rStatus)

		return l_Status;
}

#pragma warning(pop)
