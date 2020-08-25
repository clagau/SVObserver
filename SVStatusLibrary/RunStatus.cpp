//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RunStatus
//* .File Name       : $Workfile:   RunStatus.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:21:20  $
//******************************************************************************

#include "stdafx.h"
#include "RunStatus.h"
#include "Definitions/Color.h"

RunStatus::RunStatus()
{
	run.state = SV_INVALID;
	run.status.valid = true;
}

void RunStatus::ResetRunStateAndToolSetTimes()
{
	run.state = SV_INVALID;
	run.status.valid = true;
	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;
}


DWORD RunStatus::GetStatusColor() const
{
	if (IsDisabled() || IsDisabledByCondition())
	{
		return SvDef::DefaultDisabledColor;
	}

	return (IsFailed() ? SvDef::DefaultFailedColor : (IsWarned() ? SvDef::DefaultWarnedColor : SvDef::DefaultGoodColor));
}



