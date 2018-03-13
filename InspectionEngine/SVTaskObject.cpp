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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVLibrary/SVTemplate.h"
#include "SVUtilityLibrary/SetBits.h"
#include "Definitions/Color.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "ObjectInterfaces/IRootObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "ObjectInterfaces/IToolSet.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVObjectLibrary/DependencyManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVNameVariantList.h"
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
, m_bSkipFirstFriend(false)
{
	LocalInitialize();
}

SVTaskObjectClass::SVTaskObjectClass(SVObjectClass* pOwner, int StringResourceID)
: SVObjectAppClass(pOwner, StringResourceID) 
, m_bSkipFirstFriend(false)
{
	LocalInitialize();
}

HRESULT SVTaskObjectClass::LocalInitialize()
{
	HRESULT l_hrOk = S_OK;

	m_bUseOverlays = true;	// most objects use overlays; must change if needed in derived classes

	// SetObjectDepth has already been called in SVObjectClass Ctor
	// Register Embedded Objects
	RegisterEmbeddedObject(&m_isObjectValid, SVTaskObjectClassIsObjectValidGuid, IDS_OBJECTNAME_ISVALID, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_statusTag, SVStatusObjectGuid, IDS_OBJECTNAME_STATUS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_statusColor, SVColorObjectGuid, IDS_OBJECTNAME_COLOR, false, SvOi::SVResetItemNone );
	
	// Set Embedded defaults
	m_statusColor.SetDefaultValue( static_cast<DWORD> (SvDef::DefaultInactiveColor), true);
	m_statusColor.setSaveValueFlag(false);
	m_statusTag.SetDefaultValue( DWORD(0), true);
	m_statusTag.setSaveValueFlag(false);
	m_isObjectValid.SetDefaultValue( BOOL(false), true);
	m_isObjectValid.setSaveValueFlag(false);
	
	// Add out Default Inputs and Outputs
	addDefaultInputObjects();

	m_lLastToolInputListIndex = -1;

	return l_hrOk;
}

SVTaskObjectClass::~SVTaskObjectClass()
{
	DestroyFriends();
	
	// empty the Embedded List
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject && pObject->IsCreated() )
		{
			pObject->CloseObject();
		}
	}
	m_embeddedList.clear();

	CloseObject();
}

