//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputMatchItem
//* .File Name       : $Workfile:   SVInputMatchItem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:38:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"

#include "SVRemoteControl.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE SVInputMatchItem :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVInputMatchItem, &CLSID_SVInputMatchItem>,
	public IDispatchImpl<ISVInputMatchItem, &IID_ISVInputMatchItem, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	SVInputMatchItem();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVINPUTMATCHITEM)

	BEGIN_COM_MAP(SVInputMatchItem)
		COM_INTERFACE_ENTRY(ISVInputMatchItem)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

	STDMETHOD(get_MatchString)(BSTR* pVal);
	STDMETHOD(put_MatchString)(BSTR Val);
	STDMETHOD(get_TxnIdentifier)(LONG* pVal);
	STDMETHOD(put_TxnIdentifier)(LONG Val);

protected:
	CComBSTR m_MatchString;
	long m_TxnIdentifier;
};

OBJECT_ENTRY_AUTO(__uuidof(SVInputMatchItem), SVInputMatchItem)
