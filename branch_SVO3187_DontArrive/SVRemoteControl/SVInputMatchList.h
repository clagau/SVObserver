//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputMatchList
//* .File Name       : $Workfile:   SVInputMatchList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:39:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>
#include "resource.h"
#include "SVCollection.h"
#include "SVInputMatchItem.h"
#pragma endregion Includes

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace SVInputMatchItemCollection
{
	typedef CComPtr<ISVInputMatchItem> SVInputMatchItemPtr;
	typedef CAdapt<SVInputMatchItemPtr> SVInputMatchItemPtrAdapt;
	typedef std::deque<SVInputMatchItemPtrAdapt> ContainerType;
	typedef ISVInputMatchItem* CollectionExposedType;

	typedef SVCollection::_CopyCollectionEntryToVariant<ISVInputMatchItem> EnumeratorCopyType;
	typedef SVCollection::_CopyItFromAdapter<ISVInputMatchItem> CollectionCopyType;

	typedef IDispatchImpl<ISVInputMatchList, &IID_ISVInputMatchList, &LIBID_SVRemoteControlLib> CollectionInterface;

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

class ATL_NO_VTABLE SVInputMatchList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVInputMatchList, &CLSID_SVInputMatchList>,
	public SVInputMatchItemCollection::CollectionType
{
public:
	SVInputMatchList();

	DECLARE_REGISTRY_RESOURCEID(IDR_SVINPUTMATCHLIST)

	BEGIN_COM_MAP(SVInputMatchList)
		COM_INTERFACE_ENTRY(ISVInputMatchList)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHOD(get_Item)(VARIANT index, VARIANT* pVal);
	STDMETHOD(Remove)(LONG index);
	STDMETHOD(Clear)(void);
	STDMETHOD(Add)(ISVInputMatchItem* pItem);
	STDMETHOD(AddItem)(BSTR matchString, long txn_Identifier);
};

OBJECT_ENTRY_AUTO(__uuidof(SVInputMatchList), SVInputMatchList)
