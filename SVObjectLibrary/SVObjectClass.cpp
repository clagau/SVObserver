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
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVObjectAttributeClass.h"
#include "SVObjectLevelCreateStruct.h"
#include "SVObjectManagerClass.h"
#include "JoinType.h"
#include "SVToolsetScriptTags.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVObjectClass, SV_GUID_NULL )

//This is the default constructor for this object.  This constructor initializes the name objects, clears all owner information, and calls the init method.
SVObjectClass::SVObjectClass()
{
	m_Name = m_ObjectName = _T( "Base Class: SVResearch Base Object" );
	m_resourceID = 0;
	SetObjectOwner( nullptr );

	init();
}

/*
This constructor initializes the name objects from the provided parameter, clears all owner information, and calls the init method.
*/
SVObjectClass::SVObjectClass( LPCSTR ObjectName )
{
	m_Name = ObjectName;
	m_ObjectName	= ObjectName;
	m_resourceID = 0;

	init();
}

/*
This constructor initializes the name objects from the provided parameter, sets the owner information from the provided parameter, and calls the init method.
*/
SVObjectClass::SVObjectClass( SVObjectClass* pOwner /* = nullptr */, int StringResourceID /* = IDS_CLASSNAME_SVOBJECT */ )
{
	m_ObjectName.LoadString( StringResourceID );
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
	m_lImageDepth = 0;
	m_objectDepth = 0;	// Standard Depth

	m_isObjectValid		  = false;
	m_validationReferenceID = SV_GUID_NULL;
	m_embeddedID = SV_GUID_NULL;

	// Set object Info...
	SVObjectManagerClass::Instance().CreateUniqueObjectID( this );

	m_outObjectInfo.PObject = this;

	m_ObjectAttributesAllowed = SV_NO_ATTRIBUTES; 
	m_ObjectAttributesSet.resize( 1 );
	m_ObjectAttributesSet[0]  = SV_NO_ATTRIBUTES;
	m_DefaultObjectAttributesSet = SV_NO_ATTRIBUTES;
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
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID );
		if( pFriend )
		{
			SVObjectClass* pOwner = pFriend->GetOwner();
			if( pOwner )
			{
				// Close, Disconnect and Delete Friend...
				::SVSendMessage( pOwner, SVM_DESTROY_FRIEND_OBJECT, reinterpret_cast<DWORD_PTR>(pFriend), 0 );
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

SVObjectClass::SVObjectPtrDeque SVObjectClass::GetPreProcessObjects() const
{
	SVObjectPtrDeque l_Objects;

	for( size_t i = 0; i < m_friendList.size(); i++ )
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];

		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID );
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

/*
This method is a placeholder for the object reset functionality.  This method will be overridden by derived classes.
*/
HRESULT SVObjectClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	return l_hrOk;
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
BOOL SVObjectClass::ConnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if( pObjectInInfo )
	{
		SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

		if( l_AutoLock.Assign( &m_outObjectInfo ) )
		{
			SVObjectManagerClass::Instance().connectDependency( GetUniqueObjectID(), pObjectInInfo->UniqueObjectID, SvOl::JoinType::Dependent );
			m_outObjectInfo.AddInput( *pObjectInInfo );

			return true;
		}
	}
	return false;
}

// Disconnect Input from somebody else from my Output...( from me )
BOOL SVObjectClass::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if( nullptr != pObjectInInfo )
	{
		SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

		if( l_AutoLock.Assign( &m_outObjectInfo ) )
		{
			SVObjectManagerClass::Instance().disconnectDependency( GetUniqueObjectID(), pObjectInInfo->UniqueObjectID, SvOl::JoinType::Dependent );
			m_outObjectInfo.RemoveInput( *pObjectInInfo );

			return true;
		}
	}
	return false;
}


/*
After the object construction, the object must be created using this function with an object level depending create structure.
*/
BOOL SVObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = nullptr != pCreateStructure;

	if( l_bOk )
	{
		if( pCreateStructure->OwnerObjectInfo.PObject != this && pCreateStructure->OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
		{
			SetObjectOwner( pCreateStructure->OwnerObjectInfo.PObject );
		}
		else
		{
			ASSERT(false);
		}
	}

	m_isCreated = l_bOk;

	return l_bOk;
}

