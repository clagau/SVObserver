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

	m_WorkloadInfoRsc.Reset();
}



void SVRunStatusClass::ResetRunStateAndToolSetTimes()
{ 
	run.state = SV_INVALID;

	run.status.valid = true;

	m_UpdateCounters = true;

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;
}

DWORD SVRunStatusClass::GetStatusColor() const
{
	if( IsDisabled() || IsDisabledByCondition() )
		return SV_DEFAULT_DISABLED_COLOR;

	return ( IsFailed() ? SV_DEFAULT_FAILED_COLOR : 
				( IsWarned() ? SV_DEFAULT_WARNED_COLOR : SV_DEFAULT_GOOD_COLOR ) );
}



