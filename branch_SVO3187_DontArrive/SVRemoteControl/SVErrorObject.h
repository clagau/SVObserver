//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVErrorObject
//* .File Name       : $Workfile:   SVErrorObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:36:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"       // main symbols
#include "SVRemoteControl.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE SVErrorObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVErrorObject, &CLSID_SVErrorObject>,
	public IDispatchImpl<ISVErrorObject, &IID_ISVErrorObject, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	SVErrorObject();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVERROROBJECT)

	BEGIN_COM_MAP(SVErrorObject)
		COM_INTERFACE_ENTRY(ISVErrorObject)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_Status)(LONG* pVal);
	STDMETHOD(put_Status)(LONG newVal);

private:
	CComBSTR m_Name;
	LONG m_Status{0L};
};

OBJECT_ENTRY_AUTO(__uuidof(SVErrorObject), SVErrorObject)
