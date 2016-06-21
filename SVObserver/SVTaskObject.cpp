//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObject
//* .File Name       : $Workfile:   SVTaskObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Jul 2014 20:39:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTaskObject.h"

#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/SVTemplate.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVAnalyzer.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "RootObject.h"
#include "ObjectSelectorLibrary/SelectorItemVector.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVTaskObjectClass, SVTaskObjectClassGuid)
#pragma endregion Declarations

SVTaskObjectClass::SVTaskObjectClass(LPCSTR ObjectName)
	:SVObjectAppClass(ObjectName) 
{
	LocalInitialize();
}

SVTaskObjectClass::SVTaskObjectClass(SVObjectClass* POwner, int StringResourceID)
	:SVObjectAppClass(POwner, StringResourceID) 
{
	LocalInitialize();
}

HRESULT SVTaskObjectClass::LocalInitialize()
{
	HRESULT l_hrOk = S_OK;

	m_taskObjectValueInterface.SetTaskObject(this);

	m_bUseOverlays = true;	// most objects use overlays; must change if needed in derived classes

	// SetObjectDepth has already been called in SVObjectClass Ctor
	// Register Embedded Objects
	RegisterEmbeddedObject(&isObjectValid, SVTaskObjectClassIsObjectValidGuid, IDS_OBJECTNAME_ISVALID, false, SVResetItemNone );
	RegisterEmbeddedObject(&statusTag, SVStatusObjectGuid, IDS_OBJECTNAME_STATUS, false, SVResetItemNone );
	RegisterEmbeddedObject(&statusColor, SVColorObjectGuid, IDS_OBJECTNAME_COLOR, false, SVResetItemNone );
	
	// Set Embedded defaults
	statusColor.SetDefaultValue((const DWORD&) SV_DEFAULT_INACTIVE_COLOR, TRUE);
	statusTag.SetDefaultValue(FALSE, TRUE);
	isObjectValid.SetDefaultValue(FALSE, TRUE);
	
	// Add out Default Inputs and Outputs
	addDefaultInputObjects();

	m_lLastToolInputListIndex = -1;

	return l_hrOk;
}

SVTaskObjectClass::~SVTaskObjectClass()
{
	DestroyFriends();
	
	// empty the Embedded List
	for (int i = embeddedList.GetSize() - 1; i >= 0; i--)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject && pObject->IsCreated())
		{
			pObject->CloseObject();
		}
		embeddedList.RemoveAt(i);
	}
	CloseObject();
}

HRESULT SVTaskObjectClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT SVTaskObjectClass::GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const
{
	HRESULT l_Status( S_OK );

	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[ i ];

		// Check if Friend is alive...
		SVTaskObjectClass* l_pObject( nullptr );

		l_pObject = dynamic_cast< SVTaskObjectClass* >( SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID ) );

		if( nullptr != l_pObject )
		{
			HRESULT l_Temp = l_pObject->GetOutputList( p_rOutputInfoList );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}
		}
	}

	for( long i = 0; i < embeddedList.GetSize(); i++ )
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if( nullptr != pObject )
		{
			p_rOutputInfoList.Add( &(pObject->GetObjectOutputInfo()) );
		}
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::GetOutputListFiltered(std::vector<SVValueObjectReference>& rvecObjects, UINT uiAttributes, bool bAND )
{
	rvecObjects.clear();
	SVOutputInfoListClass l_OutputList;

	GetOutputList( l_OutputList );

	int iSize = l_OutputList.GetSize();
	for ( int i = 0; i < iSize; ++i )
	{
		SVOutObjectInfoStruct* pInfoItem = l_OutputList.GetAt(i);
		if ( pInfoItem )
		{
			SVValueObjectReference ref = pInfoItem->GetObjectReference();
			if ( ref.Object() )
			{
				if ( !ref->IsArray() )
				{
					bool bAttributesOK = bAND ? (ref.ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
					                          : (ref.ObjectAttributesSet() & uiAttributes) > 0;            // OR
					if ( bAttributesOK )
					{
						rvecObjects.push_back( ref );
					}
				}
				else
				{
					int iArraySize = ref->GetArraySize();
					for ( int i=0; i < iArraySize; i++ )
					{
						ref.SetArrayIndex(i);
						bool bAttributesOK = bAND ? (ref.ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
						                          : (ref.ObjectAttributesSet() & uiAttributes) > 0;            // OR
						if ( bAttributesOK )
						{
							rvecObjects.push_back( ref );
						}
					}

				}
			}
		}
	}
	return S_OK;
}

HRESULT SVTaskObjectClass::GetNonToolsetOutputList( SVOutputInfoListClass& rOutputInfoList ) const
{
	HRESULT Result( S_OK );

	BasicValueObjects::ValueVector list;

	RootObject::getRootChildObjectList( list, _T(""), 0 );
	for(BasicValueObjects::ValueVector::const_iterator iter = list.begin(); iter != list.end(); ++iter) 
	{
		rOutputInfoList.Add( &((*iter)->GetObjectOutputInfo()) );
	}

	return Result;
}

HRESULT SVTaskObjectClass::IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::FindNextInputImageInfo( SVInObjectInfoStruct*& p_rpsvFoundInfo, const SVInObjectInfoStruct* p_psvLastInfo )
{
	HRESULT l_svOk = S_FALSE;

	long l_lCount = m_svToolInputList.GetSize();

	bool l_bFoundLast = nullptr != p_psvLastInfo  && 0 < l_lCount &&
		0 <= m_lLastToolInputListIndex && m_lLastToolInputListIndex < l_lCount &&
		p_psvLastInfo != m_svToolInputList[ m_lLastToolInputListIndex ];

	if( ! l_bFoundLast )
	{
		l_bFoundLast = (nullptr == p_psvLastInfo);

		m_lLastToolInputListIndex = -1;

		m_svToolInputList.RemoveAll();

		// Try to get input interface...
		::SVSendMessage( this, SVM_GET_INPUT_INTERFACE | SVM_NOTIFY_FRIENDS, reinterpret_cast<DWORD_PTR>(&m_svToolInputList), 0 );

		l_lCount = m_svToolInputList.GetSize();
	}

	p_rpsvFoundInfo = nullptr;

	for( int i = m_lLastToolInputListIndex + 1; nullptr == p_rpsvFoundInfo && i < l_lCount; ++ i )
	{
		SVInObjectInfoStruct* l_psvInputInfo = m_svToolInputList[ i ];

		if( l_bFoundLast )
		{
			if( nullptr != l_psvInputInfo && 
				SVImageObjectType == l_psvInputInfo->GetInputObjectInfo().ObjectTypeInfo.ObjectType  )
			{
				if( S_FALSE == IsAuxInputImage( l_psvInputInfo ) )
				{
					p_rpsvFoundInfo = l_psvInputInfo;

					m_lLastToolInputListIndex = i;

					l_svOk = S_OK;
				}
			}
		}
		else
		{
			l_bFoundLast = p_psvLastInfo == l_psvInputInfo;
		}
	}

	if( S_OK != l_svOk )
	{
		m_lLastToolInputListIndex = -1;

		m_svToolInputList.RemoveAll();
	}

	return l_svOk;
}

SvStl::MessageContainer SVTaskObjectClass::getFirstTaskMessage() const
{
	if( 0 < m_TaskMessages.size() )
	{
		return m_TaskMessages[0];
	}

	return SvStl::MessageContainer();
}


HRESULT SVTaskObjectClass::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	HRESULT l_Status = SVObjectAppClass::GetChildObject( rpObject, rNameInfo, Index );

	if( S_OK != l_Status )
	{
		if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[ Index ] == GetName() )
		{
			// Notify friends...
			for (size_t i = 0; nullptr == rpObject && i < m_friendList.size(); ++ i)
			{
				const SVObjectInfoStruct& rfriend = m_friendList[i];
				if( nullptr != rfriend.PObject)
				{
					l_Status = rfriend.PObject->GetChildObject( rpObject, rNameInfo, Index + 1 );
				}
			}

			if( ( S_OK != l_Status ) && ( ( Index + 1 ) == ( rNameInfo.m_NameArray.size() - 1 ) ) )
			{
				for( long j = 0; nullptr == rpObject && j < embeddedList.GetSize(); j++ )
				{
					SVObjectClass* l_pObject = embeddedList.GetAt(j);

					if( nullptr != l_pObject )
					{
						l_Status = l_pObject->GetChildObject( rpObject, rNameInfo, Index + 1 );
					}
				}
			}
		}
		else
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}
	
#pragma region virtual method (ITaskObject)
HRESULT SVTaskObjectClass::AddInputRequestMarker()
{
	return m_taskObjectValueInterface.AddInputRequestMarker();
}

HRESULT SVTaskObjectClass::RunOnce(IObjectClass* pTool)
{
	SVToolClass* pToolClass = nullptr;
	if (nullptr != pTool)
	{
		pToolClass = dynamic_cast<SVToolClass*>(pTool);
		ASSERT(pToolClass);
	}
	return m_taskObjectValueInterface.RunOnce(pToolClass);
}

SvOi::ISelectorItemVectorPtr SVTaskObjectClass::GetSelectorList(SvOi::IsObjectInfoAllowed isAllowed, UINT Attribute, bool WholeArray) const
{
	SvOsl::SelectorItemVector* pSelectorList = new SvOsl::SelectorItemVector();
	SvOi::ISelectorItemVectorPtr Result = static_cast<SvOi::ISelectorItemVector*> (pSelectorList);

	if (isAllowed)
	{
		SVOutputInfoListClass OutputList;
		GetOutputList( OutputList );

		// Filter the list
		std::for_each(OutputList.begin(), OutputList.end(), [&pSelectorList, &isAllowed, &Attribute, &WholeArray](SVOutputInfoListClass::value_type info)->void
		{
			SVObjectReference ObjectRef = info->GetObjectReference();
			if( ObjectRef->IsArray() || isAllowed(info->PObject, Attribute, -1) )
			{
				SvOsl::SelectorItem InsertItem;
				
				InsertItem.setName( ObjectRef.GetName() );
				InsertItem.setItemKey( ObjectRef->GetUniqueObjectID().ToVARIANT() );
				if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (ObjectRef.Object()) )
				{
					CString typeName = _T("");
					pValueObject->GetTypeName( typeName );
					InsertItem.setItemTypeName( typeName );
				}
				else if( const BasicValueObject* pBasicObject = dynamic_cast<const BasicValueObject*> (ObjectRef.Object()) )
				{
					InsertItem.setItemTypeName( pBasicObject->getTypeName().c_str() );
				}

				if( ObjectRef->IsArray() )
				{
					if ( WholeArray && isAllowed(info->PObject, Attribute, -1) )
					{
						ObjectRef.SetEntireArray();
						UINT AttributesSet = ObjectRef.ObjectAttributesSet();
						InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName() );
						InsertItem.setArrayIndex( -1 );
						InsertItem.setArray( true );
						InsertItem.setSelected( (AttributesSet & Attribute) == Attribute );
						pSelectorList->push_back( InsertItem );
					}

					// add array elements
					int iArraySize = ObjectRef->GetArraySize();
					for ( int i = 0; i < iArraySize; i++ )
					{
						if( isAllowed(info->PObject, Attribute, i) )
						{
							ObjectRef.SetArrayIndex( i );
							UINT AttributesSet = ObjectRef.ObjectAttributesSet();
							InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName() );
							InsertItem.setArrayIndex( i );
							InsertItem.setSelected( (AttributesSet & Attribute) == Attribute );
							pSelectorList->push_back( InsertItem );
						}
					}
				}
				else
				{
					UINT AttributesSet = ObjectRef.ObjectAttributesSet();
					InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName() );
					InsertItem.setSelected( (AttributesSet & Attribute) == Attribute );
					pSelectorList->push_back( InsertItem );
				}
			}
		});
	}
	else
	{
		assert(false);
		::OutputDebugString(_T("SVTaskObjectClass::SelectorList - empty functor"));
	}
	return Result;
}

