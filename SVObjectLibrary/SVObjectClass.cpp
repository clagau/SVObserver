//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObject
//* .File Name       : $Workfile:   SVObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.10  $
//* .Check In Date   : $Date:   08 Jan 2015 05:41:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/Color.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVObjectAttributeClass.h"
#include "SVObjectLevelCreateStruct.h"
#include "SVObjectManagerClass.h"
#include "JoinType.h"
#include "SVToolsetScriptTags.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVObjectClass, GUID_NULL )

//This is the default constructor for this object.  This constructor initializes the name objects, clears all owner information, and calls the init method.
SVObjectClass::SVObjectClass()
{
	m_Name = m_ObjectName = _T("Base Class: SVResearch Base Object");
	m_resourceID = 0;
	SetObjectOwner( nullptr );

	init();
}

/*
This constructor initializes the name objects from the provided parameter, clears all owner information, and calls the init method.
*/
SVObjectClass::SVObjectClass( LPCTSTR ObjectName )
{
	m_Name = ObjectName;
	m_ObjectName = ObjectName;
	m_resourceID = 0;

	init();
}

/*
This constructor initializes the name objects from the provided parameter, sets the owner information from the provided parameter, and calls the init method.
*/
SVObjectClass::SVObjectClass( SVObjectClass* pOwner /* = nullptr */, int StringResourceID /* = IDS_CLASSNAME_SVOBJECT */ )
{
	m_ObjectName = SvUl::LoadStdString( StringResourceID );
	m_Name = m_ObjectName;
	m_resourceID = StringResourceID;
	SetObjectOwner( pOwner );

	init();
}

/*
This method initializes all the local attributes of the object and registers this object with the object manager.

The object depth gives you the possibility to generate an object member data table. That means, if you want to have arrays of member variables to keep access to earlier results, use the objectDepth as max size of these arrays. ( Sample: SVBoolValueObjectClass ! )

NOTE: Each class derived from this class have to manage its own object data table.

If this Object is valid, the validationReferenceID refers to the last object on which this object valid state is proofed.  It should normally refers to itself.

If this Object is NOT valid, the validationReferenceID refers to the object on that the proofing fails.
*/
void SVObjectClass::init()
{
	m_isCreated	  = false;
	m_objectDepth = 0;	// Standard Depth

	m_isObjectValid		  = false;
	m_embeddedID = GUID_NULL;

	// Set object Info...
	SVObjectManagerClass::Instance().CreateUniqueObjectID( this );

	m_outObjectInfo.SetObject(this);

	m_ObjectAttributesAllowed = SvDef::SV_NO_ATTRIBUTES; 
	m_ObjectAttributesSet.resize( 1 );
	m_ObjectAttributesSet[0]  = SvDef::SV_NO_ATTRIBUTES;
	m_DefaultObjectAttributesSet = SvDef::SV_NO_ATTRIBUTES;
}

/*
This method destroys all friends on the friends list owned by this object.  All other objects are disconnected from this object.
*/
void SVObjectClass::DestroyFriends()
{
	for( size_t i = 0; i < m_friendList.size(); ++i )
	{
		const SVObjectInfoStruct& rFriend = m_friendList[ i ];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject( rFriend.getUniqueObjectID() );
		if( pFriend )
		{
			SVObjectClass* pOwner = pFriend->GetParent();
			if( pOwner )
			{
				// Close, Disconnect and Delete Friend...
				pOwner->DestroyFriend(pFriend);
			}
			else
			{
				// Friend has no owner...
				pFriend->CloseObject();
				delete pFriend;
			}
		}
	}
	m_friendList.RemoveAll();
}

/*
This destructor unregisters itself from the object manager and frees all of the allocated names.
*/
SVObjectClass::~SVObjectClass()
{
	m_outObjectInfo.DisconnectAllInputs();

	SVObjectManagerClass::Instance().CloseUniqueObjectID( this );
}

bool SVObjectClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	m_isObjectValid = m_isCreated;
	if (!m_isObjectValid)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotCreated, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}

SVObjectClass::SVObjectPtrDeque SVObjectClass::GetPreProcessObjects() const
{
	SVObjectPtrDeque l_Objects;

	for( size_t i = 0; i < m_friendList.size(); i++ )
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];

		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject( rFriend.getUniqueObjectID() );
		if( pFriend )
		{
			l_Objects.push_back( pFriend );
		}
	}

	return l_Objects;
}

