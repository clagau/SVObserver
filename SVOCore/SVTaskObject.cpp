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
#include "SVImageClass.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "CameraLibrary/SVTemplate.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVValueObjectLibrary/SVValueObjectReference.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "ObjectSelectorLibrary/SelectorItemVector.h"
#include "ObjectInterfaces/IBasicValueObject.h"
#include "ObjectInterfaces/IRootObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "ObjectInterfaces/IToolSet.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVTaskObjectClass, SVTaskObjectClassGuid)
#pragma endregion Declarations

SVTaskObjectClass::SVTaskObjectClass(LPCSTR ObjectName)
: SVObjectAppClass(ObjectName) 
{
	LocalInitialize();
}

SVTaskObjectClass::SVTaskObjectClass(SVObjectClass* pOwner, int StringResourceID)
: SVObjectAppClass(pOwner, StringResourceID) 
{
	LocalInitialize();
}

HRESULT SVTaskObjectClass::LocalInitialize()
{
	HRESULT l_hrOk = S_OK;

	m_bUseOverlays = true;	// most objects use overlays; must change if needed in derived classes

	// SetObjectDepth has already been called in SVObjectClass Ctor
	// Register Embedded Objects
	RegisterEmbeddedObject(&m_isObjectValid, SVTaskObjectClassIsObjectValidGuid, IDS_OBJECTNAME_ISVALID, false, SVResetItemNone );
	RegisterEmbeddedObject(&m_statusTag, SVStatusObjectGuid, IDS_OBJECTNAME_STATUS, false, SVResetItemNone );
	RegisterEmbeddedObject(&m_statusColor, SVColorObjectGuid, IDS_OBJECTNAME_COLOR, false, SVResetItemNone );
	
	// Set Embedded defaults
	m_statusColor.SetDefaultValue((const DWORD&) SV_DEFAULT_INACTIVE_COLOR, true);
	m_statusTag.SetDefaultValue(false, true);
	m_isObjectValid.SetDefaultValue(false, true);
	
	// Add out Default Inputs and Outputs
	addDefaultInputObjects();

	m_lLastToolInputListIndex = -1;

	return l_hrOk;
}

SVTaskObjectClass::~SVTaskObjectClass()
{
	DestroyFriends();
	
	// empty the Embedded List
	for (int i = m_embeddedList.GetSize() - 1; i >= 0; i--)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
		if (pObject && pObject->IsCreated())
		{
			pObject->CloseObject();
		}
		m_embeddedList.RemoveAt(i);
	}
	CloseObject();
}

bool SVTaskObjectClass::resetAllObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = true;
	clearTaskMessages();
	m_isObjectValid.SetValue(1, true);

	// Notify friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (rfriend.PObject)
		{
			Result = rfriend.PObject->resetAllObjects(&m_ResetErrorMessages) && Result;
		}
	}

	Result = resetAllOutputListObjects(&m_ResetErrorMessages) && Result;

	Result = __super::resetAllObjects(&m_ResetErrorMessages) && Result;

	if (nullptr != pErrorMessages && !m_ResetErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_ResetErrorMessages.begin(), m_ResetErrorMessages.end());
	}
	m_isObjectValid.SetValue(1, Result);
	return Result;
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

	for( long i = 0; i < m_embeddedList.GetSize(); i++ )
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
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

	SvOi::addRootChildObjects(rOutputInfoList);

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
		GetInputInterface( m_svToolInputList, true );

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

