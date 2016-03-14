//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEnums
//* .File Name       : $Workfile:   SVPPQEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:32:54  $
//******************************************************************************

#ifndef SVPPQENUMS_H
#define SVPPQENUMS_H

enum SVPPQOutputModeEnum
{
	SVPPQUnknownMode                          = -1,

	SVPPQNextTriggerMode                      = 0,  // Resets outputs on trigger. Writes outputs immediately.

	SVPPQTimeDelayMode                        = 1,  // Resets outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
	                                                // Aborts waiting if new trigger occurs.

	SVPPQTimeDelayAndDataCompleteMode         = 2,  // Resets outputs on trigger. Writes outputs after delay time is over and data is complete.
	                                                // Aborts waiting if new trigger occurs.

	SVPPQExtendedTimeDelayMode                = 3,  // Doesn't reset outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
	                                                // Aborts waiting if product leaves PPQ.

	SVPPQExtendedTimeDelayAndDataCompleteMode = 4,  // Doesn't reset outputs on trigger. Writes outputs after delay time is over and data is complete.
	                                                // Aborts waiting if product leaves PPQ.
};

#endif

