//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefObjectList
//* .File Name       : $Workfile:   SVDataDefObjectList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 18:57:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>

#include "resource.h" // main symbols
#include "SVRemoteControl.h"
#include "SVCollection.h"
#pragma endregion Includes

#if defined( _WIN32_WCE ) && !defined( _CE_DCOM ) && !defined( _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA )
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace SVDataDefObjectListCollection
{
	typedef CComPtr< ISVDataDefObject > SVObjectPtr;
	typedef CAdapt< SVObjectPtr > SVObjectPtrAdapt;
	typedef std::deque< SVObjectPtrAdapt > ContainerType;
	typedef ISVDataDefObject* CollectionExposedType;

	typedef SVCollection::_CopyCollectionEntryToVariant< ISVDataDefObject > EnumeratorCopyType;
	typedef SVCollection::_CopyItFromAdapter< ISVDataDefObject > CollectionCopyType;

	typedef IDispatchImpl< ISVDataDefObjectList, &IID_ISVDataDefObjectList, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0 > CollectionInterface;

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

// SVDataDefObjectList
class ATL_NO_VTABLE SVDataDefObjectList :
	public CComObjectRootEx< CComSingleThreadModel >,
	public CComCoClass< SVDataDefObjectList, &CLSID_SVDataDefObjectList >,
	public SVDataDefObjectListCollection::CollectionType
{
public:
	SVDataDefObjectList();

	DECLARE_REGISTRY_RESOURCEID( IDR_SVDATADEFOBJECTLIST )

	BEGIN_COM_MAP( SVDataDefObjectList )
		COM_INTERFACE_ENTRY( ISVDataDefObjectList )
		COM_INTERFACE_ENTRY( IDispatch )
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHOD( get_Item )( VARIANT index, VARIANT* pVal );
	STDMETHOD( Remove )( LONG index );
	STDMETHOD( Clear )();
	STDMETHOD( Add )( ISVDataDefObject* pItem );
};

OBJECT_ENTRY_AUTO( __uuidof( SVDataDefObjectList ), SVDataDefObjectList )
