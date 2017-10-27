//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectList
//* .File Name       : $Workfile:   SVValueObjectList.h  $
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

namespace SVValueObjectListCollection
{
	typedef CComPtr<ISVValueObject> SVObjectPtr;
	typedef CAdapt<SVObjectPtr> SVObjectPtrAdapt;
	typedef std::deque<SVObjectPtrAdapt> ContainerType;
	typedef ISVValueObject* CollectionExposedType;

	typedef SVCollection::_CopyCollectionEntryToVariant<ISVValueObject> EnumeratorCopyType;
	typedef SVCollection::_CopyItFromAdapter<ISVValueObject> CollectionCopyType;

	typedef IDispatchImpl<ISVValueObjectList, &IID_ISVValueObjectList, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0> CollectionInterface;

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

// SVValueObjectList

class ATL_NO_VTABLE SVValueObjectList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVValueObjectList, &CLSID_SVValueObjectList>,
	public SVValueObjectListCollection::CollectionType
{
public:
	SVValueObjectList();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVVALUEOBJECTLIST)

	BEGIN_COM_MAP(SVValueObjectList)
		COM_INTERFACE_ENTRY(ISVValueObjectList)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHOD(get_Item)(VARIANT index, VARIANT* pVal);
	STDMETHOD(Remove)(LONG index);
	STDMETHOD(Clear)(void);
	STDMETHOD(Add)(ISVValueObject* pItem);
};

OBJECT_ENTRY_AUTO(__uuidof(SVValueObjectList), SVValueObjectList)