SVObjectClass::SVObjectPtrDeque SVObjectClass::GetPostProcessObjects() const
{
	SVObjectPtrDeque l_Objects;

	return l_Objects;
}

HRESULT SVObjectClass::ResetObjectInputs()
{
	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT SVObjectClass::RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type )
{
	HRESULT Result = S_OK;

	return Result;
}

// Connect Input from somebody else to my Output...( to me )
bool SVObjectClass::ConnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo )
{
	if( pObjectInInfo )
	{
		SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

		if( l_AutoLock.Assign( &m_outObjectInfo ) )
		{
			const SVGUID& rGuid = pObjectInInfo->getUniqueObjectID();
			SVObjectManagerClass::Instance().connectDependency( GetUniqueObjectID(), rGuid, SvOl::JoinType::Dependent );
			m_outObjectInfo.AddInput( *pObjectInInfo );

			return true;
		}
	}
	return false;
}

// Disconnect Input from somebody else from my Output...( from me )
bool SVObjectClass::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo )
{
	if( nullptr != pObjectInInfo )
	{
		SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

		if( l_AutoLock.Assign( &m_outObjectInfo ) )
		{
			const SVGUID& rGuid = pObjectInInfo->getUniqueObjectID();
			SVObjectManagerClass::Instance().disconnectDependency( GetUniqueObjectID(), rGuid, SvOl::JoinType::Dependent );
			m_outObjectInfo.RemoveInput( *pObjectInInfo );

			return true;
		}
	}
	return false;
}


/*
After the object construction, the object must be created using this function with an object level depending create structure.
*/
bool SVObjectClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result(false);

	if(rCreateStructure.OwnerObjectInfo.getObject() != this && rCreateStructure.OwnerObjectInfo.getUniqueObjectID() != GetUniqueObjectID() )
	{
		SetObjectOwner(rCreateStructure.OwnerObjectInfo.getObject());
		Result = true;
	}
	else
	{
		assert(false);
	}

	m_isCreated = Result;

	return Result;
}

void SVObjectClass::ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	if(rCreateStructure.OwnerObjectInfo.getObject() != this && rCreateStructure.OwnerObjectInfo.getUniqueObjectID() != GetUniqueObjectID() )
	{
		SetObjectOwner(rCreateStructure.OwnerObjectInfo.getObject());
	}
}

/*
This method executes the close object method on all objects that use this object that are not closed.
*/
bool SVObjectClass::CloseObject()
{
	m_isCreated = false;

	return true;
}

/*
This method is a placeholder for derived functionality.  This method performs not operation.
*/
void SVObjectClass::ResetPrivateInputInterface()
{
}

/*
Reset user changeable name to the NOT user changeable object instance name.
*/
void SVObjectClass::ResetName()
{
	// Resets the changeable names to the name given at construction time
	// or set with SVObjectClass::SetObjectName()
	m_Name = m_ObjectName;
}

#pragma region virtual method (IObjectClass)
/*
This method return the object name.  This name is changeable by the user.
*/
LPCTSTR SVObjectClass::GetName() const
{
	return m_Name.c_str();
}

std::string SVObjectClass::GetCompleteName() const
{
	std::string Result;

	if( nullptr != m_ownerObjectInfo.getObject() && m_ownerObjectInfo.getObject() != this )
	{
		Result = m_ownerObjectInfo.getObject()->GetCompleteName();
	}

	if( 0 < Result.size() )
	{
		Result += _T(".");
	}

	Result += GetName();

	return Result;
}

HRESULT SVObjectClass::GetCompleteNameToType(SvDef::SVObjectTypeEnum objectType, std::string& rName) const
{
	HRESULT hr = S_OK;

	if (GetObjectType() == objectType)
	{
		rName = GetCompleteName();
	}
	else
	{
		const SvOi::IObjectClass* pObject = GetAncestorInterface(objectType);
		if( nullptr != pObject )
		{
			rName = pObject->GetCompleteName();
		}
		else
		{
			hr = E_POINTER;
		}
	}
	return hr;
}

SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SvDef::SVObjectTypeEnum ancestorObjectType)
{
	if (ancestorObjectType == GetObjectType())
	{
		return this;
	}
	return GetAncestor(ancestorObjectType);
}

const SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SvDef::SVObjectTypeEnum ancestorObjectType) const
{
	if (GetObjectType() == ancestorObjectType)
	{
		return this;
	}
	return GetAncestor(ancestorObjectType);
}

SvDef::SVObjectSubTypeEnum SVObjectClass::GetObjectSubType() const
{
	return static_cast<SvDef::SVObjectSubTypeEnum>(m_outObjectInfo.m_ObjectTypeInfo.SubType);
}

bool SVObjectClass::is_Created() const 
{
	return IsCreated(); 
}

SvUl::NameGuidList SVObjectClass::GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const
{
	SvUl::NameGuidList list;

	return list;
}

void SVObjectClass::SetName( LPCTSTR Name )
{
	m_Name = Name;
}

SvOi::IObjectClass* SVObjectClass::getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	// check the owner of this class
	if( nullptr != pRequestor && (pRequestor == this || pRequestor == GetParent()) )
	{
		// Do not reference self or owner
		return nullptr;
	}

	// Find best match....EmbeddedID, Type, SubType...
	if(( GUID_NULL       == rObjectTypeInfo.EmbeddedID || rObjectTypeInfo.EmbeddedID == GetEmbeddedID() ) &&
		( SvDef::SVNotSetObjectType == rObjectTypeInfo.ObjectType || rObjectTypeInfo.ObjectType == GetObjectType() ) &&
		( SvDef::SVNotSetSubObjectType == rObjectTypeInfo.SubType || rObjectTypeInfo.SubType    == GetObjectSubType() )
		)
	{
		if( GUID_NULL         != rObjectTypeInfo.EmbeddedID ||
			SvDef::SVNotSetObjectType    != rObjectTypeInfo.ObjectType ||
			SvDef::SVNotSetSubObjectType != rObjectTypeInfo.SubType
			)
		{
			// But object must be specified!
			return const_cast<SVObjectClass*>(this);
		}
	}

	return nullptr;
}

void SVObjectClass::moveFriendObject(const SVGUID& objectToMoveId, const SVGUID& preObjectId)
{
	int currentPos = -1;
	int newPos = -1;
	for (int i = 0; i <= m_friendList.size(); i++)
	{
		if (m_friendList[i].getUniqueObjectID() == objectToMoveId)
		{
			currentPos = i;
		}
		if (m_friendList[i].getUniqueObjectID() == preObjectId)
		{
			newPos = i;
		}
	}

	if (0 <= currentPos && m_friendList.size() > currentPos)
	{
		auto object = m_friendList[currentPos];
		//change first object which is later in the list.
		if (currentPos > newPos)
		{
			m_friendList.RemoveAt(currentPos);
			m_friendList.Insert(newPos, object);
		}
		else
		{
			m_friendList.Insert(newPos, object);
			m_friendList.RemoveAt(currentPos);
		}
	}
}
#pragma endregion virtual method (IObjectClass)

/*
This method return the internal object name.  This name is not changeable by the user.
*/
LPCTSTR SVObjectClass::GetObjectName() const
{
	return m_ObjectName.c_str();
}

/*
Set the NOT user changeable object instance name.
*/
void SVObjectClass::SetObjectName( LPCTSTR ObjectName )
{
	m_ObjectName = ObjectName;
	ResetName();
}

/*
Set new object owner using owner pointer.
*/
bool SVObjectClass::SetObjectOwner( SVObjectClass* pNewOwner )
{
	// Check if object exists...
	if( nullptr != pNewOwner )
	{
		assert(pNewOwner != this); // can't own yourself...

		//First disconnect the previous owner
		SVGUID Guid = m_ownerObjectInfo.getUniqueObjectID();
		if(GUID_NULL !=Guid)
		{
			SVObjectManagerClass::Instance().disconnectDependency(Guid, GetUniqueObjectID(), SvOl::JoinType::Owner);
		}

		m_ownerObjectInfo.SetObject(pNewOwner);
		Guid = m_ownerObjectInfo.getUniqueObjectID();
		SVObjectManagerClass::Instance().connectDependency(Guid, GetUniqueObjectID(), SvOl::JoinType::Owner);
		return true;
	}
	return false;
}

/*
Set new object owner using owner GUID.  Uses TheSVObjectManager to get the owner pointer.
*/
bool SVObjectClass::SetObjectOwner( const GUID& rNewOwnerGUID )
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rNewOwnerGUID );

	return SetObjectOwner( pObject );
}

