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

#pragma once

//Moved to precompiled header: #include <deque>
#include "SVImageIndexStruct.h"
#include "ProductWorkloadInformation.h"
#include "SVUtilityLibrary/SVGUID.h"

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

	ProductWorkloadInformation m_WorkloadInfoRsc; ///< workload information for the most recently completed product 
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