HRESULT SVTaskObjectClass::SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject )
{
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		if (m_embeddedList.GetAt(i))
		{
			// check if it's this object
			GUID uniqueID = m_embeddedList.GetAt(i)->GetUniqueObjectID();
			if (rAimObjectID == uniqueID)
			{
				// Set the Object's Data Member Value
				if ( S_OK == m_embeddedList.GetAt(i)->SetObjectValue(pDataObject) )
				{
					return S_OK;
				}
			}
		}
	}

	return __super::SetValuesForAnObject(rAimObjectID, pDataObject);
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
				for( long j = 0; nullptr == rpObject && j < m_embeddedList.GetSize(); j++ )
				{
					SVObjectClass* l_pObject = m_embeddedList.GetAt(j);

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
				
				InsertItem.setName( ObjectRef.GetName().c_str() );
				InsertItem.setItemKey( ObjectRef->GetUniqueObjectID().ToVARIANT() );
				if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (ObjectRef.Object()) )
				{
					InsertItem.setItemTypeName( pValueObject->GetTypeName().c_str() );
				}
				else if( const SvOi::IBasicValueObject* pBasicObject = dynamic_cast<const SvOi::IBasicValueObject*> (ObjectRef.Object()) )
				{
					InsertItem.setItemTypeName( pBasicObject->getTypeName().c_str() );
				}

				if( ObjectRef->IsArray() )
				{
					if ( WholeArray && isAllowed(info->PObject, Attribute, -1) )
					{
						ObjectRef.SetEntireArray();
						UINT AttributesSet = ObjectRef.ObjectAttributesSet();
						InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName().c_str() );
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
							InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName().c_str() );
							InsertItem.setArrayIndex( i );
							InsertItem.setSelected( (AttributesSet & Attribute) == Attribute );
							pSelectorList->push_back( InsertItem );
						}
					}
				}
				else
				{
					UINT AttributesSet = ObjectRef.ObjectAttributesSet();
					InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName().c_str() );
					InsertItem.setSelected( (AttributesSet & Attribute) == Attribute );
					pSelectorList->push_back( InsertItem );
				}
			}
		});
	}
	else
	{
		assert(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
		::OutputDebugString(_T("SVTaskObjectClass::SelectorList - empty functor"));
#endif
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
			SVStringPair rel(item.first->GetCompleteObjectNameToObjectType(nullptr, nameToObjectType), 
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

void SVTaskObjectClass::GetInputs(SvUl::InputNameGuidPairList& rList, const SVObjectTypeInfoStruct& typeInfo, SVObjectTypeEnum objectTypeToInclude )
{
	SVInputInfoListClass toolInputList;
	// Try to get input interface...
	GetInputInterface(toolInputList, true );
	long lCount = toolInputList.GetSize();

	for( int i = 0; i < lCount; ++ i )
	{
		SVInObjectInfoStruct* pInputInfo = toolInputList[ i ];

		if( nullptr != pInputInfo && (SVNotSetObjectType == typeInfo.ObjectType || 
					( typeInfo.ObjectType == pInputInfo->GetInputObjectInfo().ObjectTypeInfo.ObjectType && 
					( SVNotSetSubObjectType == typeInfo.SubType || typeInfo.SubType == typeInfo.SubType == pInputInfo->GetInputObjectInfo().ObjectTypeInfo.SubType))) )
		{
			SvOi::IObjectClass* pObject = pInputInfo->GetInputObjectInfo().PObject;
			SVString name = "";
			SVGUID objectGUID = SV_GUID_NULL;
			if (pInputInfo->IsConnected() && nullptr != pObject)
			{
				SvOi::ISVImage* pImage = dynamic_cast <SvOi::ISVImage*>(pObject);
				if (nullptr != pImage)
				{
					name = pImage->getDisplayedName();
				}
				else
				{
					if (SVNotSetObjectType == objectTypeToInclude)
					{
						name = pObject->GetName();
					}
					else
					{
						name = pObject->GetObjectNameToObjectType(nullptr, objectTypeToInclude);
					}
				}
				objectGUID = pObject->GetUniqueObjectID();
			}
			rList.insert(std::make_pair(pInputInfo->GetInputName(), std::make_pair(name, objectGUID)));
		}
	}
}

SvStl::MessageContainerVector SVTaskObjectClass::validateAndSetEmmeddedValues(const SvOi::SetValuePairVector& valueVector, bool shouldSet)
{
	SvStl::MessageContainerVector messages;
	HRESULT Result(S_OK);
	for(SvOi::SetValuePairVector::const_iterator it = valueVector.begin(); valueVector.end() != it && S_OK == Result; ++it)
	{
		try
		{
			it->first->ValidateValue(it->second);
		}
		catch ( const SvStl::MessageContainer& rSvE )
		{
			messages.push_back(rSvE);
			Result = rSvE.getMessage().m_MessageCode;
		}
		
	}

	if (shouldSet)
	{
		for(SvOi::SetValuePairVector::const_iterator it = valueVector.begin(); valueVector.end() != it && S_OK == Result; ++it)
		{
			Result = it->first->SetValue(it->second);
			if (S_OK != Result)
			{
				SvStl::MessageMgrStd Msg( SvStl::LogOnly );
				SVStringVector msgList;
				SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (it->first);
				if( nullptr != pObject )
				{
					msgList.push_back( pObject->GetName() );
				}
				//! Check if general error or specific error for detailed information
				if( E_FAIL ==  Result || S_FALSE == Result )
				{
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SetEmbeddedValueFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				}
				else
				{
					Msg.setMessage( Result, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				}
				messages.push_back(Msg.getMessageContainer());
			}
		}
	}

	return messages;
}

void SVTaskObjectClass::ResolveDesiredInputs(const SvOi::SVInterfaceList& rDesiredInputs)
{
	// Get Input Interface...
	SVInputInfoListClass inputInterface;
	GetPrivateInputList( inputInterface );

	// Apply desired input interface...
	for( int i = 0; i < rDesiredInputs.size() && i < inputInterface.size(); ++i )
	{
		SVInObjectInfoStruct* pInInfo = inputInterface[ i ];

		const SVObjectTypeInfoStruct& rDesiredInType = rDesiredInputs[ i ];

		if( pInInfo )
		{
			pInInfo->SetInputObjectType( rDesiredInType );
		}

		const SVObjectTypeInfoStruct& info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

		assert ( SV_GUID_NULL != pInInfo->GetInputObjectInfo().UniqueObjectID ||
			SV_GUID_NULL != info.EmbeddedID || 
			SVNotSetObjectType != info.ObjectType ||
			SVNotSetSubObjectType != info.SubType );
	}
}

SvStl::MessageContainer SVTaskObjectClass::getFirstTaskMessage() const
{
	if( 0 < m_ResetErrorMessages.size() )
	{
		return m_ResetErrorMessages[0];
	}

	return SvStl::MessageContainer();
}
#pragma endregion virtual method (ITaskObject)

SVObjectClass* SVTaskObjectClass::OverwriteEmbeddedObject(const GUID& rUniqueID, const GUID& rEmbeddedID)
{
	// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		SVObjectClass* pEmbeddedObject = m_embeddedList.GetAt(i);
		if ( nullptr != pEmbeddedObject )
		{
			const GUID& ObjectID = pEmbeddedObject->GetEmbeddedID();

			if( ObjectID == rEmbeddedID)
			{
				return pEmbeddedObject->OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
			}
		}
	}
	return __super::OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
}

void SVTaskObjectClass::GetInputInterface(SVInputInfoListClass& rInputList, bool bAlsoFriends) const
{
	if (bAlsoFriends)
	{
		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			if( nullptr != m_friendList[i].PObject)
			{
				m_friendList[i].PObject->GetInputInterface(rInputList, bAlsoFriends);
			}
		}
	}

	// Local input list...
	SVInputInfoListClass localInputList;
	GetPrivateInputList(localInputList);
	rInputList.Append(localInputList);
}

void SVTaskObjectClass::DestroyFriend(SVObjectClass* pObject)
{
	// This was Added because SVTaskObjectClasses can also have Friends
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pObject);

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
	}
}

