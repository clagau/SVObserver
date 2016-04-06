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
#include "SVToolsetScriptTags.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVObjectClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVObjectClass, SVInvalidGUID )

//******************************************************************************
// Message Map Entries
//******************************************************************************

/*
This is the default constructor for this object.  This constructor initializes the name objects, clears all owner information, and calls the init method.
*/
SVObjectClass::SVObjectClass()
{
	strName = strObjectName = "Base Class: SVResearch Base Object";
	resourceID    = 0;
	SetObjectOwner( NULL );

	init();
}

/*
This constructor initializes the name objects from the provided parameter, clears all owner information, and calls the init method.
*/
SVObjectClass::SVObjectClass( LPCSTR ObjectName )
{
	strName			= ObjectName;
	strObjectName	= ObjectName;
	resourceID      = 0;

	init();
}

/*
This constructor initializes the name objects from the provided parameter, sets the owner information from the provided parameter, and calls the init method.
*/
SVObjectClass::SVObjectClass( SVObjectClass* POwner /* = NULL */, int StringResourceID /* = IDS_CLASSNAME_SVOBJECT */ )
{
	strObjectName.LoadString( StringResourceID );
	strName = strObjectName;
	resourceID    = StringResourceID;
	SetObjectOwner( POwner );

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
	isCreated	  = FALSE;

	mlImageDepth = 0;

	objectDepth	= 0;	// Standard Depth

	isObjectValid		  = FALSE;
	validationReferenceID = SVInvalidGUID;

	embeddedID			  = SVInvalidGUID;

	// Set object Info...
	SVObjectManagerClass::Instance().CreateUniqueObjectID( this );

	outObjectInfo.PObject = this;

	m_uObjectAttributesAllowed = SV_NO_ATTRIBUTES; 
	m_uObjectAttributesSet     = SV_NO_ATTRIBUTES;
	m_auObjectAttributesSet.resize( 1 );
	m_auObjectAttributesSet[0]  = SV_NO_ATTRIBUTES;
	m_uDefaultObjectAttributesSet = SV_NO_ATTRIBUTES;
}

/*
This method destroys all friends on the friends list owned by this object.  All other objects are disconnected from this object.
*/
void SVObjectClass::DestroyFriends()
{
	for( size_t i = 0; i < friendList.size(); ++i )
	{
		const SVObjectInfoStruct& rFriend = friendList[ i ];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject( rFriend.UniqueObjectID );
		if( pFriend )
		{
			SVObjectClass* pOwner = pFriend->GetOwner();
			if( pOwner )
			{
				// Close, Disconnect and Delete Friend...
				::SVSendMessage( pOwner, SVM_DESTROY_FRIEND_OBJECT, reinterpret_cast<DWORD_PTR>(pFriend), NULL );
			}
			else
			{
				// Friend has no owner...
				pFriend->CloseObject();
				delete pFriend;
			}
		}
	}
	friendList.RemoveAll();
}

/*
This destructor unregisters itself from the object manager and frees all of the allocated names.
*/
SVObjectClass::~SVObjectClass()
{
	outObjectInfo.DisconnectAllInputs();

	SVObjectManagerClass::Instance().CloseUniqueObjectID( this );
}