SvOi::DependencyList SVTaskObjectClass::GetDependents(bool bImagesOnly, SVObjectTypeEnum nameToObjectType) const
{
	SvOi::DependencyList dependents;
	SVObjectPairVector v;
	HRESULT hr = const_cast<SVTaskObjectClass*>(this)->GetDependentsList(v, bImagesOnly);
	if (S_OK == hr)
	{
		std::for_each(v.begin(), v.end(), [&dependents, &nameToObjectType](const SVObjectPair& item)->void
		{
			SvOi::Relation rel(item.first->GetCompleteObjectNameToObjectType(nullptr, nameToObjectType), 
								item.second->GetCompleteObjectNameToObjectType(nullptr, nameToObjectType));
			dependents.push_back(rel);
		}
		);
	}
	return dependents;
}

void SVTaskObjectClass::GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries)
{
	SVInObjectInfoStruct* psvImageInfo(nullptr);
	SVInObjectInfoStruct* psvLastImageInfo(nullptr);

	while (!psvImageInfo && S_OK == FindNextInputImageInfo(psvImageInfo, psvLastImageInfo))
	{
		if (psvImageInfo)
		{
			if (psvImageInfo->IsConnected())
			{
				SvOi::IObjectClass* pObject = psvImageInfo->GetInputObjectInfo().PObject;
				if (nullptr != pObject)
				{
					SvOi::ISVImage* pImage = dynamic_cast <SvOi::ISVImage*>(pObject);
					if (nullptr != pImage)
					{
						SVString name = pImage->getDisplayedName();
						rList.insert(std::make_pair(psvImageInfo->GetInputName(), std::make_pair(name, pObject->GetUniqueObjectID())));
					}
				}
				if (static_cast<int>(rList.size()) < maxEntries)
				{
					psvLastImageInfo = psvImageInfo;
					psvImageInfo = nullptr;
				}
			}
			else // get First Connected Image Info
			{
				psvLastImageInfo = psvImageInfo;
				psvImageInfo = nullptr;
			}
		}
	}
}
#pragma endregion virtual method (ITaskObject)

HRESULT SVTaskObjectClass::IsInputImage( SVImageClass* p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if( nullptr != p_psvImage)
	{
		SVTaskObjectClass* l_psvObject = nullptr;

		// Notify friends...
		for( size_t i = 0; S_OK != l_hrOk && i < m_friendList.size(); ++i )
		{
			const SVObjectInfoStruct &l_rsvFriend = m_friendList[i];

			l_psvObject = dynamic_cast<SVTaskObjectClass*>(l_rsvFriend.PObject);

			if( nullptr != l_psvObject && l_psvObject->GetOwner() == this )
			{
				l_hrOk = l_psvObject->IsInputImage( p_psvImage );
			}
		}

		// Notify embeddeds...
		for( int i = 0; S_OK != l_hrOk && i < embeddedList.GetSize(); ++i )
		{
			l_psvObject = dynamic_cast<SVTaskObjectClass*>(embeddedList[i]);

			if( nullptr != l_psvObject )
			{
				l_hrOk = l_psvObject->IsInputImage( p_psvImage );
			}
		}
	}

	return l_hrOk;
}

SVTaskObjectClass* SVTaskObjectClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVTaskObjectClass* l_psvObject = nullptr;

	// Notify friends...
	for( size_t i = 0; nullptr == l_psvObject && i < m_friendList.size(); ++i )
	{
		const SVObjectInfoStruct &l_rsvFriend = m_friendList[i];

		l_psvObject = dynamic_cast<SVTaskObjectClass*>(l_rsvFriend.PObject);

		if( nullptr != l_psvObject && l_psvObject->GetOwner() == this )
		{
			l_psvObject = l_psvObject->GetObjectAtPoint( p_rsvPoint );
		}
	}

	// Notify embeddeds...
	for( int i = 0; nullptr == l_psvObject && i < embeddedList.GetSize(); ++i )
	{
		l_psvObject = dynamic_cast<SVTaskObjectClass*>(embeddedList[i]);

		if( nullptr != l_psvObject )
		{
			l_psvObject = l_psvObject->GetObjectAtPoint( p_rsvPoint );
		}
	}

	return l_psvObject;
}

bool SVTaskObjectClass::DoesObjectHaveExtents() const
{
	return false;
}

HRESULT SVTaskObjectClass::GetImageExtent( SVImageExtentClass &p_rsvImageExtent )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtentToParent( unsigned long p_ulIndex )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::GetOutputRectangle( RECT &l_roRect )
{
	return S_FALSE;
}

// Must and will be called if ObjectID of this object has been changed.
// ( Refer to SVObjectManagerClass::ChangeUniqueObjectID() )
void SVTaskObjectClass::ResetPrivateInputInterface()
{
	for (int i = 0; i < inputInterfaceList.GetSize(); ++ i)
	{
		SVInObjectInfoStruct* pInInfo =	inputInterfaceList[i];
		if (pInInfo)
		{
			const SVObjectInfoStruct &l_rsvInfo = GetObjectInfo();

			SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID &&
			         SV_GUID_NULL == info.EmbeddedID && 
			         info.ObjectType == SVNotSetObjectType &&
			         info.SubType == SVNotSetSubObjectType )
			{
				//ASSERT( FALSE );
			}

			pInInfo->SetObject( l_rsvInfo );

			info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID &&
			         SV_GUID_NULL == info.EmbeddedID && 
			         info.ObjectType == SVNotSetObjectType &&
			         info.SubType == SVNotSetSubObjectType )
			{
				//ASSERT( FALSE );
			}
		}
	}
}

