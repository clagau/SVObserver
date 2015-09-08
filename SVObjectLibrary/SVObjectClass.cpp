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

const SVGUID& SVObjectClass::GetParentID() const
{
	return GetOwnerID();
}

SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SVObjectTypeEnum ancestorObjectType)
{
	return GetAncestor(ancestorObjectType);
}

SvOi::IObjectClass* SVObjectClass::GetFirstObject(const SVObjectTypeInfoStruct& type)
{
	return reinterpret_cast<SVObjectClass*>(processMessage(SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&type) ));
}

DWORD_PTR SVObjectClass::resetAllObjects()
{
	return processMessage(SVM_RESET_ALL_OBJECTS, NULL, NULL);
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

#ifdef USE_OBJECT_SCRIPT
/*
This method get the object subscript for this objects.
*/
void SVObjectClass::GetObjectSubScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	// script PQ Data Link Info...
	PutPQDataLinkInfoInObjectScript( RStrScript, RStrAliasTable, Indent );

	// script attributes
	PutAttributesInObjectScript( RStrScript, RStrAliasTable, Indent );

	// script friend guid
	PutFriendGuidsInObjectScript( RStrScript, RStrAliasTable, Indent );
}

/*
Get parseable script of this object. This script is human readable and is used to make this object persistent. Use TheObjectScriptParser parse function to build in runtime an object based on this script. Be careful, if this object already exists, it would be overwritten.
*/
void SVObjectClass::GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	CString script,aliasTable;

	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 

	// preallocate 1K
	aliasTable.GetBuffer(1024);  
	aliasTable.ReleaseBuffer();  

	// Alias Name is delimited by single quotes - SEJ july 23,1999
	aliasTable = CString( _T( "\n" ) ) + _T( "alias " ) + nameStrDelimiter + GetObjectName() + nameStrDelimiter + _T( " = " ) + _T( "class$" ) + GetClassID().ToString().c_str() + _T( ";" );

	// Alias Object Name is delimited by single quotes - SEJ july 23,1999
	script = strIndent + _T( "alias ") + nameStrDelimiter + _T("_object_ID_" ) + GetName() + nameStrDelimiter + _T( " = " ) + _T( "object$" ) + outObjectInfo.UniqueObjectID.ToString().c_str() + _T( ";" );

	// Set up object definition...
	if( GetEmbeddedID() != SVInvalidGUID )
	{
		// Alias Embedded Name is delimited by single quotes - SEJ july 23,1999
		script += strIndent + _T( "alias ") + nameStrDelimiter + _T( "_embedded_ID_" ) + GetObjectName() + _T( "'" ) + _T( " = " ) + _T( "embedded$" ) + GetEmbeddedID().ToString().c_str() + _T( ";" );

		// Embedded Name is delimited by single quotes - SEJ july 23,1999
		script += strIndent + nameStrDelimiter + _T( "_embedded_ID_" ) + GetObjectName() + nameStrDelimiter + _T( " " );
	}
	else
		script += strIndent;

	// Names are delimited by single quotes - SEJ july 23,1999
	script += nameStrDelimiter + GetObjectName() + nameStrDelimiter + _T( " " ) + nameStrDelimiter + GetName() + nameStrDelimiter + _T( " = " ) + nameStrDelimiter + _T( "_object_ID_" ) + GetName() + nameStrDelimiter + _T( ";" );

	GetObjectSubScript( script, RStrAliasTable, Indent );

	script.FreeExtra();  
	aliasTable.FreeExtra();

	RStrScript += script;

	// check if alias needs to be added
	if( RStrAliasTable.Find( aliasTable ) == -1 )
		RStrAliasTable += aliasTable;
}

/*
Override this function, if you have attributes to script which are not coming from embedded objects!!! You must call the base class PutAttributesInObjectScript(...) !!! 
*/
void SVObjectClass::PutAttributesInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString nameStrDelimiter = _T( "'" );

	CString script;

	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Name is delimited by single quotes - SEJ july 23,1999
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName();

	// Get the Data Values (Member Info, Values)
	CString tmp;

	// Add object attributes as trivial members
	tmp.Format( "DWORD 0x%08x", m_uObjectAttributesAllowed );
	script += strIndent + objectTag + _T( ".AttributesAllowed" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	{
		script += strIndent + objectTag + _T( ".AttributesSet" ) + nameStrDelimiter + _T( " = DWORD [ " );
		for ( unsigned int i = 0; i < m_auObjectAttributesSet.size(); i++ )
		{
			if ( i > 0 )
				script += _T(", ");
			tmp.Format( "0x%08x", m_auObjectAttributesSet.at(i) );
			script += tmp;
		}
		script += _T( " ];" );
	}

	script.FreeExtra();

	RStrScript += script;
}

