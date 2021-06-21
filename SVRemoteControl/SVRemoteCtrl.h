//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrl
//* .File Name       : $Workfile:   SVRemoteCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.13  $
//* .Check In Date   : $Date:   16 Jan 2015 10:31:38  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <atlctl.h>
#include <string>
#include <map>
#include <boost/function.hpp>

#include "resource.h"       // main symbols
#include "SVRemoteControl.h"
#include "_ISVRemoteCtrlEvents_CP.h"
#include "SVAsyncCommandHelper.h"
#include "SVEventThread.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace svr
{
	enum dev { unknown, svobserver, svcontrol, any };
}

// SVRemoteCtrl
class ATL_NO_VTABLE SVRemoteCtrl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<SVRemoteCtrl, ISVRemoteCtrl>,
	public IPersistStreamInitImpl<SVRemoteCtrl>,
	public IOleControlImpl<SVRemoteCtrl>,
	public IOleObjectImpl<SVRemoteCtrl>,
	public IOleInPlaceActiveObjectImpl<SVRemoteCtrl>,
	public IViewObjectExImpl<SVRemoteCtrl>,
	public IOleInPlaceObjectWindowlessImpl<SVRemoteCtrl>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<SVRemoteCtrl>,
	public CProxy_ISVRemoteCtrlEvents<SVRemoteCtrl>,
	public IPersistStorageImpl<SVRemoteCtrl>,
	public ISpecifyPropertyPagesImpl<SVRemoteCtrl>,
	public IQuickActivateImpl<SVRemoteCtrl>,
	public IPropertyNotifySinkCP<SVRemoteCtrl>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<SVRemoteCtrl>,
