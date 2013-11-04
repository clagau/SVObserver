// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIServerStruct
// * .File Name       : $Workfile:   SVCIServerStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:08  $
// ******************************************************************************

#ifndef SVCISERVERSTRUCT_H
#define SVCISERVERSTRUCT_H

#include <corapi.h>
#include "SVContainerLibrary/SVVector.h"

/*
This class is the data class for each server available for use.
*/
struct SVCIServerStruct  
{
public:
	SVCIServerStruct();
	SVCIServerStruct( const SVCIServerStruct &p_rValue );
	virtual ~SVCIServerStruct();

	const SVCIServerStruct &operator=( const SVCIServerStruct &p_rValue );

	//This attribute holds the server name.
	CString m_csName;

	//This attribute holds the Coreco server handle.
	CORSERVER m_hCorServer;
};

/*
This specialization of the CArray template creates a container object to hold the SVCIServerStruct objects.
*/
typedef SVVector< SVCIServerStruct > SVCIServerStructCArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCIServerStruct.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 15:38:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 08:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