SvOi::IObjectClass* SVTaskObjectClass::getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	SvOi::IObjectClass* retObject = nullptr;
	if (useFriends)
	{
		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rfriend = m_friendList[i];
			if (nullptr != rfriend.PObject)
			{
				retObject = rfriend.PObject->getFirstObject(rObjectTypeInfo, true, pRequestor);
				if (nullptr != retObject)
				{
					return retObject;
				}
			}
		}
	}

	if (pRequestor && pRequestor == this || pRequestor == GetOwner())
	{
		// Do not reference self 
		return nullptr;
	}

	retObject = __super::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	if (nullptr != retObject)
	{
		return retObject;
	}

	for( long i = 0; i < m_embeddedList.GetSize(); i++ )
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);

		if( nullptr != pObject )
		{
			retObject = pObject->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
			if (nullptr != retObject)
			{
				return retObject;
			}
		}
	}
	return retObject;
}

void SVTaskObjectClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.PObject)
		{
			rfriend.PObject->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}

	for( long i = 0; i < m_embeddedList.GetSize(); i++ )
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);

		if( nullptr != pObject )
		{
			pObject->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}
}

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
		for( int i = 0; S_OK != l_hrOk && i < m_embeddedList.GetSize(); ++i )
		{
			l_psvObject = dynamic_cast<SVTaskObjectClass*>(m_embeddedList[i]);

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
	for( int i = 0; nullptr == l_psvObject && i < m_embeddedList.GetSize(); ++i )
	{
		l_psvObject = dynamic_cast<SVTaskObjectClass*>(m_embeddedList[i]);

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

// Must and will be called if ObjectID of this object has been changed.
// ( Refer to SVObjectManagerClass::ChangeUniqueObjectID() )
void SVTaskObjectClass::ResetPrivateInputInterface()
{
	for (int i = 0; i < m_inputInterfaceList.GetSize(); ++ i)
	{
		SVInObjectInfoStruct* pInInfo =	m_inputInterfaceList[i];
		if (pInInfo)
		{
			const SVObjectInfoStruct &l_rsvInfo = GetObjectInfo();

			SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID &&
			     SV_GUID_NULL == info.EmbeddedID && 
			     SVNotSetObjectType == info.ObjectType &&
			     SVNotSetSubObjectType == info.SubType )
			{
				//assert( false );
			}

			pInInfo->SetObject( l_rsvInfo );

			info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID &&
			     SV_GUID_NULL == info.EmbeddedID && 
			     SVNotSetObjectType == info.ObjectType &&
			     SVNotSetSubObjectType == info.SubType )
			{
				//assert( false );
			}
		}
	}
}

