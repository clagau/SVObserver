//******************************************************************************
//* COPYRIGHT (c) 2012 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerConstants
//* .File Name       : $Workfile:   SVTriggerConstants.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:26  $
//******************************************************************************
#pragma once

namespace SvTrig
{
	constexpr const char* SoftwareTriggerName = _T("SoftwareTrigger");
	constexpr const char* CameraTriggerName = _T("CameraTrigger");
	constexpr long TimerPeriod = 200;
	constexpr long MinTimerPeriod_ms = 4;
	constexpr long MaxTimerPeriod_ms = 120000;
} //namespace SvTrig