HRESULT SVObjectClass::ConnectObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pCreateStructure )
	{
		if( pCreateStructure->OwnerObjectInfo.PObject != this && pCreateStructure->OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
		{
			SetObjectOwner( pCreateStructure->OwnerObjectInfo.PObject );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

/*
This method executes the close object method on all objects that use this object that are not closed.
*/
BOOL SVObjectClass::CloseObject()
{
	DWORD_PTR dwResult = SVMR_NOT_PROCESSED;

	SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

	long l_lCount = static_cast<long>(m_outObjectInfo.GetInputSize());

	if( 0 < l_lCount && l_AutoLock.Assign( &m_outObjectInfo ) )
	{
		for( int i = 0; i < l_lCount; ++ i )
		{
			SVInObjectInfoStruct& rUserInInfo = m_outObjectInfo.GetInputAt( i );
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rUserInInfo.UniqueObjectID );
			if( pObject && pObject->IsCreated() )
			{
				// Close only user of our output which are still not closed!
				dwResult = ::SVSendMessage( pObject, SVM_CLOSE_OBJECT, reinterpret_cast<DWORD_PTR> (static_cast<SVObjectClass*> (this)), 0 ) | dwResult;
			}
		}
	}

	if( dwResult != SVMR_NO_SUCCESS )
	{
		m_isCreated = false;
		return true;
	}

	return false;
}

/*
This method return the validity state of this object.
*/
BOOL SVObjectClass::IsObjectValid( GUID& rValidationReferenceID )
{
	rValidationReferenceID = m_validationReferenceID;
	return m_isObjectValid;
}

/*
This method returns whether this object is a descendant of PAncestorObject.
*/
BOOL SVObjectClass::IsDescendantOf( SVObjectClass* pAncestorObject )
{
	BOOL l_Status = true;
	
	l_Status = l_Status && ( nullptr != pAncestorObject );
	l_Status = l_Status && ( pAncestorObject == this );

	if( !l_Status )
	{
		SVObjectClass* l_pOwner = GetOwner();

		if( nullptr != l_pOwner && l_pOwner != this )
		{
			l_Status = l_pOwner->IsDescendantOf( pAncestorObject );
		}
	}
	return l_Status;
}

/*
This method returns whether this object is a descendant of type (rAncestorInfo.ObjectTypeInfo).
*/
BOOL SVObjectClass::IsDescendantOfType( const SVObjectInfoStruct& rAncestorInfo )
{
	BOOL l_Status = true;

	const SVObjectInfoStruct& rInfo = GetObjectInfo();

	l_Status = l_Status && ( rAncestorInfo.ObjectTypeInfo.ObjectType == rInfo.ObjectTypeInfo.ObjectType );
	l_Status = l_Status && ( rAncestorInfo.ObjectTypeInfo.SubType == rInfo.ObjectTypeInfo.SubType );

	if( !l_Status )
	{
		SVObjectClass* l_pOwner = GetOwner();

		if( nullptr != l_pOwner && l_pOwner != this )
		{
			l_Status = l_pOwner->IsDescendantOfType( rAncestorInfo );
		}
	}
	return l_Status;
}

/*
This method is a placeholder for derived functionality.  This method performs not operation.
*/
void SVObjectClass::ResetPrivateInputInterface()
{
}

/*
This method sets the new object owner using owner pointer instead of using TheSVObjectManager to get information about the new owner, it validates the given object pointer and gets the GUID from the new owner itself.

This method will return an error state if owner not exists or not a valid object.
*/
BOOL SVObjectClass::InitObject( SVObjectClass* pObject )
{
	if( nullptr != pObject )
	{
		m_ownerObjectInfo.PObject			= pObject;
		m_ownerObjectInfo.UniqueObjectID	= pObject->GetUniqueObjectID();
		return true;
	}
	return false;
}

/*
Routing Version of Validate.  Validates Local Scope and all owned objects. Here only for polymorphism.
*/
BOOL SVObjectClass::Validate()
{
	BOOL l_bOk = OnValidate();

	ASSERT( l_bOk );

	return l_bOk;
}

/*
Non Routing Version of Validate. Validates only Local Scope.
*/
BOOL SVObjectClass::OnValidate()
{
	return ( m_isObjectValid = m_isCreated );
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
	return static_cast<LPCTSTR> (m_Name);
}

SVString SVObjectClass::GetCompleteName() const 
{
	CString name = GetCompleteObjectName();
	return SVString( static_cast<LPCTSTR> (name) );
}

HRESULT SVObjectClass::GetCompleteNameToType(SVObjectTypeEnum objectType, SVString& rName) const
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

const SVGUID& SVObjectClass::GetParentID() const
{
	return GetOwnerID();
}

SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SVObjectTypeEnum ancestorObjectType)
{
	if (ancestorObjectType == GetObjectType())
	{
		return this;
	}
	return GetAncestor(ancestorObjectType);
}

const SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) const
{
	if (GetObjectType() == ancestorObjectType)
	{
		return this;
	}
	return GetAncestor(ancestorObjectType);
}

