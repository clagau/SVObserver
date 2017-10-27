//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputBroker
//* .File Name       : $Workfile:   SVInputBroker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Oct 2014 10:11:44  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "resource.h"       // main symbols
#include "SVInputMatchList.h"
#include "SVJsonCommandHelper.h"
#include "SVRCClientSocket.h"
#include "SVRemoteControl.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE SVInputBroker :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVInputBroker, &CLSID_SVInputBroker>,
	public IDispatchImpl<ISVInputBroker, &IID_ISVInputBroker, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	SVInputBroker();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVINPUTBROKER)

	BEGIN_COM_MAP(SVInputBroker)
		COM_INTERFACE_ENTRY(ISVInputBroker)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHOD(get_RemoteCtrl)(ISVRemoteCtrl** pVal);
	STDMETHOD(put_RemoteCtrl)(ISVRemoteCtrl* newVal);
	STDMETHOD(get_TimeoutInSeconds)(long* pVal);
	STDMETHOD(put_TimeoutInSeconds)(long Val);
	STDMETHOD(QueryListNames)(SAFEARRAY** ppNames);
	STDMETHOD(QueryListItems)(BSTR ListName, ISVInputMatchList** ppItems);
	STDMETHOD(Clear)(BSTR ListName);
	STDMETHOD(Add)(BSTR ListName, ISVInputMatchList* pItems);

protected:
	HRESULT AssignRemoteControl( ISVRemoteCtrl* newVal );
	void ClearRemoteControl();

	HRESULT CheckSocketConnection();
	HRESULT WriteToSocket( const std::string& data );

	void OnConnectionStatus(SvSol::ConnectionState state );
	void OnControlDataReceived( std::istream& is );
	CComPtr<ISVRemoteCtrl> m_RemoteCtrl;

	_bstr_t m_ServerAddress;
	unsigned short m_ServerPort;
	SVRCClientSocket m_ClientSocket;

	long m_TimeoutInSeconds;
	SVJsonCommandHelper m_Command;
};

OBJECT_ENTRY_AUTO(__uuidof(SVInputBroker), SVInputBroker)
