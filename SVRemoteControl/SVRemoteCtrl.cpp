//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrl
//* .File Name       : $Workfile:   SVRemoteCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.23  $
//* .Check In Date   : $Date:   11 Sep 2014 09:53:22  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <boost/algorithm/string.hpp>
#include <Winver.h>
#include "SVRemoteCtrl.h"
#include "SVRemoteControlConstants.h"
#include "SVAsyncControlCommandGetConfig.h"
#include "SVAsyncControlCommandPutConfig.h"
#include "SVAsyncControlCommandPutFile.h"
#include "SVAsyncControlCommandSetMode.h"
#include "SVCommandTemplate.h"
#include "SVControlCommands.h"
#include "SVProductItems.h"
#include "SVValueObject.h"
#include "SVValueObjectList.h"
#include "EventLog.h"
#include "SVStringConversions.h"

#pragma endregion Includes

#pragma comment (lib, "version.lib")

#ifdef SVERROR
#undef SVERROR
#endif

#define SVERROR(msg, iid, hr) SVLOG(msg); Error(msg, iid, hr)


static std::string GetVersionString()
{
	std::string verStr;

	HINSTANCE hInst = (HINSTANCE)_AtlBaseModule.m_hInst;
	char moduleFilename[512];
	::GetModuleFileNameA(hInst, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSizeA(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfoA(moduleFilename, NULL, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = nullptr;
		UINT Len = 0;
		if (::VerQueryValueA(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len))
		{
			std::stringstream buf;

			buf << HIWORD(pFileInfo->dwFileVersionMS);
			buf << ".";
			buf << std::setfill('0') << std::setw(2) << LOWORD(pFileInfo->dwFileVersionMS);

			if (HIWORD(pFileInfo->dwFileVersionLS) < 255)
			{
				buf << "b" << HIWORD(pFileInfo->dwFileVersionLS);
			}
			verStr = buf.str();
		}
	}
	delete[] lpData;

#ifdef _DEBUG
	verStr += "d";        // For debug builds.
#endif

	return verStr;
}

SVRemoteCtrl::SVRemoteCtrl()
	: m_ctlStatic(_T("Static"), this, 1),
	m_cookie(0),
	c_success(L"Success"),
	c_svcontrol(L"SVControl"),
	c_svobserver(L"SVObserver"),
	m_device(svr::svobserver),
	m_servername(L""),
	m_VPName(L""),
	m_imageScale(100),
m_dispatcher(new SVControlCommands(boost::bind(&SVRemoteCtrl::NotifyClient, this, _1, _2)))
{
	m_bWindowOnly = TRUE;

	m_AsyncThread.Create(boost::bind(&SVRemoteCtrl::AsyncThreadFunc, this, _1), _T("SVRemoteCtrl"));
}

LRESULT SVRemoteCtrl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = CComControl<SVRemoteCtrl>::OnSetFocus(uMsg, wParam, lParam, bHandled);
	if (m_bInPlaceActive)
	{
		if (!IsChild(::GetFocus()))
			m_ctlStatic.SetFocus();
	}
	return lRes;
}

LRESULT SVRemoteCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;
	GetWindowRect(&rc);
	rc.right -= rc.left;
	rc.bottom -= rc.top;
	rc.top = rc.left = 0;
	m_ctlStatic.Create(m_hWnd, rc);
	return 0;
}

STDMETHODIMP SVRemoteCtrl::SetObjectRects(LPCRECT prcPos, LPCRECT prcClip)
{
	IOleInPlaceObjectWindowlessImpl<SVRemoteCtrl>::SetObjectRects(prcPos, prcClip);
	int cx, cy;
	cx = prcPos->right - prcPos->left;
	cy = prcPos->bottom - prcPos->top;
	::SetWindowPos(m_ctlStatic.m_hWnd, NULL, 0,
		0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	return S_OK;
}

// ISupportsErrorInfo
STDMETHODIMP SVRemoteCtrl::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ISVRemoteCtrl,
	};

	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i], riid))
			return S_OK;
	}
	return S_FALSE;
}