SVObjectSubTypeEnum SVObjectClass::GetObjectSubType() const
{
	return static_cast<SVObjectSubTypeEnum>(m_outObjectInfo.ObjectTypeInfo.SubType);
}

bool SVObjectClass::is_Created() const 
{
	return IsCreated() ? true : false; 
}

SvUl::NameGuidList SVObjectClass::GetCreatableObjects(const SVObjectTypeInfoStruct& rObjectTypeInfo) const
{
	SvUl::NameGuidList list;

	return list;
}

void SVObjectClass::SetName( LPCTSTR Name )
{
	m_Name = Name;
}
#pragma endregion virtual method (IObjectClass)

/*
This method return the internal object name.  This name is not changeable by the user.
*/
LPCTSTR SVObjectClass::GetObjectName() const
{
	return m_ObjectName;
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
BOOL SVObjectClass::SetObjectOwner( SVObjectClass* pNewOwner )
{
	// Check if object exists...
	if( nullptr != pNewOwner )
	{
		ASSERT(pNewOwner != this); // can't own yourself...

		//First disconnect the previous owner
		if( SV_GUID_NULL != m_ownerObjectInfo.UniqueObjectID )
		{
			SVObjectManagerClass::Instance().disconnectDependency( m_ownerObjectInfo.UniqueObjectID, GetUniqueObjectID(), SvOl::JoinType::Owner);
		}

		m_ownerObjectInfo.PObject = pNewOwner;
		m_ownerObjectInfo.UniqueObjectID = pNewOwner->GetUniqueObjectID();
		SVObjectManagerClass::Instance().connectDependency( m_ownerObjectInfo.UniqueObjectID, GetUniqueObjectID(), SvOl::JoinType::Owner);
		return true;
	}
	return false;
}

/*
Set new object owner using owner GUID.  Uses TheSVObjectManager to get the owner pointer.
*/
BOOL SVObjectClass::SetObjectOwner( const GUID& rNewOwnerGUID )
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
	m_outObjectInfo.ObjectTypeInfo.EmbeddedID = rEmbeddedID;
	SetObjectName( NewObjectName );
	SetObjectOwner( pOwner );	
}

HRESULT SVObjectClass::GetObjectValue( const SVString& rValueName, VARIANT& rVariantValue ) const
{
	HRESULT hr = S_OK;

	_variant_t l_TempVariant;

	l_TempVariant.Attach( rVariantValue );

	if( rValueName == _T( "Friend" ) )
	{
		SVSAFEARRAY l_SafeArray;

		// for all friends in the list - get GUIDs
		for( size_t i = 0;i < m_friendList.size();i++ )
		{
			SVGUID friendGuid = m_friendList[ i ].UniqueObjectID;

			l_SafeArray.Add( friendGuid.ToVARIANT() );
		}

		l_TempVariant = l_SafeArray;
	}
	else if( rValueName == _T( "AttributesAllowed" ) )
	{
		l_TempVariant = m_ObjectAttributesAllowed;
	}
	else if( rValueName == _T( "AttributesSet" ) )
	{
		SVSAFEARRAY l_SafeArray;

		for( size_t i = 0; i < m_ObjectAttributesSet.size(); i++ )
		{
			_variant_t l_Value = m_ObjectAttributesSet.at(i);

			l_SafeArray.Add( l_Value );
		}

		l_TempVariant = l_SafeArray;
	}
	else
	{
		l_TempVariant.Clear();

		hr = S_FALSE;
	}

	l_TempVariant.Detach();

	return hr;
}