SVObjectClass::SVObjectPtrDeque SVObjectClass::GetPreProcessObjects() const
{
	SVObjectPtrDeque l_Objects;

	for( size_t i = 0; i < friendList.size(); i++ )
	{
		const SVObjectInfoStruct& rFriend = friendList[i];

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
BOOL SVObjectClass::ConnectObjectInput( SVInObjectInfoStruct* PObjectInInfo )
{
	if( PObjectInInfo )
	{
		SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

		if( l_AutoLock.Assign( &outObjectInfo ) )
		{
			outObjectInfo.AddInput( *PObjectInInfo );

			return TRUE;
		}
	}
	return FALSE;
}

// Disconnect Input from somebody else from my Output...( from me )
BOOL SVObjectClass::DisconnectObjectInput( SVInObjectInfoStruct* PObjectInInfo )
{
	if( PObjectInInfo )
	{
		SVAutoLockAndReleaseTemplate< SVOutObjectInfoStruct > l_AutoLock;

		if( l_AutoLock.Assign( &outObjectInfo ) )
		{
			outObjectInfo.RemoveInput( *PObjectInInfo );

			return TRUE;
		}
	}
	return FALSE;
}


/*
After the object construction, the object must be created using this function with an object level depending create structure.
*/
BOOL SVObjectClass::CreateObject( SVObjectLevelCreateStruct *PCreateStructure )
{
	BOOL l_bOk = PCreateStructure != NULL;

	if( l_bOk )
	{
		if( PCreateStructure->OwnerObjectInfo.PObject != this && PCreateStructure->OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
		{
			ownerObjectInfo	= PCreateStructure->OwnerObjectInfo;
		}
		else
		{
			ASSERT(false);
		}
	}

	isCreated = l_bOk;

	return l_bOk;
}

HRESULT SVObjectClass::ConnectObject( SVObjectLevelCreateStruct *PCreateStructure )
{
	HRESULT l_Status = S_OK;

	if( PCreateStructure != NULL )
	{
		if( PCreateStructure->OwnerObjectInfo.PObject != this && PCreateStructure->OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
		{
			ownerObjectInfo	= PCreateStructure->OwnerObjectInfo;
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

	long l_lCount = static_cast<long>(outObjectInfo.GetInputSize());

	if( 0 < l_lCount && l_AutoLock.Assign( &outObjectInfo ) )
	{
		for( int i = 0; i < l_lCount; ++ i )
		{
			SVInObjectInfoStruct& rUserInInfo = outObjectInfo.GetInputAt( i );
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rUserInInfo.UniqueObjectID );
			if( pObject && pObject->IsCreated() )
			{
				// Close only user of our output which are still not closed!
				dwResult = ::SVSendMessage( pObject, SVM_CLOSE_OBJECT, reinterpret_cast<DWORD_PTR> (static_cast<SVObjectClass*> (this)), NULL ) | dwResult;
			}
		}
	}

	if( dwResult != SVMR_NO_SUCCESS )
	{
		isCreated = FALSE;
		return TRUE;
	}

	return FALSE;
}

/*
This method return the validity state of this object.
*/
BOOL SVObjectClass::IsObjectValid( GUID& RValidationReferenceID )
{
	RValidationReferenceID = validationReferenceID;
	return isObjectValid;
}

/*
This method returns whether this object is a descendant of PAncestorObject.
*/
BOOL SVObjectClass::IsDescendantOf( SVObjectClass* PAncestorObject )
{
	BOOL l_Status = true;
	
	l_Status = l_Status && ( PAncestorObject != NULL );
	l_Status = l_Status && ( PAncestorObject == this );

	if( !l_Status )
	{
		SVObjectClass* l_pOwner = GetOwner();

		if( l_pOwner != NULL && l_pOwner != this )
		{
			l_Status = l_pOwner->IsDescendantOf( PAncestorObject );
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

		if( l_pOwner != NULL && l_pOwner != this )
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
BOOL SVObjectClass::InitObject( SVObjectClass* PObject )
{
	if( PObject != NULL )
	{
		ownerObjectInfo.PObject			= PObject;
		ownerObjectInfo.UniqueObjectID	= PObject->GetUniqueObjectID();
		return TRUE;
	}
	return FALSE;
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
	return ( isObjectValid = isCreated );
}

/*
Reset user changeable name to the NOT user changeable object instance name.
*/
void SVObjectClass::ResetName()
{
	// Resets the changeable names to the name given at construction time
	// or set with SVObjectClass::SetObjectName()
	strName = strObjectName;
}

#pragma region virtual method (IObjectClass)
/*
This method return the object name.  This name is changeable by the user.
*/
LPCTSTR SVObjectClass::GetName() const
{
	return strName;
}

SVString SVObjectClass::GetCompleteName() const 
{
	CString name = GetCompleteObjectName();
	return SVString(static_cast<LPCSTR>(name));
}

HRESULT SVObjectClass::GetCompleteNameToType(SVObjectTypeEnum objectType, SVString& rName) const
{
	HRESULT hr = S_OK;
	SVString name;
	if (GetObjectType() == objectType)
	{
		rName = GetCompleteName();
	}
	else
	{
		const SvOi::IObjectClass* pObject = GetAncestorInterface(objectType);
		if (pObject)
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
	if (GetObjectType() == ancestorObjectType)
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

SvOi::IObjectClass* SVObjectClass::GetFirstObject(const SVObjectTypeInfoStruct& type)
{
	return reinterpret_cast<SVObjectClass*>(processMessage(SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&type) ));
}

SVObjectSubTypeEnum SVObjectClass::GetObjectSubType() const
{
	return static_cast<SVObjectSubTypeEnum>(outObjectInfo.ObjectTypeInfo.SubType);
}

SvUl::NameGuidList SVObjectClass::GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const
{
	SvUl::NameGuidList list;

	return list;
}
#pragma endregion virtual method (IObjectClass)

/*
Set user changeable name.
*/
void SVObjectClass::SetName( LPCTSTR StrString )
{
	strName = StrString;
}

/*
This method return the internal object name.  This name is not changeable by the user.
*/
LPCTSTR SVObjectClass::GetObjectName() const
{
	return strObjectName;
}

/*
Set the NOT user changeable object instance name.
*/
void SVObjectClass::SetObjectName( int StringResourceID )
{
	strObjectName.LoadString( StringResourceID );
	ResetName();
}

/*
Set new object owner using owner pointer.
*/
BOOL SVObjectClass::SetObjectOwner( SVObjectClass* PNewOwner )
{
	// Check if object exists...
	if( PNewOwner != NULL )
	{
		ASSERT(PNewOwner != this); // can't own yourself...

		ownerObjectInfo.PObject		   = PNewOwner;
		ownerObjectInfo.UniqueObjectID = PNewOwner->GetUniqueObjectID();

		return TRUE;
	}

	return FALSE;
}

/*
Set new object owner using owner GUID.  Uses TheSVObjectManager to get the owner pointer.
*/
BOOL SVObjectClass::SetObjectOwner( const GUID& RNewOwnerGUID )
{
	// Check if object exists...
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( RNewOwnerGUID );
	if( pObject != NULL )
	{
		ASSERT(pObject != this); // can't own yourself...

		ownerObjectInfo.PObject			= pObject;
		ownerObjectInfo.UniqueObjectID	= RNewOwnerGUID;
		return TRUE;
	}

	return FALSE;
}

/*
Set embedded object info.  Use this only for real embedded objects.
*/
void SVObjectClass::SetObjectEmbedded( const GUID& REmbeddedID, SVObjectClass* POwner, int NewStringResourceID )
{
	embeddedID = REmbeddedID;
	outObjectInfo.ObjectTypeInfo.EmbeddedID = REmbeddedID;
	SetObjectName( NewStringResourceID );
	SetObjectOwner( POwner );	
}

HRESULT SVObjectClass::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	_variant_t l_TempVariant;

	l_TempVariant.Attach( p_rVariantValue );

	if( p_rValueName == _T( "Friend" ) )
	{
		SVSAFEARRAY l_SafeArray;

		// for all friends in the list - get GUIDs
		for( size_t i = 0;i < friendList.size();i++ )
		{
			SVGUID friendGuid = friendList[ i ].UniqueObjectID;

			l_SafeArray.Add( friendGuid.ToVARIANT() );
		}

		l_TempVariant = l_SafeArray;
	}
	else if( p_rValueName == _T( "AttributesAllowed" ) )
	{
		l_TempVariant = m_uObjectAttributesAllowed;
	}
	else if( p_rValueName == _T( "AttributesSet" ) )
	{
		SVSAFEARRAY l_SafeArray;

		for( size_t i = 0; i < m_auObjectAttributesSet.size(); i++ )
		{
			_variant_t l_Value = m_auObjectAttributesSet.at(i);

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

HRESULT SVObjectClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "Friend" ) )
	{
		if( ( p_rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( p_rVariantValue );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( l_SafeArray.GetElement( i, l_Value ) == S_OK && l_Value.vt == VT_BSTR )
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
	else if( p_rValueName == _T( "AttributesAllowed" ) )
	{
		if( ( p_rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( p_rVariantValue );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( l_SafeArray.GetElement( i, l_Value ) == S_OK )
				{
					m_uObjectAttributesAllowed = l_Value;
				}
			}
		}
		else
		{
			m_uObjectAttributesAllowed = p_rVariantValue;
		}
	}
	else if( p_rValueName == _T( "AttributesSet" ) )
	{
		if( ( p_rVariantValue.vt & VT_ARRAY ) == VT_ARRAY )
		{
			SVSAFEARRAY l_SafeArray( p_rVariantValue );

			m_auObjectAttributesSet.resize( l_SafeArray.size() );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;

				if( l_SafeArray.GetElement( i, l_Value ) == S_OK )
				{
					m_auObjectAttributesSet.at(i) = l_Value;
				}
				else
				{
					m_auObjectAttributesSet.at(i) = 0;
				}
			}
		}
		else
		{
			hr = S_FALSE;
		}
	}
	else if( p_rValueName != _T( "DataLinkID" ) )
	{
		hr = S_FALSE;
	}

	return hr;
}

/*
Must override this in the derived class if you wish to set any values upon restoration from a script.  This method will only handle the "friend" member assignment.
*/
HRESULT SVObjectClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	if( PDataObject )
	{
		SvCl::SVObjectDWordArrayClass svDWordArray;
		SvCl::SVObjectCStringArrayClass svCStringArray;

		if ( ( bOk = PDataObject->GetAttributeData( "DataLinkID", svCStringArray ) ) )
		{
			for( int i = 0; i < svCStringArray.GetSize(); i++ )
			{
				// convert the guidStr to a Guid
				int x = 33;
			}
		}
		else if ( ( bOk = PDataObject->GetAttributeData( "Friend", svCStringArray ) ) )
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
		else if ( ( bOk = PDataObject->GetAttributeData( "AttributesAllowed", svDWordArray ) ) )
		{
			for( int i = 0; i < svDWordArray.GetSize(); i++ )
			{
				m_uObjectAttributesAllowed = svDWordArray[i];
			}
		}
		else if ( ( bOk = PDataObject->GetAttributeData( "AttributesSet", svDWordArray ) ) )
		{
			int iSize = svDWordArray.GetSize();
			{
				m_auObjectAttributesSet.resize( iSize );
				for( int i = 0; i < iSize; i++ )
				{
					m_auObjectAttributesSet.at(i) = svDWordArray[i];
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
	isObjectValid = FALSE;
}

// Override this function to implement object behavior...
void SVObjectClass::SetDisabled()
{
}

/*
This method is a placeholder for derived functionality.  This method performs not operation.
*/
SVObjectClass *SVObjectClass::UpdateObject( const GUID &p_oFriendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner )
{
	return NULL;
}

/*
This method is used to add an object to the friends list via the object's unique object identifier.
*/
BOOL SVObjectClass::AddFriend( const GUID& RFriendGUID )
{
	// Check GUID...
	if( RFriendGUID == SVInvalidGUID )
		return FALSE;

	// Check if friend is already applied...
	if (friendList.size())
	{
		for( int i = static_cast<int>(friendList.size()) - 1; i >= 0; -- i )
		{
			if( friendList[ i ].UniqueObjectID == RFriendGUID )
			{
				return FALSE;
			}
		}
	}

	SVObjectInfoStruct newFriendInfo;
	// Check if Friend is alive...
	SVObjectClass* pNewFriend = SVObjectManagerClass::Instance().GetObject( RFriendGUID );
	if( pNewFriend )
	{
		// Check if we are the Owner
		// Note:: Special hack for friend scripting SEJ Aug 28,1999
		// if we are the owner - it's not from the script
		SVObjectClass* l_psvOwner = pNewFriend->GetOwner();

		if ( l_psvOwner == this )
		{
			newFriendInfo = pNewFriend;
		}
		else
		{
			if( l_psvOwner != NULL )
			{
				SVObjectClass* l_psvNewObject = l_psvOwner->UpdateObject( RFriendGUID, pNewFriend, this );

				ASSERT( l_psvNewObject != NULL );

				newFriendInfo = l_psvNewObject;
			}
		}
	}
	else
	{
		newFriendInfo.UniqueObjectID = RFriendGUID;
	}

	BOOL bRetVal = ( friendList.Add( newFriendInfo ) >= 0  );
	return bRetVal;
}




SVObjectClass*  SVObjectClass::GetFriend( const SVObjectTypeInfoStruct& rObjectType ) const 
{
	// Check if friend is already applied...
	for(int i =0; i < static_cast<int>(friendList.size()); i++ ) 
	{
		const SVObjectTypeInfoStruct* pInfoStruct =  &(friendList[ i ].ObjectTypeInfo); 
		if( pInfoStruct->ObjectType   == rObjectType.ObjectType && 
			pInfoStruct->SubType   == rObjectType.SubType
			)
		{
			if(pInfoStruct->EmbeddedID  ==  SVInvalidGUID || pInfoStruct->EmbeddedID  ==  rObjectType.EmbeddedID )
			{
				return 	friendList[ i ].PObject;

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
	if (SVInvalidGUID != rFriendGUID)
	{
		// Check if friend is applied...
		if (friendList.size())
		{
			for (int i = static_cast<int>(friendList.size()) - 1; i >= 0; --i)
			{
				if (friendList[i].UniqueObjectID == rFriendGUID)
				{
					// Remove Friend...
					friendList.RemoveAt(i);

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
	BOOL l_bOk = TRUE;

	objectDepth = NewObjectDepth;

	return l_bOk;
}

/*
Set the object data depth. ( Data Table )  Should be overridden and must be called in derived classes...
*/
BOOL SVObjectClass::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_bOk = TRUE;

	objectDepth = NewObjectDepth;

	return l_bOk;
}

/*
Set the object image depth.  Should be overridden and must be called in derived classes...
*/
BOOL SVObjectClass::SetImageDepth( long lDepth )
{
	BOOL l_bOk = TRUE;
	
	mlImageDepth = lDepth;
	
	return l_bOk;
}

/*
This method returns the current state of the public attributes.
*/
SVPublicAttributeEntryStruct* SVObjectClass::GetPublicAttribute()
{
	return &publicAttribute;
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
			return pOwner;

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
	return strName.GetLength();
}

/*
Get the length in Byte of the NOT user changeable object instance name.
*/
int SVObjectClass::GetObjectNameLength() const
{
	return strObjectName.GetLength();
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
	if( ownerObjectInfo.PObject != NULL && ownerObjectInfo.PObject != this )
	{
		ownerObjectInfo.PObject->GetCompleteObjectName( rString );
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
SVString SVObjectClass::GetObjectNameToObjectType(LPCSTR LPSZCompleteName, SVObjectTypeEnum objectTypeToInclude) const
{
	return SVString(GetCompleteObjectNameToObjectType(LPSZCompleteName, objectTypeToInclude).GetString());
}

/*
Get the complete object name including selected SVObjectTypeEnum value.
*/
CString SVObjectClass::GetCompleteObjectNameToObjectType( LPCSTR LPSZCompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	CString completeName;
	const CString name = GetName();

	if( LPSZCompleteName )
	{
		if (0 >= strlen(name))
		{
			completeName.Format( _T( "%s" ), LPSZCompleteName );
		}
		else
		{
			completeName.Format( _T( "%s.%s" ), name.GetString(), LPSZCompleteName );
		}
	}
	else
	{
		completeName = name;
	}

	//
	// Look for Tool Set type object.
	//
	SVObjectTypeEnum objectType = GetObjectType();
	if(objectType != objectTypeToInclude)
	{
		if( ownerObjectInfo.PObject != nullptr && ownerObjectInfo.PObject != this )
		{
			completeName = ownerObjectInfo.PObject->GetCompleteObjectNameToObjectType( completeName, objectTypeToInclude );
		}
	}

	return completeName;
}

/*
Get the complete object name, beginning from highest owner level.
*/
LPTSTR SVObjectClass::GetCompleteObjectName2( LPCTSTR LPSZCompleteName ) const
{
	int Length = 0;
	if( LPSZCompleteName )
		Length += static_cast<int>(_tcslen( LPSZCompleteName ) + 2);

	Length += GetNameLength() + 2;

	if( ownerObjectInfo.PObject != NULL && ownerObjectInfo.PObject != this )
		Length = ownerObjectInfo.PObject->GetCompleteObjectNameLength( Length );

	TCHAR* pName = new TCHAR[ Length + 1 ];
	if( pName )
	{
		pName[ 0 ] = _TCHAR( '\0' );
		if( ownerObjectInfo.PObject != NULL )
			ownerObjectInfo.PObject->buildCompleteObjectName( pName, Length );

		// Append this name...
		_tcsncat( pName, GetName(), min( Length - ( int ) _tcslen( pName ), GetNameLength() ) );

		// Append postfix...
		if( LPSZCompleteName )
		{
			_tcsncat( pName, _T( "." ), min( Length - _tcslen( pName ), 1 ) );
			_tcsncat( pName, LPSZCompleteName, min( Length - _tcslen( pName ), _tcslen( LPSZCompleteName ) ) );
		}
	}

	return pName;
}

/*
This method returns a reference to the friends list attribute of this object.
*/
const SVObjectInfoArrayClass& SVObjectClass::GetFriendList() const
{
	return friendList;
}

/*
Get the complete object name, beginning from highest owner level.
*/
void SVObjectClass::buildCompleteObjectName( LPTSTR LPSZCompleteName, int MaxLength )
{
	if( ownerObjectInfo.PObject != NULL && ownerObjectInfo.PObject != this )
		ownerObjectInfo.PObject->buildCompleteObjectName( LPSZCompleteName, MaxLength );

	_tcsncat( LPSZCompleteName, GetName(), min( MaxLength - ( int ) _tcslen( LPSZCompleteName ), GetNameLength() ) );	
	_tcsncat( LPSZCompleteName, _T( "." ), min( MaxLength - ( int ) _tcslen( LPSZCompleteName ), 1 ) );
}

HRESULT SVObjectClass::RemoveObjectConnection( const GUID& p_rObjectID )
{
	return S_OK;
}

/*
Reinit all object pointers used object infos... 
*/
BOOL SVObjectClass::ReinitObjectInfos()
{
	// Check owner info... ( must not be parent!!!! )
	BOOL BRetVal = ( ownerObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( ownerObjectInfo.UniqueObjectID ) ) ? TRUE : FALSE;
	// Check object info...
	BRetVal = ( outObjectInfo.PObject = SVObjectManagerClass::Instance().GetObject( outObjectInfo.UniqueObjectID ) ) ? BRetVal : FALSE;
	// Check children infos...
	for( int i = 0; i < outObjectInfo.GetInputSize(); ++ i )
		BRetVal = ( outObjectInfo.GetInputAt( i ).PObject = SVObjectManagerClass::Instance().GetObject( outObjectInfo.GetInputAt( i ).UniqueObjectID ) ) ? BRetVal : FALSE;

	return FALSE;
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
				ASSERT( FALSE );

				return SVMR_NO_SUCCESS;
			}

			return SVMR_SUCCESS;
		}

	case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( ConnectObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) != S_OK )
			{
				ASSERT( FALSE );

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
					return NULL;
				}

				// Find best match....EmbeddedID, Type, SubType...
				if( ( pObjectTypeInfo->EmbeddedID == SVInvalidGUID         || pObjectTypeInfo->EmbeddedID == GetEmbeddedID() ) &&
					( pObjectTypeInfo->ObjectType == SVNotSetObjectType    || pObjectTypeInfo->ObjectType == GetObjectType() ) &&
					( pObjectTypeInfo->SubType    == SVNotSetSubObjectType || pObjectTypeInfo->SubType    == GetObjectSubType() )
					)
				{
					if( pObjectTypeInfo->EmbeddedID != SVInvalidGUID         ||
						pObjectTypeInfo->ObjectType != SVNotSetObjectType    ||
						pObjectTypeInfo->SubType    != SVNotSetSubObjectType
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
			return static_cast<DWORD_PTR>(NULL); //SVMR_NOT_PROCESSED;
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

			if( SetObjectValue( dataObject ) == S_OK )
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

	if( ownerObjectInfo.PObject != NULL && ownerObjectInfo.PObject != this )
	{
		Length = ownerObjectInfo.PObject->GetCompleteObjectNameLength( Length );
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

	value.SetString(outObjectInfo.UniqueObjectID.ToString().c_str());
	rWriter.WriteAttribute( scUniqueReferenceIDTag, value );
	value.Clear();

	// Set up object definition...
	if( GetEmbeddedID() != SVInvalidGUID )
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
	//@WARNING [gra][7.30][13.01.2016] The attribute is still saved into the configuration for forward compatibility with version 7.20 can be removed in next version
	_variant_t value(m_uObjectAttributesAllowed); 
	value.ChangeType(VT_UI4);
	rWriter.WriteAttribute( scAttributesAllowedTag, value );

	rWriter.StartElement(scAttributesSetTag);
	SVVariantList list;

	for ( unsigned int i = 0; i < m_auObjectAttributesSet.size(); i++ )
	{
		value.ulVal = m_auObjectAttributesSet.at(i);
		list.push_back(value);
	}
	rWriter.WriteAttribute( scAttributeTag, list);
	rWriter.EndElement();
}

/*
This method returns the resource identifier contained as an object attribute.
*/
int SVObjectClass::GetResourceID() const
{	
	return resourceID;
}

/*
This method walks the object hierichary to find a child object.
*/
BOOL SVObjectClass::GetChildObjectByName( LPCTSTR tszChildName, SVObjectClass** ppObject )
{
	ASSERT( ppObject != NULL );
	bool bReturn = false;

	if ( ppObject != NULL )
	{
		*ppObject = NULL;

		CString sChildName = tszChildName;
		CString sName = GetCompleteObjectName();
		if ( sChildName.Left(sName.GetLength()) == sName )
		{
			*ppObject = reinterpret_cast<SVObjectClass*>( ::SVSendMessage( this, 
				( SVM_GET_OBJECT_BY_NAME | SVM_PARENT_TO_CHILD ) & ~SVM_NOTIFY_ONLY_THIS, 
				reinterpret_cast<DWORD_PTR>(tszChildName), NULL ) );
			bReturn = ( *ppObject != NULL );
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
const UINT SVObjectClass::ObjectAttributesAllowed() const
{
	return m_uObjectAttributesAllowed;
}

/*
This method returns the set attributes of this object.
*/
const UINT SVObjectClass::ObjectAttributesSet(int iIndex) const
{
	if ( iIndex < 0 )
		return m_uObjectAttributesSet;
	else
		return m_auObjectAttributesSet.at(iIndex);
}

/*
This method returns a reference to the allowed attributes of this object.
*/
UINT& SVObjectClass::ObjectAttributesAllowedRef()
{
	return m_uObjectAttributesAllowed;
}

/*
This method sets attributes of this object.
*/
SVObjectAttributeShim SVObjectClass::ObjectAttributesSetRef(int iIndex)
{
	if ( iIndex < 0 )
		return SVObjectAttributeShim(this, m_uObjectAttributesSet, iIndex);
	else
	{
		ASSERT( (long)m_auObjectAttributesSet.size() > (long)iIndex );
		return SVObjectAttributeShim(this, m_auObjectAttributesSet.at(iIndex), iIndex);
	}
}

/*
This method sets attributes of this object.
*/
void SVObjectClass::SetObjectAttributesSet(UINT uAttributes, int iIndex, UINT WhichBits)
{
	if ( iIndex < 0 )
	{
		m_uObjectAttributesSet = uAttributes;
	}
	else
	{
		if ( (long)m_auObjectAttributesSet.size() > (long)iIndex )
		{
			//m_auObjectAttributesSet.at(iIndex) = uAttributes;
			CopyBits(m_auObjectAttributesSet.at(iIndex), uAttributes, WhichBits);
		}
	}
}

/*
This method sets the default attributes of this object.
*/
void SVObjectClass::SetDefaultObjectAttributesSet(UINT uAttributes)
{
	m_uDefaultObjectAttributesSet = uAttributes;
}