bool SVTaskObjectClass::ConnectAllInputs()
{
	bool Result( true );

	SVInputInfoListClass inputList;
	
	// Add the defaults
	addDefaultInputObjects(TRUE, &inputList);
	
	// tell friends to connect...
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID );
		if (nullptr != pObject)
		{
			pObject->ConnectAllInputs();
		}
	}

	// find our inputs
	for (int i = 0; i < inputList.GetSize(); ++ i)
	{
		SVInObjectInfoStruct* pInInfo = inputList.GetAt(i);
		if (pInInfo)
		{
			// Is not yet connected...
			if (!pInInfo->IsConnected())
			{
				// if Connect to Default
				if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID )
				{
					// Input Object is not set...Try to get one...
					SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;
					// At least one item from the SVObjectTypeInfoStruct is required, but not all
					if ( SV_GUID_NULL != info.EmbeddedID || SVNotSetObjectType != info.ObjectType || SVNotSetSubObjectType != info.SubType )
					{
						SVObjectClass* pOwner = GetOwner();
						SVObjectClass* pRequestor = pInInfo->PObject;
						const SvOi::IObjectClass* pObject( nullptr );
						BOOL bSuccess = false;
						
						if (hasToAskFriendForConnection(info, pOwner))
						{
							// Ask first friends...
							for (size_t j = 0; j < m_friendList.size(); ++ j)
							{
								pObject = SVObjectManagerClass::Instance().getFirstObject(m_friendList[j].UniqueObjectID, info);
								if (pObject)
								{
									// Connect input ...
									pInInfo->SetInputObject( pObject->GetUniqueObjectID() );
									bSuccess = true;
									break;
								}
							}
						}
						
						// Then ask owner...
						if (! bSuccess)
						{
							//GetInspection is still nullptr because in SVToolSetClass::createAllObjectsFromChild SetDefaultInputs is called before CreateObject !
							//To still get the appropriate inspection we call GetAncestor
							SvOi::IInspectionProcess* pInspection =  dynamic_cast< SvOi::IInspectionProcess* > (GetAncestor( SVInspectionObjectType ));
							while (pOwner)
							{
								pObject = nullptr;
								// if color system & pOwner == SVToolSetClass
								const SVObjectInfoStruct& ownerInfo = pOwner->GetObjectInfo();
								if (nullptr != pInspection && pInspection->IsColorCamera() && SVToolSetObjectType == ownerInfo.ObjectTypeInfo.ObjectType && SVImageObjectType == info.ObjectType )
								{
									SvOi::IToolSet* pToolSet( dynamic_cast<SvOi::IToolSet*> (pOwner) );
									if( nullptr != pToolSet)
									{
										pObject = pToolSet->getBand0Image();
									}
								}
								else
								{
									pObject = pOwner->getFirstObject(info, true, pRequestor);
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
					Result = SVObjectManagerClass::Instance().ConnectObjectInput(pInInfo->GetInputObjectInfo().UniqueObjectID, pInInfo);
				}
			}
		}
		else
		{
			Result = false;
		}
	}
	return Result;
}

bool SVTaskObjectClass::IsObjectValid() const
{ 
	return const_cast<SVTaskObjectClass*>(this)->IsValid() ? true : false; 
}

SvStl::MessageContainerVector SVTaskObjectClass::getErrorMessages() const
{ 
	SvStl::MessageContainerVector list = m_ResetErrorMessages;
	list.insert(list.end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	return list;
};

struct CompareInputName
{
	SVString m_Name;
	CompareInputName(const SVString& rName) : m_Name(rName) {}
	bool operator()(const SVInObjectInfoStruct* pInfo) const { return pInfo->GetInputName() == m_Name; }
};

HRESULT SVTaskObjectClass::ConnectToObject(const SVString& rInputName, const SVGUID& newGUID, SVObjectTypeEnum objectType)
{
	HRESULT hr = S_OK;

	SVInputInfoListClass toolInputList;
	GetInputInterface( toolInputList, true );
	// Find SVInObjectInfoStruct that has this name
	SVVector<SVInObjectInfoStruct*>::const_iterator it = std::find_if(toolInputList.begin(), toolInputList.end(), CompareInputName(rInputName));
	if (it != toolInputList.end())
	{
		SVObjectClass* pNewObject = SVObjectManagerClass::Instance().GetObject(newGUID);
		if (nullptr != pNewObject && (SVNotSetObjectType == objectType || pNewObject->GetObjectType() == objectType))
		{
			hr = ConnectToObject((*it), pNewObject);
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

HRESULT SVTaskObjectClass::ConnectToObject( SVInObjectInfoStruct* p_psvInputInfo, SVObjectClass* pNewObject )
{
	HRESULT l_svOk = S_OK;

	if( nullptr != p_psvInputInfo )
	{
		const GUID guidOldInputObjectID = p_psvInputInfo->GetInputObjectInfo().UniqueObjectID;
		SVObjectClass* pOldObject = dynamic_cast<SVObjectClass*>( SVObjectManagerClass::Instance().GetObject( guidOldInputObjectID ) );

		// Disconnect input info of input object...
		if( p_psvInputInfo->IsConnected() )
		{
			// Send to the Object we are using
			SVObjectManagerClass::Instance().DisconnectObjectInput( guidOldInputObjectID, p_psvInputInfo );
		}

		// Set new input...
		p_psvInputInfo->SetInputObject( pNewObject );

		if( nullptr != pNewObject )
		{
			// Connect input info to new input object...
			if( !pNewObject->ConnectObjectInput( p_psvInputInfo) )
			{
				// Unable to connect to new input object....
				SVStringVector msgList;
				if( pNewObject )
				{
					msgList.push_back(pNewObject->GetName());
				}
				else
				{
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_UnknownString));
				}
				// Should we really be doing this here?
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CriticalUnableToConnectTo, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10203 ); 

				// Try to recover old state...
				if( nullptr != pOldObject )
				{
					p_psvInputInfo->SetInputObject( pOldObject );
					pOldObject->ConnectObjectInput(p_psvInputInfo);			
				}

				l_svOk = S_FALSE;
			}
		}
		else
		{
			l_svOk = S_FALSE;
		}

		SVObjectClass* pTool = GetTool();
		if (nullptr != pTool)
		{
			pTool->ConnectAllInputs();
			pTool->resetAllObjects();
		}
	}
	else
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

BOOL SVTaskObjectClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	BOOL Result( true );
	
	if (!PCreateStruct)
	{
		return false;
	}
	
	Result = SVObjectAppClass::CreateObject(PCreateStruct);
	
	// Create our friends
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			Result = CreateChildObject(pFriend) && Result;

			assert( Result );
		}
		else
		{
			Result = false;
			assert( Result );
		}
	}

	// Create the embeddeds...
	// Save the owner and set the owner of our embeddeds to us!
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
		if (pObject)
		{
			Result = CreateChildObject(pObject) && Result;

			assert(Result);
		}
	}

	m_isObjectValid.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_statusTag.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_statusColor.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	
	m_isCreated = Result;
	
	return Result;	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsValid member function of class SVTaskObjectClass
// -----------------------------------------------------------------------------
// .Description : Returns the Validity state of this object
//				: must be overridden
////////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectClass::IsValid() const
{
	return IsErrorMessageEmpty();
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
	
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		if (m_embeddedList.GetAt(i))
		{
			m_embeddedList.GetAt(i)->SetObjectDepth(NewObjectDepth);
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
	
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		if (m_embeddedList.GetAt(i))
		{
			m_embeddedList.GetAt(i)->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
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

	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		if (m_embeddedList.GetAt(i))
		{
			m_embeddedList.GetAt(i)->SetImageDepth(lDepth);
		}
	}
	
	return SVObjectAppClass::SetImageDepth(lDepth);
}

void SVTaskObjectClass::addDefaultInputObjects(BOOL BCallBaseClass, SVInputInfoListClass* PInputListToFill)
{
	int l_iCount = m_inputInterfaceList.GetSize();
	int i( 0 );

	for (i = 0; i < l_iCount; i++)
	{
		SVInObjectInfoStruct* pInInfo = m_inputInterfaceList.GetAt(i);
		if (pInInfo)
		{
			SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

			if ( SV_GUID_NULL == pInInfo->GetInputObjectInfo().UniqueObjectID &&
			     SV_GUID_NULL == info.EmbeddedID && 
			     SVNotSetObjectType == info.ObjectType &&
			     SVNotSetSubObjectType == info.SubType )
			{
				//assert( false );
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

BOOL SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID)
{
	SVString Name = SvUl_SF::LoadSVString( StringResourceID );
	return RegisterEmbeddedObject( pEmbeddedObject, rGuidEmbeddedID, Name.c_str() );
}

BOOL SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, const GUID& p_rguidEmbeddedID, LPCTSTR newString)
{
	SVObjectClass *pObject = dynamic_cast<SVObjectClass *> (pEmbeddedObject);

	BOOL l_bOk = nullptr != pObject && RegisterEmbeddedObjectAsClass( pObject, p_rguidEmbeddedID, newString );

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
BOOL SVTaskObjectClass::RegisterEmbeddedObject( SVValueObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID, bool ResetAlways, SVResetItemEnum RequiredReset, LPCTSTR TypeName )
{
	SVString Name = SvUl_SF::LoadSVString( StringResourceID );
	return RegisterEmbeddedObject( pEmbeddedObject, rGuidEmbeddedID, Name.c_str(), ResetAlways, RequiredReset, TypeName);
}

BOOL SVTaskObjectClass::RegisterEmbeddedObject( SVValueObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR Name, bool ResetAlways, SVResetItemEnum RequiredReset, LPCTSTR TypeName )
{
	BOOL l_bOk = S_OK == pEmbeddedObject->SetResetOptions( ResetAlways, RequiredReset );

	if( l_bOk )
	{
		if( nullptr != TypeName )
		{
			pEmbeddedObject->SetTypeName( TypeName );
		}
		SVObjectClass* l_psvObject = dynamic_cast<SVObjectClass*>(pEmbeddedObject);
		l_bOk = nullptr != l_psvObject && RegisterEmbeddedObjectAsClass( l_psvObject, rGuidEmbeddedID, Name );
	}

	return l_bOk;
}

BOOL SVTaskObjectClass::RegisterEmbeddedObjectAsClass(SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR ObjectName)
{
	assert( nullptr != pEmbeddedObject);
	if( nullptr != pEmbeddedObject)
	{
		for (int i = 0; i < m_embeddedList.GetSize(); i++)
		{
			SVObjectClass* pObject = m_embeddedList.GetAt(i);
			if (pObject)
			{
				if (pObject->GetEmbeddedID() == rGuidEmbeddedID)
				{
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_DuplicateEmbeddedId, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10204 ); 
					return FALSE;
				}
			}
		}
		// Set object embedded to Setup the Embedded GUID
		pEmbeddedObject->SetObjectEmbedded( rGuidEmbeddedID, this, ObjectName );
		
		// Add to embedded object to List of Embedded Objects
		AddEmbeddedObject(pEmbeddedObject);
		
		return TRUE;
	}
	return FALSE;
}

bool SVTaskObjectClass::resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = true;

	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[ i ];

		// Check if Friend is alive...
		SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*> ( SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID ) );

		if( nullptr != pObject )
		{
			//return-value and error-messages do not be saved here, because this object will call resetAllOutputListObjects by its own and return error message to the parents.
			//this call here is important to reset (resize) the embedded images, so the parents can use it for its reset.
			pObject->resetAllOutputListObjects();
		}
	}

	// Try to send message to outputObjectList members
	for( long i = 0; i < m_embeddedList.GetSize(); i++ )
	{
		SVObjectClass* l_pObject = m_embeddedList.GetAt(i);

		if( nullptr != l_pObject )
		{
			//the error of this embedded objects must be saved by this object.
			Result = l_pObject->resetAllObjects( pErrorMessages) && Result;
		}
	}

	return Result;
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
	for( l_Iter = m_embeddedList.begin(); l_Iter != m_embeddedList.end(); ++l_Iter )
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
		if (m_inputInterfaceList.Add(PInObjectInfo) >= 0)
		{
			PInObjectInfo->SetInputName( p_rInputName );

			return TRUE;
		}

		PInObjectInfo->SetInputName( SVString() );
	}
	return FALSE;
}