HRESULT SVObjectClass::SetObjectValue( const SVString& rValueName, const _variant_t& rVariantValue )
{
	HRESULT hr = S_OK;

	if( rValueName == _T( "Friend" ) )
	{
		if( ( rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( rVariantValue );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( S_OK == l_SafeArray.GetElement( i, l_Value ) && VT_BSTR == l_Value.vt )
				{
					GUID friendGuid;

					// convert the guidStr to a Guid
					AfxGetClassIDFromString( _bstr_t( l_Value ), &friendGuid );
					
					// call AddFriend
					AddFriend( friendGuid );
				}
			}
		}
		else
		{
			hr = S_FALSE;
		}
	}
	else if( rValueName == _T( "AttributesAllowed" ) )
	{
		if( VT_ARRAY == ( rVariantValue.vt & VT_ARRAY ) )
		{
			SVSAFEARRAY l_SafeArray( rVariantValue );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( S_OK == l_SafeArray.GetElement( i, l_Value ) )
				{
					m_ObjectAttributesAllowed = l_Value;
				}
			}
		}
		else
		{
			m_ObjectAttributesAllowed = rVariantValue;
		}
	}
	else if( rValueName == _T( "AttributesSet" ) )
	{
		if( VT_ARRAY == ( rVariantValue.vt & VT_ARRAY ) )
		{
			SVSAFEARRAY l_SafeArray( rVariantValue );

			m_ObjectAttributesSet.resize( l_SafeArray.size() );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( S_OK == l_SafeArray.GetElement( i, l_Value )  )
				{
					m_ObjectAttributesSet.at(i) = l_Value;
				}
				else
				{
					m_ObjectAttributesSet.at(i) = 0;
				}
			}
		}
		else
		{
			hr = S_FALSE;
		}
	}
	else if( rValueName != _T( "DataLinkID" ) )
	{
		hr = S_FALSE;
	}

	return hr;
}

