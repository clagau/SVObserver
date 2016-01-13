//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRunStatus
//* .File Name       : $Workfile:   SVRunStatus.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:21:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVRunStatus.h"
#include "SVRunControlLibrary.h"

SVRunStatusClass::SVRunStatusClass()
{
	ResetRunStatus(); 
	m_lResultDataIndex  = 1;
	m_lTriggerCount = -1;
}


void SVRunStatusClass::ResetRunStatus()
{
	ResetRunStateAndToolSetTimes();
	ResetTriggerInformation();
}


void SVRunStatusClass::ResetTriggerInformation()
{
	m_PreviousTriggerTime = 0.0;
	m_CurrentTriggerTime = 0.0;

	m_WorkLoadCurrentProduct.Reset();
}



void SVRunStatusClass::ResetRunStateAndToolSetTimes()
{ 
	run.state = SV_INVALID;

	run.status.valid = true;

	m_UpdateCounters = true;

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;
}

DWORD SVRunStatusClass::GetStatusColor()
{
	if( IsDisabled() || IsDisabledByCondition() )
		return SV_DEFAULT_DISABLED_COLOR;

	return ( IsFailed() ? SV_DEFAULT_FAILED_COLOR : 
				( IsWarned() ? SV_DEFAULT_WARNED_COLOR : SV_DEFAULT_GOOD_COLOR ) );
}



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVRunControlLibrary\SVRunStatus.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:21:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Mar 2013 12:38:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the AddError method and the error list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Feb 2013 11:17:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added m_ErrorList to the RunStatus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jul 2012 16:06:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Apr 2011 13:28:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated run status object to include a flag for updating counters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:38:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   File moved to here to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   14 Jul 2009 15:10:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   02 Nov 2005 09:02:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed long m_lResultImageIndex to SVImageIndexStruct Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   02 Sep 2005 14:33:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made speed optimizations by making functions inline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 May 2005 07:28:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  486
 * SCR Title:  Add Trigger Count to Tool Set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add trigger count value object to tool set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 15:12:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 15:12:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   14 Feb 2003 16:59:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with RunStatus resetting the wrong things. Changed SVIPDoc to do its own resetting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   05 Feb 2003 17:30:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   20 Nov 2002 13:16:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:37:30   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Sep 1999 16:20:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetWarned not to call SetFailed. (sticky bit problem)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Sep 1999 17:34:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ClearAll() to clear entire state.
 * Revised SetWarned to checked if failed already set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Aug 1999 18:59:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   New Class to control run status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
