//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputBroker
//* .File Name       : $Workfile:   SVOutputBroker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Oct 2014 10:11:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"       // main symbols
#include "SVCriticalSection.h"
#include "SVJsonCommandHelper.h"
#include "SVRCClientSocket.h"
#include "SVRemoteControl.h"
#include "_ISVOutputBrokerEvents_CP.h"
#include "JsonLib/include/json.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE SVOutputBroker :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVOutputBroker, &CLSID_SVOutputBroker>,
	public IConnectionPointContainerImpl<SVOutputBroker>,
	public CProxy_ISVOutputBrokerEvents<SVOutputBroker>,
	public IDispatchImpl<ISVOutputBroker, &IID_ISVOutputBroker, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IProvideClassInfo2Impl<&CLSID_SVOutputBroker, &__uuidof(_ISVOutputBrokerEvents), &LIBID_SVRemoteControlLib>
{
public:
	SVOutputBroker();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVOUTPUTBROKER)

	BEGIN_COM_MAP(SVOutputBroker)
		COM_INTERFACE_ENTRY(ISVOutputBroker)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(SVOutputBroker)
		CONNECTION_POINT_ENTRY(__uuidof(_ISVOutputBrokerEvents))
	END_CONNECTION_POINT_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

	STDMETHOD(get_RemoteCtrl)(ISVRemoteCtrl** pVal);
	STDMETHOD(put_RemoteCtrl)(ISVRemoteCtrl* newVal);
	STDMETHOD(get_TimeoutInSeconds)(long* pVal);
	STDMETHOD(put_TimeoutInSeconds)(long Val);
	STDMETHOD(Start)(BSTR ListName);
	STDMETHOD(Stop)(BSTR ListName);
	STDMETHOD(QueryListNames)(SAFEARRAY** ppNames);
	STDMETHOD(QueryListItems)(BSTR ListName, SAFEARRAY** ppNames);

protected:
	HRESULT AssignRemoteControl( ISVRemoteCtrl* newVal );
	void ClearRemoteControl();

	HRESULT CheckSocketConnection();
	HRESULT WriteToSocket( const std::string& data );

	void OnConnectionStatus(SvSol::ConnectionState state );
	void OnControlDataReceived( std::istream& is );

	HRESULT SendOutputStreamData(const Json::Value& rJsonValue);
	HRESULT BuildDataStream(ISVProductItems* pProductItemsPtr, const Json::Value& rJsonValue);

	CComPtr<ISVRemoteCtrl> m_RemoteCtrl;

	_bstr_t m_ServerAddress;
	unsigned short m_ServerPort;
	SVRCClientSocket m_ClientSocket;

	long m_TimeoutInSeconds;
	SVCriticalSection m_CriticalSection;
	SVJsonCommandHelper m_Command;

};

OBJECT_ENTRY_AUTO(__uuidof(SVOutputBroker), SVOutputBroker)