BOOL SVTaskObjectClass::ConnectAllInputs()
{
	BOOL l_bOk = true;

	SVInputInfoListClass inputList;
	
	// Add the defaults
	addDefaultInputObjects(TRUE, &inputList);
	
	// tell friends to connect...
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		::SVSendMessage(rFriend.UniqueObjectID, SVM_CONNECT_ALL_INPUTS, 0, 0);
	}

	// find our inputs
	for (int i = 0; i < inputList.GetSize(); ++ i)
	{
		SVInObjectInfoStruct* pInInfo = inputList.GetAt(i);
		if (pInInfo)
		{
			// Input Object is not set...Try to get one...
			SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			// Is not yet connected...
			if (!pInInfo->IsConnected())
			{
				// if Connect to Default
				if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID )
				{
					// At least one item from the SVObjectTypeInfoStruct is required, but not all
					if ( SV_GUID_NULL != info.EmbeddedID || SVNotSetObjectType != info.ObjectType || SVNotSetSubObjectType != info.SubType )
					{
						SVObjectClass* pOwner = GetOwner();
						SVObjectClass* pRequestor = pInInfo->PObject;
						SVObjectClass* pObject = nullptr;
						BOOL bSuccess = false;
						
						// Ask first friends...
						for (size_t j = 0; j < m_friendList.size(); ++ j)
						{
							const SVObjectInfoStruct& rFriend = m_friendList[j];
							pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage(rFriend.UniqueObjectID, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&info)));
							if (pObject)
							{
								// Connect input ...
								pInInfo->SetInputObject( pObject->GetUniqueObjectID() );
								bSuccess = true;
								break;
							}
						}
						
						// Then ask owner...
						if (! bSuccess)
						{
							//GetInspection is still nullptr because in SVToolSetClass::createAllObjectsFromChild SetDefaultInputs is called before CreateObject !
							//To still get the appropriate inspection we call GetAncestor
							SVInspectionProcess* pInspection =  dynamic_cast< SVInspectionProcess* > (GetAncestor( SVInspectionObjectType ));
							while (pOwner)
							{
								// if color system & pOwner == SVToolSetClass
								if (nullptr != pInspection && pInspection->IsColorCamera() && (SV_IS_KIND_OF(pOwner, SVToolSetClass)) && SVImageObjectType == info.ObjectType )
								{
									pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage(pOwner, SVM_GET_IMAGE_BAND0_OBJECT, reinterpret_cast<DWORD_PTR>(pRequestor), reinterpret_cast<DWORD_PTR>(&info)));
								}
								else
								{
									pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage(pOwner, SVM_GETFIRST_OBJECT, reinterpret_cast<DWORD_PTR>(pRequestor), reinterpret_cast<DWORD_PTR>(&info)));
								}
								if (pObject)
								{
									// Connect input ...
									pInInfo->SetInputObject( pObject->GetUniqueObjectID() );
									break;
								}
								else
								{
									pOwner = pOwner->GetOwner();
									pRequestor = pRequestor->GetOwner();
								}
							}// end while (pOwner)
						}// end if (! bSuccess)
					}
				}// end if (SV_GUID_NULL == pInInfo->InputObjectInfo.UniqueObjectID )
				
				// Finally try to connect...
				if ( SV_GUID_NULL != pInInfo->GetInputObjectInfo().UniqueObjectID )
				{
					DWORD_PTR dwConnectResult = ::SVSendMessage(pInInfo->GetInputObjectInfo().UniqueObjectID, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(pInInfo), 0);

					l_bOk = SVMR_SUCCESS == dwConnectResult;
				}
			}
		}
		else
		{
			l_bOk = false;
		}
	}
	return l_bOk;
}

bool SVTaskObjectClass::IsObjectValid() const
{ 
	return const_cast<SVTaskObjectClass*>(this)->IsValid() ? true : false; 
}

struct CompareInputName
{
	SVString m_Name;
	CompareInputName(const SVString& rName) : m_Name(rName) {}
	bool operator()(const SVInObjectInfoStruct* pInfo) const { return pInfo->GetInputName() == m_Name; }
};

HRESULT SVTaskObjectClass::ConnectToImage(const SVString& rInputName, const SVGUID& newImageGUID)
{
	HRESULT hr = S_OK;

	SVInputInfoListClass toolInputList;
	::SVSendMessage( this, SVM_GET_INPUT_INTERFACE | SVM_NOTIFY_FRIENDS, reinterpret_cast<DWORD_PTR>(&toolInputList), 0 );
	// Find SVInObjectInfoStruct that has this name
	SVVector<SVInObjectInfoStruct*>::const_iterator it = std::find_if(toolInputList.begin(), toolInputList.end(), CompareInputName(rInputName));
	if (it != toolInputList.end())
	{
		SVImageClass* pNewImage = dynamic_cast<SVImageClass*>(SVObjectManagerClass::Instance().GetObject(newImageGUID));
		if (pNewImage)
		{
			hr = ConnectToImage((*it), pNewImage);
		}
		else
		{
			hr = E_POINTER;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT SVTaskObjectClass::ConnectToImage( SVInObjectInfoStruct* p_psvInputInfo, SVImageClass* p_psvNewImage )
{
	HRESULT l_svOk = S_OK;

	if( nullptr != p_psvInputInfo )
	{
		const GUID l_guidOldInputObjectID = p_psvInputInfo->GetInputObjectInfo().UniqueObjectID;
		SVImageClass* l_psvOldImage = dynamic_cast<SVImageClass*>( SVObjectManagerClass::Instance().GetObject( l_guidOldInputObjectID ) );

		// Disconnect input info of input object...
		if( p_psvInputInfo->IsConnected() )
		{
			// Send to the Object we are using
			::SVSendMessage( l_guidOldInputObjectID, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(p_psvInputInfo), 0 );
		}

		// Set new input...
		p_psvInputInfo->SetInputObject( p_psvNewImage );

		if( nullptr != p_psvNewImage )
		{
			// Connect input info to new input object...
			DWORD_PTR dwConnectResult = ::SVSendMessage( p_psvNewImage, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(p_psvInputInfo), 0 );
			if( dwConnectResult != SVMR_SUCCESS )
			{
				// Unable to connect to new input object....
				SVStringArray msgList;
				if( p_psvNewImage )
				{
					msgList.push_back(p_psvNewImage->GetName());
				}
				else
				{
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_UnknownString));
				}
				// Should we really be doing this here?
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CriticalUnableToConnectTo, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10203 ); 

				// Try to recover old state...
				if( nullptr != l_psvOldImage )
				{
					p_psvInputInfo->SetInputObject( l_psvOldImage );
					dwConnectResult = ::SVSendMessage( l_psvOldImage, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(p_psvInputInfo), 0 );			
				}

				l_svOk = S_FALSE;
			}
		}
		else
		{
			l_svOk = S_FALSE;
		}

		// Tell Tool image source has changed
		::SVSendMessage( GetTool(), SVM_CONNECT_ALL_INPUTS, 0, 0 );

		::SVSendMessage( GetTool(), SVM_IMAGE_SOURCE_CHANGED, 0, 0 );

		::SVSendMessage( GetTool(), SVM_RESET_ALL_OBJECTS, 0, 0 );
	}
	else
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

BOOL SVTaskObjectClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	BOOL retVal = true;
	
	if (!PCreateStruct)
		return false;
	
	retVal = SVObjectAppClass::CreateObject(PCreateStruct);
	
	// Create our friends
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			DwResult = ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pFriend), 0 );

			ASSERT(SVMR_SUCCESS == DwResult);
		}
		else
		{
			ASSERT(0);
			retVal = FALSE;
		}
			
		retVal = (SVMR_SUCCESS == DwResult) && retVal;
	}

	// Create the embeddeds...
	// Save the owner and set the owner of our embeddeds to us!
	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject)
		{
			DwResult = ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), 0 );

			ASSERT(SVMR_SUCCESS == DwResult);

			retVal = (SVMR_SUCCESS == DwResult) && retVal;
		}
	}

	isObjectValid.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;	//	Embedded
	statusTag.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	statusColor.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	
	m_isCreated = retVal;
	
	return retVal;	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsValid member function of class SVTaskObjectClass
// -----------------------------------------------------------------------------
// .Description : Returns the Validity state of this object
//				: must be overridden
////////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectClass::IsValid()
{
	BOOL bIsValid;
	
	isObjectValid.GetValue(bIsValid);
	
	return bIsValid;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate member function of class SVTaskObjectClass
// -----------------------------------------------------------------------------
// .Description : validates the inputs of this object
//				: Note: Only Override in special cases
////////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectClass::Validate()
{
	BOOL retVal = TRUE;

	clearTaskMessages();

	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject)
		{
			BOOL l_bTemp = pObject->Validate();

			ASSERT( l_bTemp );

			retVal &= l_bTemp;
		}
		else
		{
			retVal = FALSE;
		}
	}

	retVal &= SVObjectClass::Validate();
	
	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate member function of class SVTaskObjectClass