// IPropertyNotifySink
STDMETHODIMP SVRemoteCtrl::OnRequestEdit(DISPID)
{
	return S_OK;
}

STDMETHODIMP SVRemoteCtrl::OnChanged(DISPID)
{
	return S_OK;
}

HRESULT SVRemoteCtrl::FinalConstruct()
{
	return S_OK;
}

void SVRemoteCtrl::FinalRelease()
{
	m_AsyncThread.Destroy();
}

STDMETHODIMP SVRemoteCtrl::BeginGetConfig(BSTR fileName)
{
	SVCommandTemplatePtr l_CommandPtr(new SVAsyncControlCommandGetConfig(fileName));

	HRESULT l_Status = m_AsyncCommandHelper.SetCommand(l_CommandPtr, INFINITE);

	if (l_Status == S_OK)
	{
		l_Status = m_AsyncThread.Signal();

		if (l_Status != S_OK)
		{
			m_AsyncCommandHelper.ClearCommand();
			m_AsyncCommandHelper.NotifyRequestComplete();

			SVERROR(L"Error signaling command thread to execute.", IID_ISVRemoteCtrl, l_Status);
		}
	}
	else
	{
		SVERROR(L"Error setting command in asynchronous helper object.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::FinishGetConfig(LONG Timeout, VARIANT_BOOL CancelAfterTimeout)
{
	m_AsyncCommandHelper.SetCancel(CancelAfterTimeout != VARIANT_FALSE);

	HRESULT l_Status = m_AsyncCommandHelper.WaitForRequest(Timeout);

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus = m_AsyncCommandHelper.GetStatus();

		l_Status = l_CommandStatus.hResult;

		if (l_Status != S_OK)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		SVERROR(L"Wait for asynchronous command was not successful.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::BeginPutConfig(BSTR fileName)
{
	SVCommandTemplatePtr l_CommandPtr(new SVAsyncControlCommandPutConfig(fileName, ""));

	HRESULT l_Status = m_AsyncCommandHelper.SetCommand(l_CommandPtr, INFINITE);

	if (l_Status == S_OK)
	{
		l_Status = m_AsyncThread.Signal();

		if (l_Status != S_OK)
		{
			m_AsyncCommandHelper.ClearCommand();
			m_AsyncCommandHelper.NotifyRequestComplete();

			SVERROR(L"Error signaling command thread to execute.", IID_ISVRemoteCtrl, l_Status);
		}
	}
	else
	{
		SVERROR(L"Error setting command in asynchronous helper object.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::FinishPutConfig(LONG Timeout, VARIANT_BOOL CancelAfterTimeout)
{
	m_AsyncCommandHelper.SetCancel(CancelAfterTimeout != VARIANT_FALSE);

	HRESULT l_Status = m_AsyncCommandHelper.WaitForRequest(Timeout);

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus = m_AsyncCommandHelper.GetStatus();

		l_Status = l_CommandStatus.hResult;

		if (l_Status != S_OK)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		SVERROR(L"Wait for asynchronous command was not successful.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::BeginPutDeviceFile(BSTR sourcePath, BSTR destinationPath, VARIANT_BOOL saveInConfig)
{
	SVCommandTemplatePtr l_CommandPtr(new SVAsyncControlCommandPutFile(sourcePath, destinationPath, saveInConfig));

	HRESULT l_Status = m_AsyncCommandHelper.SetCommand(l_CommandPtr, INFINITE);

	if (l_Status == S_OK)
	{
		l_Status = m_AsyncThread.Signal();

		if (l_Status != S_OK)
		{
			m_AsyncCommandHelper.ClearCommand();
			m_AsyncCommandHelper.NotifyRequestComplete();

			SVERROR(L"Error signaling command thread to execute.", IID_ISVRemoteCtrl, l_Status);
		}
	}
	else
	{
		SVERROR(L"Error setting command in asynchronous helper object.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::FinishPutDeviceFile(LONG Timeout, VARIANT_BOOL CancelAfterTimeout)
{
	m_AsyncCommandHelper.SetCancel(CancelAfterTimeout != VARIANT_FALSE);

	HRESULT l_Status = m_AsyncCommandHelper.WaitForRequest(Timeout);

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus = m_AsyncCommandHelper.GetStatus();

		l_Status = l_CommandStatus.hResult;

		if (l_Status != S_OK)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		SVERROR(L"Wait for asynchronous command was not successful.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::BeginSetDeviceMode(SVObserverModesEnum desiredMode)
{
	SVCommandTemplatePtr l_CommandPtr(new SVAsyncControlCommandSetMode(desiredMode));

	HRESULT l_Status = m_AsyncCommandHelper.SetCommand(l_CommandPtr, INFINITE);

	if (l_Status == S_OK)
	{
		l_Status = m_AsyncThread.Signal();

		if (l_Status != S_OK)
		{
			m_AsyncCommandHelper.ClearCommand();
			m_AsyncCommandHelper.NotifyRequestComplete();

			SVERROR(L"Error signaling command thread to execute.", IID_ISVRemoteCtrl, l_Status);
		}
	}
	else
	{
		SVERROR(L"Error setting command in asynchronous helper object.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::FinishSetDeviceMode(LONG Timeout, VARIANT_BOOL CancelAfterTimeout)
{
	m_AsyncCommandHelper.SetCancel(CancelAfterTimeout != VARIANT_FALSE);

	HRESULT l_Status = m_AsyncCommandHelper.WaitForRequest(Timeout);

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus = m_AsyncCommandHelper.GetStatus();

		l_Status = l_CommandStatus.hResult;

		if (l_Status != S_OK)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		SVERROR(L"Wait for asynchronous command was not successful.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::LoadConfig(BSTR filePath)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->LoadConfig(filePath, CommandStatus);

	if (l_Status != S_OK)
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetConfig(BSTR filePath)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->GetConfig(filePath, CommandStatus);

	if (l_Status != S_OK)
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::RunOnce(BSTR InspectionName)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->RunOnce(InspectionName, CommandStatus);

	if (l_Status != S_OK)
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetDeviceConfigReport(BSTR* report)
{
	HRESULT l_Status = S_OK;

	if (report != NULL)
	{
		SVCommandStatus l_CommandStatus;

		if ((*report) != NULL)
		{
			::SysFreeString(*report);

			(*report) = NULL;
		}

		l_Status = m_dispatcher->GetConfigReport(*report, l_CommandStatus);

		if (l_Status != S_OK)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Report parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}


STDMETHODIMP SVRemoteCtrl::GetDeviceMode(LONG* mode)
{
	HRESULT l_Status = S_OK;

	if (mode != NULL)
	{
		SVCommandStatus l_CommandStatus;

		*mode = 0;

		unsigned long l_Mode = 0;

		l_Status = m_dispatcher->GetMode(l_Mode, l_CommandStatus);

		if (l_Status != S_OK)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
		else
		{
			*mode = l_Mode;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Mode parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetDeviceOfflineCount(LONG* count)
{
	HRESULT l_Status = S_OK;

	if (count != NULL)
	{
		SVCommandStatus l_CommandStatus;

		*count = 0;

		unsigned long l_Count = 0;

		l_Status = m_dispatcher->GetOfflineCount(l_Count, l_CommandStatus);

		if (l_Status == S_OK)
		{
			*count = static_cast<long>(l_Count);
		}
		else
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Count parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetItems(VARIANT itemNames, ISVProductItems** items)
{
	HRESULT l_Status = S_OK;

	if (items != NULL)
	{
		if (*items != NULL)
		{
			(*items)->Release();
			(*items) = NULL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Items parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (l_Status == S_OK && itemNames.vt != (VT_ARRAY | VT_BSTR))
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Item names parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (l_Status == S_OK)
	{
		SVCommandStatus CommandStatus;

		l_Status = m_dispatcher->GetItems(itemNames, items, CommandStatus);

		if (!SUCCEEDED(l_Status))
		{
			SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
		}
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetState(LONG* state)
{
	HRESULT l_Status = S_OK;

	if (state != NULL)
	{
		SVCommandStatus l_CommandStatus;

		*state = 0;

		unsigned long l_State = 0;

		l_Status = m_dispatcher->GetState(l_State, l_CommandStatus);

		if (l_Status == S_OK)
		{
			*state = static_cast<long>(l_State);
		}
		else
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"State parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::PutConfig(BSTR filePath, LONG Timeout)
{
	HRESULT l_Status = BeginPutConfig(filePath);

	if (l_Status == S_OK)
	{
		l_Status = FinishPutConfig(Timeout, VARIANT_TRUE);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetVersion(BSTR* svobserver_ver, BSTR* svremotecontrol_ver, BSTR* runrejectserver_ver)
{
	HRESULT l_Status = S_OK;

	if (nullptr != svobserver_ver)
	{
		if (nullptr != (*svobserver_ver))
		{
			::SysFreeString(*svobserver_ver);
			(*svobserver_ver) = nullptr;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
		SVERROR(L"SVObserver version parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (nullptr != svremotecontrol_ver)
	{
		if (nullptr != (*svremotecontrol_ver))
		{
			::SysFreeString(*svremotecontrol_ver);
			(*svremotecontrol_ver) = nullptr;
		}
	}
	else if (S_OK == l_Status)
	{
		l_Status = E_INVALIDARG;
		SVERROR(L"SVRemoteControl version parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (nullptr != runrejectserver_ver)
	{
		if (nullptr != (*runrejectserver_ver))
		{
			::SysFreeString(*runrejectserver_ver);
			(*runrejectserver_ver) = nullptr;
		}
	}
	else if (S_OK == l_Status)
	{
		l_Status = E_INVALIDARG;
		SVERROR(L"Run/Reject server version parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}
	if (S_OK == l_Status)
	{
		SVCommandStatus l_CommandStatus;

		_bstr_t l_SVObserverVersion = L"";
		_bstr_t l_SVRemoteControlVersion = L"";
		_bstr_t l_RunRejectServerVersion = L"";

		std::wstring ov = SVStringConversions::to_utf16(GetVersionString());
		l_SVRemoteControlVersion = ov.c_str();
		(*svremotecontrol_ver) = l_SVRemoteControlVersion.Detach();

		l_Status = m_dispatcher->GetVersion(l_SVObserverVersion, l_RunRejectServerVersion, l_CommandStatus);

		if (S_OK == l_Status)
		{
			(*svobserver_ver) = l_SVObserverVersion.Detach();
			//( *SVC_ver ) = _bstr_t(L"3.0 \u03B2 7").Detach();
			(*runrejectserver_ver) = l_RunRejectServerVersion.Detach();
		}
		else
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}
	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetDeviceFile(BSTR sourcePath, BSTR destinationPath)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->GetFile(sourcePath, destinationPath, CommandStatus);

	if (l_Status != S_OK)
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::PutDeviceFile(BSTR sourcePath, BSTR destinationPath, VARIANT_BOOL saveInConfig)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->PutFile(sourcePath, destinationPath, saveInConfig, CommandStatus);

	if (l_Status != S_OK)
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::SetDeviceMode(SVObserverModesEnum desiredMode)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->SetMode(desiredMode, CommandStatus);

	if (l_Status != S_OK)
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::SetItems(ISVProductItems* pItems, VARIANT_BOOL RunOnce, ISVProductItems ** ppErrors)
{
	SVCommandStatus CommandStatus;

	HRESULT l_Status = m_dispatcher->SetItems(pItems, RunOnce, ppErrors, CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, CommandStatus.hResult);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::get_ServerAddress(BSTR* pVal)
{
	HRESULT l_Status = S_OK;

	if (pVal != NULL)
	{
		if ((*pVal) != NULL)
		{
			::SysFreeString(*pVal);

			(*pVal) = NULL;
		}

		(*pVal) = m_servername.copy();
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Value parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::get_VPAddress(BSTR* pVal)
{
	HRESULT l_Status = S_OK;

	if (pVal != NULL)
	{
		if ((*pVal) != NULL)
		{
			::SysFreeString(*pVal);

			(*pVal) = NULL;
		}

		(*pVal) = m_VPName.copy();
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Value parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::get_Connected(VARIANT_BOOL* pVal)
{
	*pVal = m_dispatcher->isConnected();
	return S_OK;
}

STDMETHODIMP SVRemoteCtrl::get_ServerType(BSTR* pVal)
{
	HRESULT l_Status = S_OK;

	if (pVal != NULL)
	{
		if ((*pVal) != NULL)
		{
			::SysFreeString(*pVal);

			(*pVal) = NULL;
		}

		(*pVal) = (m_device == svr::svcontrol) ? c_svcontrol.Copy() : c_svobserver.Copy();
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Value parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	return l_Status;
}

void SVRemoteCtrl::AsyncThreadFunc(bool& p_rWaitForEvent)
{
	SVCommandTemplatePtr l_CommandPtr;

	m_AsyncCommandHelper.SetCommandActive();

	if ((m_AsyncCommandHelper.GetCommand(l_CommandPtr, INFINITE) == S_OK) && (l_CommandPtr.get() != NULL))
	{
		try
		{
			l_CommandPtr->Execute(m_dispatcher);

			m_AsyncCommandHelper.SetStatus(l_CommandPtr->GetStatus());
		}
		catch (...)
		{
			m_AsyncCommandHelper.SetStatus(SVCommandStatus(E_UNEXPECTED, L"Unknown exception when executing asynchronous command."));
		}

		m_AsyncCommandHelper.NotifyRequestComplete();
	}

	m_AsyncCommandHelper.SetCommandComplete();
}

HRESULT SVRemoteCtrl::SetControlServer(SVCommandStatus& p_rStatus)
{
	HRESULT l_Status = S_OK;

	if (0 < m_servername.length())
	{
		m_VPName = m_servername;
	}
	else
	{
		m_VPName = L"";
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::Disconnect(void)
{
	m_dispatcher->ResetConnection();
	m_servername = _T("");
	m_VPName = _T("");
	return S_OK;
}

STDMETHODIMP SVRemoteCtrl::Connect(BSTR address, LONG Timeout)
{

	SVCommandStatus l_CommandStatus;
	HRESULT l_Status = m_dispatcher->SetConnectionData(address,  boost::posix_time::milliseconds(Timeout));

	if (l_Status == S_OK)
	{
		m_servername = address;

		l_Status = SetControlServer(l_CommandStatus);

		if (l_Status != NULL)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}

	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::get_ImageScale(LONG* pVal)
{
	*pVal = m_imageScale;
	return S_OK;
}

STDMETHODIMP SVRemoteCtrl::put_ImageScale(LONG newVal)
{
	m_imageScale = (0 < newVal && newVal <= 100) ? newVal : 100;
	return S_OK;
}

void SVRemoteCtrl::NotifyClient(_variant_t& p_Data, SVNotificationTypesEnum p_Type)
{
	Fire_OnNotify(p_Data, p_Type);
}

STDMETHODIMP SVRemoteCtrl::RegisterMonitorList(BSTR listName, BSTR ppqName, int rejectDepth, VARIANT productItemList,
	VARIANT rejectCondList, VARIANT failStatusList, ISVErrorObjectList ** errors)
{
	HRESULT l_Status = S_OK;
	SVCommandStatus l_CommandStatus;

	if (errors != NULL)
	{
		if (*errors != NULL)
		{
			(*errors)->Release();
			(*errors) = NULL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Errors parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	l_Status = m_dispatcher->RegisterMonitorList(listName, ppqName, rejectDepth, productItemList, rejectCondList, failStatusList, errors, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}

	return l_Status;
}


STDMETHODIMP SVRemoteCtrl::QueryProductList(BSTR listName, VARIANT* itemNames)
{
	SVCommandStatus l_CommandStatus;
	CComVariant result;
	VariantClear(itemNames);

	HRESULT l_Status = m_dispatcher->QueryMonitorList(listName, SvPb::ListType::productItem, result, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	else
	{
		result.Detach(itemNames);
	}

	return l_Status;
}


STDMETHODIMP SVRemoteCtrl::QueryRejectCondList(BSTR listName, VARIANT* itemNames)
{
	SVCommandStatus l_CommandStatus;
	CComVariant result;
	VariantClear(itemNames);

	HRESULT l_Status = m_dispatcher->QueryMonitorList(listName, SvPb::ListType::rejectCondition, result, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	else
	{
		result.Detach(itemNames);
	}

	return l_Status;
}


STDMETHODIMP SVRemoteCtrl::QueryFailStatusList(BSTR listName, VARIANT* itemNames)
{
	SVCommandStatus l_CommandStatus;
	CComVariant result;

	if (nullptr != itemNames && VT_EMPTY != itemNames->vt)
	{
		VariantClear(itemNames);
	}

	HRESULT l_Status = m_dispatcher->QueryMonitorList(listName, SvPb::ListType::failStatus, result, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	else
	{
		result.Detach(itemNames);
	}

	return l_Status;
}


STDMETHODIMP SVRemoteCtrl::GetProduct(BSTR listName, LONG triggerCount, LONG imageScale, ISVProductItems** currentViewItems)
{
	HRESULT l_Status = S_OK;

	if (currentViewItems != NULL)
	{
		if (*currentViewItems != NULL)
		{
			(*currentViewItems)->Release();
			(*currentViewItems) = NULL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Items parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus;

		l_Status = m_dispatcher->GetProduct(listName, triggerCount, imageScale, currentViewItems, l_CommandStatus);

		if (!SUCCEEDED(l_Status))
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}

	return l_Status;
}


STDMETHODIMP SVRemoteCtrl::GetReject(BSTR listName, LONG triggerCount, LONG imageScale, ISVProductItems** currentViewItems)
{
	HRESULT l_Status = S_OK;

	if (currentViewItems != NULL)
	{
		if (*currentViewItems != NULL)
		{
			(*currentViewItems)->Release();
			(*currentViewItems) = NULL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Items parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus;

		l_Status = m_dispatcher->GetRejects(listName, triggerCount, imageScale, currentViewItems, l_CommandStatus);

		if (!SUCCEEDED(l_Status))
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetDataDefinitionList(BSTR inspectionName, SVDataDefinitionListTypeEnum listType, ISVDataDefObjectList** entries)
{
	HRESULT l_Status = S_OK;

	if (NULL != entries)
	{
		if (NULL != *entries)
		{
			(*entries)->Release();
			(*entries) = NULL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Items parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}

	if (l_Status == S_OK)
	{
		SVCommandStatus l_CommandStatus;

		l_Status = m_dispatcher->GetDataDefinitionList(inspectionName, listType, entries, l_CommandStatus);

		if (!SUCCEEDED(l_Status))
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::ActivateMonitorList(BSTR listName)
{
	SVCommandStatus l_CommandStatus;

	HRESULT l_Status = m_dispatcher->ActivateMonitorList(listName, true, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::DeactivateMonitorList(BSTR listName)
{
	SVCommandStatus l_CommandStatus;

	HRESULT l_Status = m_dispatcher->ActivateMonitorList(listName, false, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetFailStatus(BSTR listName, VARIANT* values)
{
	SVCommandStatus l_CommandStatus;
	CComVariant result;

	if (nullptr != values && VT_EMPTY != values->vt)
	{
		VariantClear(values);
	}

	HRESULT l_Status = m_dispatcher->GetFailStatus(listName, result, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	else
	{
		result.Detach(values);
	}

	return l_Status;
}

//TODO: This global parameter is just for the dummy
long g_productFilterMode = LastInspected;

STDMETHODIMP SVRemoteCtrl::GetProductFilter(BSTR listName, LONG * pFilter)
{
	HRESULT l_Status = S_OK;

	if (pFilter != nullptr)
	{
		SVCommandStatus l_CommandStatus;
		*pFilter = LastInspected;
		unsigned long l_Filter = LastInspected;

		l_Status = m_dispatcher->GetProductFilter(listName, l_Filter, l_CommandStatus);
		if (S_OK != l_Status)
		{
			SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
		}
		else
		{
			*pFilter = l_Filter;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;

		SVERROR(L"Filter parameter is invalid.", IID_ISVRemoteCtrl, l_Status);
	}
	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::SetProductFilter(BSTR listName, SVProductFilterModesEnum filter)
{
	SVCommandStatus l_CommandStatus;

	HRESULT l_Status = m_dispatcher->SetProductFilter(listName, filter, l_CommandStatus);
	if (S_OK != l_Status)
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::Shutdown(SVShutdownOptionsEnum options)
{
	SVCommandStatus commandStatus;

	HRESULT status = m_dispatcher->ShutDown(options, commandStatus);

	if (status != S_OK)
	{
		SVERROR(commandStatus.errorText.c_str(), IID_ISVRemoteCtrl, commandStatus.hResult);
	}

	return status;
}

STDMETHODIMP SVRemoteCtrl::GetInspectionNames(VARIANT* listNames)
{
	SVCommandStatus l_CommandStatus;
	CComVariant result;

	if (nullptr != listNames && VT_EMPTY != listNames->vt)
	{
		VariantClear(listNames);
	}

	HRESULT l_Status = m_dispatcher->GetInspectionNames(result, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	else
	{
		result.Detach(listNames);
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::QueryMonitorListNames(VARIANT* listNames)
{
	SVCommandStatus l_CommandStatus;
	CComVariant result;

	if (nullptr != listNames && VT_EMPTY != listNames->vt)
	{
		VariantClear(listNames);
	}

	HRESULT l_Status = m_dispatcher->QueryMonitorListNames(result, l_CommandStatus);

	if (!SUCCEEDED(l_Status))
	{
		SVERROR(l_CommandStatus.errorText.c_str(), IID_ISVRemoteCtrl, l_CommandStatus.hResult);
	}
	else
	{
		result.Detach(listNames);
	}
	return l_Status;
}

STDMETHODIMP SVRemoteCtrl::GetMonitorListProperties(BSTR listName, LONG* RejectDepth, VARIANT_BOOL* isActive, BSTR* ppqName)
{
	HRESULT Status = S_OK;
	SVCommandStatus l_CommandStatus;
	if (nullptr == RejectDepth || nullptr == isActive || nullptr == ppqName)
	{
		Status = E_INVALIDARG;
		SVERROR(L"Invalid Parameter.", IID_ISVRemoteCtrl, Status);
		return Status;
	}

	if (nullptr != (*ppqName))
	{
		::SysFreeString(*ppqName);
		(*ppqName) = nullptr;
	}
	bool bIsactive = *isActive ? true : false;
	Status = m_dispatcher->GetMonitorListProperties(listName, *RejectDepth, bIsactive, *ppqName, l_CommandStatus);
	*isActive = static_cast<VARIANT_BOOL>(bIsactive);
	return Status;
}

STDMETHODIMP SVRemoteCtrl::GetMaxRejectQeueDepth(LONG* MaxRejectDepth)
{
	HRESULT Status = S_OK;
	SVCommandStatus CommandStatus;
	if (nullptr == MaxRejectDepth)
	{
		Status = E_INVALIDARG;
		SVERROR(L"Invalid Parameter.", IID_ISVRemoteCtrl, Status);
		return Status;
	}
	*MaxRejectDepth = 0;
	unsigned long Depth;
	Status = m_dispatcher->GetMaxRejectQeueDepth(Depth, CommandStatus);
	*MaxRejectDepth = static_cast<long>(Depth);
	return Status;
}
