//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefObject
//* .File Name       : $Workfile:   SVDataDefObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 18:57:56  $
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

namespace SVDataDefObjectCollection
{
	typedef std::deque< CComVariant > ContainerType;
	typedef VARIANT CollectionExposedType;

	typedef _Copy< VARIANT > EnumeratorCopyType;
	typedef _Copy< VARIANT > CollectionCopyType;

	typedef IDispatchImpl< ISVDataDefObject, &IID_ISVDataDefObject, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0 > CollectionInterface;

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

class ATL_NO_VTABLE SVDataDefObject :
	public CComObjectRootEx< CComSingleThreadModel >,
	public CComCoClass< SVDataDefObject, &CLSID_SVDataDefObject >,
	public SVDataDefObjectCollection::CollectionType
{
public:
	SVDataDefObject();

	DECLARE_REGISTRY_RESOURCEID( IDR_SVDATADEFOBJECT )

	BEGIN_COM_MAP( SVDataDefObject )
		COM_INTERFACE_ENTRY( ISVDataDefObject )
		COM_INTERFACE_ENTRY( IDispatch )
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

	STDMETHOD( get_Name )( BSTR* pVal );
	STDMETHOD( put_Name )( BSTR newVal );
	STDMETHOD( get_Writable )( VARIANT_BOOL* pVal );
	STDMETHOD( put_Writable )( VARIANT_BOOL newVal );
	STDMETHOD( get_Published )( VARIANT_BOOL* pVal );
	STDMETHOD( put_Published )( VARIANT_BOOL newVal );
	STDMETHOD( get_DataType )( BSTR* pVal );
	STDMETHOD( put_DataType )( BSTR newVal );

	STDMETHOD( get_Item )( VARIANT index, VARIANT* pVal );
	STDMETHOD( Remove )( LONG index );
	STDMETHOD( Clear )();
	STDMETHOD( Add )( VARIANT Item );

private:
	CComBSTR m_Name;
	VARIANT_BOOL m_Writable;
	VARIANT_BOOL m_Published;
	CComBSTR m_DataType;
};

OBJECT_ENTRY_AUTO( __uuidof( SVDataDefObject ), SVDataDefObject )
