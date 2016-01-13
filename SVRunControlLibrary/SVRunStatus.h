//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRunStatus
//* .File Name       : $Workfile:   SVRunStatus.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:21:36  $
//******************************************************************************

#ifndef SVRUNSTATUS_H
#define SVRUNSTATUS_H

#include "SVImageIndexStruct.h"
#include "ProductWorkloadInformation.h"
#include "SVUtilityLibrary/SVGUID.h"
#include <deque>


class SVRunStatusClass
{
public:
	enum
	{
		SV_INVALID = 0,
	};

private:
	union runstateUnion
	{
		struct statusStruct
		{
			unsigned int passed					     : 1;	// everything is OK!
			unsigned int failed					     : 1;	// ran, but failed
			unsigned int warned					     : 1;	// ran, but warned
			unsigned int valid               : 1;	// did anything say it was not valid
			unsigned int run                 : 1;	// did it run ? (inspection started)
			unsigned int disabled            : 1;	// not enabled ?
			unsigned int disabledByCondition : 1;	// disabled because of condition
			unsigned int criticalFailure     : 1;	// a critical error occurred and will NAK the inspection
		} status;
		unsigned int state;		// did anything run ?
	} run;

public:
	SVRunStatusClass();

	void ResetRunStatus();
	void ResetRunStateAndToolSetTimes(); 
	void ResetTriggerInformation(); ///< clears trigger-related information

	void SetPassed();
	void SetFailed();
	void SetWarned();
	void SetInvalid();
	void SetValid();
	void SetInspectionStarted( bool state );
	void SetDisabled();
	void SetDisabledByCondition();
	void SetCriticalFailure();
	
	bool IsPassed();
	bool IsFailed();
	bool IsWarned();
	bool IsValid();

	bool IsDisabled();
	bool IsDisabledByCondition();
	bool IsCriticalFailure();
	bool HasInspectionStarted();

	DWORD GetState();
	DWORD GetStatusColor();

	long m_lResultDataIndex;
	SVImageIndexStruct Images;
	long m_lTriggerCount;	// TB
	bool m_UpdateCounters;

	double m_ToolSetEndTime;
	double m_ToolSetAvgTime;

	double m_PreviousTriggerTime; ///< time stamp at which the previous Inspection was started
	double m_CurrentTriggerTime; ///< time stamp at which the current Inspection was started

	ProductWorkloadInformation m_WorkLoadCurrentProduct;
};

inline void SVRunStatusClass::SetPassed() 
{ 
	run.status.passed = ( ( !run.status.failed && !run.status.warned && run.status.valid ) ? true : false );
}

inline void SVRunStatusClass::SetFailed() 
{ 
	run.status.failed = true; 
	run.status.warned = false; 
	run.status.passed = false; 
}

inline void SVRunStatusClass::SetWarned() 
{ 
	run.status.warned = ( ( !run.status.failed ) ? true : false ); 
}

inline void SVRunStatusClass::SetInvalid() 
{ 
	run.status.valid = false; 
}

inline void SVRunStatusClass::SetValid() 
{ 
	run.status.valid = true;
}

inline void SVRunStatusClass::SetDisabled() 
{ 
	run.status.disabled = true;
}

inline void SVRunStatusClass::SetDisabledByCondition() 
{ 
	run.status.disabledByCondition = true;
}

inline void SVRunStatusClass::SetCriticalFailure() 
{ 
	run.status.criticalFailure = true;
}

inline void SVRunStatusClass::SetInspectionStarted( bool state ) 
{ 
	run.status.run = state;
}

inline bool SVRunStatusClass::IsPassed() 
{ 
	return ( run.status.passed && !run.status.failed && !run.status.warned && run.status.valid );
}

inline bool SVRunStatusClass::IsFailed() 
{ 
	return ( run.status.failed && !run.status.warned && run.status.valid ); 
}

inline bool SVRunStatusClass::IsWarned() 
{ 
	return ( run.status.warned && run.status.valid ); 
}

inline bool SVRunStatusClass::IsValid()  
{ 
	return run.status.valid; 
}

inline bool SVRunStatusClass::IsDisabled()
{
	return run.status.disabled;
}

inline bool SVRunStatusClass::IsDisabledByCondition()
{
	return run.status.disabledByCondition;
}

inline bool SVRunStatusClass::IsCriticalFailure()
{
	return run.status.criticalFailure;
}

inline bool SVRunStatusClass::HasInspectionStarted()
{
	return run.status.run;
}

inline DWORD SVRunStatusClass::GetState()
{
	return run.state;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVRunControlLibrary\SVRunStatus.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:21:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   25 Mar 2013 12:38:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the AddError method and the error list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Feb 2013 11:17:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added m_ErrorList to the RunStatus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:06:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Apr 2011 13:28:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated run status object to include a flag for updating counters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 16:08:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2010 10:56:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element to run status for critical errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:38:52   jspila
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
 *    Rev 3.7   02 Nov 2005 09:02:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed long m_lResultImageIndex to SVImageIndexStruct Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   02 Sep 2005 14:33:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made speed optimizations by making functions inline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 May 2005 07:28:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  486
 * SCR Title:  Add Trigger Count to Tool Set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add trigger count value object to tool set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 15:12:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 13:16:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   04 Apr 2002 10:23:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   03 May 2000 10:26:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  119
 * SCR Title:  SVFocusNT Support
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Removed the "#pragma message" because it was making compiler results too difficult to comprehend.
 * 
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
 *    Rev 1.0   30 Aug 1999 18:59:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   New Class to control run status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
