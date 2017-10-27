//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:40:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>

#include "resource.h"       // main symbols
#include "SVRemoteControl.h"
#include "SVCollection.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace SVValueObjectCollection
{
	typedef std::deque< CComVariant > ContainerType;
	typedef VARIANT CollectionExposedType;

	typedef _Copy<VARIANT> EnumeratorCopyType;
	typedef _Copy<VARIANT> CollectionCopyType;

	typedef IDispatchImpl<ISVValueObject, &IID_ISVValueObject, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0> CollectionInterface;

	typedef CComEnumOnSTL< SVCollection::EnumeratorInterface, 
							&__uuidof(SVCollection::EnumeratorInterface), 
							SVCollection::ItemType,
							EnumeratorCopyType,
							ContainerType > EnumeratorType;

	typedef ICollectionOnSTLImpl< CollectionInterface, 
								ContainerType, 
								CollectionExposedType,
								CollectionCopyType, 
								EnumeratorType > CollectionType;
}

// SVValueObject

class ATL_NO_VTABLE SVValueObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVValueObject, &CLSID_SVValueObject>,
	public SVValueObjectCollection::CollectionType
{
public:
	SVValueObject();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVVALUEOBJECT)

	BEGIN_COM_MAP(SVValueObject)
		COM_INTERFACE_ENTRY(ISVValueObject)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_TriggerCount)(LONG* pVal);
	STDMETHOD(put_TriggerCount)(LONG newVal);
	STDMETHOD(get_Status)(LONG* pVal);
	STDMETHOD(put_Status)(LONG newVal);

	STDMETHOD(get_Item)(VARIANT index, VARIANT* pVal);
	STDMETHOD(Remove)(LONG index);
	STDMETHOD(Clear)(void);
	STDMETHOD(Add)(VARIANT Item);

private:
	_bstr_t m_Name;
	LONG m_TriggerCount;
	LONG m_Status;
};

OBJECT_ENTRY_AUTO(__uuidof(SVValueObject), SVValueObject)
