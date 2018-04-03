//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFailStatusStream
//* .File Name       : $Workfile:   SVFailStatusStream.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Jan 2015 10:17:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"       // main symbols
#include "SVCriticalSection.h"
#include "SVJsonCommandHelper.h"
#include "SVRCClientSocket.h"
#include "SVRemoteControl.h"
#include "_ISVFailStatusStreamEvents_CP.h"
#include "JsonLib/include/json.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE SVFailStatusStream :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVFailStatusStream, &CLSID_SVFailStatusStream>,
	public IConnectionPointContainerImpl<SVFailStatusStream>,
	public CProxy_ISVFailStatusStreamEvents<SVFailStatusStream>,
	public IDispatchImpl<ISVFailStatusStream, &IID_ISVFailStatusStream, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IProvideClassInfo2Impl<&CLSID_SVFailStatusStream, &__uuidof(_ISVFailStatusStreamEvents), &LIBID_SVRemoteControlLib>
{
public:
	SVFailStatusStream();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVFAILSTATUSSTREAM)

	BEGIN_COM_MAP(SVFailStatusStream)
		COM_INTERFACE_ENTRY(ISVFailStatusStream)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(SVFailStatusStream)
		CONNECTION_POINT_ENTRY(__uuidof(_ISVFailStatusStreamEvents))
	END_CONNECTION_POINT_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

	STDMETHOD(get_RemoteCtrl)(ISVRemoteCtrl** pVal)
	{
		return E_NOTIMPL;
	};
	STDMETHOD(put_RemoteCtrl)(ISVRemoteCtrl* newVal)
	{
		return E_NOTIMPL;
	};
	STDMETHOD(get_TimeoutInSeconds)(long* pVal);
	STDMETHOD(put_TimeoutInSeconds)(long Val);
	STDMETHOD(Start)(BSTR ListName)
	{
		return E_NOTIMPL;
	};
	STDMETHOD(Stop)(BSTR ListName)
	{
		return E_NOTIMPL;
	};

protected:
	HRESULT AssignRemoteControl(ISVRemoteCtrl* newVal);
	void ClearRemoteControl();

	HRESULT CheckSocketConnection();
	HRESULT WriteToSocket(const std::string& data);

	void OnConnectionStatus(SvSol::ConnectionState state);
	void OnControlDataReceived(std::istream& is);

	HRESULT SendFailStatusStreamData(const Json::Value& rJsonValue);
	HRESULT BuildDataStream(ISVProductItems* pProductItems, const Json::Value& rJsonValue);

	CComPtr<ISVRemoteCtrl> m_RemoteCtrl;

	_bstr_t m_ServerAddress;
	unsigned short m_ServerPort;
	SVRCClientSocket m_ClientSocket;

	long m_TimeoutInSeconds;
	SVCriticalSection m_CriticalSection;
	SVJsonCommandHelper m_Command;
};

OBJECT_ENTRY_AUTO(__uuidof(SVFailStatusStream), SVFailStatusStream)
