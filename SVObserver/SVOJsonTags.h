//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOJsonTags
//* .File Name       : $Workfile:   SVOJsonTags.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:08:38  $
//******************************************************************************
#ifndef INCL_SVOJSONTAGS_H
#define INCL_SVOJSONTAGS_H

#include "SVUtilityLibrary/SVString.h"

// Note: All Json tags are meant to be camelcase (first letter in each word capitalized) with no underscores
namespace SVOJsonTags
{
	static const SVString scProductMonitorListTag = _T("ProductMonitorList");
	static const SVString scLastInspectedImageMonitorListTag = _T("LastInspectedImageMonitorList");
	static const SVString scInspectionsTag = _T("Inspections");
	static const SVString scLastInspectedValuesTag = _T("LastInspectedValues");
	static const SVString scLastInspectedImagesTag = _T("LastInspectedImages");
	static const SVString scRejectValuesTag = _T("RejectValues");
	static const SVString scRejectImagesTag = _T("RejectImages");
	static const SVString scConditionalValuesTag = _T("ConditionalValues");
	static const SVString scNameTag = _T("Name");
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOJsonTags.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:08:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 15:24:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files associated with socket and shared memory functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
