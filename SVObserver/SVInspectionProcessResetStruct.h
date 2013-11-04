//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcessResetStruct
//* .File Name       : $Workfile:   SVInspectionProcessResetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:02:48  $
//******************************************************************************

#ifndef SVINSPECTIONPROCESSRESETSTRUCT_H
#define SVINSPECTIONPROCESSRESETSTRUCT_H

#include <map>
#include "SVObjectLibrary/SVObjectClass.h"

struct SVInspectionProcessResetStruct
{
	SVInspectionProcessResetStruct();
	SVInspectionProcessResetStruct( const SVInspectionProcessResetStruct &p_rsvObject );

	const SVInspectionProcessResetStruct &operator=( const SVInspectionProcessResetStruct &p_rsvObject );

	SVResetItemEnum SetResetData( SVResetItemEnum p_eResetItem, SVObjectClass *p_psvObject );

	SVResetItemEnum m_eResetItem;

	SVStdSetSVObjectClassPtr m_svObjectSet;
};

typedef std::map< SVToolClass *, SVInspectionProcessResetStruct > SVStdMapSVToolClassPtrSVInspectionProcessResetStruct;

#include "SVInspectionProcessResetStruct.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInspectionProcessResetStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:02:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 11:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Aug 2005 09:42:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new structure to hold reset information to speed up the reset process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
