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

#pragma once

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

static const int  DefaultNakParameter = 25;

//*********************************
//! Enum for possible values of the NAKmode. The NAKmode can be set with SVIM.ini file. The defaultvalue is burst.
//  The NAKmode   determines for what product the next inspection is started in the case that NAKs occurs.
//	(without NAKS of course the oldest Product will be inspected next)
//************************************

enum NakGeneration
{
	Legacy = 0,				//! the same behavior as it is in SVObserver 7.30 
	Bursts = 1,				//! When more then 2 NAKS occurs: Starts an inspection with the newest Product and then take the following,
	RepairedLegacy = 2,		//! reduces the number of inspection which are considered to be taken next proportional to the NAK number.
	FixedMaximum =3			//!  Regardless of a nack occurred or not take the inspection from the front part from the PPQbuffer. Front part is determined by NAKPar .
};