#endif
	public IProvideClassInfo2Impl<&CLSID_SVRemoteCtrl, &__uuidof(_ISVRemoteCtrlEvents), &LIBID_SVRemoteControlLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<SVRemoteCtrl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<SVRemoteCtrl, &CLSID_SVRemoteCtrl>,
	public CComControl<SVRemoteCtrl>
{
public:
	SVRemoteCtrl();

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
		OLEMISC_INVISIBLEATRUNTIME |
		OLEMISC_CANTLINKINSIDE |
		OLEMISC_INSIDEOUT |
		OLEMISC_ACTIVATEWHENVISIBLE |
		OLEMISC_SETCLIENTSITEFIRST
	)

	DECLARE_REGISTRY_RESOURCEID(IDR_SVREMOTECTRL)

	BEGIN_COM_MAP(SVRemoteCtrl)
		COM_INTERFACE_ENTRY(ISVRemoteCtrl)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY(IViewObject2)
		COM_INTERFACE_ENTRY(IViewObject)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceObject)
		COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
		COM_INTERFACE_ENTRY(IPersistStreamInit)
		COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
		COM_INTERFACE_ENTRY(IQuickActivate)
		COM_INTERFACE_ENTRY(IPersistStorage)
	#ifndef _WIN32_WCE
		COM_INTERFACE_ENTRY(IDataObject)
	#endif
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
	#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
		COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
	#endif
	END_COM_MAP()

	BEGIN_PROP_MAP(SVRemoteCtrl)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_CONNECTION_POINT_MAP(SVRemoteCtrl)
		CONNECTION_POINT_ENTRY(__uuidof(_ISVRemoteCtrlEvents))
		CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	END_CONNECTION_POINT_MAP()

	BEGIN_MSG_MAP(SVRemoteCtrl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		CHAIN_MSG_MAP(CComControl<SVRemoteCtrl>)
	ALT_MSG_MAP(1)
		// Replace this with message map entries for superclassed Static
	END_MSG_MAP()

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IPropertyNotifySink
	STDMETHODIMP OnRequestEdit(DISPID);
	STDMETHODIMP OnChanged(DISPID);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// ISVRemoteCtrl
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHOD(BeginGetConfig)(BSTR fileName);
	STDMETHOD(FinishGetConfig)(LONG Timeout, VARIANT_BOOL CancelAfterTimeout);
	STDMETHOD(BeginPutConfig)(BSTR fileName);
	STDMETHOD(FinishPutConfig)(LONG Timeout, VARIANT_BOOL CancelAfterTimeout);
	STDMETHOD(BeginPutDeviceFile)(BSTR sourcePath, BSTR destinationPath, VARIANT_BOOL saveInConfig);
	STDMETHOD(FinishPutDeviceFile)(LONG Timeout, VARIANT_BOOL CancelAfterTimeout);
	STDMETHOD(BeginSetDeviceMode)(SVObserverModesEnum desiredMode);
	STDMETHOD(FinishSetDeviceMode)(LONG Timeout, VARIANT_BOOL CancelAfterTimeout);
	STDMETHOD(RunOnce)(BSTR InspectionName);
	STDMETHOD(LoadConfig)(BSTR filePath);
	STDMETHOD(GetConfig)(BSTR filePath);
	STDMETHOD(GetDeviceConfigReport)(BSTR* report);
	STDMETHOD(GetDeviceMode)(LONG* mode);
	STDMETHOD(GetDeviceOfflineCount)(LONG* count);
	STDMETHOD(GetItems)(VARIANT itemNames, ISVProductItems** items);
	STDMETHOD(GetState)(LONG* state);
	STDMETHOD(PutConfig)(BSTR filePath, LONG Timeout);
	STDMETHOD(GetVersion)(BSTR* svobserver_ver, BSTR* svremotecontrol_ver, BSTR* runrejectserver_ver);
	STDMETHOD(GetDeviceFile)(BSTR sourcePath, BSTR destinationPath);
	STDMETHOD(PutDeviceFile)(BSTR sourcePath, BSTR destinationPath);
	STDMETHOD(PutDeviceFileSave)(BSTR sourcePath, BSTR destinationPath, VARIANT_BOOL saveInConfig);
	STDMETHOD(SetDeviceMode)(SVObserverModesEnum desiredMode);
	STDMETHOD(SetItems)(ISVProductItems* items, ISVProductItems ** faults);
	STDMETHOD(SetItemsRunOnce)(ISVProductItems* items, VARIANT_BOOL RunOnce, ISVProductItems ** faults);
	STDMETHOD(get_ServerAddress)(BSTR* pVal);
	STDMETHOD(get_VPAddress)(BSTR* pVal);
	STDMETHOD(get_ServerType)(BSTR* pVal);
	STDMETHOD(get_Connected)(VARIANT_BOOL* pVal);
	STDMETHOD(Disconnect)(void);
	STDMETHOD(Connect)(BSTR address, LONG Timeout);
	STDMETHOD(get_ImageScale)(LONG* pVal);
	STDMETHOD(put_ImageScale)(LONG newVal);
	STDMETHOD(RegisterMonitorList)(BSTR listName, BSTR ppqName, int rejectDepth, VARIANT productItemList, VARIANT rejectCondList, VARIANT failStatusList, ISVErrorObjectList ** errors);
	STDMETHOD(QueryProductList)(BSTR listName, VARIANT* itemNames);
	STDMETHOD(QueryRejectCondList)(BSTR listName, VARIANT* itemNames);
	STDMETHOD(QueryFailStatusList)(BSTR listName, VARIANT* itemNames);
	STDMETHOD(GetProduct)(BSTR listName, LONG triggerCount, LONG imageScale, ISVProductItems** currentViewItems);
	STDMETHOD(GetReject)(BSTR listName, LONG triggerCount, LONG imageScale, ISVProductItems** currentViewItems);
	STDMETHOD(GetDataDefinitionList)(BSTR inspectionName, SVDataDefinitionListTypeEnum listType, ISVDataDefObjectList** entries);
	STDMETHOD(DeactivateMonitorList)(BSTR listName);					
	STDMETHOD(ActivateMonitorList)(BSTR listName);					
	STDMETHOD(GetFailStatus)(BSTR listName, VARIANT * values);	
	STDMETHOD(SetProductFilter)(BSTR listName, SVProductFilterModesEnum filter);
	STDMETHOD(GetProductFilter)(BSTR listName, LONG * pFilter);
	STDMETHOD(Shutdown)(SVShutdownOptionsEnum options);
	STDMETHOD(QueryMonitorListNames)(VARIANT* listNames);
	STDMETHOD(GetInspectionNames)(VARIANT* listNames);
	STDMETHOD(GetMonitorListProperties)(BSTR listName,LONG* RejectDepth, VARIANT_BOOL*  isActive, BSTR*  ppqName);
	STDMETHODIMP GetMaxRejectQeueDepth( LONG* MaxRejectDepth);
	

protected:
	typedef std::function<void ( bool& )> SVAsyncThreadFunc;

	CContainedWindow m_ctlStatic;
	DWORD m_cookie;
	svr::dev m_device;
	_bstr_t m_servername;
	_bstr_t m_VPName;
	SVEventThread<SVAsyncThreadFunc> m_AsyncThread;
	SVAsyncCommandHelper m_AsyncCommandHelper;

	void AsyncThreadFunc( bool& p_rWaitForEvent );

	HRESULT SetControlServer( SVCommandStatus& p_rStatus );

private:
	void NotifyClient( _variant_t& rData, SVNotificationTypesEnum type );

	CComBSTR c_success;
	CComBSTR c_svcontrol;
	CComBSTR c_svobserver;
	LONG	 m_imageScale;
	 
	CommandDispatcher m_dispatcher;
};

OBJECT_ENTRY_AUTO(__uuidof(SVRemoteCtrl), SVRemoteCtrl)