// -----------------------------------------------------------------------------
// .Description : validates the inputs of this object
//				: Note: Do Not Route , Validate Routes!
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectClass::OnValidate()
{
	BOOL l_bOk = SVObjectClass::OnValidate();

	if( l_bOk )
	{
		isObjectValid.SetValue(1, TRUE);
	}
	else
	{
		SetInvalid();
	}

	ASSERT( l_bOk );

	return l_bOk;
}

BOOL SVTaskObjectClass::SetObjectDepth(int NewObjectDepth)
{
	// Set object depth of members here...
	
	// Set Depth of our Friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			pFriend->SetObjectDepth(NewObjectDepth);
		}
	}
	
	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		if (embeddedList.GetAt(i))
		{
			embeddedList.GetAt(i)->SetObjectDepth(NewObjectDepth);
		}
	}
	
	return SVObjectAppClass::SetObjectDepth(NewObjectDepth);
}

BOOL SVTaskObjectClass::SetObjectDepthWithIndex(int NewObjectDepth, int NewLastSetIndex)
{
	// Set object depth of members here...
	
	// Set Depth of our Friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			pFriend->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
	}
	
	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		if (embeddedList.GetAt(i))
		{
			embeddedList.GetAt(i)->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
	}
	
	return SVObjectAppClass::SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
}

BOOL SVTaskObjectClass::SetImageDepth(long lDepth)
{
	// Set object depth of members here...
	
	// Set Depth of our Friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			pFriend->SetImageDepth(lDepth);
		}
	}

	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		if (embeddedList.GetAt(i))
		{
			embeddedList.GetAt(i)->SetImageDepth(lDepth);
		}
	}
	
	return SVObjectAppClass::SetImageDepth(lDepth);
}

void SVTaskObjectClass::addDefaultInputObjects(BOOL BCallBaseClass, SVInputInfoListClass* PInputListToFill)
{
	int l_iCount = inputInterfaceList.GetSize();
	int i( 0 );

	for (i = 0; i < l_iCount; i++)
	{
		SVInObjectInfoStruct* pInInfo = inputInterfaceList.GetAt(i);
		if (pInInfo)
		{
			SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID &&
			         SV_GUID_NULL == info.EmbeddedID && 
			         info.ObjectType == SVNotSetObjectType &&
			         info.SubType == SVNotSetSubObjectType )
			{
				//ASSERT( FALSE );
			}

			if (PInputListToFill)
			{
				PInputListToFill->Add(pInInfo);
			}
			else
			{
				m_InputObjectList.Add(pInInfo);
			}
		}
	}
}

BOOL SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, int p_iStringResourceID)
{
	CString Name;
	Name.LoadString(p_iStringResourceID);
	return RegisterEmbeddedObject(p_psvEmbeddedObject, p_rguidEmbeddedID, Name);
}

BOOL SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, LPCTSTR newString)
{
	SVObjectClass *l_psvObject = dynamic_cast<SVObjectClass *>(p_psvEmbeddedObject);

	BOOL l_bOk = nullptr != l_psvObject && RegisterEmbeddedObjectAsClass( l_psvObject, p_rguidEmbeddedID, newString );

	return l_bOk;
}

// bool p_bResetAlways - input 
//    true - means that the object will initiate a reset whenever the value is 
//        set. Even if set to the same value.
//    false - means that the object will only initiate a reset if its value 
//        changes.
//
// The only place that this is ever set to true is for the color HSI 
// conversion value (Color Tool) and it is probably not necessary there.
//
BOOL SVTaskObjectClass::RegisterEmbeddedObject( SVValueObjectClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, int p_iStringResourceID, bool p_bResetAlways, SVResetItemEnum p_eRequiredReset, LPCTSTR p_pszTypeName )
{
	CString Name;
	Name.LoadString(p_iStringResourceID);
	return RegisterEmbeddedObject(p_psvEmbeddedObject, p_rguidEmbeddedID, Name, p_bResetAlways, p_eRequiredReset, p_pszTypeName);
}

BOOL SVTaskObjectClass::RegisterEmbeddedObject( SVValueObjectClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, LPCTSTR strName, bool p_bResetAlways, SVResetItemEnum p_eRequiredReset, LPCTSTR p_pszTypeName )
{
	BOOL l_bOk = S_OK == p_psvEmbeddedObject->SetResetOptions( p_bResetAlways, p_eRequiredReset );

	if( l_bOk )
	{
		if( nullptr != p_pszTypeName )
		{
			p_psvEmbeddedObject->SetTypeName( p_pszTypeName );
		}
		SVObjectClass* l_psvObject = dynamic_cast<SVObjectClass*>(p_psvEmbeddedObject);
		l_bOk = nullptr != l_psvObject && RegisterEmbeddedObjectAsClass( l_psvObject, p_rguidEmbeddedID, strName );
	}

	return l_bOk;
}

BOOL SVTaskObjectClass::RegisterEmbeddedObjectAsClass(SVObjectClass* PEmbeddedObject, const GUID& REmbeddedID, LPCTSTR newObjectName)
{
	ASSERT(PEmbeddedObject);
	if (PEmbeddedObject)
	{
		for (int i = 0; i < embeddedList.GetSize(); i++)
		{
			SVObjectClass* pObject = embeddedList.GetAt(i);
			if (pObject)
			{
				if (pObject->GetEmbeddedID() == REmbeddedID)
				{
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_DuplicateEmbeddedId, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10204 ); 
					return FALSE;
				}
			}
		}
		// Set object embedded to Setup the Embedded GUID
		PEmbeddedObject->SetObjectEmbedded(REmbeddedID, this, newObjectName);
		
		// Add to embedded object to List of Embedded Objects
		AddEmbeddedObject(PEmbeddedObject);
		
		return TRUE;
	}
	return FALSE;
}

SVTaskObjectClass::SVObjectPtrDeque SVTaskObjectClass::GetPreProcessObjects() const
{
	SVObjectPtrDeque l_Objects = SVObjectAppClass::GetPreProcessObjects();

	return l_Objects;
}

SVTaskObjectClass::SVObjectPtrDeque SVTaskObjectClass::GetPostProcessObjects() const
{
	SVObjectPtrDeque l_Objects = SVObjectAppClass::GetPostProcessObjects();

	SVObjectClassPtrArray::const_iterator l_Iter;
	for( l_Iter = embeddedList.begin(); l_Iter != embeddedList.end(); ++l_Iter )
	{
		SVObjectClass* pObject = *l_Iter;
		if( nullptr != pObject )
		{
			l_Objects.push_back( pObject ); 
		}
	}

	return l_Objects;
}

BOOL SVTaskObjectClass::RegisterInputObject(SVInObjectInfoStruct* PInObjectInfo, const SVString& p_rInputName)
{
	if (PInObjectInfo)
	{
		if (inputInterfaceList.Add(PInObjectInfo) >= 0)
		{
			PInObjectInfo->SetInputName( p_rInputName );

			return TRUE;
		}

		PInObjectInfo->SetInputName( SVString() );
	}
	return FALSE;
}

void SVTaskObjectClass::GetPrivateInputList(SVInputInfoListClass& RInputInterface)
{
	RInputInterface.Copy(inputInterfaceList);
}

// Set an Embedded Object's data Value(s)
BOOL SVTaskObjectClass::SetEmbeddedObjectValue(const GUID& RObjectID, SVObjectAttributeClass* PDataObject)
{
	BOOL rc = FALSE;
	GUID uniqueID;
	
	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		if (embeddedList.GetAt(i))
		{
			// check if it's this object
			uniqueID = embeddedList.GetAt(i)->GetUniqueObjectID();
			if (RObjectID == uniqueID)
			{
				// Set the Object Depth ?
				
				// Set the Object's Data Member Value
				rc = ( S_OK == embeddedList.GetAt(i)->SetObjectValue(PDataObject) );
				break;
			}
		}
	}
	return rc;
}

// Set the valid flag to Invalid
// and all our embedded objects ???
void SVTaskObjectClass::SetInvalid()
{
	// Set all embeddeds ( outputs ) to invalid...
	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject)
		{
			pObject->SetInvalid();
		}
	}
	
	// Set yourself to invalid...
	isObjectValid.SetValue(1, FALSE);
}