/*
This method adds the product queue link information to the object data script privided by the parameters.
*/
void SVObjectClass::PutPQDataLinkInfoInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString nameStrDelimiter = _T( "'" );

	CString script;

	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Name is delimited by single quotes
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName();

	// Name is delimited by single quotes
	script += strIndent + objectTag + _T( ".DataLinkID" ) + nameStrDelimiter + _T( " = STRING " );

	script += _T( "[ " );

	const GUID dataLinkID = SVInvalidGUID;
	CString strDataLinkID;

	strDataLinkID.Format( _T("\"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\""),
		dataLinkID.Data1, 
		dataLinkID.Data2, 
		dataLinkID.Data3,
		dataLinkID.Data4[0], 
		dataLinkID.Data4[1], 
		dataLinkID.Data4[2], 
		dataLinkID.Data4[3],
		dataLinkID.Data4[4], 
		dataLinkID.Data4[5], 
		dataLinkID.Data4[6], 
		dataLinkID.Data4[7]
	);


	script += strDataLinkID + _T( " ]" );

	// Add termination...
	script += _T( ";" );

	script.FreeExtra();

	RStrScript += script;
}

/*
This method adds the friend information to the object data script privided by the parameters.
*/
void SVObjectClass::PutFriendGuidsInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString script;

	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 

	if( ! friendList.GetSize() )
	{
		return;
	}

	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Name is delimited by single quotes
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetName();

	// Name is delimited by single quotes
	script += strIndent + objectTag + _T( ".Friend" ) + nameStrDelimiter + _T( " = STRING " );

	if( friendList.GetSize() > 1 )
	{
		script += _T( "[ " );

		// for all friends in the list - get GUIDs
		for( size_t i = 0;i < friendList.GetSize();i++ )
		{
			GUID friendGuid = friendList[ i ].UniqueObjectID;
			CString guidStr;
			//CString guidStr = AfxStringFromCLSID( friendList[ i ].UniqueObjectID );

			guidStr.Format( _T("\"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\""),
				friendGuid.Data1, friendGuid.Data2, friendGuid.Data3,
				friendGuid.Data4[0], friendGuid.Data4[1], friendGuid.Data4[2], friendGuid.Data4[3],
				friendGuid.Data4[4], friendGuid.Data4[5], friendGuid.Data4[6], friendGuid.Data4[7]);


			if( i )
				script += _T( ", " );

			script += guidStr;
		}
		script += _T( " ]" );

	}
	else
	{
		GUID friendGuid = friendList[ 0 ].UniqueObjectID;
		CString guidStr;
		//CString guidStr = AfxStringFromCLSID( friendList[ i ].UniqueObjectID );

		guidStr.Format( _T("\"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\""),
			friendGuid.Data1, friendGuid.Data2, friendGuid.Data3,
			friendGuid.Data4[0], friendGuid.Data4[1], friendGuid.Data4[2], friendGuid.Data4[3],
			friendGuid.Data4[4], friendGuid.Data4[5], friendGuid.Data4[6], friendGuid.Data4[7]);

		script += guidStr;
	}
	// Add termination...
	script += _T( ";" );

	script.FreeExtra();
	RStrScript += script;
}

/*
Special Code to adjust the friend Aliases so they are Unique.

Note: At this time there can only be one friend class of the Same ClassID attached to an object.
*/
void SVObjectClass::MakeUniqueFriendAlias( CString& RStrScript )
{
	// Original Alias
	CString originalAlias = _T( "_object_ID_" );
	originalAlias += GetName();

	// New Alias
	CString newAlias = _T( "_object_ID_" );
	newAlias += GetOwner()->GetName();
	newAlias += _T( " " );
	newAlias += GetName();

	// Replace with '_object_ID_OwnerName.Name'
	RStrScript.Replace( originalAlias, newAlias );
}
#endif

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
	_variant_t value; 
	value.ChangeType(VT_UI4);

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

