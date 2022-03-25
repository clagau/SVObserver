//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductItems
//* .File Name       : $Workfile:   SVProductItems.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:10:24  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "resource.h"       // main symbols

#include "SVRemoteControl.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// SVProductItems

class ATL_NO_VTABLE SVProductItems :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVProductItems, &CLSID_SVProductItems>,
	public IDispatchImpl<ISVProductItems, &IID_ISVProductItems, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	SVProductItems()
	{
	}

	DECLARE_NOT_AGGREGATABLE(SVProductItems)

DECLARE_REGISTRY_RESOURCEID(IDR_SVPRODUCTITEMS)

BEGIN_COM_MAP(SVProductItems)
	COM_INTERFACE_ENTRY(ISVProductItems)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

public:
	STDMETHOD(get_Values)(ISVValueObjectList ** pVal);
	STDMETHOD(put_Values)(ISVValueObjectList * newVal);

	STDMETHOD(get_Images)(ISVImageObjectList ** pVal);
	STDMETHOD(put_Images)(ISVImageObjectList * newVal);

	STDMETHOD(get_Errors)(ISVErrorObjectList ** pVal);
	STDMETHOD(put_Errors)(ISVErrorObjectList * newVal);

	HRESULT AddValue(ISVValueObject * pValue);
	HRESULT AddImage(ISVImageObject * pValue);
	HRESULT AddError(ISVErrorObject * pValue);

private:
	CComPtr<ISVValueObjectList> m_values;
	CComPtr<ISVImageObjectList> m_images;
	CComPtr<ISVErrorObjectList> m_errors;

};

OBJECT_ENTRY_AUTO(__uuidof(SVProductItems), SVProductItems)