// Override this function to implement object behavior...
void SVTaskObjectClass::SetDisabled()
{
	// Set all embeddeds ( outputs ) to disabled...
	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject)
		{
			pObject->SetDisabled();
		}
	}
	
	// Set yourself to disabled...
	
	// Special implementation here:
	// Set just your object color to disabled...
	statusColor.SetValue(1, SV_DEFAULT_DISABLED_COLOR);
	
	// Set friends to disabled...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		if (rFriend.PObject)
		{
			rFriend.PObject->SetDisabled();
		}
	}
}

DWORD SVTaskObjectClass::GetObjectColor() const
{
	DWORD dwColor;
	statusColor.GetValue(dwColor);
	return dwColor;
}

DWORD SVTaskObjectClass::GetObjectState() const
{
	DWORD dwState;
	statusTag.GetValue(dwState);
	return dwState;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetInputOutputObjects
// -----------------------------------------------------------------------------
// .Description : Use this function to get the in- and outputs of this object.
//				: Normally used from an overlaying TaskListObject or TaskObject.
////////////////////////////////////////////////////////////////////////////////
void SVTaskObjectClass::GetInputObjects(SVInputInfoListClass& RInputObjectList)
{
	// Add our Friends first
	
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			pTaskObjectFriend->GetInputObjects(RInputObjectList);
		}
	}
	
	int j( 0 );

	for (j = 0; j < m_InputObjectList.GetSize(); ++ j)
	{
		if (m_InputObjectList.GetAt(j))
		{
			RInputObjectList.Add(m_InputObjectList.GetAt(j));
		}
	}
}

void SVTaskObjectClass::GetAllInputObjects()
{
	// Tell Friends to rebuild their lists
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			pTaskObjectFriend->GetAllInputObjects();
		}
	}
}

void SVTaskObjectClass::Persist(SVObjectWriter& rWriter)
{
	SVObjectAppClass::Persist(rWriter);

	PersistFriends(rWriter);
	PersistEmbeddeds(rWriter);
	PersistInputs(rWriter);
}

void SVTaskObjectClass::PersistFriends(SVObjectWriter& rWriter)
{
	// Get script of close friends list members...
	if (m_friendList.size())
	{
		rWriter.StartElement(scFriendsTag);

		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rFriend = m_friendList[i];
			// Check if Friend is alive...
			SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
			if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
			{
				rWriter.StartElement(pTaskObjectFriend->GetObjectName()); // use internal name for node name
				pTaskObjectFriend->Persist(rWriter);
				rWriter.EndElement();
			}
		}
		rWriter.EndElement();
	}
}

void SVTaskObjectClass::PersistInputs(SVObjectWriter& writer)
{
	// Set up input list...
	SVInputInfoListClass inputList;
	addDefaultInputObjects(TRUE, &inputList);
	
	if (inputList.GetSize())
	{
		SVNameVariantList list;
		//SVVariantList list;
		for (int i = 0;i < inputList.GetSize(); ++i)
		{
			if (inputList.GetAt(i))
			{
				_bstr_t name;
				_variant_t value;
				
				const SVInObjectInfoStruct& inputInfo = *(inputList.GetAt(i));
				value.SetString(inputInfo.GetInputObjectInfo().UniqueObjectID.ToString().c_str());
				name = _bstr_t(inputInfo.GetInputName().c_str());
				// hold name value pair...
				list.insert(std::make_pair(name, value));
				value.Clear();
			}
		}
		writer.StartElement(scInputsTag);
		if (list.size())
		{
			for (SVNameVariantList::const_iterator it = list.begin();it != list.end();++it)
			{
				writer.StartElement(scInputTag);
				writer.WriteAttribute(scNameTag, it->first);
				writer.WriteAttribute(scValueTag, it->second);
				writer.EndElement();
			}
		}
		writer.EndElement();
	}
}

void SVTaskObjectClass::PersistEmbeddeds(SVObjectWriter& rWriter)
{
	// Set up embedded object definitions...
	if (embeddedList.GetSize())
	{
		rWriter.StartElement(scEmbeddedsTag);
		// Get embedded object script...
		for (int j = 0; j < embeddedList.GetSize(); j++)
		{
			embeddedList.GetAt(j)->Persist(rWriter);
		}
		rWriter.EndElement();
	}
}

BOOL SVTaskObjectClass::Run(SVRunStatusClass& RRunStatus)
{
	// Run yourself...
	BOOL bRetVal = onRun(RRunStatus);
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	return bRetVal;
}

BOOL SVTaskObjectClass::onRun(SVRunStatusClass& RRunStatus)
{
	SVToolClass* pTool = GetTool();

	// set error display
	if(nullptr != pTool)
	{
		msvError.m_bDisplayError = pTool->msvError.m_bDisplayError;
	}

	// Run first friends...
	BOOL bRetVal = runFriends(RRunStatus);
	
	// Now Validate yourself...
	if (! OnValidate())
	{
		RRunStatus.SetInvalid();
		bRetVal = FALSE;
	}
	
	return bRetVal;
}

