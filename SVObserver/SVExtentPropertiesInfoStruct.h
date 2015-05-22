//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPropertiesInfoStruct
//* .File Name       : $Workfile:   SVExtentPropertiesInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:30:12  $
//******************************************************************************


#pragma once

struct SVExtentPropertyInfoStruct
{
	bool bFormula;
	bool bHidden;
	bool bSetByReset; //< property will be set in ToolSizeAdjustTask::ResetObject()

	SVExtentPropertyInfoStruct() : bFormula(false), bHidden(false), bSetByReset(false) {}
};



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExtentPropertiesInfoStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:30:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2005 14:26:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element for hiding rotation angle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:09:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/