void SVTaskObjectClass::GetPrivateInputList(SVInputInfoListClass& RInputInterface) const
{
	RInputInterface.Copy(m_inputInterfaceList);
}

// Set the valid flag to Invalid
// and all our embedded objects ???
void SVTaskObjectClass::SetInvalid()
{
	// Set all embeddeds ( outputs ) to invalid...
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
		if (pObject)
		{
			pObject->SetInvalid();
		}
	}
	
	// Set yourself to invalid...
	m_isObjectValid.SetValue(1, false);
}

// Override this function to implement object behavior...
void SVTaskObjectClass::SetDisabled()
{
	// Set all embeddeds ( outputs ) to disabled...
	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
		if (pObject)
		{
			pObject->SetDisabled();
		}
	}
	
	// Set yourself to disabled...
	
	// Special implementation here:
	// Set just your object color to disabled...
	m_statusColor.SetValue(1, SV_DEFAULT_DISABLED_COLOR);
	
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
	m_statusColor.GetValue(dwColor);
	return dwColor;
}

DWORD SVTaskObjectClass::GetObjectState() const
{
	DWORD dwState;
	m_statusTag.GetValue(dwState);
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
	if (m_embeddedList.GetSize())
	{
		rWriter.StartElement(scEmbeddedsTag);
		// Get embedded object script...
		for (int j = 0; j < m_embeddedList.GetSize(); j++)
		{
			m_embeddedList.GetAt(j)->Persist(rWriter);
		}
		rWriter.EndElement();
	}
}

