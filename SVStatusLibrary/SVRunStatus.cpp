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
#include "Definitions/Color.h"

SVRunStatusClass::SVRunStatusClass()
{
	run.state = SV_INVALID;
	run.status.valid = true;
}

void SVRunStatusClass::ResetRunStateAndToolSetTimes()
{
	run.state = SV_INVALID;
	run.status.valid = true;
	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;
}


DWORD SVRunStatusClass::GetStatusColor() const
{
	if (IsDisabled() || IsDisabledByCondition())
	{
		return SvDef::DefaultDisabledColor;
	}

	return (IsFailed() ? SvDef::DefaultFailedColor : (IsWarned() ? SvDef::DefaultWarnedColor : SvDef::DefaultGoodColor));
}