/*
Set embedded object info.  Use this only for real embedded objects.
*/
void SVObjectClass::SetObjectEmbedded( const GUID& rEmbeddedID, SVObjectClass* pOwner, LPCTSTR NewObjectName )
{
	m_embeddedID = rEmbeddedID;
	m_outObjectInfo.m_ObjectTypeInfo.EmbeddedID = rEmbeddedID;
	SetObjectName( NewObjectName );
	SetObjectOwner( pOwner );	
}

HRESULT SVObjectClass::GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const
{
	HRESULT Result( S_OK );

	if( _T("AttributesSet") == rValueName )
	{
		SVSAFEARRAY l_SafeArray;

		for( size_t i = 0; i < m_ObjectAttributesSet.size(); i++ )
		{
			_variant_t l_Value = m_ObjectAttributesSet.at(i);

			l_SafeArray.Add( l_Value );
		}

		rValue = l_SafeArray;
	}
	else
	{
		rValue.Clear();

		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectClass::SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject )
{
	// check if it is for us
	if ( GetUniqueObjectID() == rAimObjectID)
	{
		return SetObjectValue(pDataObject);
	}
	else
	{
		assert(false);
		return E_FAIL;
	} 
}

/*
Must override this in the derived class if you wish to set any values upon restoration from a script.
*/
HRESULT SVObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	bool bOk = false;

	if( nullptr != pDataObject )
	{
		SvCl::SVObjectDWordArrayClass svDWordArray;
		SvCl::SVObjectStdStringArrayClass StringArray;

		if ( ( bOk = pDataObject->GetAttributeData( _T("AttributesSet"), svDWordArray ) ) )
		{
			int iSize = static_cast<int> (svDWordArray.size());
			{
				m_ObjectAttributesSet.resize( iSize );
				for( int i = 0; i < iSize; i++ )
				{
					m_ObjectAttributesSet.at(i) = svDWordArray[i];
				}
			}
		}
		else if ((bOk = pDataObject->GetAttributeData(_T("AttributesAllowed"), svDWordArray)))
		{
			; // Do nothing as it's obsolete
		}
	}
	
	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

// Sets the valid flag to Invalid
void SVObjectClass::SetInvalid()
{
	m_isObjectValid = false;
}

// Override this function to implement object behavior...
void SVObjectClass::SetDisabled()
{
}

/*
This method is a placeholder for derived functionality.  This method performs no operation.
*/
SVObjectClass* SVObjectClass::UpdateObject( const GUID &rFriendGuid, SVObjectClass* pObject, SVObjectClass* pNewOwner )
{
	return nullptr;
}

/*
This method is used to add an object to the friends list via the object's unique object identifier.
*/
bool SVObjectClass::AddFriend( const GUID& rFriendGUID, const GUID& rAddPreGuid )
{
	size_t position = m_friendList.size();
	// Check GUID...
	if( GUID_NULL == rFriendGUID )
	{
		return false;
	}
	// Check if friend is already applied...
	if (m_friendList.size())
	{
		for( int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; -- i )
		{
			if( m_friendList[i].getUniqueObjectID() == rFriendGUID )
			{
				return false;
			}

			if( m_friendList[i].getUniqueObjectID() == rAddPreGuid)
			{
				position = i;
			}
		}
	}

	SVObjectInfoStruct newFriendInfo;
	// Check if Friend is alive...
	SVObjectClass* pNewFriend = SVObjectManagerClass::Instance().GetObject( rFriendGUID );
	if( pNewFriend )
	{
		// Check if we are the Owner
		// Note:: Special hack for friend scripting
		// if we are the owner - it's not from the script
		SVObjectClass* l_psvOwner = pNewFriend->GetParent();

		if ( l_psvOwner == this )
		{
			newFriendInfo.SetObject(pNewFriend);
		}
		else
		{
			if( nullptr != l_psvOwner )
			{
				SVObjectClass* l_psvNewObject = l_psvOwner->UpdateObject( rFriendGUID, pNewFriend, this );

				assert( nullptr != l_psvNewObject );

				newFriendInfo.SetObject(l_psvNewObject);
			}
		}
	}
	else
	{
		newFriendInfo.GetObjectReference().setGuid(rFriendGUID);
	}

	return ( m_friendList.Insert( position, newFriendInfo ) >= 0  );
}

SVObjectClass*  SVObjectClass::GetFriend( const SvDef::SVObjectTypeInfoStruct& rObjectType ) const 
{
	// Check if friend is already applied...
	for(int i =0; i < static_cast<int>(m_friendList.size()); i++ ) 
	{
		const SvDef::SVObjectTypeInfoStruct* pInfoStruct =  &(m_friendList[ i ].m_ObjectTypeInfo); 
		if( pInfoStruct->ObjectType == rObjectType.ObjectType && pInfoStruct->SubType == rObjectType.SubType)
		{
			if( GUID_NULL == pInfoStruct->EmbeddedID || pInfoStruct->EmbeddedID == rObjectType.EmbeddedID )
			{
				return 	m_friendList[i].getObject();
			}
		}
	}
	return nullptr;
}

/*
Set the object data depth. ( Data Table )  Should be overridden and must be called in derived classes...
*/
void SVObjectClass::SetObjectDepth( int NewObjectDepth )
{
	m_objectDepth = NewObjectDepth;
}

/*
Set the object data depth. ( Data Table )  Should be overridden and must be called in derived classes...
*/
void SVObjectClass::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	m_objectDepth = NewObjectDepth;
}