/*
Must override this in the derived class if you wish to set any values upon restoration from a script.  This method will only handle the "friend" member assignment.
*/
HRESULT SVObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	if( nullptr != pDataObject )
	{
		SvCl::SVObjectDWordArrayClass svDWordArray;
		SvCl::SVObjectCStringArrayClass svCStringArray;

		if ( ( bOk = pDataObject->GetAttributeData( "DataLinkID", svCStringArray ) ) )
		{
			; // Do nothing as it's obsolete
		}
		else if ( ( bOk = pDataObject->GetAttributeData( "Friend", svCStringArray ) ) )
		{
			for( int i = 0; i < svCStringArray.GetSize(); i++ )
			{
				GUID friendGuid;

				// convert the guidStr to a Guid
				AfxGetClassIDFromString( svCStringArray[i], &friendGuid );
				
				// call AddFriend
				AddFriend( friendGuid );
			}
		}
		else if ( ( bOk = pDataObject->GetAttributeData( "AttributesAllowed", svDWordArray ) ) )
		{
			for( int i = 0; i < svDWordArray.GetSize(); i++ )
			{
				m_ObjectAttributesAllowed = svDWordArray[i];
			}
		}
		else if ( ( bOk = pDataObject->GetAttributeData( "AttributesSet", svDWordArray ) ) )
		{
			int iSize = svDWordArray.GetSize();
			{
				m_ObjectAttributesSet.resize( iSize );
				for( int i = 0; i < iSize; i++ )
				{
					m_ObjectAttributesSet.at(i) = svDWordArray[i];
				}
			}
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
BOOL SVObjectClass::AddFriend( const GUID& rFriendGUID, const GUID& rAddPreGuid )
{
	size_t position = m_friendList.size();
	// Check GUID...
	if( SV_GUID_NULL == rFriendGUID )
	{
		return false;
	}
	// Check if friend is already applied...
	if (m_friendList.size())
	{
		for( int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; -- i )
		{
			if( m_friendList[ i ].UniqueObjectID == rFriendGUID )
			{
				return false;
			}

			if( m_friendList[i].UniqueObjectID == rAddPreGuid)
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
		SVObjectClass* l_psvOwner = pNewFriend->GetOwner();

		if ( l_psvOwner == this )
		{
			newFriendInfo = pNewFriend;
		}
		else
		{
			if( nullptr != l_psvOwner )
			{
				SVObjectClass* l_psvNewObject = l_psvOwner->UpdateObject( rFriendGUID, pNewFriend, this );

				ASSERT( nullptr != l_psvNewObject );

				newFriendInfo = l_psvNewObject;
			}
		}
	}
	else
	{
		newFriendInfo.UniqueObjectID = rFriendGUID;
	}

	BOOL bRetVal = ( m_friendList.Insert( position, newFriendInfo ) >= 0  );
	return bRetVal;
}

SVObjectClass*  SVObjectClass::GetFriend( const SVObjectTypeInfoStruct& rObjectType ) const 
{
	// Check if friend is already applied...
	for(int i =0; i < static_cast<int>(m_friendList.size()); i++ ) 
	{
		const SVObjectTypeInfoStruct* pInfoStruct =  &(m_friendList[ i ].ObjectTypeInfo); 
		if( pInfoStruct->ObjectType   == rObjectType.ObjectType && 
			pInfoStruct->SubType   == rObjectType.SubType
			)
		{
			if( SV_GUID_NULL == pInfoStruct->EmbeddedID || pInfoStruct->EmbeddedID == rObjectType.EmbeddedID )
			{
				return 	m_friendList[ i ].PObject;
			}
		}
	}
	return nullptr;
}

/*
This method is used to remove an object from the friends list via the object's unique object identifier.
*/
BOOL SVObjectClass::RemoveFriend( const GUID& rFriendGUID )
{
	// Check GUID...
	if (SV_GUID_NULL != rFriendGUID)
	{
		// Check if friend is applied...
		if (m_friendList.size())
		{
			for (int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; --i)
			{
				if (m_friendList[i].UniqueObjectID == rFriendGUID)
				{
					// Remove Friend...
					m_friendList.RemoveAt(i);

					// Success...
					return true;
				}
			}
		}
	}
	return false;
}

/*
Set the object data depth. ( Data Table )  Should be overridden and must be called in derived classes...
*/
BOOL SVObjectClass::SetObjectDepth( int NewObjectDepth )
{
	BOOL l_bOk = true;

	m_objectDepth = NewObjectDepth;

	return l_bOk;
}

/*
Set the object data depth. ( Data Table )  Should be overridden and must be called in derived classes...
*/
BOOL SVObjectClass::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_bOk = true;

	m_objectDepth = NewObjectDepth;

	return l_bOk;
}

/*
Set the object image depth.  Should be overridden and must be called in derived classes...
*/
BOOL SVObjectClass::SetImageDepth( long lDepth )
{
	BOOL l_bOk = true;
	
	m_lImageDepth = lDepth;
	
	return l_bOk;
}

/*
This method returns the current state of the public attributes.
*/
SVPublicAttributeEntryStruct* SVObjectClass::GetPublicAttribute()
{
	return &m_publicAttribute;
}

/*
This method returns Ancestor Object of specified Object Type of this Object, if any.  Otherwise it returns NULL.
*/
SVObjectClass* SVObjectClass::GetAncestor( SVObjectTypeEnum AncestorObjectType ) const
{
	SVObjectClass* pOwner = this->GetOwner();
	
	while( nullptr != pOwner )
	{
		if( pOwner->GetObjectType() == AncestorObjectType )
		{
			return pOwner;
		}
		pOwner = pOwner->GetOwner();
	}
	return nullptr;
}

// Get the local object color...
DWORD SVObjectClass::GetObjectColor() const
{
	// Not defined here!
	return SV_DEFAULT_INACTIVE_COLOR;
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
	return m_Name.GetLength();
}

/*
Get the length in Byte of the NOT user changeable object instance name.
*/
int SVObjectClass::GetObjectNameLength() const
{
	return m_ObjectName.GetLength();
}

/*
Get the complete object name, beginning from highest owner level.
*/
CString SVObjectClass::GetCompleteObjectName() const
{
	CString StrCompleteName;

	GetCompleteObjectName( StrCompleteName );

	return StrCompleteName;
}

/*
Get the complete object name, beginning from highest owner level.
*/
void SVObjectClass::GetCompleteObjectName( CString& rString ) const
{
	if( nullptr != m_ownerObjectInfo.PObject && m_ownerObjectInfo.PObject != this )
	{
		m_ownerObjectInfo.PObject->GetCompleteObjectName( rString );
	}

	if( rString.GetLength() > 0 )
	{
		rString += _T(".");
	}

	rString += GetName();
}

/*
Get the complete object name including selected SVObjectTypeEnum value.
*/
SVString SVObjectClass::GetObjectNameToObjectType(LPCSTR CompleteName, SVObjectTypeEnum objectTypeToInclude) const
{
	return SVString(GetCompleteObjectNameToObjectType(CompleteName, objectTypeToInclude).GetString());
}

/*
Get the complete object name including selected SVObjectTypeEnum value.
*/
CString SVObjectClass::GetCompleteObjectNameToObjectType( LPCSTR CompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	CString Result;
	const CString name = GetName();

	if( CompleteName )
	{
		if (0 >= strlen(name))
		{
			Result.Format( _T( "%s" ), CompleteName );
		}
		else
		{
			Result.Format( _T( "%s.%s" ), name.GetString(), CompleteName );
		}
	}
	else
	{
		Result = name;
	}

	//
	// Look for Tool Set type object.
	//
	SVObjectTypeEnum objectType = GetObjectType();
	if(objectType != objectTypeToInclude)
	{
		if( nullptr != m_ownerObjectInfo.PObject && m_ownerObjectInfo.PObject != this )
		{
			Result = m_ownerObjectInfo.PObject->GetCompleteObjectNameToObjectType( Result, objectTypeToInclude );
		}
	}
	return Result;
}

/*
Get the complete object name, beginning from highest owner level.
*/
LPTSTR SVObjectClass::GetCompleteObjectName2( LPCTSTR CompleteName ) const
{
	int Length = 0;
	if( CompleteName )
	{
		Length += static_cast<int>(_tcslen( CompleteName ) + 2);
	}
	Length += GetNameLength() + 2;

	if( nullptr != m_ownerObjectInfo.PObject && m_ownerObjectInfo.PObject != this )
	{
		Length = m_ownerObjectInfo.PObject->GetCompleteObjectNameLength( Length );
	}
	TCHAR* pName = new TCHAR[ Length + 1 ];
	if( pName )
	{
		pName[ 0 ] = _TCHAR( '\0' );
		if( nullptr != m_ownerObjectInfo.PObject )
		{
			m_ownerObjectInfo.PObject->buildCompleteObjectName( pName, Length );
		}
		// Append this name...
		_tcsncat( pName, GetName(), min( Length - ( int ) _tcslen( pName ), GetNameLength() ) );

		// Append postfix...
		if( CompleteName )
		{
			_tcsncat( pName, _T( "." ), min( Length - _tcslen( pName ), 1 ) );
			_tcsncat( pName, CompleteName, min( Length - _tcslen( pName ), _tcslen( CompleteName ) ) );
		}
	}
	return pName;
}

/*
This method returns a reference to the friends list attribute of this object.
*/
const SVObjectInfoArrayClass& SVObjectClass::GetFriendList() const
{
	return m_friendList;
}

/*
Get the complete object name, beginning from highest owner level.
*/
void SVObjectClass::buildCompleteObjectName( LPTSTR CompleteName, int MaxLength )
{
	if( nullptr != m_ownerObjectInfo.PObject && m_ownerObjectInfo.PObject != this )
	{
		m_ownerObjectInfo.PObject->buildCompleteObjectName( CompleteName, MaxLength );
	}
	_tcsncat( CompleteName, GetName(), min( MaxLength - ( int ) _tcslen( CompleteName ), GetNameLength() ) );	
	_tcsncat( CompleteName, _T( "." ), min( MaxLength - ( int ) _tcslen( CompleteName ), 1 ) );
}

HRESULT SVObjectClass::RemoveObjectConnection( const GUID& rObjectID )
{
	return S_OK;
}

/*
SVM_ message process function, should be overridden in derived classes. Refer to SVObject.h for information about possible messages. Use one of these global functions to send a message to an object:

DWORD_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
DWORD_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
*/
DWORD_PTR SVObjectClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	// Try to process message by yourself...
	// ( if necessary process here the incoming messages )
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( false );

				return SVMR_NO_SUCCESS;
			}

			return SVMR_SUCCESS;
		}

	case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( S_OK != ConnectObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( false );

				return SVMR_NO_SUCCESS;
			}

			return SVMR_SUCCESS;
		}

	case SVMSGID_GETFIRST_OBJECT:
		{
			// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo!
			//( SVObjectTypeInfoStruct->objectType => SVObjectTypeEnum )
			SVObjectTypeInfoStruct* pObjectTypeInfo = reinterpret_cast<SVObjectTypeInfoStruct*>(DwMessageContext);
			if( pObjectTypeInfo )
			{
				// ...use second message parameter ( DwMessageValue ) to specify the object requesting
				// Note: if second parameter is specified - stop when requesting object encountered.
				// Optional - to get an input
				SVObjectClass* pRequestor = reinterpret_cast<SVObjectClass*>( DwMessageValue );

				// check the owner of this class
				if( pRequestor && pRequestor == this || pRequestor == GetOwner() )
				{
					// Do not reference self or owner
					return SVMR_NOT_PROCESSED;
				}

				// Find best match....EmbeddedID, Type, SubType...
				if( ( SV_GUID_NULL  == pObjectTypeInfo->EmbeddedID      || pObjectTypeInfo->EmbeddedID == GetEmbeddedID() ) &&
					( SVNotSetObjectType == pObjectTypeInfo->ObjectType || pObjectTypeInfo->ObjectType == GetObjectType() ) &&
					( SVNotSetSubObjectType == pObjectTypeInfo->SubType || pObjectTypeInfo->SubType    == GetObjectSubType() )
					)
				{
					if( SV_GUID_NULL          != pObjectTypeInfo->EmbeddedID ||
						SVNotSetObjectType    != pObjectTypeInfo->ObjectType ||
						SVNotSetSubObjectType != pObjectTypeInfo->SubType
						)
					{
						// But object must be specified!
						return reinterpret_cast<DWORD_PTR> (static_cast<SVObjectClass*> (this));
					}
				}
			}
			break;
		}

	case SVMSGID_OVERWRITE_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
			// ...use third message parameter ( DwMessageContext ) as embeddedID ( GUID* ) 
			// ...returns pointer to embedded SVObjectClass
			const GUID taskObjectID = *( ( GUID* ) DwMessageValue );
			const GUID l_guidEmbeddedID	= *( ( GUID* ) DwMessageContext );
			// NOTE:	Only for embedded objects !!! 
			//			Dynamically generated objects must be replaced,
			//			using SVM_REPLACE_OBJECT!!!

			// Check embedded member here ( embedded objects could be only identified by embeddedID!!!! )... 
			// ... and if object could not be identified then return NULL //SVMR_NOT_PROCESSED
			if( GetEmbeddedID() == l_guidEmbeddedID )
			{
				SVObjectManagerClass::Instance().ChangeUniqueObjectID( this, taskObjectID );
				return reinterpret_cast<DWORD_PTR> (static_cast<SVObjectClass*> (this));

			}
			return SVMR_NOT_PROCESSED;
		}

	case SVMSGID_CONNECT_OBJECT_INPUT:
		{
			// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVInObjectInfoStruct* pInObjectInfo = reinterpret_cast<SVInObjectInfoStruct*>(DwMessageValue);
			if( ConnectObjectInput( pInObjectInfo ) )
			{
				return SVMR_SUCCESS;
			}

			return SVMR_NO_SUCCESS;
		}

	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVInObjectInfoStruct* pInObjectInfo = reinterpret_cast<SVInObjectInfoStruct*>(DwMessageValue);
			if( DisconnectObjectInput( pInObjectInfo ) )
				return SVMR_SUCCESS;

			return SVMR_NO_SUCCESS;
		}

	case SVMSGID_CLOSE_OBJECT:
		{
			if( CloseObject() )
				return SVMR_SUCCESS;

			return SVMR_NO_SUCCESS;
		}

	case SVMSGID_SET_OBJECT_VALUE:
		{
			// try to set our trivial member
			SVObjectAttributeClass* dataObject = reinterpret_cast<SVObjectAttributeClass*>(DwMessageContext);

			if( S_OK == SetObjectValue( dataObject ) )
				return SVMR_SUCCESS;

			return SVMR_NO_SUCCESS;
		}

	case SVMSGID_GET_OBJECT_BY_NAME:
		{
			CString strName = (LPCSTR)DwMessageValue;

			if( strName == GetCompleteObjectName() )
			{
				return reinterpret_cast<DWORD_PTR> (static_cast<SVObjectClass*> (this));
			}
			else
			{
				return SVMR_NOT_PROCESSED;
			}
		}// end SVMSGID_GET_OBJECT_BY_NAME
	}
	return DwResult;
}

