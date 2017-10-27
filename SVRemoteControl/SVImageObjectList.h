//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// COM Collection object for Images
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

using namespace ATL;

namespace SVImageObjectListCollection
{
	typedef CComPtr<ISVImageObject> SVObjectPtr;
	typedef CAdapt<SVObjectPtr> SVObjectPtrAdapt;
	typedef std::deque<SVObjectPtrAdapt> ContainerType;
	typedef ISVImageObject* CollectionExposedType;

	typedef SVCollection::_CopyCollectionEntryToVariant<ISVImageObject> EnumeratorCopyType;
	typedef SVCollection::_CopyItFromAdapter<ISVImageObject> CollectionCopyType;

	typedef IDispatchImpl<ISVImageObjectList, &IID_ISVImageObjectList, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0> CollectionInterface;

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

// SVImageObjectList

class ATL_NO_VTABLE SVImageObjectList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVImageObjectList, &CLSID_SVImageObjectList>,
	public SVImageObjectListCollection::CollectionType
{
public:
	SVImageObjectList()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SVIMAGEOBJECTLIST)


BEGIN_COM_MAP(SVImageObjectList)
	COM_INTERFACE_ENTRY(ISVImageObjectList)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		m_coll.clear();
	}

public:

	STDMETHOD(get_Item)(VARIANT index, VARIANT* pVal);
	STDMETHOD(Clear)(void);
	STDMETHOD(Remove)(LONG index);
	STDMETHOD(Add)(ISVImageObject* object);
};

OBJECT_ENTRY_AUTO(__uuidof(SVImageObjectList), SVImageObjectList)