/*
This method returns Ancestor Object of specified Object Type of this Object, if any.  Otherwise it returns NULL.
*/
SVObjectClass* SVObjectClass::GetAncestor( SvDef::SVObjectTypeEnum AncestorObjectType ) const
{
	SVObjectClass* pOwner = this->GetParent();
	
	while( nullptr != pOwner )
	{
		if( pOwner->GetObjectType() == AncestorObjectType )
		{
			return pOwner;
		}
		pOwner = pOwner->GetParent();
	}
	return nullptr;
}

// Get the local object color...
DWORD SVObjectClass::GetObjectColor() const
{
	// Not defined here!
	return SvDef::DefaultInactiveColor;
}

// Get the local object state...
DWORD SVObjectClass::GetObjectState() const
{
	return 0x0000; // ERROR_INSPECTED_STATE;
}

/*
Get user changeable name length.
*/
int SVObjectClass::GetNameLength() const
{
	return static_cast<int> (m_Name.size());
}

/*
Get the length in Byte of the NOT user changeable object instance name.
*/
int SVObjectClass::GetObjectNameLength() const
{
	return static_cast<int> (m_ObjectName.size());
}

/*
Get the complete object name including selected SvDef::SVObjectTypeEnum value.
*/
std::string SVObjectClass::GetObjectNameToObjectType(SvDef::SVObjectTypeEnum objectTypeToInclude) const
{
	std::string Result;

	SvDef::SVObjectTypeEnum objectType = GetObjectType();
	if (objectType != objectTypeToInclude)
	{
		SVObjectClass* pObject = m_ownerObjectInfo.getObject();
		if (nullptr != pObject && pObject != this)
		{
			Result = pObject->GetObjectNameToObjectType(objectTypeToInclude);
		}
		}
	if (!Result.empty())
	{
		Result += _T(".");
	}
	Result += GetName();

	return Result;
}

/*
Get the complete object name before selected SvDef::SVObjectTypeEnum value.
*/
std::string SVObjectClass::GetObjectNameBeforeObjectType(SvDef::SVObjectTypeEnum objectTypeToInclude) const
{
	std::string Result;

	SvDef::SVObjectTypeEnum objectType = GetObjectType();
	if (objectType != objectTypeToInclude)
	{
		SVObjectClass* pObject = m_ownerObjectInfo.getObject();
		if (nullptr != pObject && pObject != this && pObject->GetObjectType() != objectTypeToInclude)
		{
			Result = pObject->GetObjectNameBeforeObjectType(objectTypeToInclude);
		}
	}
	if (!Result.empty())
	{
		Result += _T(".");
	}
	Result += GetName();

	return Result;
}

/*
This method returns a reference to the friends list attribute of this object.
*/
const SVObjectInfoArrayClass& SVObjectClass::GetFriendList() const
{
	return m_friendList;
}

bool SVObjectClass::createAllObjects(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if( !IsCreated() && !CreateObject( rCreateStructure ) )
	{
		assert( false );

		SvDef::StringVector msgList;
		msgList.push_back( GetName() );
		msgList.push_back( GetCompleteName() );
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreationOf2Failed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10209 );

		return false;
	}

	return true;
}