/*
This method gets the length of the complete object name length.
*/
int SVObjectClass::GetCompleteObjectNameLength( int Length ) const
{
	Length += GetNameLength() + 1;

	if( nullptr != m_ownerObjectInfo.PObject && m_ownerObjectInfo.PObject != this )
	{
		Length = m_ownerObjectInfo.PObject->GetCompleteObjectNameLength( Length );
	}
	return Length;
}

void SVObjectClass::Persist( SVObjectWriter& rWriter )
{
	_variant_t value; 
	value.SetString(GetName()); // use user editable name for Data Element ObjectName attribute
	rWriter.WriteAttribute( scObjectNameTag, value );
	value.Clear();

	value.SetString(GetClassID().ToString().c_str());
	rWriter.WriteAttribute( scClassIDTag, value );
	value.Clear();

	value.SetString(m_outObjectInfo.UniqueObjectID.ToString().c_str());
	rWriter.WriteAttribute( scUniqueReferenceIDTag, value );
	value.Clear();

	// Set up object definition...
	if( SV_GUID_NULL != GetEmbeddedID() )
	{
		value.SetString(GetEmbeddedID().ToString().c_str());
		rWriter.WriteAttribute( scEmbeddedIDTag, value );
		value.Clear();
	}
	PersistAttributes(rWriter);
}