bool SVTaskObjectClass::resetAllObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = true;
	clearTaskMessages();
	m_isObjectValid.SetValue(BOOL(true));

	// Notify friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if( nullptr != rfriend.getObject())
		{
			Result = rfriend.getObject()->resetAllObjects(&m_ResetErrorMessages) && Result;
		}
	}

	Result = resetAllOutputListObjects(&m_ResetErrorMessages) && Result;

	Result = __super::resetAllObjects(&m_ResetErrorMessages) && Result;

	if (nullptr != pErrorMessages && !m_ResetErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_ResetErrorMessages.begin(), m_ResetErrorMessages.end());
	}
	m_isObjectValid.SetValue(BOOL(Result));
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

		l_pObject = dynamic_cast< SVTaskObjectClass* >( SVObjectManagerClass::Instance().GetObject( rFriend.getUniqueObjectID() ) );

		if( nullptr != l_pObject )
		{
			HRESULT l_Temp = l_pObject->GetOutputList( p_rOutputInfoList );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}
		}
	}

	for( SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			p_rOutputInfoList.Add( &(pObject->GetObjectOutputInfo()) );
		}
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::GetOutputListFiltered(SVObjectReferenceVector& rObjectList, UINT uiAttributes, bool bAND )
{
	rObjectList.clear();
	SVOutputInfoListClass l_OutputList;

	GetOutputList( l_OutputList );

	int iSize = l_OutputList.GetSize();
	for ( int i = 0; i < iSize; ++i )
	{
		SVOutObjectInfoStruct* pInfoItem = l_OutputList.GetAt(i);
		if ( pInfoItem )
		{
			SVObjectReference ObjectRef = pInfoItem->GetObjectReference();
			if ( ObjectRef.getObject() )
			{
				if ( !ObjectRef.isArray() )
				{
					bool bAttributesOK = bAND ? (ObjectRef.ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
					                          : (ObjectRef.ObjectAttributesSet() & uiAttributes) > 0;            // OR
					if ( bAttributesOK )
					{
						rObjectList.push_back( ObjectRef );
					}
				}
				else
				{
					int iArraySize = ObjectRef.getValueObject()->getArraySize();
					for ( int i=0; i < iArraySize; i++ )
					{
						ObjectRef.SetArrayIndex(i);
						bool bAttributesOK = bAND ? (ObjectRef.ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
						                          : (ObjectRef.ObjectAttributesSet() & uiAttributes) > 0;            // OR
						if ( bAttributesOK )
						{
							rObjectList.push_back( ObjectRef );
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

HRESULT SVTaskObjectClass::IsAuxInputImage( const SvOl::SVInObjectInfoStruct* p_psvInfo )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::FindNextInputImageInfo(SvOl::SVInObjectInfoStruct*& p_rpsvFoundInfo, const SvOl::SVInObjectInfoStruct* p_psvLastInfo )
{
	HRESULT l_svOk = S_FALSE;

	long l_lCount = static_cast<long> (m_svToolInputList.size());

	bool l_bFoundLast = nullptr != p_psvLastInfo  && 0 < l_lCount &&
		0 <= m_lLastToolInputListIndex && m_lLastToolInputListIndex < l_lCount &&
		p_psvLastInfo != m_svToolInputList[ m_lLastToolInputListIndex ];

	if( ! l_bFoundLast )
	{
		l_bFoundLast = (nullptr == p_psvLastInfo);

		m_lLastToolInputListIndex = -1;

		m_svToolInputList.clear();

		// Try to get input interface...
		GetInputInterface( m_svToolInputList, true );

		l_lCount = static_cast<long> (m_svToolInputList.size());
	}

	p_rpsvFoundInfo = nullptr;

	for( int i = m_lLastToolInputListIndex + 1; nullptr == p_rpsvFoundInfo && i < l_lCount; ++ i )
	{
		SvOl::SVInObjectInfoStruct* l_psvInputInfo = m_svToolInputList[ i ];

		if( l_bFoundLast )
		{
			if( nullptr != l_psvInputInfo && 
				SvDef::SVImageObjectType == l_psvInputInfo->GetInputObjectInfo().m_ObjectTypeInfo.ObjectType  )
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

		m_svToolInputList.clear();
	}

	return l_svOk;
}

HRESULT SVTaskObjectClass::SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject )
{
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			// check if it's this object
			GUID uniqueID = pObject->GetUniqueObjectID();
			if( rAimObjectID == uniqueID )
			{
				// Set the Object's Data Member Value
				if ( S_OK == pObject->SetObjectValue( pDataObject ) )
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
				if( nullptr != rfriend.getObject())
				{
					l_Status = rfriend.getObject()->GetChildObject( rpObject, rNameInfo, Index + 1 );
				}
			}

			if( ( S_OK != l_Status ) && ( ( Index + 1 ) == ( rNameInfo.m_NameArray.size() - 1 ) ) )
			{
				for( SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); nullptr == rpObject && m_embeddedList.end() != Iter; ++Iter )
				{
					SVObjectClass* pObject = *Iter;
					if( nullptr != pObject )
					{
						l_Status = pObject->GetChildObject( rpObject, rNameInfo, Index + 1 );
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
SvCl::SelectorItemVectorPtr SVTaskObjectClass::GetSelectorList(SvOi::IsObjectInfoAllowed isAllowed, UINT Attribute, bool WholeArray) const
{
	SvCl::SelectorItemVectorPtr pResult{ new SvCl::SelectorItemVector() };

	if (isAllowed)
	{
		SVOutputInfoListClass OutputList;
		GetOutputList( OutputList );

		// Filter the list
		std::for_each(OutputList.begin(), OutputList.end(), [&pResult, &isAllowed, &Attribute, &WholeArray](SVOutputInfoListClass::value_type info)->void
		{
			SVObjectReference ObjectRef = info->GetObjectReference();
			if( ObjectRef.isArray() || isAllowed(info->getObject(), Attribute, -1) )
			{
				SvCl::SelectorItem InsertItem;
				
				InsertItem.m_Name = ObjectRef.GetName();
				InsertItem.m_ItemKey = ObjectRef.getObject()->GetUniqueObjectID().ToVARIANT();

				SvOi::IValueObject* pValueObject = ObjectRef.getValueObject();
				if ( nullptr != pValueObject )
				{
					InsertItem.m_ItemTypeName = pValueObject->getTypeName();
				}

				if( ObjectRef.isArray() )
				{
					if ( WholeArray && isAllowed(info->getObject(), Attribute, -1) )
					{
						ObjectRef.SetEntireArray();
						UINT AttributesSet = ObjectRef.ObjectAttributesSet();
						InsertItem.m_Location = ObjectRef.GetCompleteName(true);
						//! Array index -1 and array true means i is the whole array
						InsertItem.m_ArrayIndex = -1;
						InsertItem.m_Array = true;
						InsertItem.m_Selected = (AttributesSet & Attribute) == Attribute;
						pResult->push_back( InsertItem );
					}

					// add array elements
					int iArraySize = ObjectRef.getValueObject()->getArraySize();
					for ( int i = 0; i < iArraySize; i++ )
					{
						if( isAllowed(info->getObject(), Attribute, i) )
						{
							ObjectRef.SetArrayIndex( i );
							UINT AttributesSet = ObjectRef.ObjectAttributesSet();
							InsertItem.m_Location = ObjectRef.GetCompleteName(true);
							InsertItem.m_Array = true;
							InsertItem.m_ArrayIndex = i;
							InsertItem.m_Selected = (AttributesSet & Attribute) == Attribute;
							pResult->push_back( InsertItem );
						}
					}
				}
				else
				{
					UINT AttributesSet = ObjectRef.ObjectAttributesSet();
					InsertItem.m_Location = ObjectRef.GetCompleteName(true);
					InsertItem.m_Selected = (AttributesSet & Attribute) == Attribute;
					pResult->push_back( InsertItem );
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
	return pResult;
}

void SVTaskObjectClass::GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries)
{
	SvOl::SVInObjectInfoStruct* psvImageInfo(nullptr);
	SvOl::SVInObjectInfoStruct* psvLastImageInfo(nullptr);

	while (!psvImageInfo && S_OK == FindNextInputImageInfo(psvImageInfo, psvLastImageInfo))
	{
		if (psvImageInfo)
		{
			if (psvImageInfo->IsConnected())
			{
				SvOi::IObjectClass* pObject = psvImageInfo->GetInputObjectInfo().getObject();
				if (nullptr != pObject)
				{
					SvOi::ISVImage* pImage = dynamic_cast <SvOi::ISVImage*>(pObject);
					if (nullptr != pImage)
					{
						std::string name = pImage->getDisplayedName();
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

void SVTaskObjectClass::GetInputs(SvUl::InputNameGuidPairList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo, SvDef::SVObjectTypeEnum objectTypeToInclude )
{
	SvOl::SVInputInfoListClass toolInputList;
	// Try to get input interface...
	GetInputInterface(toolInputList, true );
	long lCount = static_cast<long> (toolInputList.size());

	for( int i = 0; i < lCount; ++ i )
	{
		SvOl::SVInObjectInfoStruct* pInputInfo = toolInputList[i];

		if( nullptr != pInputInfo && (SvDef::SVNotSetObjectType == typeInfo.ObjectType || 
					( typeInfo.ObjectType == pInputInfo->GetInputObjectInfo().m_ObjectTypeInfo.ObjectType && 
					( SvDef::SVNotSetSubObjectType == typeInfo.SubType || typeInfo.SubType == pInputInfo->GetInputObjectInfo().m_ObjectTypeInfo.SubType))) )
		{
			SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (pInputInfo->GetInputObjectInfo().getObject());
			std::string name = "";
			SVGUID objectGUID = GUID_NULL;
			if (pInputInfo->IsConnected() && nullptr != pObject)
			{
				SvOi::ISVImage* pImage = dynamic_cast <SvOi::ISVImage*>(pObject);
				if (nullptr != pImage)
				{
					name = pImage->getDisplayedName();
				}
				else
				{
					if (SvDef::SVNotSetObjectType == objectTypeToInclude)
					{
						name = pObject->GetName();
					}
					else
					{
						name = pObject->GetObjectNameToObjectType(objectTypeToInclude);
					}
				}
				objectGUID = pObject->GetUniqueObjectID();
			}
			rList.insert(std::make_pair(pInputInfo->GetInputName(), std::make_pair(name, objectGUID)));
		}
	}
}

SvStl::MessageContainerVector SVTaskObjectClass::validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet)
{
	SvStl::MessageContainerVector messages;
	HRESULT Result(S_OK);
	
	for(auto const& rEntry : rValueVector)
	{
		try
		{
			if (nullptr != rEntry.m_pValueObject)
			{
				rEntry.m_pValueObject->validateValue( rEntry.m_Value );
			}
		}
		catch ( const SvStl::MessageContainer& rSvE )
		{
			messages.push_back(rSvE);
			Result = rSvE.getMessage().m_MessageCode;
		}
		
	}

	if (shouldSet)
	{
		for (auto const& rEntry : rValueVector)
		{
			if (nullptr != rEntry.m_pValueObject)
			{
				Result = rEntry.m_pValueObject->setValue(rEntry.m_Value, rEntry.m_ArrayIndex);
			}
			if (S_OK != Result)
			{
				SvStl::MessageMgrStd Msg( SvStl::LogOnly );
				SvDef::StringVector msgList;
				SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (rEntry.m_pValueObject);
				if( nullptr != pObject )
				{
					msgList.push_back( pObject->GetName() );
				}
				//! Check if general error or specific error for detailed information
				if( E_FAIL ==  Result || S_FALSE == Result )
				{
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SetEmbeddedValueFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				}
				else
				{
					Msg.setMessage( Result, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				}
				messages.push_back(Msg.getMessageContainer());
				break;
			}
		}
	}

	return messages;
}

SvStl::MessageContainerVector SVTaskObjectClass::setEmbeddedDefaultValues(const SvOi::SetValueStructVector& rValueVector)
{
	SvStl::MessageContainerVector messages;
	HRESULT Result(S_OK);

	for (auto const& rEntry : rValueVector)
	{
		if (nullptr != rEntry.m_pValueObject)
		{
			Result = rEntry.m_pValueObject->setDefaultValue(rEntry.m_Value);
		}
		if (S_OK != Result)
		{
			SvStl::MessageMgrStd Msg(SvStl::LogOnly);
			SvDef::StringVector msgList;
			SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (rEntry.m_pValueObject);
			if (nullptr != pObject)
			{
				msgList.push_back(pObject->GetName());
			}
			//! Check if general error or specific error for detailed information
			if (E_FAIL == Result || S_FALSE == Result)
			{
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SetEmbeddedValueFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			}
			else
			{
				Msg.setMessage(Result, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			}
			messages.push_back(Msg.getMessageContainer());
			break;
		}
	}

	return messages;
}

void SVTaskObjectClass::ResolveDesiredInputs(const SvDef::SVObjectTypeInfoVector& rDesiredInputs)
{
	// Get Input Interface...
	SvOl::SVInputInfoListClass inputInterface;
	GetPrivateInputList( inputInterface );

	// Apply desired input interface...
	for( int i = 0; i < rDesiredInputs.size() && i < inputInterface.size(); ++i )
	{
		SvOl::SVInObjectInfoStruct* pInInfo = inputInterface[ i ];

		const SvDef::SVObjectTypeInfoStruct& rDesiredInType = rDesiredInputs[ i ];

		if( pInInfo )
		{
			pInInfo->SetInputObjectType( rDesiredInType );
		}

		const SvDef::SVObjectTypeInfoStruct& info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

		assert ( GUID_NULL != pInInfo->GetInputObjectInfo().getUniqueObjectID() ||
			GUID_NULL != info.EmbeddedID || 
			SvDef::SVNotSetObjectType != info.ObjectType ||
			SvDef::SVNotSetSubObjectType != info.SubType );
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

SVGuidVector SVTaskObjectClass::getEmbeddedList() const
{
	SVGuidVector Result;

	for (auto const& rEntry : m_embeddedList)
	{
		Result.push_back(rEntry->GetUniqueObjectID());
	}

	return Result;
}
#pragma endregion virtual method (ITaskObject)

SVObjectClass* SVTaskObjectClass::OverwriteEmbeddedObject(const GUID& rUniqueID, const GUID& rEmbeddedID)
{
	// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if ( nullptr != pObject )
		{
			const GUID& ObjectID = pObject->GetEmbeddedID();

			if( ObjectID == rEmbeddedID )
			{
				return pObject->OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
			}
		}
	}
	return __super::OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
}

void SVTaskObjectClass::GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const
{
	if (bAlsoFriends)
	{
		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			if( nullptr != m_friendList[i].getObject())
			{
				m_friendList[i].getObject()->GetInputInterface(rInputList, bAlsoFriends);
			}
		}
	}

	// Local input list...
	SvOl::SVInputInfoListClass localInputList;
	GetPrivateInputList(localInputList);
	for (auto rInputInfo : localInputList)
	{
		rInputList.push_back(rInputInfo);
	}
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

SvOi::IObjectClass* SVTaskObjectClass::getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	SvOi::IObjectClass* Result = nullptr;
	if (useFriends)
	{
		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rfriend = m_friendList[i];
			if (nullptr != rfriend.getObject())
			{
				Result = rfriend.getObject()->getFirstObject(rObjectTypeInfo, true, pRequestor);
				if (nullptr != Result)
				{
					return Result;
				}
			}
		}
	}

	if (pRequestor && pRequestor == this || pRequestor == GetParent())
	{
		// Do not reference self 
		return nullptr;
	}

	Result = __super::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	if (nullptr != Result)
	{
		return Result;
	}

	for( SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); nullptr == Result && m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;

		if( nullptr != pObject )
		{
			Result = pObject->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
		}
	}
	return Result;
}

void SVTaskObjectClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.getObject())
		{
			rfriend.getObject()->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}

	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;

		if( nullptr != pObject )
		{
			pObject->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}
}

bool SVTaskObjectClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	if(GUID_NULL != rImageGuid)
	{
		// Notify friends...
		for( size_t i = 0; !Result && i < m_friendList.size(); ++i )
		{
			const SVObjectInfoStruct &rFriend = m_friendList[i];

			if( nullptr != rFriend.getObject() && rFriend.getObject()->GetParent() == this )
			{
				Result = rFriend.getObject()->isInputImage(rImageGuid);
			}
		}

		// Notify embeddeds...
		for( SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); !Result && m_embeddedList.end() != Iter; ++Iter )
		{
			if( nullptr != *Iter)
			{
				Result = (*Iter)->isInputImage(rImageGuid);
			}
		}
	}

	return Result;
}

SVTaskObjectClass* SVTaskObjectClass::GetObjectAtPoint( const SVExtentPointStruct &rPoint )
{
	SVTaskObjectClass* pObject = nullptr;

	// Notify friends...
	for( size_t i = 0; nullptr == pObject && i < m_friendList.size(); ++i )
	{
		const SVObjectInfoStruct &l_rsvFriend = m_friendList[i];

		pObject = dynamic_cast<SVTaskObjectClass*>(l_rsvFriend.getObject());

		if( nullptr != pObject && pObject->GetParent() == this )
		{
			pObject = pObject->GetObjectAtPoint( rPoint );
		}
	}

	// Notify embeddeds...
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); nullptr == pObject && m_embeddedList.end() != Iter; ++Iter )
	{
		pObject = dynamic_cast<SVTaskObjectClass*> (*Iter);

		if( nullptr != pObject )
		{
			pObject = pObject->GetObjectAtPoint( rPoint );
		}
	}

	return pObject;
}

bool SVTaskObjectClass::DoesObjectHaveExtents() const
{
	return false;
}

HRESULT SVTaskObjectClass::GetImageExtent( SVImageExtentClass& rImageExtent )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtent( const SVImageExtentClass& rImageExtent )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtentToParent()
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtentToFit( const SVImageExtentClass& rImageExtent )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::GetPropertyInfo( SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
{
	return S_FALSE;
}

// Must and will be called if ObjectID of this object has been changed.
// ( Refer to SVObjectManagerClass::ChangeUniqueObjectID() )
void SVTaskObjectClass::ResetPrivateInputInterface()
{
	for (int i = 0; i < static_cast<int> (m_inputInterfaceList.size()); ++ i)
	{
		SvOl::SVInObjectInfoStruct* pInInfo =	m_inputInterfaceList[i];
		if (pInInfo)
		{
			const SVObjectInfoStruct &l_rsvInfo = GetObjectInfo();

			SvDef::SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

			if ( GUID_NULL == pInInfo->GetInputObjectInfo().getUniqueObjectID() &&
			     GUID_NULL == info.EmbeddedID && 
			     SvDef::SVNotSetObjectType == info.ObjectType &&
			     SvDef::SVNotSetSubObjectType == info.SubType )
			{
				//assert( false );
			}

			pInInfo->SetObject( l_rsvInfo );

			info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

			if ( GUID_NULL == pInInfo->GetInputObjectInfo().getUniqueObjectID() &&
			     GUID_NULL == info.EmbeddedID && 
			     SvDef::SVNotSetObjectType == info.ObjectType &&
			     SvDef::SVNotSetSubObjectType == info.SubType )
			{
				//assert( false );
			}
		}
	}
}

bool SVTaskObjectClass::ConnectAllInputs()
{
	bool Result( true );

	SvOl::SVInputInfoListClass inputList;
	
	// Add the defaults
	addDefaultInputObjects(&inputList);
	
	// tell friends to connect...
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rFriend.getUniqueObjectID() );
		if (nullptr != pObject)
		{
			pObject->ConnectAllInputs();
		}
	}

	// find our inputs
	for (auto pInputInfo : inputList)
	{
		if (nullptr != pInputInfo)
		{
			// Is not yet connected...
			if (!pInputInfo->IsConnected())
			{
				// if Connect to Default
				if ( GUID_NULL == pInputInfo->GetInputObjectInfo().getUniqueObjectID() )
				{
					// Input Object is not set...Try to get one...
					SvDef::SVObjectTypeInfoStruct info = pInputInfo->GetInputObjectInfo().m_ObjectTypeInfo;
					// At least one item from the SvDef::SVObjectTypeInfoStruct is required, but not all
					if ( GUID_NULL != info.EmbeddedID || SvDef::SVNotSetObjectType != info.ObjectType || SvDef::SVNotSetSubObjectType != info.SubType )
					{
						SVObjectClass* pOwner = GetParent();
						SVObjectClass* pRequestor = pInputInfo->getObject();
						const SvOi::IObjectClass* pObject( nullptr );
						bool bSuccess = false;
						
						if (hasToAskFriendForConnection(info, pOwner))
						{
							// Ask first friends...
							for (size_t j = 0; j < m_friendList.size(); ++ j)
							{
								pObject = SVObjectManagerClass::Instance().getFirstObject(m_friendList[j].getUniqueObjectID(), info);
								if (pObject)
								{
									// Connect input ...
									pInputInfo->SetInputObject( pObject->GetUniqueObjectID() );
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
							SvOi::IInspectionProcess* pInspection =  dynamic_cast< SvOi::IInspectionProcess* > (GetAncestor( SvDef::SVInspectionObjectType ));
							while (pOwner)
							{
								bool isColorInspection = (nullptr != pInspection) ? pInspection->IsColorCamera() : false;
								pObject = nullptr;
								// if color system & pOwner == SVToolSetClass
								const SVObjectInfoStruct& ownerInfo = pOwner->GetObjectInfo();
								if (isColorInspection && SvDef::SVToolSetObjectType == ownerInfo.m_ObjectTypeInfo.ObjectType && SvDef::SVImageObjectType == info.ObjectType && SvDef::SVImageMonoType == info.SubType )
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
									pInputInfo->SetInputObject( pObject->GetUniqueObjectID() );
									break;
								}
								else
								{
									pOwner = pOwner->GetParent();
									pRequestor = pRequestor->GetParent();
								}
							}// end while (pOwner)
						}// end if (! bSuccess)
					}
				}// end if (GUID_NULL == pInInfo->InputObjectInfo.UniqueObjectID )
				
				// Finally try to connect...
				if ( GUID_NULL != pInputInfo->GetInputObjectInfo().getUniqueObjectID() )
				{
					Result = SVObjectManagerClass::Instance().ConnectObjectInput(pInputInfo->GetInputObjectInfo().getUniqueObjectID(), pInputInfo);
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
	std::string m_Name;
	CompareInputName(const std::string& rName) : m_Name(rName) {}
	bool operator()(const SvOl::SVInObjectInfoStruct* pInfo) const { return pInfo->GetInputName() == m_Name; }
};

HRESULT SVTaskObjectClass::ConnectToObject(const std::string& rInputName, const SVGUID& newGUID, SvDef::SVObjectTypeEnum objectType)
{
	HRESULT hr = S_OK;

	SvOl::SVInputInfoListClass toolInputList;
	GetInputInterface( toolInputList, true );
	// Find SVInObjectInfoStruct that has this name
	SvOl::SVInputInfoListClass::const_iterator it = std::find_if(toolInputList.begin(), toolInputList.end(), CompareInputName(rInputName));
	if (it != toolInputList.end())
	{
		SVObjectClass* pNewObject = SVObjectManagerClass::Instance().GetObject(newGUID);
		if (nullptr != pNewObject && (SvDef::SVNotSetObjectType == objectType || pNewObject->GetObjectType() == objectType))
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

HRESULT SVTaskObjectClass::ConnectToObject(SvOl::SVInObjectInfoStruct* p_psvInputInfo, SVObjectClass* pNewObject )
{
	HRESULT l_svOk = S_OK;

	if( nullptr != p_psvInputInfo )
	{
		const GUID guidOldInputObjectID = p_psvInputInfo->GetInputObjectInfo().getUniqueObjectID();
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
				SvDef::StringVector msgList;
				if( pNewObject )
				{
					msgList.push_back(pNewObject->GetName());
				}
				else
				{
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_UnknownString));
				}
				// Should we really be doing this here?
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CriticalUnableToConnectTo, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10203 ); 

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

bool SVTaskObjectClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result = SVObjectAppClass::CreateObject(rCreateStructure);
	
	// Create our friends
	for (size_t j = 0; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
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
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			Result = CreateChildObject( pObject ) && Result;

			assert(Result);
		}
	}

	m_isObjectValid.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_statusTag.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_statusColor.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	
	m_isCreated = Result;
	
	return Result;	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsValid member function of class SVTaskObjectClass
// -----------------------------------------------------------------------------
// .Description : Returns the Validity state of this object
//				: must be overridden
////////////////////////////////////////////////////////////////////////////////
bool SVTaskObjectClass::IsValid() const
{
	return IsErrorMessageEmpty();
}

void SVTaskObjectClass::SetObjectDepth(int NewObjectDepth)
{
	// Set object depth of members here...
	
	// Set Depth of our Friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
		if (pFriend)
		{
			pFriend->SetObjectDepth(NewObjectDepth);
		}
	}
	
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			pObject->SetObjectDepth(NewObjectDepth);
		}
	}
	
	SVObjectAppClass::SetObjectDepth(NewObjectDepth);
}

void SVTaskObjectClass::SetObjectDepthWithIndex(int NewObjectDepth, int NewLastSetIndex)
{
	// Set object depth of members here...
	
	// Set Depth of our Friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
		if (pFriend)
		{
			pFriend->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
	}
	
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			pObject->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
	}
	
	SVObjectAppClass::SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
}

bool SVTaskObjectClass::SetImageDepth(long lDepth)
{
	// Set object depth of members here...
	
	// Set Depth of our Friends...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
		if (pFriend)
		{
			pFriend->SetImageDepth(lDepth);
		}
	}

	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			pObject->SetImageDepth(lDepth);
		}
	}
	
	return SVObjectAppClass::SetImageDepth(lDepth);
}

void SVTaskObjectClass::addDefaultInputObjects(SvOl::SVInputInfoListClass* PInputListToFill)
{
	int l_iCount = static_cast<int> (m_inputInterfaceList.size());
	int i( 0 );


	for (i = 0; i < l_iCount; i++)
	{
		SvOl::SVInObjectInfoStruct* pInInfo = m_inputInterfaceList[i];
		if (pInInfo)
		{
			SvDef::SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

			if ( GUID_NULL == pInInfo->GetInputObjectInfo().getUniqueObjectID() &&
			     GUID_NULL == info.EmbeddedID && 
			     SvDef::SVNotSetObjectType == info.ObjectType &&
			     SvDef::SVNotSetSubObjectType == info.SubType )
			{
				//assert( false );
			}

			if (PInputListToFill)
			{
				PInputListToFill->push_back(pInInfo);
			}
			else
			{
				m_InputObjectList.push_back(pInInfo);
			}
		}
	}
}

bool SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID)
{
	std::string Name = SvUl::LoadStdString( StringResourceID );
	return RegisterEmbeddedObject( pEmbeddedObject, rGuidEmbeddedID, Name.c_str() );
}

bool SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, const GUID& p_rguidEmbeddedID, LPCTSTR newString)
{
	bool l_bOk = nullptr != pEmbeddedObject && RegisterEmbeddedObjectAsClass( pEmbeddedObject, p_rguidEmbeddedID, newString );

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
bool SVTaskObjectClass::RegisterEmbeddedObject( SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID, bool ResetAlways, SvOi::SVResetItemEnum RequiredReset )
{
	std::string Name = SvUl::LoadStdString( StringResourceID );
	return RegisterEmbeddedObject( pEmbeddedObject, rGuidEmbeddedID, Name.c_str(), ResetAlways, RequiredReset);
}

bool SVTaskObjectClass::RegisterEmbeddedObject( SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR Name, bool ResetAlways, SvOi::SVResetItemEnum RequiredReset )
{
	bool Result(false);

	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pEmbeddedObject);
	if( nullptr != pValueObject )
	{
		pValueObject->setResetOptions( ResetAlways, RequiredReset );

		Result = RegisterEmbeddedObjectAsClass( pEmbeddedObject, rGuidEmbeddedID, Name );
	}

	return Result;
}

bool SVTaskObjectClass::RegisterEmbeddedObjectAsClass(SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR ObjectName)
{
	assert( nullptr != pEmbeddedObject);
	if( nullptr != pEmbeddedObject)
	{
		for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
		{
			SVObjectClass* pObject = *Iter;
			if( nullptr != pObject )
			{
				if( rGuidEmbeddedID == pObject->GetEmbeddedID() )
				{
					SvStl::MessageMgrStd Msg( SvStl::LogOnly );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_DuplicateEmbeddedId, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10204 ); 
					assert(false);
					return false;
				}
			}
		}
		// Set object embedded to Setup the Embedded GUID
		pEmbeddedObject->SetObjectEmbedded( rGuidEmbeddedID, this, ObjectName );
		
		// Add to embedded object to List of Embedded Objects
		AddEmbeddedObject(pEmbeddedObject);
		
		return true;
	}
	return false;
}

void SVTaskObjectClass::MovedEmbeddedObject(SVObjectClass* pToMoveObject, SVObjectClass* pPosObject)
{
	auto currentIter = std::find(m_embeddedList.begin(), m_embeddedList.end(), pToMoveObject);
	auto newPosIter = std::find(m_embeddedList.begin(), m_embeddedList.end(), pPosObject);
	if (currentIter != newPosIter && m_embeddedList.end() != currentIter && m_embeddedList.end() != newPosIter)
	{
		//change first object which is later in the list.
		if (currentIter > newPosIter)
		{   
			m_embeddedList.erase(currentIter);
			m_embeddedList.insert(newPosIter, pToMoveObject);
		}
		else
		{
			m_embeddedList.insert(newPosIter, pToMoveObject);
			m_embeddedList.erase(currentIter);
		}
	}
}

bool SVTaskObjectClass::resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = true;

	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[ i ];

		// Check if Friend is alive...
		SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*> ( SVObjectManagerClass::Instance().GetObject( rFriend.getUniqueObjectID()) );

		if( nullptr != pObject )
		{
			//return-value and error-messages do not be saved here, because this object will call resetAllOutputListObjects by its own and return error message to the parents.
			//this call here is important to reset (resize) the embedded images, so the parents can use it for its reset.
			pObject->resetAllOutputListObjects();
		}
	}

	// Try to send message to outputObjectList members
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;

		if( nullptr != pObject )
		{
			//the error of this embedded objects must be saved by this object.
			Result = pObject->resetAllObjects( pErrorMessages) && Result;
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
	SVObjectPtrDeque ObjectList = SVObjectAppClass::GetPostProcessObjects();

	for( SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			ObjectList.push_back( pObject ); 
		}
	}

	return ObjectList;
}

bool SVTaskObjectClass::RegisterInputObject(SvOl::SVInObjectInfoStruct* PInObjectInfo, const std::string& p_rInputName)
{
	if (PInObjectInfo)
	{
		if (m_inputInterfaceList.push_back(PInObjectInfo) >= 0)
		{
			PInObjectInfo->SetInputName( p_rInputName );

			return true;
		}

		PInObjectInfo->SetInputName( std::string() );
	}
	return false;
}

void SVTaskObjectClass::GetPrivateInputList(SvOl::SVInputInfoListClass& rInputInterface) const
{
	rInputInterface = m_inputInterfaceList;
}

// Set the valid flag to Invalid
void SVTaskObjectClass::SetInvalid()
{
	// Set yourself to invalid...
	m_isObjectValid.SetValue(BOOL(false));
}

// Override this function to implement object behavior...
void SVTaskObjectClass::SetDisabled()
{
	// Set all embeddeds ( outputs ) to disabled...
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject )
		{
			pObject->SetDisabled();
		}
	}
	
	// Set yourself to disabled...
	
	// Special implementation here:
	// Set just your object color to disabled...
	m_statusColor.SetValue(SvDef::DefaultDisabledColor);
	
	// Set friends to disabled...
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		if (rFriend.getObject())
		{
			rFriend.getObject()->SetDisabled();
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
void SVTaskObjectClass::GetInputObjects(SvOl::SVInputInfoListClass& RInputObjectList)
{
	// Add our Friends first
	
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			pTaskObjectFriend->GetInputObjects(RInputObjectList);
		}
	}
	
	int j( 0 );

	for (j = 0; j < static_cast<int> (m_InputObjectList.size()); ++ j)
	{
		if (nullptr != m_InputObjectList[j])
		{
			RInputObjectList.push_back(m_InputObjectList[j]);
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
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			pTaskObjectFriend->GetAllInputObjects();
		}
	}
}

void SVTaskObjectClass::Persist(SvOi::IObjectWriter& rWriter)
{
	SVObjectAppClass::Persist(rWriter);

	PersistFriends(rWriter);
	PersistEmbeddeds(rWriter);
	PersistInputs(rWriter);
}

void SVTaskObjectClass::PersistFriends(SvOi::IObjectWriter& rWriter)
{
	// Get script of close friends list members...
	if (m_friendList.size())
	{
		rWriter.StartElement(scFriendsTag);

		for (size_t i = 0; i < m_friendList.size(); ++ i)
		{
			const SVObjectInfoStruct& rFriend = m_friendList[i];
			// Check if Friend is alive...
			SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
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

void SVTaskObjectClass::PersistInputs(SvOi::IObjectWriter& rWriter)
{
	// Set up input list...
	SvOl::SVInputInfoListClass inputList;
	addDefaultInputObjects(&inputList);
	
	if (0 < inputList.size())
	{
		SVNameVariantList list;
		//SVVariantList list;
		for (int i = 0;i < static_cast<int> (inputList.size()); ++i)
		{
			if (nullptr != inputList[i])
			{
				_bstr_t name;
				_variant_t value;
				
				const SvOl::SVInObjectInfoStruct& rInputInfo = *inputList[i];
				value.SetString(rInputInfo.GetInputObjectInfo().GetObjectReference().GetGuidAndIndexOneBased().c_str());
				name = _bstr_t(rInputInfo.GetInputName().c_str());
				// hold name value pair...
				list.insert(std::make_pair(name, value));
				value.Clear();
			}
		}
		rWriter.StartElement(scInputsTag);
		if (list.size())
		{
			for (SVNameVariantList::const_iterator it = list.begin();it != list.end();++it)
			{
				rWriter.StartElement(scInputTag);
				rWriter.WriteAttribute(scNameTag, it->first);
				rWriter.WriteAttribute(scValueTag, it->second);
				rWriter.EndElement();
			}
		}
		rWriter.EndElement();
	}
}

void SVTaskObjectClass::PersistEmbeddeds(SvOi::IObjectWriter& rWriter)
{
	// Set up embedded object definitions...
	if( 0 < m_embeddedList.size() )
	{
		rWriter.StartElement(scEmbeddedsTag);
		// Get embedded object script...
		for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
		{
			SVObjectClass* pObject = *Iter;
			if( nullptr != pObject )
			{
				pObject->Persist(rWriter);
			}
		}
		rWriter.EndElement();
	}
}

bool SVTaskObjectClass::Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	clearRunErrorMessages();

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);
	
	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);
	
	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}
	
	return bRetVal;
}

bool SVTaskObjectClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	// Run first friends...
	bool bRetVal = runFriends(rRunStatus, pErrorMessages);
	
	// Now Validate yourself...
	if (bRetVal && IsErrorMessageEmpty())
	{
		m_isObjectValid.SetValue(BOOL(true));
	}
	else
	{
		m_isObjectValid.SetValue(BOOL(false));
		rRunStatus.SetInvalid();
		bRetVal = false;
	}
	
	return bRetVal;
}

bool SVTaskObjectClass::runFriends(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;
	
	// Run your friends
	size_t j = m_bSkipFirstFriend ? 1 : 0;
	for ( ; j < m_friendList.size(); ++ j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		if (SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(rFriend.getObject()))
		{
			bRetVal = pTaskObject->Run(rRunStatus, pErrorMessages) && bRetVal;
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
	for (int inIndex = static_cast<int> (m_InputObjectList.size() - 1); inIndex >= 0; inIndex--)
	{
		SvOl::SVInObjectInfoStruct* pInObjectInfo = m_InputObjectList[inIndex];
		if (pInObjectInfo)
		{
			if( pInObjectInfo->IsConnected() && GUID_NULL != pInObjectInfo->getUniqueObjectID())
			{
				// Send to the Object we are using
				SVObjectManagerClass::Instance().DisconnectObjectInput( pInObjectInfo->GetInputObjectInfo().getUniqueObjectID(), pInObjectInfo );
			}
		}
		// remove it from the list
	}
	m_InputObjectList.clear();

	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	// Notify all Objects dependent on our Outputs that 
	// the outputs are not available anymore
	for (int outIndex = l_OutputInfoList.GetSize() - 1; outIndex >= 0; outIndex--)
	{
		SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt(outIndex);
		if (pOutObjectInfo)
		{
			if ( SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast <SvOi::ITaskObjectListClass*> (GetParent()) )
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
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
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
bool SVTaskObjectClass::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pInObjectInfo)
{
	bool Result( false );

	if( nullptr != pInObjectInfo )
	{
		for( int i = 0; ! Result && i < static_cast<int> (m_inputInterfaceList.size()); i++ )
		{
			SvOl::SVInObjectInfoStruct* pInputObjectInfo = m_inputInterfaceList[i];
			
			if (pInputObjectInfo)
			{
				if (pInObjectInfo->GetInputObjectInfo().getUniqueObjectID() == pInputObjectInfo->GetInputObjectInfo().getUniqueObjectID())
				{
					pInputObjectInfo->SetInputObject( nullptr );
					
					Result = true;
				}
			}
		}

		for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
		{
			SVImageClass* pImage( dynamic_cast< SVImageClass* > (*Iter) );

			if( nullptr != pImage )
			{
				if( pInObjectInfo->GetInputObjectInfo().getUniqueObjectID() == pImage->GetUniqueObjectID() )
				{
					pImage->UpdateImage( GUID_NULL, pImage->GetImageInfo() );
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
void SVTaskObjectClass::AddEmbeddedObject(SVObjectClass* pObject )
{
	assert(nullptr != pObject);
	
	// Add to Owner's List of Embedded Objects
	m_embeddedList.push_back( pObject );
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVTaskObjectClass::RemoveEmbeddedObject(SVObjectClass* pObjectToRemove )
{
	if (nullptr != pObjectToRemove)
	{
		pObjectToRemove->SetObjectAttributesAllowed(SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
		// Reset any selection
		pObjectToRemove->SetObjectAttributesSet(0, SvOi::SetAttributeType::OverwriteAttribute);

		// Get this object's outputInfo
		SVOutObjectInfoStruct& rOutObjectInfo = pObjectToRemove->GetObjectOutputInfo();
		rOutObjectInfo.DisconnectAllInputs();
	}

	// iterate and remove object if in embedded list.
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject && pObject == pObjectToRemove )
		{
			m_embeddedList.erase( Iter );
			break;
		}
	}

	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	// if object is on output list, remove it there as well!
	for (int outIndex = l_OutputInfoList.GetSize() - 1; outIndex >= 0; outIndex--)
	{
		SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt(outIndex);
		if (pOutObjectInfo && pOutObjectInfo->getObject() == pObjectToRemove)
		{
			// remove from owner's list
			if ( SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast <SvOi::ITaskObjectListClass*> (GetParent()) )
			{
				pTaskObjectList->RemoveOutputObject( pOutObjectInfo );
			}
		}
	}
}

// Added to process friends
bool SVTaskObjectClass::CloseObject()
{
	// Close ourself first
	bool Result = SVObjectAppClass::CloseObject();
	
	// Close our Friends
	for (size_t i = 0; i < m_friendList.size(); ++ i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getUniqueObjectID());
		if (pFriend)
		{
			// Close Friend...
			Result = pFriend->CloseObject() && Result;
		}
	}
	
	// Close Embeddeds...
	for( SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject = *Iter;
		if( nullptr != pObject && pObject->IsCreated() )
		{
			pObject->CloseObject();
		}
	}
	return Result;
}

HRESULT SVTaskObjectClass::GetImageList( SVImageClassPtrVector& p_rImageList, UINT uiAttributes, bool bAND )
{
	HRESULT hr = S_OK;

    SvDef::SVObjectTypeInfoStruct  info;

	info.ObjectType = SvDef::SVImageObjectType;
	info.SubType = SvDef::SVNotSetSubObjectType;

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
				p_rImageList.push_back(pImage);
			}
		}
	}

	return hr;
}

HRESULT SVTaskObjectClass::RegisterSubObject( SVObjectClass* pObject )
{
	HRESULT Result( E_FAIL );
	SvOi::IValueObject* pValueObject(nullptr);

	if( nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)) )
	{
		m_ValueObjectSet.insert(pValueObject);
		Result = S_OK;
	}

	return Result;
}

HRESULT SVTaskObjectClass::UnregisterSubObject( SVObjectClass* pObject )
{
	HRESULT Result( E_FAIL );
	SvOi::IValueObject* pValueObject(nullptr);

	if( nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)) )
	{
		m_ValueObjectSet.erase( pValueObject );
		Result = S_OK;
	}

	return Result;
}

//namespace
//{
struct local
{
	struct IsInput
	{
		IsInput(const SVObjectClass* pInput) : m_pInput(pInput) {}
		bool operator () (const SvOl::SVInObjectInfoStruct* pInput) {return pInput->getObject() == m_pInput;}
		const SVObjectClass* m_pInput;
	};
	struct IsOutput
	{
		IsOutput(const SVObjectClass* pOutput) : m_pOutput(pOutput) {}
		bool operator () (const SVOutObjectInfoStruct* pOutput) {return pOutput->getObject() == m_pOutput;}
		const SVObjectClass* m_pOutput;
	};
};
//}

HRESULT SVTaskObjectClass::DisconnectInputsOutputs(SVObjectPtrVector& rListOfObjects)
{
	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	SVObjectPtrVector::const_iterator iterList;
	SvOl::SVInputInfoListClass::iterator iterInput;
	SVOutputInfoListClass::iterator iterOutput;
	SVObjectPtrVector::iterator iterEmbedded;

	// code borrowed from ::Disconnect and ::CloseObject
	for ( iterList = rListOfObjects.begin(); iterList != rListOfObjects.end(); ++iterList)
	{
		// check inputs
		if ( (iterInput = std::find_if(m_InputObjectList.begin(), m_InputObjectList.end(), local::IsInput(*iterList) )) != m_InputObjectList.end() )
		{
			SvOl::SVInObjectInfoStruct* pInObjectInfo = *iterInput;
			if (pInObjectInfo)
			{
				if( pInObjectInfo->IsConnected() && GUID_NULL != pInObjectInfo->getUniqueObjectID())
				{
					// Send to the Object we are using
					SVObjectManagerClass::Instance().DisconnectObjectInput( pInObjectInfo->GetInputObjectInfo().getUniqueObjectID(), pInObjectInfo );
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

HRESULT SVTaskObjectClass::HideInputsOutputs(SVObjectPtrVector& rListOfObjects)
{
	SVObjectPtrVector::iterator iter;
	for ( iter = rListOfObjects.begin(); iter != rListOfObjects.end(); ++iter )
	{
		(*iter)->SetObjectAttributesAllowed( SvDef::SV_VIEWABLE , SvOi::SetAttributeType::RemoveAttribute );
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

HRESULT SVTaskObjectClass::CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
{
	HRESULT hrRet = S_OK;
		
	if ( m_bUseOverlays )
	{
		hrRet = onCollectOverlays(p_Image, p_MultiLineArray);
	}

	for ( size_t j = 0; j < m_friendList.size(); j++ )
	{
		const SVObjectInfoStruct& l_rFriend = m_friendList[j];

		SVTaskObjectClass* pObject = dynamic_cast< SVTaskObjectClass* >( l_rFriend.getObject() );
		
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

HRESULT SVTaskObjectClass::onCollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
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

				p_MultiLineArray.push_back(l_MultiLine);
			}
		}
	}

	return l_Status;
}

SVObjectClass* SVTaskObjectClass::GetEmbeddedValueObject( GUID classguid )
{
	SVObjectClass* pResult = nullptr;

	for( SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter )
	{
		SVObjectClass* pObject =  *Iter;
		if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
		{
			if( pObject->GetEmbeddedID() == classguid )
			{
				pResult = pObject;
				break;
			}
		}
	}

	return pResult;
}

HRESULT SVTaskObjectClass::ResetObjectInputs()
{
	HRESULT l_Status = SVObjectAppClass::ResetObjectInputs();

	addDefaultInputObjects();

	return l_Status;
}