SVObjectClass* SVObjectClass::OverwriteEmbeddedObject(const GUID& rUniqueID, const GUID& rEmbeddedID)
{
	if( GetEmbeddedID() == rEmbeddedID )
	{
		SVObjectManagerClass::Instance().ChangeUniqueObjectID( this, rUniqueID );
		return this;
	}
	return nullptr;
}

HRESULT SVObjectClass::RemoveObjectConnection( const GUID& rObjectID )
{
	return S_OK;
}

void SVObjectClass::Persist( SvOi::IObjectWriter& rWriter )
{
	_variant_t value; 
	value.SetString(GetName()); // use user editable name for Data Element ObjectName attribute
	rWriter.WriteAttribute( scObjectNameTag, value );
	value.Clear();

	value.SetString(GetClassID().ToString().c_str());
	rWriter.WriteAttribute( scClassIDTag, value );
	value.Clear();

	std::string UniqueID{ m_outObjectInfo.getUniqueObjectID().ToString() };
	value.SetString(UniqueID.c_str());
	rWriter.WriteAttribute( scUniqueReferenceIDTag, value );
	value.Clear();

	// Set up object definition...
	if( GUID_NULL != GetEmbeddedID() )
	{
		value.SetString(GetEmbeddedID().ToString().c_str());
		rWriter.WriteAttribute( scEmbeddedIDTag, value );
		value.Clear();
	}
}

HRESULT SVObjectClass::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	HRESULT l_Status = S_OK;

	if( Index == ( rNameInfo.m_NameArray.size() - 1 ) )
	{
		if( rNameInfo.m_NameArray[Index] == GetName() )
		{
			rpObject = const_cast< SVObjectClass* >( this );
		}
		else
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

/*
This method returns the allowed attributes of this object.
*/
UINT SVObjectClass::ObjectAttributesAllowed() const
{
	return m_ObjectAttributesAllowed;
}

/*
This method returns a reference to the allowed attributes of this object.
*/
UINT SVObjectClass::SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type )
{
	switch( Type )
	{
	case SvOi::SetAttributeType::AddAttribute:
		m_ObjectAttributesAllowed |= Attributes;
		break;
	case SvOi::SetAttributeType::RemoveAttribute:
		m_ObjectAttributesAllowed &= ~Attributes;
		break;
	case SvOi::SetAttributeType::OverwriteAttribute:
		m_ObjectAttributesAllowed = Attributes;
		break;
	}
	return m_ObjectAttributesAllowed;
}

/*
This method returns the set attributes of this object.
*/
UINT SVObjectClass::ObjectAttributesSet(int iIndex) const
{
	if (static_cast<int> (m_ObjectAttributesSet.size()) > iIndex)
	{
		return m_ObjectAttributesSet.at(iIndex);
	}
	else
	{
		assert(false);
		return 0;
	}
}

/*
This method sets attributes of this object.
*/
UINT SVObjectClass::SetObjectAttributesSet(UINT Attributes, SvOi::SetAttributeType Type, int Index)
{
	if (static_cast<int> (m_ObjectAttributesSet.size()) > Index)
	{

		UINT& rAttributesSet = m_ObjectAttributesSet.at(Index);
		switch (Type)
		{
		case SvOi::SetAttributeType::AddAttribute:
			rAttributesSet |= Attributes;
			break;
		case SvOi::SetAttributeType::RemoveAttribute:
			rAttributesSet &= ~Attributes;
			break;
		case SvOi::SetAttributeType::OverwriteAttribute:
			rAttributesSet = Attributes;
			break;
		}
		return rAttributesSet;
	}
	else
	{
		assert(false);
		return 0;
	}
}

/*
This method sets the default attributes of this object.
*/
void SVObjectClass::SetDefaultObjectAttributesSet(UINT uAttributes)
{
	m_DefaultObjectAttributesSet = uAttributes;
}

/*
This method must be overridden if the derived class desires this functionality
*/
HRESULT SVObjectClass::RegisterSubObject( SVObjectClass* pObject )
{
	return S_FALSE;
}

/*
This method must be overridden if the derived class desires this functionality
*/
HRESULT SVObjectClass::UnregisterSubObject( SVObjectClass* pObject )
{
	return S_FALSE;
}