void SVObjectClass::PersistAttributes( SVObjectWriter& rWriter )
{
	// Get the Data Values (Member Info, Values)

	// Add object attributes as trivial members
	_variant_t value; 
	value.ChangeType(VT_UI4);	
	rWriter.StartElement(scAttributesSetTag);
	SVVariantList list;

	for ( unsigned int i = 0; i < m_ObjectAttributesSet.size(); i++ )
	{
		value.ulVal = m_ObjectAttributesSet.at(i);
		list.push_back(value);
	}
	rWriter.WriteAttribute( scAttributeTag, list);
	rWriter.EndElement();
}

/*
This method walks the object hierichary to find a child object.
*/
BOOL SVObjectClass::GetChildObjectByName( LPCTSTR tszChildName, SVObjectClass** ppObject )
{
	ASSERT( nullptr != ppObject );
	bool bReturn = false;

	if ( nullptr != ppObject )
	{
		*ppObject = nullptr;

		CString sChildName = tszChildName;
		CString sName = GetCompleteObjectName();
		if ( sChildName.Left(sName.GetLength()) == sName )
		{
			*ppObject = reinterpret_cast<SVObjectClass*>( ::SVSendMessage( this, 
				( SVM_GET_OBJECT_BY_NAME | SVM_PARENT_TO_CHILD ) & ~SVM_NOTIFY_ONLY_THIS, 
				reinterpret_cast<DWORD_PTR>(tszChildName), 0 ) );
			bReturn = ( nullptr != *ppObject );
		}
	}
	return bReturn;
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
This method returns whether this object contains an array of data or not.
*/
bool SVObjectClass::IsArray() const
{
	return false;
}

/*
This method returns the depth of the array if this object contains arrays of data.
*/
int SVObjectClass::GetArraySize() const
{
	return 1;
}

/*
This method returns the allowed attributes of this object.
*/
const UINT& SVObjectClass::ObjectAttributesAllowed() const
{
	return m_ObjectAttributesAllowed;
}

/*
This method returns a reference to the allowed attributes of this object.
*/
UINT& SVObjectClass::ObjectAttributesAllowedRef()
{
	return m_ObjectAttributesAllowed;
}

/*
This method returns the set attributes of this object.
*/
const UINT& SVObjectClass::ObjectAttributesSet(int iIndex) const
{
	return m_ObjectAttributesSet.at(iIndex);
}

/*
This method sets attributes of this object.
*/
UINT& SVObjectClass::ObjectAttributesSetRef(int Index)
{
	ASSERT( (long)m_ObjectAttributesSet.size() > (long)Index );
	return m_ObjectAttributesSet.at(Index);
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