BOOL SVTaskObjectClass::runFriends(SVRunStatusClass& RRunStatus)
{
	BOOL bRetVal = true;
	
	// Run your friends
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		if (SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(rFriend.PObject))
		{
			bRetVal = pTaskObject->Run(RRunStatus) && bRetVal;
		}
		else
		{
			bRetVal = false;
		}
	}
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
void SVTaskObjectClass::Disconnect()
{
	for (int inIndex = m_InputObjectList.GetSize() - 1; inIndex >= 0; inIndex--)
	{
		SVInObjectInfoStruct* pInObjectInfo = m_InputObjectList.GetAt(inIndex);
		if (pInObjectInfo)
		{
			if( pInObjectInfo->IsConnected() && SV_GUID_NULL != pInObjectInfo->UniqueObjectID )
			{
				// Send to the Object we are using
				::SVSendMessage(pInObjectInfo->GetInputObjectInfo().UniqueObjectID,
					SVM_DISCONNECT_OBJECT_INPUT, 
					reinterpret_cast<DWORD_PTR>(pInObjectInfo), 0);
			}
		}
		// remove it from the list
		m_InputObjectList.RemoveAt(inIndex);
	}
	
	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	// Notify all Objects dependent on our Outputs that 
	// the outputs are not available anymore
	for (int outIndex = l_OutputInfoList.GetSize() - 1; outIndex >= 0; outIndex--)
	{
		SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt(outIndex);
		if (pOutObjectInfo)
		{
			if ( SVTaskObjectListClass* pTaskObjectList = dynamic_cast <SVTaskObjectListClass*> (GetOwner()) )
			{
				pTaskObjectList->RemoveOutputObject( pOutObjectInfo );
			}
			pOutObjectInfo->DisconnectAllInputs();
		}
	}
	
	// Disconnect our Friends
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			// Tell Friends to Disconnect...
			pTaskObjectFriend->Disconnect();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectClass::DisconnectInput(SVInObjectInfoStruct* pInObjectInfo)
{
	BOOL l_Status( false );

	if( nullptr != pInObjectInfo )
	{
		for( int i = 0; ! l_Status && i < inputInterfaceList.GetSize(); i++ )
		{
			SVInObjectInfoStruct* pInputObjectInfo = inputInterfaceList.GetAt(i);
			
			if (pInputObjectInfo)
			{
				if (pInObjectInfo->GetInputObjectInfo().UniqueObjectID == pInputObjectInfo->GetInputObjectInfo().UniqueObjectID)
				{
					pInputObjectInfo->SetInputObject( nullptr );
					
					l_Status = true;
				}
			}
		}

		for( int j = 0; j < embeddedList.GetSize(); ++j )
		{
			SVImageClass* l_pImage( dynamic_cast< SVImageClass* >( embeddedList[ j ] ) );

			if( nullptr != l_pImage )
			{
				if( pInObjectInfo->GetInputObjectInfo().UniqueObjectID == l_pImage->GetUniqueObjectID() )
				{
					l_pImage->UpdateImage( SV_GUID_NULL );
				}
			}
		}
	}

	return l_Status;
}

// Add embedded object pointer to the embedded List.
// Use this only for real embedded objects.
void SVTaskObjectClass::AddEmbeddedObject(SVObjectClass* PObject)
{
	ASSERT(nullptr != PObject);
	
	// Add to Owner's List of Embedded Objects
	embeddedList.Add(PObject);
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVTaskObjectClass::RemoveEmbeddedObject(SVObjectClass* pObjectToRemove)
{
	// iterate and remove object if in embedded list.
	for (int i = embeddedList.GetSize() - 1; i >= 0; i--)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject)
		{
			if (pObject == pObjectToRemove)
			{
				if (pObject->IsCreated())
				{
					pObject->CloseObject();
				}
				embeddedList.RemoveAt(i);
				break;
			}
		}
	}

	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	// if object is on output list, remove it there as well!
	for (int outIndex = l_OutputInfoList.GetSize() - 1; outIndex >= 0; outIndex--)
	{
		SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt(outIndex);
		if (pOutObjectInfo && pOutObjectInfo->PObject == pObjectToRemove)
		{
			// remove from owner's list
			if ( SVTaskObjectListClass* pTaskObjectList = dynamic_cast <SVTaskObjectListClass*> (GetOwner()) )
			{
				pTaskObjectList->RemoveOutputObject( pOutObjectInfo );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Hide Embedded Object - UnSelectable and Not Scripted
////////////////////////////////////////////////////////////////////////////////////
void SVTaskObjectClass::hideEmbeddedObject(SVObjectClass& RObjectToHide)
{
	// Restrict selection (picked for resultView, published, used in an Equation, or selected for Archiving)
	RObjectToHide.ObjectAttributesAllowedRef() &=(~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);
	
	// Reset any selection
	RObjectToHide.ObjectAttributesSetRef() = 0;
	
	// Get this object's outputInfo
	SVOutObjectInfoStruct& valueOutObjectInfo = RObjectToHide.GetObjectOutputInfo();
	
	valueOutObjectInfo.DisconnectAllInputs();
}

// Added to process friends
BOOL SVTaskObjectClass::CloseObject()
{
	BOOL retVal = true;

	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	
	// Close ourself first
	retVal &= SVObjectAppClass::CloseObject();
	
	// Close our Friends
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			// Close Friend...
			DwResult = ::SVSendMessage(pFriend, SVM_CLOSE_OBJECT, 0, 0);
				
			retVal = (DwResult == SVMR_SUCCESS) && retVal;
		}
	}
	
	// Close Embeddeds...
	for (int i = embeddedList.GetSize() - 1; i >= 0; i--)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if (pObject && pObject->IsCreated())
		{
			pObject->CloseObject();
		}
	}
	return retVal;
}

DWORD_PTR SVTaskObjectClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	//
	// NOTE: 
	//	General rule for routing:
	//		Opening Direction ==> 1. Friends, 2. You ( and Your embeddeds )
	//		Closing Direction ==> 1. You ( and Your embeddeds ), 2. Friends
	//
	
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	
	SVAnalyzerLevelCreateStruct createStruct;

	// Try to process message by yourself...
	// ( if necessary process here the incoming messages )
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch (dwPureMessageID)
	{
		case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

		case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( S_OK != ConnectObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}
	}

	// Check if friend should process this message first....
	if ((DwMessageID & SVM_NOTIFY_FRIENDS) == SVM_NOTIFY_FRIENDS)
	{
		//
		// NOTE: Beware to route Create Object Messages here !!!
		//
		
		DWORD_PTR l_dwTmp = 0;
		// Notify friends...
		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rfriend = m_friendList[i];
			if (rfriend.PObject)
			{
				// Notify not only friends of friends...
				DWORD dwMaskID = (DwMessageID & ~SVM_NOTIFY_ONLY_FRIENDS) | SVM_NOTIFY_FRIENDS;
				l_dwTmp = ::SVSendMessage(rfriend.PObject, dwMaskID, DwMessageValue, DwMessageContext);
			}
				
			// Check if processed and Notify First Responding...
			if (l_dwTmp != SVMR_NOT_PROCESSED && (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING)
			{
				return l_dwTmp;
			}

			if( l_dwTmp != SVMR_SUCCESS )
			{
				DwResult = l_dwTmp;
			}
		}
			
		// Check if Notify Only Friends...
		if ((DwMessageID & SVM_NOTIFY_ONLY_FRIENDS) == SVM_NOTIFY_ONLY_FRIENDS)
		{
			return DwResult;
		}
	}
	
	
	// Try to process message by yourself...
	// ( if necessary process here the incoming messages )
	switch (dwPureMessageID)
	{
		case SVMSGID_GETFIRST_OBJECT:
			{
				// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo!
				//( SVObjectTypeInfoStruct->objectType => SVObjectTypeEnum )
				SVObjectTypeInfoStruct* pObjectTypeInfo = reinterpret_cast<SVObjectTypeInfoStruct*>(DwMessageContext);
				
				// ...use second message parameter ( DwMessageValue ) to specify the object requesting
				// Note: if second parameter is specified - stop when requesting object encountered.
				// Optional - to get an input
				SVObjectClass* pRequestor = reinterpret_cast<SVObjectClass*>(DwMessageValue);
				
				if (pRequestor && pRequestor == this || pRequestor == GetOwner())
				{
					// Do not reference self 
					return SVMR_NOT_PROCESSED;
				}
				
				DwResult = SVObjectAppClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
				if (DwResult != SVMR_NOT_PROCESSED)
					return DwResult;
				
				break;
			}
			
			case SVMSGID_OVERWRITE_OBJECT:
				{
					// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
					// ...use third message parameter ( DwMessageContext ) as embeddedID ( GUID* ) 
					// ...returns pointer to embedded SVObjectClass
					const GUID embeddedID	= * ((GUID*) DwMessageContext);
					// NOTE:	Only for embedded objects !!! 
					//			Dynamically generated objects must be replaced,
					//			using SVM_REPLACE_OBJECT!!!
					
					// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
					// ... and if object could not be identified then call base class processMessage member function!!!
					
					for (int i = 0; i < embeddedList.GetSize(); i++)
					{
						SVObjectClass* pEmbeddedObject = embeddedList.GetAt(i);
						if ( nullptr != pEmbeddedObject )
						{
							const GUID& ObjectID = pEmbeddedObject->GetEmbeddedID();

							if( ObjectID == embeddedID)
							{
								return ::SVSendMessage(pEmbeddedObject, DwMessageID, DwMessageValue, DwMessageContext);
							}
						}
					}
					break;
				}
				
			case SVMSGID_CONNECT_ALL_INPUTS:
				{
					if (ConnectAllInputs())
						return SVMR_SUCCESS;
					
					return SVMR_NO_SUCCESS;
				}
				
			case SVMSGID_CLOSE_OBJECT:
				{
					if (CloseObject())
						return SVMR_SUCCESS;
					
					return SVMR_NO_SUCCESS;
				}
				
				// This was Added because SVTaskObjectClasses can also have Friends
			case SVMSGID_DESTROY_FRIEND_OBJECT:
				{
					SVTaskObjectClass* pTaskObject = reinterpret_cast<SVTaskObjectClass*>(DwMessageValue);
					
					// Close the Friend Object
					if (pTaskObject)
					{
						// Notify the Owner(s) of our Friends inputs that they are not needed anymore
						pTaskObject->Disconnect();
						
						// Close the Object
						pTaskObject->CloseObject();
						
						// Destroy our Friend's Friends
						pTaskObject->DestroyFriends();
						
						// Delete the Friend Object
						delete pTaskObject;
						
						return SVMR_SUCCESS;
					}
					return SVMR_NO_SUCCESS;
				}
				
			case SVMSGID_SET_OBJECT_VALUE:
				{
					// check our embedded objects to see if it's the one
					const GUID objectID = * ((GUID*) DwMessageValue);
					SVObjectAttributeClass* pDataObject = reinterpret_cast<SVObjectAttributeClass*>(DwMessageContext);
					
					// check if it is for us (Friend assignment)
					if (objectID == GetUniqueObjectID() && 
						CString("Friend").Compare(pDataObject->GetName()) == 0)
					{
						return SVObjectAppClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
					}
					else
					{
						if (SetEmbeddedObjectValue(objectID, pDataObject))
						{
							return SVMR_SUCCESS;
						}
						else
						{
//							ASSERT(FALSE);
						}
					}
					break;
				}
				
			case SVMSGID_GET_INPUT_INTERFACE:
				{
					SVInputInfoListClass* pInputList = reinterpret_cast<SVInputInfoListClass*>(DwMessageValue);
					if (nullptr != pInputList)
					{
						// Local input list...
						SVInputInfoListClass localInputList;
						GetPrivateInputList(localInputList);
						pInputList->Append(localInputList);
						return SVMR_SUCCESS;
					}
					ASSERT(0);
					return SVMR_NO_SUCCESS;
				}
				
				// This Message occurs for two scenarios
				// 1. Some Object is using our outputs and they are no longer needed.
				// 2. We are using some Object's outputs and the outputs are no longer available
			case SVMSGID_DISCONNECT_OBJECT_INPUT:
				{
					// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
					// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
					SVInObjectInfoStruct* pInObjectInfo = reinterpret_cast<SVInObjectInfoStruct*>(DwMessageValue);
					if (DisconnectInput(pInObjectInfo))
						DwResult = SVMR_SUCCESS;
				}
				break;
	}
	
	if ((DwMessageID & SVM_NOTIFY_ONLY_THIS) != SVM_NOTIFY_ONLY_THIS  &&
		(DwMessageID & SVM_NOTIFY_ONLY_CHILDREN) != SVM_NOTIFY_ONLY_CHILDREN)
	{
		DwResult = OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );
	}
	else
	{
		// Route to Base Class if not yet processed.
		if (DwResult == SVMR_NOT_PROCESSED)
			return SVObjectAppClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
	}
	
	return DwResult;
}