HRESULT SVObjectClass::GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index ) const
{
	HRESULT l_Status = S_OK;

	if( p_Index == ( p_rNameInfo.m_NameArray.size() - 1 ) )
	{
		SVObjectNameInfo::SVNameDeque::const_iterator l_Iter = p_rNameInfo.m_NameArray.begin();

		std::advance( l_Iter, p_Index );

		if( ( *l_Iter ) == GetName() )
		{
			p_rpObject = const_cast< SVObjectClass* >( this );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectClass.cpp_v  $
* 
*    Rev 1.10   08 Jan 2015 05:41:38   mEichengruen
* Project:  SVObserver
* Change Request (SCR) nbr:  980
* SCR Title:  Add Non-Inspection Objects to the Result View
* Checked in by:  mEichengruen;  Marcus Eichengruen
* Change Description:  
*   fix parametername to avoid compile error
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.9   07 Jan 2015 16:03:08   bwalter
* Project:  SVObserver
* Change Request (SCR) nbr:  980
* SCR Title:  Add Non-Inspection Objects to the Result View
* Checked in by:  mEichengruen;  Marcus Eichengruen
* Change Description:  
*   Changed method GetCompleteObjectNameToObjectType to avoid returning a name starting with ".".
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.8   20 Nov 2014 04:39:28   mziegler
* Project:  SVObserver
* Change Request (SCR) nbr:  918
* SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
* Checked in by:  mZiegler;  Marc Ziegler
* Change Description:  
*   change method GetAncestor to const and rearrange the code without changing the behavoiur.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.7   27 Jun 2014 08:05:06   mziegler
* Project:  SVObserver
* Change Request (SCR) nbr:  885
* SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
* Checked in by:  mZiegler;  Marc Ziegler
* Change Description:  
*   remove GetObjectDepth and GetObjectIcon
* made methods const: GetObjectColor, GetObjectState, GetCompleteObjectName2, GetCompleteObjectNameLength, GetOwnerInfo, GetObjectInfo and GetFriendList
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   15 May 2014 15:30:26   sjones
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Revised comment due to SVSendMessage using DWORD_PTR
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.5   15 May 2014 09:42:26   sjones
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Revised SVSendMessage to use DWORD_PTR instead of DWORD or LONG_PTR.
* Revised processMessage to use DWORD_PTR instead of DWORD or LONG_PTR.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   17 Mar 2014 14:14:10   bwalter
* Project:  SVObserver
* Change Request (SCR) nbr:  869
* SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Added Includes and Declarations regions.
*   Removed THIS_FILE.
*   Added RefreshObject method.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   05 Feb 2014 09:20:42   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Process Message to use LONG_PTR instead of DWORD
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.2   01 Feb 2014 10:09:14   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Changed sendmessage to use LONG_PTR instead of DWORD.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.1   01 Oct 2013 11:27:02   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Add x64 platform.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.0   22 Apr 2013 16:46:52   bWalter
* Project:  SVObserver
* Change Request (SCR) nbr:  814
* SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.31   10 Jan 2013 14:58:56   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  805, 801
* SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Merged 6.01 branch code into main development code base.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.30   07 Dec 2012 10:33:28   ryoho
* Project:  SVObserver
* Change Request (SCR) nbr:  801
* SCR Title:  Add new Shift Tool to SVObserver
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   New method for GetObjectValue
* new method for SetObjectValue
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.29   04 Sep 2012 13:24:08   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  602
* SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Added new parsing and saving functionality to object framework.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.28.1.1   08 Jan 2013 11:41:02   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  805
* SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Added new virtual method to Object Class to fix adjusting Object Depth of Value Object and Last Set Index.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.28.1.0   05 Dec 2012 11:48:06   ryoho
* Project:  SVObserver
* Change Request (SCR) nbr:  801
* SCR Title:  Add new Shift Tool to SVObserver
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   Added include for SVUtilityLibrary/SVSAFEARRAY.h
* New method for GetObjectValue
* new method for SetObjectValue
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.28   27 Jul 2012 08:00:00   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  769
* SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated the visitor fucntionality to make it more flexable without having to add new elements to the framework.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.27   25 Jul 2012 15:38:24   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  769
* SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated object class to not allow setting owner object to itself.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.26   18 Jul 2012 13:14:26   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  769
* SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Removed obsolete methods assiciated with overlay drawling.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.25   12 Jul 2012 14:03:44   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  769
* SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.24   02 Jul 2012 16:02:26   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  769
* SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated source code to promote new display functionality.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.23   18 Jun 2012 17:13:22   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  744
* SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
* Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.22   30 Jan 2012 10:33:56   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  748
* SCR Title:  Add Remote Output Steams to SVObserver
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   Updated object manager to reduce unnecessary methods.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.21   01 Mar 2011 10:05:12   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  712
* SCR Title:  Fix issues with black images when using command interface (SIAC)
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated walking objects functionality to use a recursive method and added a check for circular object walking.  This could cause an infinite execution loop.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.20   25 Feb 2011 11:59:36   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  712
* SCR Title:  Fix issues with black images when using command interface (SIAC)
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.19   07 Dec 2010 15:51:50   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  707
* SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated source code to include changes in notification functionality using the Observer Design Pattern.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.18   28 Oct 2010 13:35:54   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  704
* SCR Title:  Upgrade SVObserver Version for 5.33 Release
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.17   29 Jun 2010 13:58:20   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  693
* SCR Title:  Fix Performance Issue with Inspection Process
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated source to change object validity test.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.16   01 Jun 2010 08:29:46   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  693
* SCR Title:  Fix Performance Issue with Inspection Process
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated source code to remove duplicate definitions and redundunt containers.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.15   15 Feb 2010 10:38:02   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  682
* SCR Title:  Upgrade SVObserver version for 5.31 release
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated old and added missing source code comments to existing code.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.14   15 Dec 2009 12:55:06   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  677
* SCR Title:  Fix problem in camera notify thread
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated include information and comments.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.13   03 Sep 2009 09:11:20   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  665
* SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Fixed issues with moved header includes.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.12   30 Jul 2009 08:35:30   jspila
* Project:  SVObserver
* Change Request (SCR) nbr:  665
* SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Merged branced changes into current code branch with appropriate updates.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.11   20 Jun 2007 13:27:52   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.10   21 Sep 2005 07:35:48   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  500
* SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.9   14 Sep 2005 15:40:40   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  464
* SCR Title:  Add array indexing for value objects
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   updated to handle attributes for "entire array" and first element correctly
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.8   01 Sep 2005 15:50:00   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  510
* SCR Title:  Add source image extents to all image using tools
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Added check to prevent infinite recursion on owner calls.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.7   01 Aug 2005 10:17:24   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  464
* SCR Title:  Add array indexing for value objects
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   removed GetObjectDataTable
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   29 Jul 2005 07:54:42   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  464
* SCR Title:  Add array indexing for value objects
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   consolidated common constructor code in init()
* set m_uDefaultObjectAttributesSet in init() which is called from all constructors
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.5   28 Jul 2005 15:26:36   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  464
* SCR Title:  Add array indexing for value objects
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   added m_uDefaultObjectAttributesSet and SetDefaultObjectAttributesSet
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   14 Jul 2005 11:45:06   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  464
* SCR Title:  Add array indexing for value objects
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   inlined functions
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   20 Jun 2005 15:11:06   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  464
* SCR Title:  Add array indexing for value objects
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   BOOL --> SVHRESULT on SetObjectValue
* added array functionality
* const-corrected
* added accessors to object attributes
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.2   07 Mar 2005 11:44:30   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  262
* SCR Title:  Improve performance when loading a configuration in SVObserver
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Added pragma to remove waring message and added functionality to check for existance in object manager.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.1   17 Feb 2005 11:12:52   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  456
* SCR Title:  Update Image and Tool Objects to use the new Extent Classes
* Checked in by:  Joe;  Joe Spila
* Change Description:  
*   Removed methods that used old extents and Draw Context Object.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.0   15 Feb 2005 14:54:34   ryoho
* Project:  SVObserver
* Change Request (SCR) nbr:  440
* SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   First version of file for SVObjectLibrary
* 
* /////////////////////////////////////////////////////////////////////////////////////

Rev 3.36   12 Jul 2004 13:05:18   ebeyeler
Project:  SVObserver
Change Request (SCR) nbr:  406
SCR Title:  Implement External Tool
Checked in by:  eBeyeler;  Eric Beyeler
Change Description:  
fixed merge error

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.35   12 Jul 2004 13:04:12   ebeyeler
Project:  SVObserver
Change Request (SCR) nbr:  406
SCR Title:  Implement External Tool
Checked in by:  eBeyeler;  Eric Beyeler
Change Description:  
implemented SV_HIDDEN
fixed SV_GET_KIND_OF
added copy constructor / operator = to SVInObjectInfoStruct

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.34   12 Jul 2004 13:01:40   ebeyeler
Project:  SVObserver
Change Request (SCR) nbr:  428
SCR Title:  Improve Load Configuration time
Checked in by:  eBeyeler;  Eric Beyeler
Change Description:  
implemented GetCompleteObjectName optimization

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.33   04 Dec 2003 12:59:10   ryoho
Project:  SVObserver
Change Request (SCR) nbr:  397
SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
Checked in by:  rYoho;  Rob Yoho
Change Description:  
changed SVInObjectInfoStruct to have a const copy constructor

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.32   22 Apr 2003 11:35:54   rschock
Project:  SVObserver
Change Request (SCR) nbr:  346
SCR Title:  Update SVObserver to Version 4.21 Release
Checked in by:  Joe;  Joe Spila
Change Description:  
Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.31   25 Feb 2003 15:50:50   joe
Project:  SVObserver
Change Request (SCR) nbr:  301
SCR Title:  Implement Result Image Circle Buffer
Checked in by:  rYoho;  Rob Yoho
Change Description:  
Updated GetAncestor method to verify that pointer is not NULL.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.30   30 Jan 2003 15:08:24   joe
Project:  SVObserver
Change Request (SCR) nbr:  301
SCR Title:  Implement Result Image Circle Buffer
Checked in by:  Joe;  Joe Spila
Change Description:  
Added SetImageDepth and GetImageDepth methods to class.  Updated constructor to initialize mlImageDepth attribute.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.29   10 Jan 2003 17:49:50   rschock
Project:  SVObserver
Change Request (SCR) nbr:  305
SCR Title:  Implement the ability to perform RunOnce from a SIAC client
Checked in by:  rSchock;  Rosco Schock
Change Description:  
forgot the = sign for the const UINT message defines

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.28   10 Jan 2003 15:45:56   rschock
Project:  SVObserver
Change Request (SCR) nbr:  305
SCR Title:  Implement the ability to perform RunOnce from a SIAC client
Checked in by:  rSchock;  Rosco Schock
Change Description:  
Added new function VerifyImageForOverlay to determine which object has overlays on which image.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.27   17 Dec 2002 15:20:22   ebeyeler
Project:  SVObserver
Change Request (SCR) nbr:  299
SCR Title:  SIAC needs to be able to request Source Images
Checked in by:  eBeyeler;  Eric Beyeler
Change Description:  
BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.26   09 Dec 2002 19:43:14   Joe
Project:  SVObserver
Change Request (SCR) nbr:  295
SCR Title:  Remove Result Data from Configuration Printout
Checked in by:  Joe;  Joe Spila
Change Description:  
CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.25   20 Nov 2002 09:10:44   ryoho
Project:  SVObserver
Change Request (SCR) nbr:  226, 272
SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
Checked in by:  rYoho;  Rob Yoho
Change Description:  
Changed TheClassRegister -> TheSVClassRegister
Changed TheObjectManager -> TheSVObjectManager
Added TheSVDataManager
Removed references to PPQ
removed references to SVObjectInfoStruct
processMessage
Added support for SVMSGID_GET_OBJECT_BY_NAME

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.24   19 Apr 2001 14:47:22   Steve
Project:  SVObserver
Change Request (SCR) nbr:  196
SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
Checked in by:  Steve;  Stephen E. Steffan
Change Description:  
Changes due to addition of the SVFileNameObjectClass and new GUID for the class.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.23   06 Sep 2000 16:31:40   Joe
Project:  SVObserver
Change Request (SCR) nbr:  179
SCR Title:  Create Color SVObserver
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated files to include color SVIM changes.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.22   11 May 2000 08:56:02   sjones
Project:  SVObserver
Change Request (SCR) nbr:  139
SCR Title:  Math Tool Invalid State
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised SVM_CREATE_ALL_CLOSED_OBJECTS to
also route this message to friend objects.

Added IsDescendantOfType method.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.21   07 May 2000 23:29:56   Jim
Project:  SVObserver
Change Request (SCR) nbr:  137
SCR Title:  Remove the need to explicitly reference specific version numbers.
Checked in by:  JimAdmin;  James A. Brown
Change Description:  
Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.


/////////////////////////////////////////////////////////////////////////////////////

Rev 3.20   26 Apr 2000 16:13:16   sjones
Project:  SVObserver
Change Request (SCR) nbr:  129
SCR Title:  Angular Profile Tool
Checked in by:  sJones;  Steve Jones
Change Description:  
Added GetImageInputContextGuid method.
Added new message 
SVM_SET_SHADOWED_ABSEXTENT.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.19   19 Apr 2000 16:32:18   Jim
Project:  SVObserver
Change Request (SCR) nbr:  128
SCR Title:  Suport for 3.11 versioning
Checked in by:  JimAdmin;  James A. Brown
Change Description:  
SVObjectInfoStruct::Serialize () and 
SVObjectClass::Serialize () were both changed to support program version number 3.11 beta 1.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.18   11 Apr 2000 17:45:54   Jim
Project:  SVObserver
Change Request (SCR) nbr:  121
SCR Title:  Support for 3.06 versioning
Checked in by:  JimAdmin;  James A. Brown
Change Description:  
Changed SVObjectInfoStruct::Serialize () and
SVObjectClass::Serialize () to support program version number 3.06.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.17   23 Mar 2000 14:03:12   sjones
Project:  SVObserver
Change Request (SCR) nbr:  117
SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
Checked in by:  sJones;  Steve Jones
Change Description:  
Introduced new SetDisabled() member function.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.16   14 Mar 2000 14:42:56   sjones
Project:  SVObserver
Change Request (SCR) nbr:  101
SCR Title:  Versioning 3.10
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised version number to 3.10 release (0x00030AFF)

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.15   09 Mar 2000 08:52:28   sjones
Project:  SVObserver
Change Request (SCR) nbr:  101
SCR Title:  Versioning 3.10
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised Version number from 3.10 Beta 3 to 3.10 Beta 4

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.14   07 Mar 2000 14:07:42   sjones
Project:  SVObserver
Change Request (SCR) nbr:  101
SCR Title:  Versioning 3.10
Checked in by:  sJones;  Steve Jones
Change Description:  
Changed Version number to 3.1 Beta 3.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.13   25 Feb 2000 16:07:08   sjones
Project:  SVObserver
Change Request (SCR) nbr:  105
SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
Checked in by:  sJones;  Steve Jones
Change Description:  
Added new method GetTransform.
Added new message SVM_GET_CONTEXT_EXTENT.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.12   21 Feb 2000 14:51:50   sjones
Project:  SVObserver
Change Request (SCR) nbr:  105
SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
Checked in by:  sJones;  Steve Jones
Change Description:  
Added checkDrawAllowed method.
Added comments for Move/Size messages to detail what is passed.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.11   Feb 21 2000 10:45:08   Nick
Project:  SVObserver
Change Request (SCR) nbr:  109
SCR Title:  Freeze on Reject for v3.1
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Versioning to v3.10 Beta 2.
Add new Macro for Type Safe Casting.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.10   16 Feb 2000 17:00:48   mike
Project:  SVObserver
Change Request (SCR) nbr:  105
SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
Checked in by:  sJones;  Steve Jones
Change Description:  
Modified Draw functions to use the DrawContext structure
and removed unused methods.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.9   07 Feb 2000 20:44:36   robert
Project:  SVObserver
Change Request (SCR) nbr:  93
SCR Title:  Integrate Multiple Camera Image Input per IPD
Checked in by:  Robert;  Robert Massinger
Change Description:  
Added new Object function GetObjectIcon() which can be overwritten and be used as generic 'get icon func' for any kind of list or tree icon.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.8   04 Feb 2000 13:31:36   sjones
Project:  SVObserver
Change Request (SCR) nbr:  101
SCR Title:  Versioning 3.10
Checked in by:  sJones;  Steve Jones
Change Description:  
Changed version to 3.10

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.7   Feb 03 2000 16:33:20   Nick
Project:  SVObserver
Change Request (SCR) nbr:  102
SCR Title:  Versioning v3.05
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changed version from v3.04 to v3.05.  Corrected multiple inheritance order in System.h.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.6   Jan 19 2000 15:18:50   Nick
Project:  SVObserver
Change Request (SCR) nbr:  88
SCR Title:  Version 3.04 versioning changes.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes required to change v3.04 Beta 1 to v3.04 Released.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.5   Jan 14 2000 15:15:04   Nick
Project:  SVObserver
Change Request (SCR) nbr:  85
SCR Title:  Version 3.04 Beta 1 versioning changes.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes required for v3.03 to v3.04 Beta 1.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.4   Jan 04 2000 08:58:24   Nick
Project:  SVObserver
Change Request (SCR) nbr:  81
SCR Title:  Version 3.03 versioning changes
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes required to convert v3.02 to v3.03.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.3   Dec 02 1999 08:36:36   Nick
Project:  SVObserver
Change Request (SCR) nbr:  72
SCR Title:  Version 3.02 versioning changes.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Change v3.01 to v3.02.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.2   Dec 02 1999 08:32:34   Nick
Project:  SVObserver
Change Request (SCR) nbr:  57
SCR Title:  Version 3.00 Beta 18 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changed v3.01 to v3.02.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.1   Nov 30 1999 15:48:14   Nick
Project:  SVObserver
Change Request (SCR) nbr:  67
SCR Title:  Version 3.01 versioning changes.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes for v3.00 to v3.01 versioning.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.0   23 Nov 1999 13:18:38   mike
Project:  SVObserver
Change Request (SCR) nbr:  61
SCR Title:  Update PVCS versioning to version 3.0.
Checked in by:  Mike;  Mike McCarl
Change Description:  


/////////////////////////////////////////////////////////////////////////////////////

Rev 1.46   Nov 18 1999 09:05:46   Nick
Project:  SVObserver
Change Request (SCR) nbr:  59
SCR Title:  Version 3.00 Release Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
v3.00 Release versioning changes.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.45   Nov 10 1999 12:21:02   Nick
Project:  SVObserver
Change Request (SCR) nbr:  57
SCR Title:  Version 3.00 Beta 18 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes v3.00 Beta 17 to v3.00 Beta 18.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.44   Nov 05 1999 11:18:34   Nick
Project:  SVObserver
Change Request (SCR) nbr:  56
SCR Title:  Version 3.00 Beta 17 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
v3.00 Beta 16 to v3.00 Beta 17.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.43   Nov 02 1999 09:09:18   Nick
Project:  SVObserver
Change Request (SCR) nbr:  55
SCR Title:  Version 3.00 Beta 16 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes v3.00 Beta 15 to v3.00 Beta 16.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.42   Nov 02 1999 08:59:08   Nick
Project:  SVObserver
Change Request (SCR) nbr:  55
SCR Title:  Version 3.00 Beta 16 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.41   Nov 01 1999 16:57:34   Nick
Project:  SVObserver
Change Request (SCR) nbr:  54
SCR Title:  Version 3.00 Beta 15 Versioning.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
v3.00 Beta 14 to v3.00 Beta 15

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.40   Oct 29 1999 17:58:50   steve
Added function GetFriendList () to allow the Print File function to get a list of all "friends" for the PrintObject () function in SVObserverApp

Rev 1.39   28 Oct 1999 08:42:24   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised GetObjectScript to address performance issues.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.37   Oct 22 1999 09:02:02   Nick
Project:  SVObserver
Change Request (SCR) nbr:  51
SCR Title:  Version 3.00 Beta 13 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
v3.00 Beta 12 to v3.00 Beta 13.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.36   Oct 20 1999 16:06:40   Nick
Project:  SVObserver
Change Request (SCR) nbr:  50
SCR Title:  Version 3.00 Beta 12 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changed version from v3.00 Beta 11 to v3.00 Beta 12.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.35   Oct 18 1999 12:50:06   Nick
Project:  SVObserver
Change Request (SCR) nbr:  47
SCR Title:  Version 3.00 Beta 11 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes for v3.00 Beta 10 to v3.00 Beta 11.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.34   Oct 18 1999 11:53:20   Nick
Project:  SVObserver
Change Request (SCR) nbr:  35
SCR Title:  PPQ mode to wait for input data
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
RO - Bug fix - Added DataLinkID scripting.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.33   14 Oct 1999 17:00:12   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Added HasDependents method to SVOutputInfoListClass.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.32   Oct 13 1999 15:37:12   Nick
Project:  SVObserver
Change Request (SCR) nbr:  46
SCR Title:  Version 3.00 Beta 10 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Version v3.00 Beta 9 to v3.00 Beta 10 changes.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.31   Oct 13 1999 15:14:24   Nick
Project:  SVObserver
Change Request (SCR) nbr:  21
SCR Title:  Port Gage Tool code from program version 2.42.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Roberts bug fixes for Beta 10.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.30   Oct 12 1999 08:16:50   Nick
Project:  SVObserver
Change Request (SCR) nbr:  44
SCR Title:  Version 3.00 Beta 9 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Changes required to change v3.00 Beta 8 to v3.00 Beta 9.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.29   Oct 08 1999 10:54:52   Nick
Project:  SVObserver
Change Request (SCR) nbr:  43
SCR Title:  Version 3.00 Beta 7 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Versioning changes to Beta 8.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.28   Oct 04 1999 11:32:12   Nick
Project:  SVObserver
Change Request (SCR) nbr:  43
SCR Title:  Version 3.00 Beta 7 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
V3.00 Beta 7 versioning.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.27   Oct 04 1999 09:36:20   Nick
Project:  SVObserver
Change Request (SCR) nbr:  33
SCR Title:  Port Profile Tool from v2.42
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Completed FriendList locks.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.26   Sep 29 1999 14:21:00   Nick
Project:  SVObserver
Change Request (SCR) nbr:  41
SCR Title:  Version 3.00 Beta 5 Versioning.
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
v3.00 Beta 5

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.25   Sep 27 1999 19:04:40   Nick
Project:  SVObserver
Change Request (SCR) nbr:  38
SCR Title:  Version 3.00 Beta 4 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Version from 0x00030003 to 0x00030004.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.24   23 Sep 1999 16:25:44   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Added message SVM_DESTROY_FRIEND_OBJECT.
Revised DestroyFriends to send 
SVM_DESTROY_FRIEND_OBJECT instead of
SVM_DESTROY_CHILD_OBJECTS

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.23   Sep 23 1999 13:53:52   Nick
Project:  SVObserver
Change Request (SCR) nbr:  37
SCR Title:  Version 3.00 Beta 3 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
New version specific changes to Serialize methods.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.22   22 Sep 1999 18:35:50   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised to fix restoration of friends from script
(Added method to adjust friend aliases)

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.21   Sep 21 1999 09:29:18   Nick
Project:  SVObserver
Change Request (SCR) nbr:  36
SCR Title:  Version 3.00 Beta 2 Versioning
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Added 0x00030002 (v3.00 Beta 2) case statement  to all serialization methods.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.20   Sep 20 1999 15:36:46   Nick
Project:  SVObserver
Change Request (SCR) nbr:  35
SCR Title:  PPQ mode to wait for input data
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Introduced new broadcast message router.
Added new messages: Create_All_Closed_Objects.
Notify_About_Creation.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.19   Sep 16 1999 12:38:00   Nick
Project:  SVObserver
Change Request (SCR) nbr:  34
SCR Title:  Add Build Reference Tool
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Added Product Key Struct and other from SVPPQ.h.
Added GetAncestor(ObjectType) and IsDescendantOf( PAncestorObject)

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.18   10 Sep 1999 19:35:02   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Fixed bug in PutAttributesInObjectScript and
PutFriendGuidsInObjectScript. 
(changed GetObjectName() to GetName, to support
objects named different than the class name)

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.17   04 Sep 1999 17:24:34   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised to save and restore uObjectAttributesAllowed and
uObjectAttributesSet via ObjectScript.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.16   02 Sep 1999 08:44:44   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Add Message Qualifiers to better control routing of messages.
Add new messsages for drawing/figures.
Fixed Bug in CloseObject (Friends recurrsion)
Add published attribute and defaults on construction of SVObjectClass..
Added GetObjectColor and GetObjectState

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.15   01 Sep 1999 11:47:54   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised versioning to 3.0 Beta 1

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.14   Aug 31 1999 09:34:52   Nick
Project:  SVObserver
Change Request (SCR) nbr:  27
SCR Title:  Results Picker
Checked in by:  Nick;  F Roland "Nick" Bjorklund
Change Description:  
Fixed bug in GetAllowedAttributes() method.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.13   30 Aug 1999 19:31:44   robert
Project:  SVObserver
Change Request (SCR) nbr:  21
SCR Title:  Port Gage Tool code from program version 2.42.
Checked in by:  Robert;  Robert Massinger
Change Description:  
Introduced Input Interface Handling.
( GETFIRST - Message Handler )

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.12   30 Aug 1999 19:15:30   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  sJones;  Steve Jones
Change Description:  
Revised SV_IS_KIND_OF not to call SV_VALIDATE_OBJECT.
Revised SVM_CREATE_ALL_OBJECTS to route to Children only.
Added Scripting support for Freinds Architecure.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.11   27 Aug 1999 12:58:48   robert
Project:  SVObserver
Change Request (SCR) nbr:  21
SCR Title:  Port Gage Tool code from program version 2.42.
Checked in by:  Robert Massinger
Change Description:  
Added SetupDialog() Function to SVObjectClass and Message.
This gives every derived class the possibility to support his own Setup Dialog.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.10   27 Aug 1999 09:06:26   sjones
Project:  SVObserver
Change Request (SCR) nbr:  9
SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
Checked in by:  Steve Jones
Change Description:  
Add capability to script freind links.

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.9   Aug 25 1999 22:50:12   sjones
Project:  SVObserver
Change Request (SCR) nbr:  1
SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
Checked in by:  Steve Jones
Change Description:  
Revised Validate method to be OnValidate method.
( OnValidate - local scope validation).
Added Validate method (Routes to all owned objects)

/////////////////////////////////////////////////////////////////////////////////////

Rev 1.8   Aug 25 1999 16:43:32   Nick
Project:  SVObserver
Change Request (SCR) nbr:  27
SCR Title:  Results Picker
Checked in by:  F Roland "Nick" Bjorklund
Change Description:  


/////////////////////////////////////////////////////////////////////////////////////

Rev 1.7   25 Aug 1999 11:36:14   robert
Project:  SVObserver
Change Request (SCR) nbr:  21
SCR Title:  Port Gage Tool code from program version 2.42.
Checked in by:  Robert Massinger
Change Description:  
Added SVObjectInfoArrayClass.
Added Friend Mechanism.

/////////////////////////////////////////////////////////////////////////////////////
*/
