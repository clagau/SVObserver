//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectClass
//* .File Name       : $Workfile:   SVObjectClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Dec 2014 02:45:14  $
//******************************************************************************

#ifndef SVOBJECTCLASS_INL
#define SVOBJECTCLASS_INL

/*
This method returns the object's Global Unique Identifier (GUID) of this object instance.
*/
inline const SVGUID& SVObjectClass::GetUniqueObjectID() const
{
	return outObjectInfo.UniqueObjectID;
}

/*
This method returns the embedded Global Unique Identifier (GUID) if the object is embedded in another object.
*/
inline const SVGUID& SVObjectClass::GetEmbeddedID() const
{
	return embeddedID;
}

/*
This method returns the object's Global Unique Identifier (GUID) of this object's owner.
*/
inline const SVGUID& SVObjectClass::GetOwnerID() const
{
	return ownerObjectInfo.UniqueObjectID;
}

#pragma region virtual method (IObjectClass)
/*
This method returns the object's type.
*/
inline const SVObjectTypeEnum& SVObjectClass::GetObjectType() const
{
	return outObjectInfo.ObjectTypeInfo.ObjectType;
}
#pragma endregion virtual method (IObjectClass)

/*
This method returns the object's subtype.
*/
inline long SVObjectClass::GetObjectSubType() const
{
	return outObjectInfo.ObjectTypeInfo.SubType;
}

/*
This metthod return the pointer to the owner object.
*/
inline SVObjectClass* SVObjectClass::GetOwner() const
{
	return ownerObjectInfo.PObject;
}

/*
This method return the owner object info.
*/
inline const SVObjectInfoStruct& SVObjectClass::GetOwnerInfo() const
{
	return ownerObjectInfo;
}

/*
This method returns the object info of this object instance.
*/
inline const SVObjectInfoStruct& SVObjectClass::GetObjectInfo() const
{
	return outObjectInfo;
}

/*
This method returns the out object info of this object instance.  This method is only valid for data object's used as task output.
*/
inline SVOutObjectInfoStruct& SVObjectClass::GetObjectOutputInfo()
{
	return outObjectInfo;
}

/*
This method returns the object's image depth.
*/
inline long SVObjectClass::GetImageDepth() const
{
	return mlImageDepth;
}

/*
This method return the object's data depth.
*/
inline int SVObjectClass::GetObjectDepth() const
{
	return objectDepth;
}

/*
This method returns the creation state of the object.
*/
inline BOOL SVObjectClass::IsCreated() const
{
	return isCreated;
}

/*
This method returns the object's valid state.
*/
inline BOOL SVObjectClass::IsValid()
{
	return isObjectValid;
}

template< typename SVObjectVisitor >
HRESULT SVObjectClass::Accept( SVObjectVisitor& p_rVisitor )
{
	HRESULT l_Status = S_OK;

	l_Status = p_rVisitor.VisitElement( *this );

	return l_Status;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectClass.inl_v  $
 * 
 *    Rev 1.3   19 Dec 2014 02:45:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Method added: GetObjectDepth
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2014 08:05:04   mziegler
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
 *    Rev 1.1   17 Mar 2014 14:15:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Made GetOwner() const.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:47:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jul 2012 08:00:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the visitor fucntionality to make it more flexable without having to add new elements to the framework.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Jun 2012 17:13:22   jspila
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
 *    Rev 1.4   07 Dec 2010 15:51:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 10:38:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2009 08:35:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jul 2009 10:18:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update const issues with object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Jul 2005 11:36:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration; moved functions from SVObjectClass.cpp for speed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
