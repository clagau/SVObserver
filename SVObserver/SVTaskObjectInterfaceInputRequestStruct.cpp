//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceInputRequestStruct
//* .File Name       : $Workfile:   SVTaskObjectInterfaceInputRequestStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:26:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVTaskObjectInterfaceInputRequestStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( GUID p_guid )
{
	guid = p_guid;
	ref = SVValueObjectReference( SVObjectManagerClass::Instance().GetObject( guid ) );
	strName = ref.GetCompleteObjectName();
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const CString& p_strName )
{
	strName = p_strName;
	ref = SVObjectManagerClass::Instance().GetObjectReference( strName );
	guid = ref.Guid();
}

HRESULT SVInputRequestStructMap::Add( SVValueObjectClass* p_pValueObject )
{
	ASSERT( p_pValueObject );
	HRESULT hr = S_FALSE;
	if ( p_pValueObject )
	{
		CString strValue;
		hr = p_pValueObject->GetValue( strValue );
		if ( hr == S_OK )
		{
			(*this)[ SVValueObjectReference( p_pValueObject ) ] = strValue;
		}
	}
	return hr;
}

HRESULT SVInputRequestStructMap::Add( SVValueObjectReference p_refValueObject )
{
	ASSERT( p_refValueObject.Object() );
	HRESULT hr = S_FALSE;
	if ( p_refValueObject.Object() )
	{
		CString strValue;
		hr = p_refValueObject.GetValue( strValue );
		if ( hr == S_OK )
		{
			(*this)[ p_refValueObject ] = strValue;
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTaskObjectInterfaceInputRequestStruct.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:26:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2010 13:41:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 14:16:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 13:12:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Aug 2005 10:21:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