BOOL SVTaskObjectClass::Run(SVRunStatusClass& RRunStatus)
{
	clearRunErrorMessages();

	// Run yourself...
	BOOL bRetVal = onRun(RRunStatus);
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	return bRetVal;
}

BOOL SVTaskObjectClass::onRun(SVRunStatusClass& RRunStatus)
{
	// Run first friends...
	BOOL bRetVal = runFriends(RRunStatus);
	
	// Now Validate yourself...
	if (bRetVal && IsErrorMessageEmpty())
	{
		m_isObjectValid.SetValue(1, true);
	}
	else
	{
		m_isObjectValid.SetValue(1, false);
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
				SVObjectManagerClass::Instance().DisconnectObjectInput( pInObjectInfo->GetInputObjectInfo().UniqueObjectID, pInObjectInfo );
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
			if ( SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast <SvOi::ITaskObjectListClass*> (GetOwner()) )
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
bool SVTaskObjectClass::DisconnectObjectInput(SVInObjectInfoStruct* pInObjectInfo)
{
	bool Result( false );

	if( nullptr != pInObjectInfo )
	{
		for( int i = 0; ! Result && i < m_inputInterfaceList.GetSize(); i++ )
		{
			SVInObjectInfoStruct* pInputObjectInfo = m_inputInterfaceList.GetAt(i);
			
			if (pInputObjectInfo)
			{
				if (pInObjectInfo->GetInputObjectInfo().UniqueObjectID == pInputObjectInfo->GetInputObjectInfo().UniqueObjectID)
				{
					pInputObjectInfo->SetInputObject( nullptr );
					
					Result = true;
				}
			}
		}

		for( int j = 0; j < m_embeddedList.GetSize(); ++j )
		{
			SVImageClass* l_pImage( dynamic_cast< SVImageClass* >( m_embeddedList[ j ] ) );

			if( nullptr != l_pImage )
			{
				if( pInObjectInfo->GetInputObjectInfo().UniqueObjectID == l_pImage->GetUniqueObjectID() )
				{
					l_pImage->UpdateImage( SV_GUID_NULL );
				}
			}
		}
	}

	if (!Result)
	{
		Result = __super::DisconnectObjectInput(pInObjectInfo);
	}
	return Result;
}

// Add embedded object pointer to the embedded List.
// Use this only for real embedded objects.
void SVTaskObjectClass::AddEmbeddedObject(SVObjectClass* PObject)
{
	assert(nullptr != PObject);
	
	// Add to Owner's List of Embedded Objects
	m_embeddedList.Add(PObject);
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVTaskObjectClass::RemoveEmbeddedObject(SVObjectClass* pObjectToRemove)
{
	// iterate and remove object if in embedded list.
	for (int i = m_embeddedList.GetSize() - 1; i >= 0; i--)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
		if (pObject)
		{
			if (pObject == pObjectToRemove)
			{
				if (pObject->IsCreated())
				{
					pObject->CloseObject();
				}
				m_embeddedList.RemoveAt(i);
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
			if ( SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast <SvOi::ITaskObjectListClass*> (GetOwner()) )
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
	// Close ourself first
	BOOL Result = SVObjectAppClass::CloseObject();
	
	// Close our Friends
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.UniqueObjectID);
		if (pFriend)
		{
			// Close Friend...
			Result = pFriend->CloseObject() && Result;
		}
	}
	
	// Close Embeddeds...
	for (int i = m_embeddedList.GetSize() - 1; i >= 0; i--)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
		if (pObject && pObject->IsCreated())
		{
			pObject->CloseObject();
		}
	}
	return Result;
}

HRESULT SVTaskObjectClass::GetDependentsList( SVObjectPairVector& rListOfDependents, bool bOnlyImageDependencies )
{
	assert( rListOfDependents.size() == 0 );	// should be clear before calling
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

			assert( nullptr != pOutput );	// if this asserts, the object is not on this object's output list
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
	assert( rListOfDependents.GetSize() == 0 );	// should be clear before calling
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

HRESULT SVTaskObjectClass::RegisterSubObject( SVImageClass* pImageObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pImageObject )
	{
		// What is the restriction here?
		const SVObjectInfoStruct& info = pImageObject->GetObjectInfo();
		if( SVCameraImageTemplateObjectType == info.ObjectTypeInfo.SubType )
		{
			m_svImageObjectSet.insert( pImageObject );
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

HRESULT SVTaskObjectClass::RegisterSubObject( SVObjectClass* pObject )
{
	HRESULT hr = S_FALSE;
	if (SVImageObjectType == pObject->GetObjectInfo().ObjectTypeInfo.ObjectType)
	{
		SVImageClass* pImage = dynamic_cast<SVImageClass*> (pObject);
		if (pImage)
		{
			hr = RegisterSubObject(pImage);
		}
	}
	else
	{
		SVValueObjectClass* pValue = dynamic_cast<SVValueObjectClass*>(pObject);
		if (pValue)
		{
			hr = RegisterSubObject(pValue);
		}
	}
	return hr;
}

HRESULT SVTaskObjectClass::UnregisterSubObject( SVObjectClass* pObject )
{
	HRESULT hr = S_FALSE;
	if (SVImageObjectType == pObject->GetObjectInfo().ObjectTypeInfo.ObjectType)
	{
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);
		if (pImage)
		{
			hr = UnregisterSubObject(pImage);
		}
	}
	else
	{
		SVValueObjectClass* pValue = dynamic_cast<SVValueObjectClass*>(pObject);
		if (pValue)
		{
			hr = UnregisterSubObject(pValue);
		}
	}
	return hr;
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
					SVObjectManagerClass::Instance().DisconnectObjectInput( pInObjectInfo->GetInputObjectInfo().UniqueObjectID, pInObjectInfo );
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
		if ( (iterEmbedded = std::find(m_embeddedList.begin(), m_embeddedList.end(), *iterList )) != m_embeddedList.end() )
		{
			SVObjectClass* pObject = *iterEmbedded;
			if (pObject && pObject->IsCreated())
			{
				pObject->CloseObject();
			}
			m_embeddedList.erase(iterEmbedded);
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
		SvUl::SetBits( (*iter)->ObjectAttributesSetRef(), SV_VIEWABLE , false );
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
		p_rMultiLine.m_StringPoint = l_Point;
		p_rMultiLine.m_csString = GetName();
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
				SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*> (GetTool());

				if( nullptr != pTaskObject )
				{
					l_Status = pTaskObject->UpdateOverlayColor( l_MultiLine );

					if( S_OK == l_Status )
					{
						l_Status = pTaskObject->GetDrawInfo( l_MultiLine );
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

	for (int i = 0; i < m_embeddedList.GetSize(); i++)
	{
		SVObjectClass* pObject = m_embeddedList.GetAt(i);
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