DWORD_PTR SVTaskObjectClass::OutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	if ((DwMessageID & SVM_NOTIFY_FRIENDS) == SVM_NOTIFY_FRIENDS)
	{
		DwResult = FriendOutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );
	}

	// Try to send message to outputObjectList members, if not already processed...
	if ((DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING)
	{
		if (!DwResult)
		{
			DwResult = EmbeddedOutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );
		}
	}
	else
	{
		DwResult = EmbeddedOutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult;
	}
	return DwResult;
}

DWORD_PTR SVTaskObjectClass::FriendOutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to send message to outputObjectList members, if not already processed...
	if( (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING )
	{
		for (size_t i = 0; ! DwResult && i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rFriend = m_friendList[ i ];

			// Check if Friend is alive...
			SVTaskObjectClass* l_pObject( nullptr );

			l_pObject = dynamic_cast< SVTaskObjectClass* >( SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID ) );

			if( nullptr != l_pObject )
			{
				DwResult = l_pObject->OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext);
			}
			else
			{
				DwResult = SVMR_NOT_PROCESSED;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rFriend = m_friendList[ i ];

			// Check if Friend is alive...
			SVTaskObjectClass* l_pObject( nullptr );

			l_pObject = dynamic_cast< SVTaskObjectClass* >( SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID ) );

			if( nullptr != l_pObject )
			{
				DwResult = l_pObject->OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext) | DwResult;
			}
			else
			{
				DwResult = SVMR_NOT_PROCESSED | DwResult;
			}
		}
	}

	return DwResult;
}

DWORD_PTR SVTaskObjectClass::EmbeddedOutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to send message to outputObjectList members, if not already processed...
	if ((DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING)
	{
		for( long i = 0; ! DwResult && i < embeddedList.GetSize(); i++ )
		{
			SVObjectClass* l_pObject = embeddedList.GetAt(i);

			if( nullptr != l_pObject )
			{
				DwResult = ::SVSendMessage(l_pObject, DwMessageID, DwMessageValue, DwMessageContext);
			}
			else
			{
				DwResult = SVMR_NOT_PROCESSED;
			}
		}
	}
	else
	{
		for( long i = 0; i < embeddedList.GetSize(); i++ )
		{
			SVObjectClass* l_pObject = embeddedList.GetAt(i);

			if( nullptr != l_pObject )
			{
				DwResult = ::SVSendMessage(l_pObject, DwMessageID, DwMessageValue, DwMessageContext) | DwResult;
			}
			else
			{
				DwResult = SVMR_NOT_PROCESSED | DwResult;
			}
		}
	}

	return DwResult;
}

HRESULT SVTaskObjectClass::GetDependentsList( SVObjectPairVector& rListOfDependents, bool bOnlyImageDependencies )
{
	ASSERT( rListOfDependents.size() == 0 );	// should be clear before calling
	int itemNo = 0;

	// Check for Dependents
	SVOutputInfoListClass l_OutputInfoList;
	
	GetOutputList( l_OutputInfoList );

	if( l_OutputInfoList.HasDependents() )
	{
		long l_lCount = l_OutputInfoList.GetSize();

		for( int i = 0;i < l_lCount; ++ i )
		{
			SVOutObjectInfoStruct* pOutput = l_OutputInfoList.GetAt( i );
			if( pOutput )
			{
				if( bOnlyImageDependencies && pOutput->ObjectTypeInfo.ObjectType != SVImageObjectType )
					continue;

				pOutput->GetDependentsList( this, rListOfDependents );
			}// end if( pOutput )
		}// end for( int i = 0;i < pOutputInfoList->GetSize(); ++ i )
	}// end if( pOutputInfoList && pOutputInfoList->HasDependents() )
	
	//return (itemNo != 0);
	return S_OK;
}

// Get the dependents list for a specified list of objects (owned by this object)
HRESULT SVTaskObjectClass::GetDependentsList( const SVObjectVector& rListOfObjects, SVObjectPairVector& rListOfDependents )
{
	int itemNo = 0;

	// Check for Dependents
	SVOutputInfoListClass l_OutputInfoList;
	
	GetOutputList( l_OutputInfoList );

	GetNonToolsetOutputList( l_OutputInfoList );

	if( l_OutputInfoList.HasDependents() )
	{
		SVObjectVector::const_iterator iter;
		for ( iter = rListOfObjects.begin(); iter != rListOfObjects.end(); ++iter)
		{
			SVOutObjectInfoStruct* pOutput = nullptr;
			for( int i = 0;i < l_OutputInfoList.GetSize() && (nullptr == pOutput || pOutput->PObject != *iter); ++ i )
			{
				pOutput = l_OutputInfoList.GetAt( i );
			}

			ASSERT( pOutput );	// if this asserts, the object is not on this object's output list
			if( pOutput && pOutput->PObject == *iter)
			{
				pOutput->GetDependentsList( this, rListOfDependents );
			}// end if( pOutput )
		}// end for ( iter = rListOfObject.begin(); iter != rListOfObjects.end(); iter++)
	}// end if( pOutputInfoList && pOutputInfoList->HasDependents() )
	
	//return (itemNo != 0);
	return S_OK;
}

HRESULT SVTaskObjectClass::GetImageList( SVImageListClass& p_rImageList, UINT uiAttributes, bool bAND )
{
	HRESULT hr = S_OK;

    SVObjectTypeInfoStruct  info;

	info.ObjectType = SVImageObjectType;
	info.SubType = SVNotSetSubObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( nullptr != pImage )
		{
			bool bAttributesOK = bAND ? (pImage->ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
									  : (pImage->ObjectAttributesSet() & uiAttributes) > 0;            // OR
			if ( bAttributesOK )
			{
				p_rImageList.Add(pImage);
			}
		}
	}

	return hr;
}

HRESULT SVTaskObjectClass::GetDependentsList( SVObjectListClass& rListOfDependents, bool bOnlyImageDependencies )
{
	ASSERT( rListOfDependents.GetSize() == 0 );	// should be clear before calling
	SVObjectPairVector list;
	HRESULT hr = GetDependentsList( list, bOnlyImageDependencies );
	SVObjectPairVector::iterator iter;
	for ( iter = list.begin(); iter != list.end(); ++iter )
	{
		rListOfDependents.Add( iter->first );
	}
	return hr;
}

