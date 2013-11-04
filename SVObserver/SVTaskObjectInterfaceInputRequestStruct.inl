//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceInputRequestStruct
//* .File Name       : $Workfile:   SVTaskObjectInterfaceInputRequestStruct.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:34:06  $
//******************************************************************************

#include "SVUtilityLibrary/SVUtilityGlobals.h"

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct()
{
	guid = SVInvalidGUID;
}

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref, GUID p_guid, const CString& p_strName )
{
	ref = p_ref;
	guid = p_guid;
	strName = p_strName;
}

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref )
{
	ref = p_ref;
	if ( ref.Object() )
	{
		guid = ref.Guid();
		strName = ref.GetCompleteObjectName();
	}
}

inline bool SVTaskObjectInterfaceInputRequestStruct::operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const
{
	if ( ref.Object() && rhs.ref.Object() )
	{
		return ref < rhs.ref;
	}
	else if ( guid != SVInvalidGUID && rhs.guid != SVInvalidGUID )
	{
		return guid < rhs.guid;
	}
	else
	{
		return strName < rhs.strName;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTaskObjectInterfaceInputRequestStruct.inl_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:34:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 14:16:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Aug 2005 10:21:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

