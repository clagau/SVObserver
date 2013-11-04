//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceInputRequestStruct
//* .File Name       : $Workfile:   SVTaskObjectInterfaceInputRequestStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:33:52  $
//******************************************************************************

#ifndef SVTASKOBJECTINTERFACEINPUTREQUESTSTRUCT_H
#define SVTASKOBJECTINTERFACEINPUTREQUESTSTRUCT_H

#include "SVValueObjectReference.h"

struct SVTaskObjectInterfaceInputRequestStruct
{
	SVValueObjectReference ref;
	GUID guid;
	CString strName;

	SVTaskObjectInterfaceInputRequestStruct();
	SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref, GUID p_guid, const CString& p_strName );
	SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref );
	SVTaskObjectInterfaceInputRequestStruct( GUID p_guid );
	SVTaskObjectInterfaceInputRequestStruct( const CString& p_strName );

	bool operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const;
};

struct SVInputRequestStructMap : public std::map <SVTaskObjectInterfaceInputRequestStruct, CString>
{
	HRESULT Add( SVValueObjectClass* p_pValueObject );
	HRESULT Add( SVValueObjectReference p_refValueObject );
};

#include "SVTaskObjectInterfaceInputRequestStruct.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTaskObjectInterfaceInputRequestStruct.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:33:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
 *    Rev 1.0   17 Aug 2005 10:21:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