HRESULT SVTaskObjectClass::RegisterSubObject( SVValueObjectClass* p_pValueObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pValueObject )
	{
		m_svValueObjectSet.insert( p_pValueObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::RegisterSubObject( SVImageClass* p_pImageObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pImageObject )
	{
		// What is the restriction here?
		if( nullptr == dynamic_cast< SVCameraImageTemplate* >( p_pImageObject ) )
		{
			m_svImageObjectSet.insert( p_pImageObject );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::UnregisterSubObject( SVValueObjectClass* p_pValueObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pValueObject )
	{
		m_svValueObjectSet.erase( p_pValueObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::UnregisterSubObject( SVImageClass* p_pImageObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pImageObject )
	{
		m_svImageObjectSet.erase( p_pImageObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::RegisterSubObjects( SVTaskObjectClass* p_psvOwner, SVObjectClassPtrArray &p_rsvEmbeddedList )
{
	HRESULT l_hrOk = UnregisterSubObjects( p_psvOwner );

	if( S_OK == l_hrOk )
	{
		long l_lSize = p_rsvEmbeddedList.GetSize();

		for ( long l = 0; l < l_lSize; l++ )
		{
			SVValueObjectClass* l_psvValueObject = dynamic_cast<SVValueObjectClass*>( p_rsvEmbeddedList[ l ] );

			if( nullptr != l_psvValueObject )
			{
				m_svValueObjectSet.insert( l_psvValueObject );
			}
			else if( nullptr == dynamic_cast< SVCameraImageTemplate* >( p_rsvEmbeddedList[ l ] )  )
			{
				SVImageClass* l_psvImageObject = dynamic_cast<SVImageClass*>( p_rsvEmbeddedList[ l ] );

				if( nullptr != l_psvImageObject )
				{
					m_svImageObjectSet.insert( l_psvImageObject );
				}
			}
		}
	}

	if( S_OK == l_hrOk )
	{
		if( nullptr == GetInspection() || S_OK != GetInspection()->RegisterSubObjects( p_psvOwner, p_rsvEmbeddedList ) )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT SVTaskObjectClass::UnregisterSubObjects( SVTaskObjectClass* p_psvOwner )
{
	HRESULT l_hrOk = S_OK;

	if( nullptr == GetInspection() || S_OK != GetInspection()->UnregisterSubObjects( p_psvOwner ) )
	{
		l_hrOk = S_FALSE;
	}

	SVValueObjectClassPtrSet::iterator l_oValueIter = m_svValueObjectSet.begin();

	while( l_oValueIter != m_svValueObjectSet.end() )
	{
		if( nullptr == *l_oValueIter || (*l_oValueIter)->GetOwner() == p_psvOwner )
		{
			l_oValueIter = m_svValueObjectSet.erase( l_oValueIter );
		}
		else
		{
			++l_oValueIter;
		}
	}

	SVImageClassPtrSet::iterator l_oImageIter = m_svImageObjectSet.begin();

	while( l_oImageIter != m_svImageObjectSet.end() )
	{
		if( nullptr == *l_oImageIter || (*l_oImageIter)->GetOwner() == p_psvOwner )
		{
			l_oImageIter = m_svImageObjectSet.erase( l_oImageIter );
		}
		else
		{
			++l_oImageIter;
		}
	}

	return l_hrOk;
}

//namespace
//{
struct local
{
	struct IsInput
	{
		IsInput(const SVObjectClass* pInput) : m_pInput(pInput) {}
		bool operator () (const SVInObjectInfoStruct* pInput) {return pInput->PObject == m_pInput;}
		const SVObjectClass* m_pInput;
	};
	struct IsOutput
	{
		IsOutput(const SVObjectClass* pOutput) : m_pOutput(pOutput) {}
		bool operator () (const SVOutObjectInfoStruct* pOutput) {return pOutput->PObject == m_pOutput;}
		const SVObjectClass* m_pOutput;
	};
};
//}
HRESULT SVTaskObjectClass::DisconnectInputsOutputs(SVObjectVector& rListOfObjects)
{
	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	SVObjectVector::const_iterator iterList;
	SVInputInfoListClass::iterator iterInput;
	SVOutputInfoListClass::iterator iterOutput;
	SVObjectClassPtrArray::iterator iterEmbedded;

	// code borrowed from ::Disconnect and ::CloseObject
	for ( iterList = rListOfObjects.begin(); iterList != rListOfObjects.end(); ++iterList)
	{
		// check inputs
		if ( (iterInput = std::find_if(m_InputObjectList.begin(), m_InputObjectList.end(), local::IsInput(*iterList) )) != m_InputObjectList.end() )
		{
			SVInObjectInfoStruct* pInObjectInfo = *iterInput;
			if (pInObjectInfo)
			{
				if( pInObjectInfo->IsConnected() && SV_GUID_NULL != pInObjectInfo->UniqueObjectID )
				{
					// Send to the Object we are using
					::SVSendMessage(pInObjectInfo->GetInputObjectInfo().UniqueObjectID,
						SVM_DISCONNECT_OBJECT_INPUT, 
						reinterpret_cast<DWORD_PTR>(pInObjectInfo), 0);
				}
			}
		}
		
		// check outputs
		if ( (iterOutput = std::find_if(l_OutputInfoList.begin(), l_OutputInfoList.end(), local::IsOutput(*iterList) )) != l_OutputInfoList.end() )
		{
			SVOutObjectInfoStruct* pOutObjectInfo = *iterOutput;
			if (pOutObjectInfo)
			{
				pOutObjectInfo->DisconnectAllInputs();
			}
			l_OutputInfoList.erase(iterOutput);
		}

		// check embeddeds
		if ( (iterEmbedded = std::find(embeddedList.begin(), embeddedList.end(), *iterList )) != embeddedList.end() )
		{
			SVObjectClass* pObject = *iterEmbedded;
			if (pObject && pObject->IsCreated())
			{
				pObject->CloseObject();
			}
			embeddedList.erase(iterEmbedded);
		}

		// check friend list??
	}

	return S_OK;
}

HRESULT SVTaskObjectClass::HideInputsOutputs(SVObjectVector& rListOfObjects)
{
	SVObjectVector::iterator iter;
	for ( iter = rListOfObjects.begin(); iter != rListOfObjects.end(); ++iter )
	{
		SetBits( (*iter)->ObjectAttributesSetRef(), SV_VIEWABLE , false );
	}
	return S_OK;
}

HRESULT SVTaskObjectClass::GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine )
{
	HRESULT l_Status = S_OK;

	return l_Status;
}

HRESULT SVTaskObjectClass::UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine )
{
	HRESULT l_Status = S_OK;

	p_rMultiLine.m_ObjectID = GetUniqueObjectID();

	if( nullptr != GetAnalyzer() )
	{
		p_rMultiLine.m_AnalyzerID = GetAnalyzer()->GetUniqueObjectID();
	}
	else
	{
		p_rMultiLine.m_AnalyzerID.clear();
	}

	if( nullptr != GetTool() )
	{
		p_rMultiLine.m_ToolID = GetTool()->GetUniqueObjectID();
	}
	else
	{
		p_rMultiLine.m_ToolID.clear();
	}

	if( nullptr != GetInspection() )
	{
		p_rMultiLine.m_InspectionID = GetInspection()->GetUniqueObjectID();
	}
	else
	{
		p_rMultiLine.m_InspectionID.clear();
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::UpdateOverlayColor( SVExtentMultiLineStruct& p_rMultiLine )
{
	HRESULT l_Status = S_OK;

	COLORREF color = GetObjectColor();

	p_rMultiLine.m_Color = color;

	return l_Status;
}

HRESULT SVTaskObjectClass::UpdateOverlayName( SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents )
{
	HRESULT l_Status = S_OK;

	SVExtentPointStruct l_Point( 0, 0 );

	if( S_OK == p_pImageExtents.GetTitlePoint( l_Point ) )
	{
		CString l_String = GetName();

		p_rMultiLine.m_StringPoint = l_Point;
		p_rMultiLine.m_csString = l_String;
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT hrRet = S_OK;
		
	if ( m_bUseOverlays )
	{
		hrRet = onCollectOverlays(p_Image, p_MultiLineArray);
	}

	for ( size_t j = 0; j < m_friendList.size(); j++ )
	{
		const SVObjectInfoStruct& l_rFriend = m_friendList[j];

		SVTaskObjectClass* pObject = dynamic_cast< SVTaskObjectClass* >( l_rFriend.PObject );
		
		if ( pObject )
		{
			HRESULT l_Temp = pObject->CollectOverlays(p_Image, p_MultiLineArray);

			if( S_OK == hrRet )
			{
				hrRet = l_Temp;
			}
		}
	}

	return hrRet;
}

HRESULT SVTaskObjectClass::onCollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_Image )
	{
		SVExtentFigureStruct l_ImageFigureStruct;
		SVImageExtentClass l_ImageExtents;

		if ( S_OK == GetImageExtent( l_ImageExtents ) && S_OK == l_ImageExtents.GetFigure( l_ImageFigureStruct ) )
		{
			SVExtentMultiLineStruct l_MultiLine;

			l_Status = UpdateOverlayIDs( l_MultiLine );

			if( S_OK == l_Status )
			{
				l_Status = UpdateOverlayName( l_MultiLine, l_ImageExtents );
			}

			if( S_OK == l_Status )
			{
				SVToolClass* l_pTool = GetTool();

				if( nullptr != l_pTool )
				{
					l_Status = l_pTool->UpdateOverlayColor( l_MultiLine );

					if( S_OK == l_Status )
					{
						l_Status = l_pTool->GetDrawInfo( l_MultiLine );
					}
				}
				else
				{
					l_Status = UpdateOverlayColor( l_MultiLine );

					if( S_OK == l_Status )
					{
						l_Status = GetDrawInfo( l_MultiLine );
					}
				}
			}

			if( S_OK == l_Status )
			{
				l_MultiLine.AssignExtentFigure( l_ImageFigureStruct, l_MultiLine.m_Color );

				p_MultiLineArray.Add( l_MultiLine );
			}
		}
	}

	return l_Status;
}

SVValueObjectClass* SVTaskObjectClass::GetEmbeddedValueObject( GUID classguid )
{
	SVValueObjectClass* pValueObject = nullptr;

	for (int i = 0; i < embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = embeddedList.GetAt(i);
		if ( pObject->GetEmbeddedID() == classguid )
		{
			pValueObject = dynamic_cast <SVValueObjectClass*> (pObject);
			break;
		}
	}

	return pValueObject;
}

HRESULT SVTaskObjectClass::ResetObjectInputs()
{
	HRESULT l_Status = SVObjectAppClass::ResetObjectInputs();

	addDefaultInputObjects();

	return l_Status;
}

